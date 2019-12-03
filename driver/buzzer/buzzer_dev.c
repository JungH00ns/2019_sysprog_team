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

#define BUZZER_MAJOR_NUMBER 505
#define BUZZER_DEV_NAME   "buzzer"
//gpio25
#define GPIO_BASE_ADDR 0x3F200000
#define GPFSEL2 0X08
#define GPSET0   0x1C
#define GPCLR0   0x28

static void __iomem *gpio_base;
volatile unsigned int *gpsel2;
volatile unsigned int * gpset0; 
volatile unsigned int * gpclr0;

#define IOCTL_MAGIC_NUMBER 'b'
#define IOCTL_CMD_SET_DIRECTION _IOWR(IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_CLEAR_DIRECTION _IOWR(IOCTL_MAGIC_NUMBER,1,int)

int buzzer_open(struct inode *inode, struct file *filp){
   printk(KERN_ALERT "buzzer driver open!!\n");
   
   gpio_base = ioremap(GPIO_BASE_ADDR, 0x60);
   gpsel2=(volatile unsigned int *)(gpio_base+GPFSEL2);
   gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
   gpclr0 = (volatile unsigned int *)(gpio_base + GPCLR0);
   *gpsel2&=(0<<17);
   *gpsel2&=(0<<16);
   *gpsel2|=(1<<15);
   return 0;
}

int buzzer_release(struct inode *inode, struct file *filp){
   printk(KERN_ALERT "buzzer driver close!!\n");
   iounmap((void*)gpio_base);
   return 0;
}

long buzzer_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
   int situation = 0;
   int a=0,i = 0;
   copy_from_user(&situation,(const void*)arg,4);
   switch(cmd) {
      
      case IOCTL_CMD_SET_DIRECTION:
         printk(KERN_ALERT "buzzer set direction out!!\n");
         if(situation==0){
            for(a=0;a<5;a++){
               for(i=0;i<1000;i++){
                  *gpset0 |= (0x01 << 25); 
                  udelay(100);
                  *gpclr0 |= (0x01 << 25); 
                  udelay(500);
               }
               mdelay(10);
            }
         }else if(situation==1){
            for(a=0;a<5;a++){
               for(i=0;i<200;i++){
                  *gpset0 |= (0x01 << 25); 
                  udelay(1000);
                  *gpclr0 |= (0x01 << 25); 
                  udelay(500);
               }
               mdelay(10);
            }
         }
         
         break;
   }

   return 0;
}

static struct file_operations buzzer_fops = {
   .owner = THIS_MODULE,
   .open = buzzer_open,
   .release = buzzer_release,
   .unlocked_ioctl = buzzer_ioctl,
};
   
int __init buzzer_init(void){
   if(register_chrdev(BUZZER_MAJOR_NUMBER, BUZZER_DEV_NAME, &buzzer_fops) < 0)
      printk(KERN_ALERT "buzzer driver initialization fail\n");
   else
      printk(KERN_ALERT "buzzer driver initialization success\n");
   
   return 0;
}

void __exit buzzer_exit(void){
   unregister_chrdev(BUZZER_MAJOR_NUMBER, BUZZER_DEV_NAME);
   printk(KERN_ALERT "buzzer driver exit done\n");
}

module_init(buzzer_init);
module_exit(buzzer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jmk");
MODULE_DESCRIPTION("des");

