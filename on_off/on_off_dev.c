#include <linux/init.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/uaccess.h> 
#include <linux/slab.h> 

#include <asm/mach/map.h> 
#include <asm/uaccess.h> 

#define ON_OFF_MAJOR_NUMBER	508
#define ON_OFF_DEV_NAME		"on_off" 

#define GPIO_BASE_ADDRESS	0x3F200000

#define GPFSEL2             0x08
#define GPLEV0              0x34

static void __iomem * gpio_base;

volatile unsigned int * gpsel2; 
volatile unsigned int * gplev0; 


int button_open(struct inode * inode, struct file * filp){
    printk(KERN_ALERT "button driver open\n"); 

    gpio_base = ioremap(GPIO_BASE_ADDRESS, 0x60); 
    gpsel2 = (volatile unsigned int *)(gpio_base + GPFSEL2);
    gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);
    *gpsel2 &=(0<<14);
    *gpsel2 &= (0<<13);
    *gpsel2 &= (0<<12); 
    return 0; 
}

int button_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "button driver close\n"); 
    iounmap((void *)gpio_base); 
    return 0; 
}

long button_ioctl(struct file * filp,  unsigned int cmd,unsigned long arg){
    int button_value = 0; 
     
    button_value = (*gplev0) & (0x01 << 24); 
    //printk(KERN_ALERT "gplev0 : %x \n", *gplev0); 
    printk(KERN_ALERT "%d\n",button_value);
	if(button_value){
		
	}
	
	//copy_to_user(arg,&button_value,sizeof(int));
    

    return button_value; 
}

static struct file_operations button_fops = { 
    .owner = THIS_MODULE, 
    .open = button_open, 
    .release = button_release, 
    .unlocked_ioctl = button_ioctl
}; 

int __init button_init (void) { 
    if(register_chrdev(ON_OFF_MAJOR_NUMBER, ON_OFF_DEV_NAME, &button_fops) < 0)
        printk(KERN_ALERT "button driver initalization faibutton\n"); 
    else 
        printk(KERN_ALERT "button driver initalization succeed\n");
    
    return 0; 
}

void __exit button_exit(void){ 
    unregister_chrdev(ON_OFF_MAJOR_NUMBER, ON_OFF_DEV_NAME); 
    printk(KERN_ALERT "button driver exit"); 
}

module_init(button_init); 
module_exit(button_exit);  

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Han"); 
MODULE_DESCRIPTION("foo"); 
