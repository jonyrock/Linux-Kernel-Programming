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
static const struct inode_operations my_dir_inode_operations;


static struct inode *my_make_inode(struct super_block *sb, int mode) {
    struct inode *ret = new_inode(sb);

    if (ret) {
        ret->i_mode = mode;
        ret->i_uid = ret->i_gid = 0;
//        ret->
        ret->i_blocks = 0;
        ret->i_atime = ret->i_mtime = ret->i_ctime = CURRENT_TIME;
    }
    return ret;
}


static int my_open(struct inode *inode, struct file *filp) {
    filp->private_data = inode->i_private;
    return 0;
}

static ssize_t my_read_file(struct file *filp, char *buf,
        size_t count, loff_t *offset) {
        
    char* data = (char*)filp->private_data;
    int len;
    
    len = strlen(data);
    if (count > len - *offset) {
        count = len - *offset;
    }

    if (copy_to_user(buf, data + *offset, count)) {
        return -EFAULT;
    }
    *offset += count;
    return count;
}

static struct file_operations my_file_ops = {
    .open = my_open,
    .read = my_read_file,
};


static struct inode *my_create_file (struct super_block *sb,
        struct dentry *dir, const char *name) {
        
    struct dentry *dentry;
    struct inode *inode;
    struct qstr qname;
    
    qname.name = name;
    qname.len = strlen (name);
    qname.hash = full_name_hash(name, qname.len);
    
    dentry = d_alloc(dir, &qname);
    if (!dentry) {
        return 0;
    }
    
    inode = my_make_inode(sb, S_IFREG | 0644);
    if (!inode) {
        dput(dentry);
        return 0;
    }
    
    inode->i_fop = &my_file_ops;
    d_add(dentry, inode);
    return inode;
}


static struct dentry *my_create_dir (struct super_block *sb,
        struct dentry *parent, const char *name) {
        
    struct dentry *dentry;
    struct inode *inode;
    struct qstr qname;

    qname.name = name;
    qname.len = strlen (name);
    qname.hash = full_name_hash(name, qname.len);

    dentry = d_alloc(parent, &qname);
    if (!dentry) {
        return 0;
    }

    inode = my_make_inode(sb, S_IFDIR | 0644);
    if (!inode) {
        dput(dentry);
        return 0;
    }
    
    inode->i_op = &simple_dir_inode_operations;
    inode->i_fop = &simple_dir_operations;
    d_add(dentry, inode);
    return dentry;
}

static void my_create_files (struct super_block *sb, struct dentry *root) {
    struct dentry *info;
    struct dentry *personal;
    struct dentry *rating;

    struct inode *name;
    struct inode *mail;
    struct inode *phone;
    struct inode *math;
    struct inode *programming;
    struct inode *engineering;

    info = my_create_dir(sb, root, "info");
    if (!info)
        return;
        
    personal = my_create_dir(sb, info, "personal");
    if (!personal)
        return;
        
    name = my_create_file(sb, personal, "name");
    name->i_private = "Alexey Velikiy";
    
    mail = my_create_file(sb, personal, "mail");
    mail->i_private = "gmpota@gmail.com";
    
    phone = my_create_file(sb, personal, "phone");
    phone->i_private = "911";
    
    rating = my_create_dir(sb, root, "rating");
    if (!rating)
        return;
        
    math = my_create_file(sb, rating, "math");
    math->i_private = "123213";
        
    programming = my_create_file(sb, rating, "programming");
    programming->i_private = "12331231212";
        
    engineering = my_create_file(sb, rating, "engineering");
    engineering->i_private = "12321321";
}


int my_fill_super(struct super_block *sb, void *data, int silent) 
{
    struct inode *inode = NULL;
    struct dentry *root;
    int err;
    
    sb->s_maxbytes		= MAX_LFS_FILESIZE;
    sb->s_blocksize		= PAGE_CACHE_SIZE;
    sb->s_blocksize_bits	= PAGE_CACHE_SHIFT;
    sb->s_magic		        = MYFS_MAGIC;
    sb->s_op		        = &my_ops;
    sb->s_time_gran		= 1;
    
    inode = my_make_inode(sb, S_IFDIR | 0755);
    if (!inode) {
        err = -ENOMEM;
        goto fail;
    }

    inode->i_op = &simple_dir_inode_operations;
    inode->i_fop = &simple_dir_operations;
        
    root = d_alloc_root(inode);
    sb->s_root = root;
    
    my_create_files(sb, root);

    return 0;

fail:
    sb->s_fs_info = NULL;
    iput(inode);
    return err;

}

struct dentry *my_mount(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data)
{
    printk("My fs mounting\n");
    return mount_nodev(fs_type, flags, data, my_fill_super);
}

static void my_kill_sb(struct super_block *sb)
{
    kfree(sb->s_fs_info);
    kill_litter_super(sb);
}

static const struct super_operations my_ops = {
    .statfs		= simple_statfs,
    .drop_inode	        = generic_delete_inode,
};

static struct file_system_type my_fs_type = {
    .name	= "myfs",
    .mount	= my_mount,
    .kill_sb    = my_kill_sb,
    .owner      = THIS_MODULE
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




















