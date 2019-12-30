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

char gbuf[1024];

static int proc_info_rw_show(struct seq_file *m, void *v)
{
	static int count;
	seq_printf(m, "procfs rw file printk,count:%d\n", count++);
	seq_printf(m, "procfs rw file write content:%s\n", gbuf);
	return 0;
}

static int proc_info_rw_open(struct inode *inode, struct file *file)
{
	/* If a client of seq_files a) implements file.write() and b) wishes to
	 * support pwrite() then that client will need to implement its own
	 * file.open() which calls seq_open() and then sets FMODE_PWRITE.
	 **/
	int res = -ENOMEM;
	res = single_open(file, proc_info_rw_show, NULL);
	//file->f_mode |= FMODE_PWRITE;
	return res;
}

static ssize_t proc_info_rw_write(struct file *file, const char __user *buf,
		                          size_t count, loff_t *ppos)
{
	//struct seq_file *m = (struct seq_file *)file->private_data;
	char *kbuf = (char *)kmalloc(1024, GFP_KERNEL);
	int ret;
	ret = __copy_from_user(kbuf, buf, count);
	snprintf(gbuf, count, "%s", kbuf);
	kfree(kbuf);
	return count;
}


static const struct file_operations proc_info_rw_fops = {
	.open		= proc_info_rw_open,
	.read		= seq_read,
	.write      = proc_info_rw_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int proc_info_ro_show(struct seq_file *m, void *v)
{
	static int count;
	seq_printf(m, "procfs ro printk,count:%d\n", count++);
	return 0;
}

static int proc_info_ro_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_info_ro_show, NULL);
}

static const struct file_operations proc_info_ro_fops = {
	.open		= proc_info_ro_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int debug_value = 1;

static int __init debugfile_init(void)
{
	struct proc_dir_entry *procdir = NULL;
	struct dentry *debugdir = NULL;

	/* procfs create*/
	procdir = proc_mkdir("procdir", NULL);
	if (procdir == NULL) {
		pr_err("proc fs mkdir failed\n");
		return -ENOMEM;
	}
	proc_create("ro_file", 0, procdir, &proc_info_ro_fops);
	proc_create("rw_file", 0, procdir, &proc_info_rw_fops);

	/* debugfs create */
	debugdir = debugfs_create_dir("debugdir", NULL);
	if (debugdir == NULL) {
		pr_err("debug fs mkdir failed\n");
		return -ENOMEM;
	}
	debugfs_create_u32("debug", 0600, debugdir, &debug_value);
	return 0;
}
module_init(debugfile_init);

MODULE_LICENSE("GPL v2");
