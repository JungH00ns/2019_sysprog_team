#include <linux/init.h> 
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/uaccess.h> 
#include <linux/slab.h> 

#include <asm/mach/map.h> 
#include <asm/uaccess.h> 

#define BUTTON_MAJOR_NUMBER	506
#define BUTTON_DEV_NAME		"button" 

#define GPIO_BASE_ADDRESS	0x3F200000

#define GPFSEL2             0x08
#define GPLEV0              0x34
#define BUTTON_MAGIC_NUMBER	'b'
#define IOCTL_CMD_SIMPLE     _IOW(BUTTON_MAGIC_NUMBER, 0,int)
#define IOCTL_CMD_ON_OFF     _IOW(BUTTON_MAGIC_NUMBER, 1,int)
static void __iomem * gpio_base;

volatile unsigned int * gpsel2; 
volatile unsigned int * gplev0; 


int button_open(struct inode * inode, struct file * filp){
    printk(KERN_ALERT "button driver open\n"); 

    gpio_base = ioremap(GPIO_BASE_ADDRESS, 0x60); 
    gpsel2 = (volatile unsigned int *)(gpio_base + GPFSEL2);
    gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);
    *gpsel2 &=(0<<14);
    return 0; 
}

int button_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "button driver close\n"); 
    iounmap((void *)gpio_base); 
    return 0; 
}

long button_ioctl(struct file * filp,  unsigned int cmd,unsigned long arg){
    int simple_value = 0;
	int on_off_value = 0;
	switch (cmd){
		case IOCTL_CMD_SIMPLE:
			simple_value = ((*gplev0) & (0x01 << 23))/8388608; //simple_button
			printk(KERN_ALERT "simple_value : %d\n",simple_value);
			copy_to_user(arg,&simple_value,sizeof(int));
			
			break;
		case IOCTL_CMD_ON_OFF:
			on_off_value = ((*gplev0) & (0x01 << 24))/16777216; //on_off
			printk(KERN_ALERT "on_off_value : %d\n",on_off_value);
			copy_to_user(arg,&on_off_value,sizeof(int));
			break;
		default : 
			printk(KERN_ALERT "ioctl : command error\n");
			break;			
	}
    return 0; 
}

static struct file_operations button_fops = { 
    .owner = THIS_MODULE, 
    .open = button_open, 
    .release = button_release, 
    .unlocked_ioctl = button_ioctl
}; 

int __init button_init (void) { 
    if(register_chrdev(BUTTON_MAJOR_NUMBER, BUTTON_DEV_NAME, &button_fops) < 0)
        printk(KERN_ALERT "button driver initalization faibutton\n"); 
    else 
        printk(KERN_ALERT "button driver initalization succeed\n");
    
    return 0; 
}

void __exit button_exit(void){ 
    unregister_chrdev(BUTTON_MAJOR_NUMBER, BUTTON_DEV_NAME); 
    printk(KERN_ALERT "button driver exit"); 
}

module_init(button_init); 
module_exit(button_exit);  

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Han"); 
MODULE_DESCRIPTION("foo"); 

