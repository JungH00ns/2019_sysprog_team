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

#define CLOCK_MAJOR_NUMBER	508
#define CLOCK_DEV_NAME		"/dev/clock_dev" 

#define CLOCK_BASE_ADDRESS	0x3F200000

#define GPFSEL0 0x00
#define GPFSEL1 0x04
#define GPSET0	0x1C
#define GPCLR0	0x28
#define GPLEV0  0x34

#define CLOCK_MAGIC_NUMBER  'c'
#define CLOCK_CMD_SEND     _IOWR(CLOCK_MAGIC_NUMBER, 0,int)
#define CLOCK_CMD_RECV    _IOWR(CLOCK_MAGIC_NUMBER, 1,int)
#define CLOCK_CMD_CHECK    _IOWR(CLOCK_MAGIC_NUMBER, 2,int)
#define CLOCK_CMD_CLEAR    _IOWR(CLOCK_MAGIC_NUMBER, 3,int)

static void __iomem * gpio_base;
volatile unsigned int * gpfsel0; 
volatile unsigned int * gpfsel1;
volatile unsigned int * gpset0; 
volatile unsigned int * gpclr0;
volatile unsigned int * gplev0;
// 8 ce0, 9 miso, 10 mosi, 11 sclk (all alt1)
int clock_open(struct inode * inode, struct file * filp){
    printk(KERN_ALERT "clock driver open\n"); 

    gpio_base = ioremap(CLOCK_BASE_ADDRESS, 0x60); 
    gpfsel0 = (volatile unsigned int *)(gpio_base + GPFSEL0);
    gpfsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);
    gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
    gpclr0 = (volatile unsigned int *)(gpio_base + GPCLR0);
    gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);
    
    *gpfsel0 &=(0b000111111111111);
    int sel0_29, sel0_28, sel0_27;
    int sel0_26, sel0_25, sel0_24;
    sel0_29= ((*gpfsel0) & (0x01 << 29));
    sel0_28= ((*gpfsel0) & (0x01 << 28));
    sel0_27= ((*gpfsel0) & (0x01 << 27));
    sel0_26= ((*gpfsel0) & (0x01 << 26));
    sel0_25= ((*gpfsel0) & (0x01 << 25));
    sel0_24= ((*gpfsel0) & (0x01 << 24));
   if(sel0_29==536870912)
      (*gpfsel0) ^= (0x01 << 29);
   if(sel0_28==268435456)
      (*gpfsel0) ^= (0x01 << 28);
   if(sel0_27==0)
      (*gpfsel1) |= (0x01 << 27);
   if(sel0_26==67108864)
      (*gpfsel0) ^= (0x01 << 26);
   if(sel0_25==33554432)
      (*gpfsel0) ^= (0x01 << 25);
   if(sel0_24==0)
      (*gpfsel0) |= (0x01 << 24);
    *gpfsel1 &=(0<<5);
    *gpfsel1 |=(9);
    printk(KERN_INFO "sel0 : %d sel1 ;%d \n", *gpfsel0, *gpfsel1);
    return 0; 
}

int clock_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "clock driver close\n"); 
    iounmap((void *)gpio_base); 
    return 0; 
}

long clock_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{ 
    int tmp=0,tmp2=0;
    int t1, t2=0;
    int a1, a2, a3, a4;
    a1=((*gplev0) & (0x01 << 8));
    a2=((*gplev0) & (0x01 << 9));
    a3=((*gplev0) & (0x01 << 10));
    a4=((*gplev0) & (0x01 << 11));
   // printk(KERN_INFO "%d\n",((*gplev0)&(1<<11)));

   // int cs_value=1;
   // int fre=1000000;
   // if(a1!=256)
    printk(KERN_INFO "cs0 :%d sclk :%d, mosi : %d miso :%d \n",a1/256, a2/512, a3/1024, a4/2048);
    switch (cmd){
        

        case CLOCK_CMD_SEND:
           

        break;
        
        
        case CLOCK_CMD_RECV:
        // set sclk 1 and 0
        copy_from_user(&tmp2,(const void*)arg,4);

            if(tmp2==0)
                *gpset0|=(1<<11);
            else
                *gpclr0|=(1<<11);
        break;
        
        case CLOCK_CMD_CHECK:
        // set ce 1
                    printk(KERN_ALERT "ioctl : command error!!!\n");

            *gpclr0|=(1<<8);
            tmp=1;
            copy_to_user(arg,&tmp,sizeof(int));

        break;
        // clear ce0
        case CLOCK_CMD_CLEAR:
            *gpset0|=(1<<8);

        break;
        default :
        printk(KERN_ALERT "ioctl : command error!!!\n");
    }
    return 0;
}

static struct file_operations clock_fops = { 
    .owner = THIS_MODULE, 
    .open = clock_open, 
    .release = clock_release, 
    .unlocked_ioctl = clock_ioctl
}; 

int __init clock_init (void) { 
    if(register_chrdev(CLOCK_MAJOR_NUMBER, CLOCK_DEV_NAME, &clock_fops) < 0)
        printk(KERN_ALERT "clock driver initalization fail\n"); 
    else 
        printk(KERN_ALERT "clock driver initalization succeed\n");
    
    return 0; 
}

void __exit clock_exit(void){ 
    unregister_chrdev(CLOCK_MAJOR_NUMBER, CLOCK_DEV_NAME); 
    printk(KERN_ALERT "clock driver exit"); 
}

module_init(clock_init); 
module_exit(clock_exit);  

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEAM5"); 
MODULE_DESCRIPTION("TEAM_PROJECT"); 
