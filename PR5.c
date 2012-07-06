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
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/magic.h>
#include <linux/slab.h>

#define MYFS_MAGIC 0x74736263

/** Declarations */
static struct file_system_type ramfs_fs_type;
static const struct super_operations ramfs_ops;
struct ramfs_mount_opts { umode_t mode; };
struct ramfs_fs_info { struct ramfs_mount_opts mount_opts; };
enum { Opt_mode, Opt_err };

int ramfs_fill_super(struct super_block *sb, void *data, int silent) 
{
	sb->s_maxbytes		= MAX_LFS_FILESIZE;
	sb->s_blocksize		= PAGE_CACHE_SIZE;
	sb->s_blocksize_bits	= PAGE_CACHE_SHIFT;
	sb->s_magic		= MYFS_MAGIC;
	sb->s_op		= NULL;
	sb->s_time_gran		= 1;

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

static const struct super_operations ramfs_ops = {
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= generic_show_options,
};

static struct file_system_type ramfs_fs_type = {
	.name		= "myfs",
	.mount		= ramfs_mount,
	.kill_sb	= ramfs_kill_sb,
};

static int __init start(void)
{
    return register_filesystem(&ramfs_fs_type);
}



module_init(start);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Velikiy <gmpota@gmail.com>");


/******************************************************************************/



















