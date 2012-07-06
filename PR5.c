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
static struct file_system_type my_fs_type;
static const struct super_operations my_ops;
static const struct file_operations my_dir_operations;


static struct dentry *
my_inode_lookup(struct inode *parent_inode, struct dentry *dentry, 
                  struct nameidata *nameidata) {

//    struct inode *file_inode;

    printk("my_inode_lookup%s\n", dentry->d_name.name);

    return NULL;
}


int my_f_readdir( struct file *file, void *dirent, filldir_t filldir ) {
    //int err;
    //struct dentry *de = file->f_dentry;
    
    printk( "myfs: file_operations.readdir called\n" );

    return 0;

//    printk( "rkfs: file_operations.readdir called\n" );
//    if(file->f_pos > 0)
//        return 1;
//    if(filldir(dirent, ".", 1, file->f_pos++, de->d_inode->i_ino, DT_DIR)||
//       (filldir(dirent, "..", 2, file->f_pos++, de->d_parent->d_inode->i_ino, DT_DIR)))
//        return 0;
//    if(filldir(dirent, "hello.txt", 9, file->f_pos++, FILE_INODE_NUMBER, DT_REG ))
//        return 0;
//    return 1;
}

struct inode *my_get_inode(struct super_block *sb,
				const struct inode *dir, umode_t mode, dev_t dev)
{
    struct inode * inode = new_inode(sb);
    init_special_inode(inode, mode, dev);
    return inode;
}

int my_fill_super(struct super_block *sb, void *data, int silent) 
{
    struct inode *inode = NULL;
    struct dentry *root;
    int err;
    
    sb->s_maxbytes		= MAX_LFS_FILESIZE;
    sb->s_blocksize		= PAGE_CACHE_SIZE;
    sb->s_blocksize_bits	= PAGE_CACHE_SHIFT;
    sb->s_magic		= MYFS_MAGIC;
    sb->s_op		= &my_ops;
    sb->s_time_gran		= 1;
    
    inode = my_get_inode(sb, NULL, S_IFDIR, 0);
    if (!inode) {
        err = -ENOMEM;
        goto fail;
    }
    
    inode->i_fop = &my_dir_operations;
    
    root = d_alloc_root(inode);
    sb->s_root = root;
    
    return 0;

fail:
    sb->s_fs_info = NULL;
    iput(inode);
    return err;

}

struct dentry *my_mount(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data)
{
    return mount_nodev(fs_type, flags, data, my_fill_super);
}

static void my_kill_sb(struct super_block *sb)
{
    kfree(sb->s_fs_info);
    kill_litter_super(sb);
}

static const struct super_operations my_ops = {
    .statfs		= simple_statfs,
    .drop_inode	= generic_delete_inode,
    .show_options	= generic_show_options,
};

static struct file_system_type my_fs_type = {
    .name	= "myfs",
    .mount	= my_mount,
    .kill_sb    = my_kill_sb,
    .owner      = THIS_MODULE
};

static const struct inode_operations my_dir_inode_operations = {
    .create	= NULL,
    .lookup	= my_inode_lookup,
    .link	= simple_link,
    .unlink	= simple_unlink,
    .symlink	= NULL,
    .mkdir	= NULL,
    .rmdir	= simple_rmdir,
    .mknod	= NULL,
    .rename	= simple_rename,
};


static const struct file_operations my_dir_operations = {
    .readdir = my_f_readdir,
};

static int __init start(void)
{
    return register_filesystem(&my_fs_type);
}

static void __exit stop(void)
{
    unregister_filesystem(&my_fs_type);
}


module_init(start);
module_exit(stop);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexey Velikiy <gmpota@gmail.com>");


/******************************************************************************/




















