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

#define VIB_MAJOR_NUMBER 503
#define VIB_DEV_NAME "vib_dev"

#define GPIO_BASE_ADDR 0x3F200000


#define GPFSEL2 0x08
#define GPLEV0 0x34

#define VIB_MAGIC_NUMBER  'v'

#define VIB_CMD_SEND     _IOW(VIB_MAGIC_NUMBER, 0,int)
#define VIB_CMD_RECV     _IOW(VIB_MAGIC_NUMBER, 1,int)

int dist=0;

static void __iomem *gpio_base;
volatile unsigned int *gplev0;
volatile unsigned int *gpfsel2;

int vib_open(struct inode *inode , struct file *flip)
{
   printk(KERN_ALERT "VIB driver open!!\n");
   
   gpio_base = ioremap(GPIO_BASE_ADDR , 0x60);
   
   gpfsel2 = (volatile unsigned int *)(gpio_base + GPFSEL2);
   gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);
   
   
   *gpfsel2 &= (0b000111111);

   return 0;
}

int vib_release(struct inode *inode,struct file *flip)
{
   printk(KERN_ALERT "vib driver closed!!\n");
   iounmap((void*)gpio_base);
   return 0;
}

long vib_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{
   int lev22;
   switch (cmd)
   {
      case VIB_CMD_RECV:
        
         break;
         
      case VIB_CMD_SEND:
         lev22 = ((*gplev0) & (0x01 << 22));
         copy_to_user(arg,&lev22,sizeof(int));
         break;
      default :
         printk(KERN_ALERT "ioctl : command error\n");
   }
   return 0;
}

static struct file_operations vib_fops={
   .owner = THIS_MODULE,
   .open = vib_open,
   .release = vib_release,
   .unlocked_ioctl = vib_ioctl
};

int __init vib_init(void)
{
   if(register_chrdev(VIB_MAJOR_NUMBER,VIB_DEV_NAME,&vib_fops)<0)
      printk(KERN_ALERT "VIBRATE driver init fail\n");
   else
      printk(KERN_ALERT "VIBRATE driver init success\n");
   return 0;
}

void __exit vib_exit(void){
   unregister_chrdev(VIB_MAJOR_NUMBER,VIB_DEV_NAME);
   printk(KERN_ALERT "VIB driver exit done\n");
}

module_init(vib_init);
module_exit(vib_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JH");
MODULE_DESCRIPTION("des");
