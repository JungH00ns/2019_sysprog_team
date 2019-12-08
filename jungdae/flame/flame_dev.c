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

#define GPFSEL0 0x00
#define GPFSEL1 0x04
#define GPSET0	0x1C
#define GPCLR0	0x28
#define GPLEV0  0x34

#define SPI_BASE_ADDRESS 0x3F204000

#define SPI_CS 0x00
#define SPI_FIFO 0x04
#define SPI_CLOCK 0x08
#define SPI_DLEN 0x0c
#define SPI_LTOH 0x10
#define SPI_DC 0x14

#define FLAME_MAGIC_NUMBER  'f'
#define FLAME_CMD_SEND     _IOWR(FLAME_MAGIC_NUMBER, 0,int)

static void __iomem * gpio_base;
volatile unsigned int * gpfsel0; 
volatile unsigned int * gpfsel1;
volatile unsigned int * gpset0; 
volatile unsigned int * gpclr0;
volatile unsigned int * gplev0;
volatile unsigned int * glren0;
volatile unsigned int * glfen0;

static void __iomem * spi_base;
volatile unsigned int * spi_cs;
volatile unsigned int * spi_fifo;
volatile unsigned int * spi_clock;
volatile unsigned int * spi_dlen;
volatile unsigned int * spi_ltoh;
volatile unsigned int * spi_dc;

uint8_t mosi[3]={0x01, 0x80, 0x00};
uint8_t miso[3]={0};
uint32_t txcnt=0,rxcnt=0;

int flame_open(struct inode * inode, struct file * filp){
    printk(KERN_ALERT "flame driver open\n"); 

    gpio_base = ioremap(FLAME_BASE_ADDRESS, 0x60); 
    gpfsel0 = (volatile unsigned int *)(gpio_base + GPFSEL0);
    gpfsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);

    gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
    gpclr0 = (volatile unsigned int *)(gpio_base + GPCLR0);
    gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);

    spi_base = ioremap(SPI_BASE_ADDRESS, 0x60); 
    spi_cs = (volatile unsigned int *)(spi_base + SPI_CS);
    spi_fifo = (volatile unsigned int *)(spi_base + SPI_FIFO);
    spi_clock = (volatile unsigned int *)(spi_base + SPI_CLOCK);
    spi_dlen = (volatile unsigned int *)(spi_base + SPI_DLEN);
    spi_ltoh = (volatile unsigned int *)(spi_base + SPI_LTOH);
    spi_dc = (volatile unsigned int *)(spi_base + SPI_DC);
    (*gpfsel0) &= (0x0 << 29);
    (*gpfsel0) |= (0x1 << 29);
    (*gpfsel0) |= (0x1 << 26);
    
    (*gpfsel1) |= (0x1 << 5);
    (*gpfsel1) |= (0x1 << 2);
    *spi_cs=0x00b0;
    *spi_clock&=(0<<31);
    *spi_clock|=(1<<7);
    
    return 0; 
}

int flame_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "flame driver close\n"); 
    iounmap((void *)gpio_base); 
    iounmap((void *)spi_base); 
    return 0; 
}

long flame_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{ 
    int kbuf=0;
    //printk(KERN_INFO "0: %d sel1 ;%d %d %d %d %d \n", *spi_cs,*spi_fifo,  *spi_clock,*spi_dlen, *spi_ltoh, *spi_dc);
    switch (cmd){
        case FLAME_CMD_SEND:
         *spi_cs|=(1<<7);
         
           txcnt=rxcnt=0;
            while(txcnt<3 || rxcnt<3){
                while(((*spi_cs) & (1<<18))&&(txcnt<3)){
                    (*spi_fifo)|=mosi[txcnt++];
                    *spi_fifo=0;
                    break;
                    if(txcnt==3)
                        break;
                }
                while(1){
                    while(1){
                        if(((*spi_cs)& (1<<16))!=0)
                            break;
                    } 
                    if(((*spi_cs) & (1<<17))==0){
                        rxcnt++;
                        break;
                    }
                    else if(((*spi_cs) & (1<<17))!=0){
                        if(rxcnt==0){
                            miso[rxcnt]=*spi_fifo;
                        }else if(rxcnt==1){
                            miso[rxcnt]=*spi_fifo;
                        }else if(rxcnt==2){
                            miso[rxcnt]=*spi_fifo;
                        }else{
                            break;
                        }
                    }
                }
            }
            *spi_cs&=(0<<7);
            
            kbuf+=miso[0]<<8;
            kbuf+=miso[1];
            printk(KERN_INFO "done %d\n",kbuf);

            copy_to_user(arg,&kbuf,sizeof(int));
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
