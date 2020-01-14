#define pr_fmt(fmt)  "[chardev][%s:%d] " fmt, __func__, __LINE__

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h> //printk.h
#include <linux/proc_fs.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/semaphore.h>


#define USE_DEV_ATTR_GROUP


#define CDEV_RAM_SIZE (16)
#define RINGBUF_FULL_SIZE (CDEV_RAM_SIZE - 1)

#define CHAR_DEVICE_NAME "ramchar"

static int debug_value = 1;
#define ramdev_dbg(fmt, ...) \
	if (debug_value) \
		pr_info("debug:"fmt, ##__VA_ARGS__)

#define ramdev_err(fmt, ...) \
	if (debug_value) \
		pr_info("err:"fmt, ##__VA_ARGS__)

#define ramdev_info(fmt, ...) \
	if (debug_value) \
		pr_info("info:"fmt, ##__VA_ARGS__)

struct ramdev {
	struct device *device;
	dev_t dev;
	struct cdev cdev;
	struct class *class;
	struct mutex slock;
	struct semaphore sem;
	wait_queue_head_t r_wait;
	void *buffer;
	int start;
	int end;
};

static struct ramdev *g_dev;

/*
* Ring Buffer Operations
* 环形缓冲区操作，主要关键点：
* start 表示要读取的起始位置（需要被读取）； end表示下一次要写入的起始位置（还没有写）
* 1.读操作
* 需要注意读操作的溢出问题，此时start比end要大，读取操作分为两个区间：
* 0---end---start---max_size
* [start,max_size] 以及 [0, end]
*
* 2.写操作
* 写操作相比读操作更加复杂一些，它需要处理两种溢出情况：
* a.第一种溢出情况是当前要写入的数据大小超过整个缓冲区的大小
* 这种情况下需要把写入操作细分为多次，每次大小不超过ringbuffer的大小
*
* b.第二种溢出情况是当前要写入的数据大小超过缓冲区中空闲区域大小
* 这种情况下，新写入的数据会覆盖最老的数据，需要后移start的位置，丢弃旧数据，预留出足够保存新数据的位置
* 然后拷贝数据到缓冲区。
*/
static int ring_buffer_count(struct ramdev *dev)
{
	return (dev->end - dev->start + CDEV_RAM_SIZE) % CDEV_RAM_SIZE;
}

static int ring_buffer_overflow(int size)
{
	return (size >= RINGBUF_FULL_SIZE) ? 1 : 0;
}

static int ring_buffer_read(struct ramdev *dev, char __user *buf, int size)
{
	int count, ret, len;

	mutex_lock(&dev->slock);
	count = (dev->end - dev->start + CDEV_RAM_SIZE) % CDEV_RAM_SIZE;
	if (count < size)
		size = count;

	if (dev->start + size <= CDEV_RAM_SIZE) {
		if(copy_to_user(buf, dev->buffer + dev->start, size)){
			ret = -EFAULT;
			goto out;
		}
	} else {
		len = CDEV_RAM_SIZE - dev->start;
		if (copy_to_user(buf, dev->buffer + dev->start, len)) {
			ret = -EFAULT;
			goto out;
		}
		if (copy_to_user(buf + len, dev->buffer, size - len)) {
			ret = -EFAULT;
			goto out;
		}
	}
	dev->start = (dev->start + size) % CDEV_RAM_SIZE;
	ret = size;
	ramdev_info("Read ringbuffer: start:%d end:%d count:%d size:%d\n",
		dev->start, dev->end, (dev->end - dev->start + CDEV_RAM_SIZE) % CDEV_RAM_SIZE, size);
out:
	mutex_unlock(&dev->slock);
	return ret;
}

static int __ring_buffer_write(struct ramdev *dev, const char __user *buf, int size)
{
	int count, unused, len, ret = 0;

	/* write data to ringbuffer */
	if (dev->end + size <= CDEV_RAM_SIZE) {
		if(copy_from_user(dev->buffer + dev->end, buf, size)){
			ret = -EFAULT;
			goto out;
		}
	} else {
		len = CDEV_RAM_SIZE - dev->end;
		if (copy_from_user(dev->buffer + dev->end, buf, len)) {
			ret = -EFAULT;
			goto out;
		}
		if (copy_from_user(dev->buffer, buf + len, size - len)) {
			ret = -EFAULT;
			goto out;
		}
	}
	/* move ringbuf start point to write new data */
	count = (dev->end - dev->start + CDEV_RAM_SIZE) % CDEV_RAM_SIZE;
	unused = RINGBUF_FULL_SIZE - count;
	if (size > unused) {
		dev->start = (dev->start + size - unused) % CDEV_RAM_SIZE;
	}
	/* update ringbuf end point */
	dev->end = (dev->end + size) % CDEV_RAM_SIZE;
	ret = size;
	ramdev_info("Write ringbuffer: start:%d end:%d count:%d size:%d\n",
		dev->start, dev->end, (dev->end - dev->start + CDEV_RAM_SIZE) % CDEV_RAM_SIZE, size);
out:
	return ret;
}

static int ring_buffer_write(struct ramdev *dev, const char __user *buf, int size)
{
	int ret, left;

	left = size;
	mutex_lock(&dev->slock);
	while (ring_buffer_overflow(left)) {
		ret = __ring_buffer_write(dev, buf, RINGBUF_FULL_SIZE) ;
		if (ret < 0) {
			ramdev_err("ring buffer write error\n");
			goto out;
		}
		buf += RINGBUF_FULL_SIZE;
		left -= RINGBUF_FULL_SIZE;
	}
	if (left) {
		ret = __ring_buffer_write(dev, buf, left);
		if (ret < 0) {
			ramdev_err("ring buffer write error\n");
			goto out;
		}
	}

	ret = size;
	wake_up_interruptible(&dev->r_wait);
	ramdev_info("wake up wait queue\n");
out:
	mutex_unlock(&dev->slock);
	return ret;
}

/* char device file operations */
static int dev_open(struct inode *inode, struct file *file)
{
	struct ramdev *dev;
	dev = container_of(inode->i_cdev, struct ramdev, cdev);
	file->private_data = dev;
	return 0;
}

static ssize_t dev_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	struct ramdev *dev = (struct ramdev *)file->private_data;
	int ret = 0;

	DECLARE_WAITQUEUE(wait, current);
	add_wait_queue(&dev->r_wait, &wait);
	/* Get the semaphore, avoid reentrant */
	if (down_interruptible(&dev->sem)) {
		ret = -EBUSY;
		goto out2;
	}
	if (ring_buffer_count(dev) == 0) {
		if (file->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);
		/* release semaphore before sleep */
		up(&dev->sem);
		/* schedule to sleep */
		schedule();

		/* Waken up by signal */
		if (signal_pending(current)) {
			ret = -ERESTARTSYS;
			goto out2;
		}
		/*
		 * Waken up by data, get the semaphore again to read
		 */
		if (down_interruptible(&dev->sem)) {
			ret = -EBUSY;
			goto out2;
		}
	}

	/* Real data read operation */
	ret = ring_buffer_read(dev, buf, size);
out:
	up(&dev->sem); /*释放信号量*/
out2:
	remove_wait_queue(&dev->r_wait, &wait);
	set_current_state(TASK_RUNNING);
	return ret;
}

static ssize_t dev_write(struct file *file, const char __user *buf,
		                          size_t count, loff_t *ppos)
{
	struct ramdev *dev = (struct ramdev *)file->private_data;
	count = ring_buffer_write(dev, buf, count);
	return count;
}

static int dev_release(struct inode *inode, struct file *file)
{
	return 0;
}


static loff_t dev_lseek(struct file *file, loff_t offset, int whence)
{
	return 0;
}


static const struct file_operations cdev_fops = {
	.open		= dev_open,
	.read		= dev_read,
	.write		= dev_write,
	.llseek		= dev_lseek,
	.release	= dev_release,
};

static ssize_t show_debug(struct device *dev,
							struct device_attribute *attr, char *buf)
{

	return sprintf(buf, "debug value:%d\n", debug_value);
}

static ssize_t store_debug(struct device *dev,
							struct device_attribute *attr,
							const char *buf, size_t size)
{
	sscanf(buf, "%d", &debug_value);
	return size;
}


static ssize_t show_buffer_count(struct device *dev,
							struct device_attribute *attr, char *buf)
{
	struct ramdev *ramdev = container_of(&dev, struct ramdev, device);
	return sprintf(buf, "%d\n", (ramdev->end + CDEV_RAM_SIZE - ramdev->start) % CDEV_RAM_SIZE);
}

static DEVICE_ATTR(debug, 0660, show_debug, store_debug);
static DEVICE_ATTR(count, 0440, show_buffer_count, NULL);

#ifdef USE_DEV_ATTR_GROUP
static struct attribute *ramchar_attributes[] = {
    &dev_attr_debug.attr,
	&dev_attr_count.attr,
    NULL
};

static const struct attribute_group ramchar_attr_group = {
    .attrs = ramchar_attributes,
};
#endif

static int __init ramdev_init(void)
{
	struct ramdev *ramdev;
	int rc = 0;

	ramdev = (struct ramdev *)kmalloc(sizeof(*ramdev), GFP_KERNEL);
	if (!ramdev) {
		ramdev_err("kmalloc error, memory alloc failed!\n");
		goto alloc_fail1;
	}
	ramdev->buffer= kmalloc(CDEV_RAM_SIZE, GFP_KERNEL);
	if (!ramdev) {
		ramdev_err("kmalloc error, memory alloc failed!\n");
		goto alloc_fail2;
	}

	/* ramdev struct initialize */
	mutex_init(&ramdev->slock);
	init_waitqueue_head(&ramdev->r_wait);
	sema_init(&ramdev->sem, 1);
	ramdev->start = 0;
	ramdev->end = 0;

	/* dev number alloc */
	rc = alloc_chrdev_region(&ramdev->dev, 0, 1, CHAR_DEVICE_NAME);
	if (rc < 0) {
		pr_err("region allocation failed\n");
		goto region_fail;
	}

	/* cdev initialize */
	cdev_init(&ramdev->cdev, &cdev_fops);
	ramdev->cdev.owner = THIS_MODULE;
	rc = cdev_add(&ramdev->cdev, ramdev->dev, 1);
	if (rc < 0) {
		pr_err("device registation failed\n");
		goto cdev_fail;
	}
	/* create device in sysfs */
	ramdev->class = class_create(THIS_MODULE, CHAR_DEVICE_NAME);
	ramdev->device = device_create(ramdev->class, NULL, ramdev->dev, NULL, CHAR_DEVICE_NAME);
	if (ramdev->device == NULL) {
		ramdev_err("");
		goto cdev_fail;
	}
	#ifdef USE_DEV_ATTR_GROUP
	/* create sysfs and meizu class link files */
	rc = sysfs_create_group(&ramdev->device->kobj, &ramchar_attr_group);
	if (rc < 0) {
		ramdev_err("Failed to create attribute sysfs\n");
		goto cdev_fail;
	}
	#else
	rc = device_create_file(ramdev->device, &dev_attr_debug);
	if (rc < 0) {
		ramdev_err("Failed to create attribute sysfs\n");
		goto cdev_fail;
	}
	rc = device_create_file(ramdev->device, &dev_attr_count);
	if (rc < 0) {
		ramdev_err("Failed to create attribute sysfs\n");
		goto cdev_fail;
	}
	#endif
	g_dev = ramdev;
	return 0;

cdev_fail:
	unregister_chrdev_region(ramdev->dev, 1);
region_fail:
	kfree(ramdev->buffer);
alloc_fail2:
	kfree(ramdev);
alloc_fail1:
	return -ENOMEM;
}

void ramdev_exit(void)
{
	if (g_dev) {
		#ifndef USE_DEV_ATTR_GROUP
		device_remove_file(g_dev->device, &dev_attr_debug);
		device_remove_file(g_dev->device, &dev_attr_count);
		#endif
		device_destroy(g_dev->class, g_dev->dev);
		class_destroy(g_dev->class);
		cdev_del(&g_dev->cdev);
		unregister_chrdev_region(g_dev->dev, 1);
		kfree(g_dev->buffer);
		kfree(g_dev);
	}
}

module_init(ramdev_init);
module_exit(ramdev_exit);
MODULE_AUTHOR("xiehaocheng");
MODULE_LICENSE("GPL v2");
