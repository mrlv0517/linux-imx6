/**
 * @File Name  : newchrled.c
 * @brief  : 
 * @Author     : xiaoyu (mrlv1992@163.com)
 * @Version    : V1.0
 * @Creat Date : 2022-04-27
 * 
 * @copyright Copyright (C) 2022
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/ide.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define NEWCHRLED_CNT  1
#define NEWCHRLED_NAME "newchrled"

#define LED_OFF   0     ///< led off
#define LED_ON    1     ///< led on

/* Register physical address */
#define CCM_CCGR1_BASE           (0x020C406C)
#define SW_MUX_GPIO1_IO03_BASE   (0x020E0068)
#define SW_PAD_GPIO1_IO03_BASE   (0x020E02F4)
#define GPIO1_DR_BASE            (0x0209C000)
#define GPIO1_GDIR_BASE          (0x0209C004)

/* virtual address pointer */
static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;

/* newchrled device struct */
struct newchrled_dev{
    dev_t devid;                ///< device id
    struct cdev    cdev;        ///< char dev struct
    struct class  *class;       ///< class struct
    struct device *device;      ///< device struct
    int major;                  ///< main device number
    int minor;                  ///< 
};

struct newchrled_dev newchrled; ///< led device

/**
 * @brief  : LED on/off
 * @param  : sta LED_ON(0) ON LED, LED_OFF(1) OFF LED
 */
void led_switch(u8 sta)
{
    u32 val = 0;
    val = readl(GPIO1_DR);

    if (sta == LED_ON)
    {
        val &= ~(1 << 3);
    }
    else
    {
        val |= (1 << 3);
    }

    writel(val, GPIO1_DR);
    return;
}

/**
 * @brief  : open dev
 * @param  : inode 
 * @param  : filp 
 * @return : int 
 */
static int led_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &newchrled; // set private data
    return 0;
}

/**
 * @brief  : read data from dev
 * @param  : filp 
 * @param  : buf 
 * @param  : cnt 
 * @param  : offt 
 * @return : ssize_t 
 */
static ssize_t led_read(struct file *filp, char __user *buf, size_t cnt, loff_t * offt)
{
    return 0;
}

/**
 * @brief  : write data to dev
 * @param  : filp 
 * @param  : buf 
 * @param  : cnt 
 * @param  : offt 
 * @return : ssize_t 
 */
static ssize_t led_write(struct file *filp, const char __user *buf, size_t cnt, loff_t * offt)
{
    int retVal;
    unsigned char databuf[1];
    unsigned char ledstat;

    retVal = copy_from_user(databuf, buf, cnt);

    if (retVal < 0)
    {
        printk("kernel write failed! \r\n");
        return -EFAULT;
    }

    ledstat = databuf[0];

    led_switch(ledstat);

    return 0;
}

/**
 * @brief  : 
 * @param  : inode 
 * @param  : filp 
 * @return : int 
 */
static int led_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/* dev operation function sturct */
static struct file_operations newchrled_fopts = {
    .owner   = THIS_MODULE,
    .open    = led_open,
    .read    = led_read,
    .write   = led_write,
    .release = led_release,
};

/**
 * @brief  : 
 * @return : int 
 */
static int __init led_init(void)
{
    u32 val    = 0;

    /* 1. register address mapping */
    IMX6U_CCM_CCGR1   = ioremap(CCM_CCGR1_BASE,         4);
    SW_MUX_GPIO1_IO03 = ioremap(SW_MUX_GPIO1_IO03_BASE, 4);
    SW_PAD_GPIO1_IO03 = ioremap(SW_PAD_GPIO1_IO03_BASE, 4);
    GPIO1_DR          = ioremap(GPIO1_DR_BASE,          4);
    GPIO1_GDIR        = ioremap(GPIO1_GDIR_BASE,        4);

    /* 2. enable GPIO1 clock */
    val = readl(IMX6U_CCM_CCGR1);
    val &= ~(3 << 26);
    val |=  (3 << 26);
    writel(val, IMX6U_CCM_CCGR1);

    /* 3. set GPIO1_IO3 mux , IO attribute*/
    writel(5, SW_MUX_GPIO1_IO03);
    writel(0x10B0, SW_PAD_GPIO1_IO03);

    /* 4. set GPIO1_IO3 output */
    val = readl(GPIO1_GDIR);
    val &= ~(1 << 3);
    val |=  (1 << 3);
    writel(val, GPIO1_GDIR);

    /* 5. default off led */
    led_switch(LED_OFF);

    /* register char dev driver */
    
    /* 1. create device number */
    if (newchrled.major)    /* define major */
    {
        newchrled.devid = MKDEV(newchrled.major, 0);
        register_chrdev_region(newchrled.devid, NEWCHRLED_CNT, NEWCHRLED_NAME);
    }
    else                    /* no define major */
    {
        alloc_chrdev_region(&newchrled.devid, 0, NEWCHRLED_CNT, NEWCHRLED_NAME);
        newchrled.major = MAJOR(newchrled.devid);
        newchrled.minor = MINOR(newchrled.devid);
    }

    printk("newchrled major = %d, minor = %d\r\n", newchrled.major, newchrled.minor);

    /* 2. init cdev */
    newchrled.cdev.owner = THIS_MODULE;
    cdev_init(&newchrled.cdev, &newchrled_fopts);

    /* 3. add cdev */
    cdev_add(&newchrled.cdev, newchrled.devid, NEWCHRLED_CNT);

    /* 4. create class */
    newchrled.class = class_create(THIS_MODULE, NEWCHRLED_NAME);

    if (IS_ERR(newchrled.class))
    {
        return PTR_ERR(newchrled.class);
    }

    /* 5. create device */
    newchrled.device = device_create(newchrled.class, NULL,
                                     newchrled.devid, NULL, NEWCHRLED_NAME);
    
    if (IS_ERR(newchrled.device))
    {
        return PTR_ERR(newchrled.device);
    }

    return 0;
}

/**
 * @brief  : 
 */
static void __exit led_exit(void)
{
    /* 1. unmap register address */
    iounmap(IMX6U_CCM_CCGR1);
    iounmap(SW_MUX_GPIO1_IO03);
    iounmap(SW_PAD_GPIO1_IO03);
    iounmap(GPIO1_DR);
    iounmap(GPIO1_GDIR);

    /* 2. unregister char dev driver */
    cdev_del(&newchrled.cdev);
    unregister_chrdev_region(newchrled.devid, NEWCHRLED_CNT);

    device_destroy(newchrled.class, newchrled.devid);
    class_destroy(newchrled.class);
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xiaoyu.lyu");