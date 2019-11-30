#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>
//#include <time.h>
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
#define GPEDS0 0x40
#define GPREN0 0x4c
#define GPFEN0 0x58

#define ULTRA_MAGIC_NUMBER  'u'

#define ULTRA_CMD_SEND     _IOW(ULTRA_MAGIC_NUMBER, 0,int)
#define ULTRA_CMD_RECV     _IOW(ULTRA_MAGIC_NUMBER, 1,int)
int dist=0;
static void __iomem *gpio_base;
volatile unsigned int *gpset0;
volatile unsigned int *gplev0;
volatile unsigned int *gpfsel1;
volatile unsigned int *gpclr0;
volatile unsigned int *gpeds0;
volatile unsigned int *gpren0;
volatile unsigned int *gpfen0;

int ultra_open(struct inode *inode , struct file *flip)
{
   printk(KERN_ALERT "Ultra driver open!!\n");
   
   gpio_base = ioremap(GPIO_BASE_ADDR , 0x60);
   
   gpfsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);
   gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);
   gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
   gpclr0 = (volatile unsigned int *)(gpio_base + GPCLR0);
   gpeds0 = (volatile unsigned int *)(gpio_base + GPEDS0);
   gpren0 = (volatile unsigned int *)(gpio_base + GPREN0);
   gpfen0 = (volatile unsigned int *)(gpio_base + GPFEN0);
   
   *gpfsel1 &= (0<<30);
   //*gpfsel1 |= (1<<24);
   *gpfsel1 |= (1<<21);
   return 0;
}

int ultra_release(struct inode *inode,struct file *flip)
{
   printk(KERN_ALERT "Ultra driver closed!!\n");
   iounmap((void*)gpio_base);
   return 0;
}

long ultra_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{

   int tmp,tmp2,recv;
   int ttmp,ttmp2,ttmp4,ttmp6,ttmp7;
   //*gpset0 |= (1<<17);
   *gpset0 |= (1<<18);
   //printk(KERN_ALERT "ULTRA CALC!!\n");
   
   switch (cmd)
   {
        case ULTRA_CMD_SEND:
        copy_from_user(&recv,(const void*)arg,4);
        
        if(recv == 1)
      {
         //printk(KERN_INFO "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
         *gpclr0 |= (1<<18);
         *gpset0 |= (1<<17);
      }
      else if(recv == 0)
      {
         //printk(KERN_INFO "+++++++++++++++++++++++++++");
         *gpclr0 |= (1<<17);
      }
        break;
        
        case ULTRA_CMD_RECV:
        //*gpclr0 |= (1<<17);
        ttmp = ((*gplev0) & (0x01 << 17));
        ttmp2 = ((*gplev0) & (0x01 << 18));
        //printk(KERN_INFO "lev 17 : %d lev 18 : %d , eds 18 : %d , ren 18 : %d , fen 18 : %d\n",ttmp,ttmp2,ttmp4,ttmp6,ttmp7);
        //printk(KERN_INFO "%d %d/",ttmp2,dist);
        ttmp2=ttmp2/262144;
        if(ttmp2==1){dist++;}
        else if(ttmp2 == 0 && dist >0)
        {
         copy_to_user(arg,&dist,sizeof(int));
         dist = 0;
        }
        else
        {
           copy_to_user(arg,&ttmp2,sizeof(int));
        }
        break;

        default :
        printk(KERN_ALERT "ioctl : command error\n");
    }
	return 0;
}

static struct file_operations ultra_fops={
   .owner = THIS_MODULE,
   .open = ultra_open,
   .release = ultra_release,
   .unlocked_ioctl = ultra_ioctl
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
