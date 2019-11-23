#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include <asm/mach/map.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define ULTRA_MAJOR_NUMBER 501
#define ULTRA_DEV_NAME "ultra_dev"

#define GPIO_BASE_ADDR 0x3F200000


#define GPFSEL1 0x04
#define GPCLR0 0x28
#define GPSET0 0x1c
#define GPLEV0 0x34


static void __iomem *gpio_base;
volatile unsigned int *gpset0;
volatile unsigned int *gplev0;
volatile unsigned int *gpfsel1;
volatile unsigned int *gpclr0;

int ultra_open(struct inode *inode , struct file *flip)
{
	printk(KERN_ALERT "Ultra driver open!!\n");
	
	gpio_base = ioremap(GPIO_BASE_ADDR , 0x60);
	
	gpfsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);
	gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);
	gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
	gpclr0 = (volatile unsigned int *)(gpio_base + GPCLR0);
	
	*gpfsel1 &= (000000000000000000000000000);
	*gpfsel1 ^= (1<<23);
	
	return 0;
}

int ultra_release(struct inode *inode,struct file *flip)
{
	printk(KERN_ALERT "Ultra driver closed!!\n");
	iounmap((void*)gpio_base);
	return 0;
}

int ultra_read(struct file *flip , char  *buf ,size_t len,loff_t *ti)
{
	float dist,s,e;
	printk(KERN_ALERT "ULTRA CALC!!\n");
	*gpset0 ^= (1<<17);
	printk(KERN_ALERT "%d\n",gplev0);
	while((*gplev0 & (1<<17))==0)
		printk(KERN_ALERT "S!\n");
		//s = micros();
	while((*gplev0 & (1<<17))==1)
		printk(KERN_ALERT "E!\n");
		//e = micros();
	//dist = (e-s) / 58;
	//printf("distance(cm) :%f\n",dist);
	printk(KERN_ALERT "DIST!\n");
    //ssleep(100);
	return 0;
}

static struct file_operations ultra_fops={
	.owner = THIS_MODULE,
	.open = ultra_open,
	.release = ultra_release,
	.read = ultra_read
};

int __init ultra_init(void)
{
	if(register_chrdev(ULTRA_MAJOR_NUMBER,ULTRA_DEV_NAME,&ultra_fops)<0)
		printk(KERN_ALERT "Ultra driver init fail\n");
	else
		printk(KERN_ALERT "Ultra driver init success\n");
	return 0;
}

void __exit ultra_exit(void){
	unregister_chrdev(ULTRA_MAJOR_NUMBER,ULTRA_DEV_NAME);
	printk(KERN_ALERT "ULTRA driver exit done\n");
}

module_init(ultra_init);
module_exit(ultra_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JH");
MODULE_DESCRIPTION("des");
