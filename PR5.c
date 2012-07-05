/******************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/backing-dev.h>
#include <linux/ramfs.h>
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/magic.h>
#include <linux/slab.h>

int ramfs_fill_super(struct super_block *sb, void *data, int silent) 
{
    return 0;
}

struct dentry *ramfs_mount(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data)
{
	return mount_nodev(fs_type, flags, data, ramfs_fill_super);
}

static void ramfs_kill_sb(struct super_block *sb)
{
	kfree(sb->s_fs_info);
	kill_litter_super(sb);
}


static struct file_system_type ramfs_fs_type = {
	.name		= "myfs",
	.mount		= ramfs_mount,
	.kill_sb	= ramfs_kill_sb,
};

static int __init start(void)
{
    return register_filesystem(&ramfs_fs_type);
}

static void __exit stop(void)
{
    printk("\n    --end--    \n");
}

module_init(start);
module_exit(stop);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Velikiy <gmpota@gmail.com>");




/******************************************************************************/

