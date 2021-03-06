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

struct inode *ramfs_get_inode(struct super_block *sb,
				const struct inode *dir, umode_t mode, dev_t dev)
{
    struct inode * inode = new_inode(sb);
    init_special_inode(inode, mode, dev);
    return inode;
}

int ramfs_fill_super(struct super_block *sb, void *data, int silent) 
{
        struct inode *inode = NULL;
        struct dentry *root;
        int err;
        struct ramfs_fs_info *fsi;
        
        fsi = kzalloc(sizeof(struct ramfs_fs_info), GFP_KERNEL);
	sb->s_fs_info = fsi;
	if (!fsi) {
            err = -ENOMEM;
            goto fail;
	}

	sb->s_maxbytes		= MAX_LFS_FILESIZE;
	sb->s_blocksize		= PAGE_CACHE_SIZE;
	sb->s_blocksize_bits	= PAGE_CACHE_SHIFT;
	sb->s_magic		= MYFS_MAGIC;
	sb->s_op		= &ramfs_ops;
	sb->s_time_gran		= 1;
        
        inode = ramfs_get_inode(sb, NULL, S_IFDIR | fsi->mount_opts.mode, 0);
	if (!inode) {
            err = -ENOMEM;
            goto fail;
	}


        root = d_alloc_root(inode);
	sb->s_root = root;

	return 0;

fail:
	kfree(fsi);
	sb->s_fs_info = NULL;
	iput(inode);
	return err;

}

struct dentry *ramfs_mount(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data)
{
	return mount_nodev(fs_type, flags, data, ramfs_fill_super);
}

static const struct super_operations ramfs_ops = {
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= generic_show_options,
};

static struct file_system_type ramfs_fs_type = {
	.name		= "myfs",
	.mount		= ramfs_mount,
	.kill_sb	= kill_anon_super,
};

static int __init start(void)
{
    return register_filesystem(&ramfs_fs_type);
}



module_init(start);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Velikiy <gmpota@gmail.com>");


/******************************************************************************/




















