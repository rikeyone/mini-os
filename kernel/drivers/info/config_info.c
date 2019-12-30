#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include "config_info.h"


static int config_info_proc_show(struct seq_file *m, void *v)
{
	int num = ARRAY_SIZE(config);
	int i;
	for (i = 0; i < num; i++)
		seq_printf(m, "%s\n", config[i]);
	return 0;
}

static int config_info_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, config_info_proc_show, NULL);
}

static const struct file_operations config_info_proc_fops = {
	.open		= config_info_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_config_info_init(void)
{
	proc_create("config_info", 0, NULL, &config_info_proc_fops);
	return 0;
}
module_init(proc_config_info_init);

