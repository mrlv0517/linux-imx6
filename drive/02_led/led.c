/**
 * @File Name  : led.c
 * @brief      : imx6ull led driver 
 * @Author     : xiaoyu (mrlv1992@163.com)
 * @Version    : V1.0
 * @Creat Date : 2022-04-24
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

#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define LED_MAJOR 200   ///< main dev number
#define LED_NAME  "led" ///< dev name

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
static struct file_operations led_fopts = {
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
    int retVal = 0;
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

    /* 6. register char dev driver */
    retVal = register_chrdev(LED_MAJOR, LED_NAME, &led_fopts);
    if (retVal < 0)
    {
        printk("register chrdev failed! \r\n");
        return -EIO;
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
    unregister_chrdev(LED_MAJOR, LED_NAME);
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xiaoyu.lyu");