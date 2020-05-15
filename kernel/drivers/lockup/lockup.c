#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <asm-generic/delay.h>
#include <linux/kthread.h>


static int soft_lockup = 1;
module_param(soft_lockup, int, 0644);
MODULE_PARM_DESC(soft_lockup, "soft_lockup");

static struct task_struct *taskid;
static spinlock_t slock;

static int kthread_lockup(void *data)
{
    spinlock_t *lock;
    lock = (spinlock_t *)data;
    while(!kthread_should_stop()) {
        udelay(19000);
        if (soft_lockup) {
            spin_lock(lock);
            spin_lock(lock);
        } else {
            spin_lock_irq(lock); //disable local irq
            spin_lock_irq(lock);
        }
    }
    return 0;
}

static int __init lockup_init(void)
{
    int err;
    spin_lock_init(&slock);
    taskid = kthread_run(kthread_lockup, &slock, "lockup_test");
    if (IS_ERR(taskid)) {
		err = PTR_ERR(taskid);
		return err;
	}
    return 0;
}

static void __init lockup_exit(void)
{
    kthread_stop(taskid);
}

module_init(lockup_init);
module_exit(lockup_exit);
MODULE_AUTHOR("Haocheng Xie");
MODULE_LICENSE("GPL v2");