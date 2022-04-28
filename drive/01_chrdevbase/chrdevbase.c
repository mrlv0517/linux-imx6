/**
 * @File Name  : chrdevbase.c
 * @brief  
 * @Author     : xiaoyu (mrlv1992@163.com)
 * @Version    : V1.0
 * @Creat Date : 2022-04-15
 * 
 * @copyright Copyright (C) 2022
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>


#define DEV_MAJOR 200           /// 主设备号
#define DEV_NAME  "chardev"     /// 设备名称

static char readbuf[100];       /// 读缓冲区
static char writebuf[100];      /// 写缓冲区
static char kerneldata[] = "kernel data!";

/**
 * @brief  : open dev
 * @param  : inode 
 * @param  : filep 
 * @return : int 
 */
static int chrdevbase_open(struct inode *inode, struct file *filep)
{
    printk("chrdevbase open!\r\n");
    return 0;
}

/**
 * @brief  : 
 * @param  : filep 
 * @param  : buf 
 * @param  : cnt 
 * @param  : offt 
 * @return : ssize_t 
 */
static ssize_t chrdevbase_read(struct file *filep, char __user *buf, size_t cnt, loff_t *offt)
{
    printk("chrdevbase read!\r\n");
    readbuf[0] = 1;
    return 0;
}

/**
 * @brief  : 
 * @param  : filep 
 * @param  : buf 
 * @param  : cnt 
 * @param  : offt 
 * @return : ssize_t 
 */
static ssize_t chrdevbase_write(struct file *filep, const char __user *buf, size_t cnt, loff_t *offt)
{
    memcpy(writebuf, kerneldata, sizeof(kerneldata));
    printk("chrdevbase write!\r\n");
    return 0;
}

/**
 * @brief  : 
 * @param  : inode 
 * @param  : filep 
 * @return : int 
 */
static int chrdevbase_release(struct inode *inode, struct file *filep)
{
    printk("chrdevbase release!\r\n");
    return 0;
}

/**
 * @brief  : 
 */
static struct file_operations chrdevbase_fops = {
    .owner   = THIS_MODULE,
    .open    = chrdevbase_open,
    .read    = chrdevbase_read,
    .write   = chrdevbase_write,
    .release = chrdevbase_release,
};

/**
 * @brief  : 
 * @return : int chrdevbase_init 
 */
static int __init chrdevbase_init(void)
{
    int retvalue = 0;

    retvalue = register_chrdev(DEV_MAJOR, DEV_NAME, &chrdevbase_fops);

    if (retvalue < 0){
        printk("chrdevbase driver register failed\r\n");
    }

    printk("chrdevbase_init\r\n");

    return 0;
}

/**
 * @brief  : 
 */
static void __exit chrdevbase_exit(void)
{
    unregister_chrdev(DEV_MAJOR, DEV_NAME);
    
    printk("chrdevbase_exit\r\n");

    return;
}

/**
 * @brief  : 
 */

module_init(chrdevbase_init);
module_exit(chrdevbase_exit);

/**
 * @brief  : 
 */

MODULE_LICENSE("GPL");
