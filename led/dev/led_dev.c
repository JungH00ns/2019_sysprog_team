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

#define LED_MAJOR_NUMBER	507
#define LED_DEV_NAME		"/dev/led_dev" 

#define LED_BASE_ADDRESS	0x3F200000
//gpio 16 - R
#define GPFSEL1 0x04
#define GPSET0	0x1C
#define GPCLR0	0x28
#define GPLEV0  0x34
//gpio 20 - G
#define GPFSEL2 0x08
//gpio 5 - B
#define GPFSEL0 0x00


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
    *gpsel1|=(1<<18);   //R
    *gpsel2&=(0<<2);
    *gpsel2|=(1<<0);       //G
    *gpsel0|=(1<<15);    //B
//    printk(KERN_INFO "gpsel0 : %d\n", *gpsel0);
    return 0; 
}

int led_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "led driver close\n"); 
    iounmap((void *)gpio_base); 
    return 0; 
}

long led_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{ 
    int color=0;
    switch (cmd){ 
	case LED_CMD_SEND: 
	    copy_from_user(&color,(const void*)arg,4);
	    printk(KERN_INFO "%d\n",color);
    
    	    printk(KERN_INFO "%d: %d %d init\n", *gpset0, *gpclr0 ,*gplev0);

	    *gpclr0|=(1<<5);
	    printk(KERN_INFO "%d: %d %d init\n", *gpset0, *gpclr0 ,*gplev0);

	    *gpclr0|=(1<<20);
	    printk(KERN_INFO "%d: %d %d init\n", *gpset0, *gpclr0 ,*gplev0);

	    *gpclr0|=(1<<16);
	    printk(KERN_INFO "%d: %d %d init\n", *gpset0, *gpclr0 ,*gplev0);

	    if(color==1){	//red
		printk(KERN_INFO "%d: %d %d %d red\n",color, *gpset0, *gpclr0 ,*gplev0);
		
		*gpset0|=(1<<16);			
		printk(KERN_INFO "%d: %d %d %d red\n",color, *gpset0, *gpclr0,*gplev0);
		
		*gpclr0|=(1<<20);
		printk(KERN_INFO "%d: %d %d %d red\n",color, *gpset0, *gpclr0,*gplev0);
		
		*gpclr0|=(1<<5);
		printk(KERN_INFO "%d: %d %d %d red\n",color, *gpset0, *gpclr0,*gplev0);
	    
	    }
	    else if(color==2){	//green
		
		
		
		printk(KERN_INFO "%d: %d %d %d green\n",color, *gpset0, *gpclr0,*gplev0);
		*gpset0|=1048576;
		printk(KERN_INFO "%d: %d %d %d green\n",color, *gpset0, *gpclr0,*gplev0);
		*gpclr0|=65536;
		printk(KERN_INFO "%d: %d %d %d green\n",color, *gpset0, *gpclr0,*gplev0);
		*gpclr0|=32;
		printk(KERN_INFO "%d: %d %d %d green\n",color, *gpset0, *gpclr0,*gplev0);
		
	    }
	    else if(color==4){		//blue
		printk(KERN_INFO "%d: %d %d %d blue\n",color, *gpset0, *gpclr0,*gplev0);
		
		*gpset0|=32;
				
		printk(KERN_INFO "%d: %d %d %d blue\n",color, *gpset0, *gpclr0,*gplev0);
		
		*gpclr0|=1048576;			
		printk(KERN_INFO "%d: %d %d %d blue\n",color, *gpset0, *gpclr0,*gplev0);
		*gpclr0|=65536;	
		printk(KERN_INFO "%d: %d %d %d blue\n",color, *gpset0, *gpclr0,*gplev0);
	
	
	    }
	    msleep(2000); //2sec
	    if(color==1){	//red
	    //	*gpclr0|=(1<<16);
	    
	    }
	    if(color==2){	//green
	    //	*gpclr0|=(1<<20);
	    
	    }
	    if(color==4){		//blue
	    //*gpclr0|=(1<<5);
	    }
	
	    break; 
	    
	    
	default : 
	    printk(KERN_ALERT "ioctl : command error\n");
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
