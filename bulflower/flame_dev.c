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

#define FLAME_MAJOR_NUMBER	502
#define FLAME_DEV_NAME		"/dev/flame_dev" 

#define FLAME_BASE_ADDRESS	0x3F200000

#define GPFSEL1 0x00
#define GPSET0	0x1C
#define GPCLR0	0x28
#define GPLEV0  0x34

#define FLAME_MAGIC_NUMBER  'j'
#define FLAME_CMD_SEND     _IOW(FLAME_MAGIC_NUMBER, 0,int)
#define FLAME_CMD_RECV    _IOR(FLAME_MAGIC_NUMBER, 1,int)

static void __iomem * gpio_base;
volatile unsigned int * gpsel0; 
volatile unsigned int * gpset0; 
volatile unsigned int * gpclr0;
volatile unsigned int * gplev0;

int flame_open(struct inode * inode, struct file * filp){
    printk(KERN_ALERT "flame driver open\n"); 

    gpio_base = ioremap(FLAME_BASE_ADDRESS, 0x60); 
    gpsel0 = (volatile unsigned int *)(gpio_base + GPFSEL1);
    gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
    gpclr0 = (volatile unsigned int *)(gpio_base + GPCLR0);
    gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);
<<<<<<< HEAD
    //printk(KERN_INFO "gpsel1 : %x \n",*gpsel1);

    *gpsel1 &= (0<<8);
    //*gpsel1 |= (1<<23);
    //printk(KERN_INFO "gpsel1 : %x \n",*gpsel1);
=======

    *gpsel0 &=(0b000111111111111);
>>>>>>> 44c0da42eb1997cad9ce67e3fbde38c57fe62d0b


    return 0; 
}

int flame_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "flame driver close\n"); 
    iounmap((void *)gpio_base); 
    return 0; 
}

long flame_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{ 
    int tmp=0,tmp2=0;
    int t1, t2=0;
    switch (cmd){
        case FLAME_CMD_SEND:
        copy_from_user(&tmp2,(const void*)arg,4);
		
        break;
        
        case FLAME_CMD_RECV:
        tmp = ((*gplev0) & (0x01 << 4));
        
	    //printk(KERN_INFO "signal : %d \n",tmp);
        //msleep(1000); //1sec
        copy_to_user(arg,&tmp,sizeof(int));
        break;

        default :
        printk(KERN_ALERT "ioctl : command error\n");
    }
    
    return 0;
}

static struct file_operations flame_fops = { 
    .owner = THIS_MODULE, 
    .open = flame_open, 
    .release = flame_release, 
    .unlocked_ioctl = flame_ioctl
}; 

int __init flame_init (void) { 
    if(register_chrdev(FLAME_MAJOR_NUMBER, FLAME_DEV_NAME, &flame_fops) < 0)
        printk(KERN_ALERT "flame driver initalization fail\n"); 
    else 
        printk(KERN_ALERT "flame driver initalization succeed\n");
    
    return 0; 
}

void __exit flame_exit(void){ 
    unregister_chrdev(FLAME_MAJOR_NUMBER, FLAME_DEV_NAME); 
    printk(KERN_ALERT "flame driver exit"); 
}

module_init(flame_init); 
module_exit(flame_exit);  

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEAM5"); 
MODULE_DESCRIPTION("TEAM_PROJECT"); 
