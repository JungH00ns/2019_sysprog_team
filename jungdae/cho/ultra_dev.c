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
#define GPFSEL2 0x08

#define GPCLR0 0x28
#define GPSET0 0x1c
#define GPLEV0 0x34

#define ULTRA_MAGIC_NUMBER  'u'

#define ULTRA_CMD_SEND     _IOW(ULTRA_MAGIC_NUMBER, 0,int)
#define ULTRA_CMD_RECV     _IOW(ULTRA_MAGIC_NUMBER, 1,int)

int dist=0;
static void __iomem *gpio_base;

volatile unsigned int *gpfsel1;
volatile unsigned int *gpfsel2;

volatile unsigned int *gpset0;
volatile unsigned int *gplev0;
volatile unsigned int *gpclr0;

int ultra_open(struct inode *inode , struct file *flip)
{
   printk(KERN_ALERT "Ultra driver open!!\n");
   
   gpio_base = ioremap(GPIO_BASE_ADDR , 0x60);
   
   gpfsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);
   gpfsel2 = (volatile unsigned int *)(gpio_base + GPFSEL2);
   gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);
   gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
   gpclr0 = (volatile unsigned int *)(gpio_base + GPCLR0);
   
   int sel1_23, sel1_22, sel1_21;
   int sel2_23, sel2_22, sel2_21;
   sel1_23= ((*gpfsel1) & (0x01 << 23));
   sel1_22= ((*gpfsel1) & (0x01 << 22));
   sel1_21= ((*gpfsel1) & (0x01 << 21));
   sel2_23= ((*gpfsel2) & (0x01 << 23));
   sel2_22= ((*gpfsel2) & (0x01 << 22));
   sel2_21= ((*gpfsel2) & (0x01 << 21));
   
   if(sel1_23==8388608)
      (*gpfsel1) ^= (0x01 << 23);
   if(sel1_22==4194304)
      (*gpfsel1) ^= (0x01 << 22);
   if(sel1_21==0)
      (*gpfsel1) |= (0x01 << 21);
   if(sel2_23==8388608)
      (*gpfsel2) ^= (0x01 << 23);
   if(sel2_22==4194304)
      (*gpfsel2) ^= (0x01 << 22);
   if(sel2_21==2097152)
      (*gpfsel2) ^= (0x01 << 21);
   //printk(KERN_INFO "%d %d\n", *gpfsel1, *gpfsel2);
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
   int recv_from_user;
   int lev_17,lev_27;
   //*gpset0 |= (1<<17);
   *gpset0 |= (1<<27);
   //printk(KERN_ALERT "ULTRA CALC!!\n");
   
   switch (cmd)
   {
	case ULTRA_CMD_SEND:
        copy_from_user(&recv_from_user,(const void*)arg,4);
        
        if(recv_from_user == 1)
      {
         //printk(KERN_INFO "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
         *gpclr0 |= (1<<27);
         *gpset0 |= (1<<17);
      }
      else if(recv_from_user == 0)
      {
         //printk(KERN_INFO "+++++++++++++++++++++++++++");
         *gpclr0 |= (1<<17);
      }
        break;
        
        case ULTRA_CMD_RECV:
        //*gpclr0 |= (1<<17);
        lev_17 = ((*gplev0) & (0x01 << 17));
        lev_27 = ((*gplev0) & (0x01 << 27));
        lev_27=lev_27/134217728;

       // printk(KERN_INFO "lev 17 : %d lev 27 : %d\n",lev_17,lev_27);
        //printk(KERN_INFO "%d %d/",ttmp2,dist);
        if(lev_27==1){
           dist++;
        }
        else if(lev_27 == 0 && dist >0)
        {
           copy_to_user(arg,&dist,sizeof(int));
           dist = 0;
        }
        else
        {
           copy_to_user(arg,&lev_27,sizeof(int));
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
