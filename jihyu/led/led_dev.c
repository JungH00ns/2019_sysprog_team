
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

#define LED_MAJOR_NUMBER   507
#define LED_DEV_NAME      "/dev/led_dev" 

#define LED_BASE_ADDRESS   0x3F200000
//gpio 16 - R
#define GPFSEL1 0x04
#define GPSET0   0x1C
#define GPCLR0   0x28
#define GPLEV0  0x34
//gpio 20 - G
#define GPFSEL2 0x08
//gpio 5 - B
#define GPFSEL0 0x00
#define BLACK 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define BG 4   //BLUE_GREEN

#define LED_MAGIC_NUMBER  'l'
#define LED_CMD_SEND     _IOW(LED_MAGIC_NUMBER, 0,int)

static void __iomem * gpio_base;
volatile unsigned int * gpsel0; 
volatile unsigned int * gpsel1; 
volatile unsigned int * gpsel2; 
volatile unsigned int * gpset0; 
volatile unsigned int * gpclr0;
volatile unsigned int * gplev0;

int led_open(struct inode * inode, struct file * filp){
    printk(KERN_ALERT "led driver open\n"); 

    gpio_base = ioremap(LED_BASE_ADDRESS, 0x60);
    
    gpsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);
    gpsel2 = (volatile unsigned int *)(gpio_base + GPFSEL2);
    gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
    gpclr0 = (volatile unsigned int *)(gpio_base + GPCLR0);
    gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);
    gpsel0 = (volatile unsigned int *)(gpio_base + GPFSEL0); 


    return 0; 
}

int led_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "led driver close\n"); 
    iounmap((void *)gpio_base); 
    return 0; 
}

long led_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{ 
    int color=0;      //red
    *gpsel1&=(0<<20);
    *gpsel1|=(1<<18);
    
    *gpsel0&=(0<<20);      //GREEN
    *gpsel0|=(1<<15);
    
    *gpsel2&=(0<<3);      //BLUE
    *gpsel2|=(1);
    
    switch (cmd){ 
   case LED_CMD_SEND: 
       copy_from_user(&color,(const void*)arg,4);
      switch(color){
      case RED: 
       
         *gpset0|=(1<<20);
         *gpset0|=(1<<16);
         *gpset0|=(1<<5);
         *gpsel0&=(0<<20);
         *gpsel2&=(0<<3);
         break;
      case GREEN:
         *gpset0|=(1<<20);
         *gpset0|=(1<<16);
         *gpset0|=(1<<5);
         *gpsel0&=(0<<20);
         *gpsel1&=(0<<20);
         break;
      case BLUE:
         *gpset0|=(1<<20);
         *gpset0|=(1<<16);
         *gpset0|=(1<<5);
         *gpsel1&=(0<<20);
         *gpsel2&=(0<<3);
         break;
      case BG:
      /*
         *gpset0|=(1<<20);
         *gpset0|=(1<<16);
         *gpset0|=(1<<5);
         *gpsel0&=(0<<20);
         */
      *gpset0|=(1<<20);
      *gpset0|=(1<<16);
      *gpset0|=(1<<5);
      *gpsel1&=(0<<20);
         break;   
      case BLACK:
         *gpsel0&=(0<<20);
         *gpsel1&=(0<<20);
         *gpsel2&=(0<<3);
         break;
         
      /*
       else if(color == 5 ){ //red blue
      *gpset0|=(1<<20);
      *gpset0|=(1<<16);
      *gpset0|=(1<<5);
      *gpsel2&=(0<<3);
       }
       else if(color == 6 ){ //green blue
      *gpset0|=(1<<20);
      *gpset0|=(1<<16);
      *gpset0|=(1<<5);
      *gpsel1&=(0<<20);
       }
       else if(color == 7 ){ //red green blue
      *gpset0|=(1<<20);
      *gpset0|=(1<<16);
      *gpset0|=(1<<5);
       }
      */ 
       
      default : 
         printk(KERN_ALERT "ioctl : command error\n");
         break;
      }
    }
    
    return 0; 
}

static struct file_operations led_fops = { 
    .owner = THIS_MODULE, 
    .open = led_open, 
    .release = led_release, 
    .unlocked_ioctl = led_ioctl
}; 

int __init led_init (void) { 
    if(register_chrdev(LED_MAJOR_NUMBER, LED_DEV_NAME, &led_fops) < 0)
        printk(KERN_ALERT "led driver initalization fail\n"); 
    else 
        printk(KERN_ALERT "led driver initalization succeed\n");
    
    return 0; 
}

void __exit led_exit(void){ 
    unregister_chrdev(LED_MAJOR_NUMBER, LED_DEV_NAME); 
    printk(KERN_ALERT "led driver exit"); 
}

module_init(led_init); 
module_exit(led_exit);  

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEAM5"); 
MODULE_DESCRIPTION("TEAM_PROJECT"); 
