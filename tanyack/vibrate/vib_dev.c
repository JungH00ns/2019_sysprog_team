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

#define VIB_MAJOR_NUMBER 503
#define VIB_DEV_NAME "vib_dev"

#define GPIO_BASE_ADDR 0x3F200000
#define SPI_BASE_ADDRESS 0x3F204000

#define SPI_CS 0x00
#define SPI_FIFO 0x04
#define SPI_CLOCK 0x08
#define SPI_DLEN 0x0c
#define SPI_LTOH 0x10
#define SPI_DC 0x14

#define GPFSEL0 0x00
#define GPFSEL1 0x04
#define GPLEV0 0x34

#define VIB_MAGIC_NUMBER  'v'

#define VIB_CMD_SEND     _IOW(VIB_MAGIC_NUMBER, 0,int)
#define VIB_CMD_RECV     _IOW(VIB_MAGIC_NUMBER, 1,int)


static void __iomem *gpio_base;
volatile unsigned int *gpfsel1;
volatile unsigned int *gpfsel0;


static void __iomem * spi_base;
volatile unsigned int * spi_cs;
volatile unsigned int * spi_fifo;
volatile unsigned int * spi_clock;
volatile unsigned int * spi_dlen;
volatile unsigned int * spi_ltoh;
volatile unsigned int * spi_dc;

uint8_t mosi[2][3]={{0x01, 0x80, 0x00},{0x01,0x90,0x00}};
uint8_t miso[3]={0};
uint32_t txcnt=0,rxcnt=0;
int i=0;

int vib_open(struct inode *inode , struct file *flip)
{
   spi_base = ioremap(SPI_BASE_ADDRESS, 0x60); 
   spi_cs = (volatile unsigned int *)(spi_base + SPI_CS);
   spi_fifo = (volatile unsigned int *)(spi_base + SPI_FIFO);
   spi_clock = (volatile unsigned int *)(spi_base + SPI_CLOCK);
   spi_dlen = (volatile unsigned int *)(spi_base + SPI_DLEN);
   spi_ltoh = (volatile unsigned int *)(spi_base + SPI_LTOH);
   spi_dc = (volatile unsigned int *)(spi_base + SPI_DC);

   gpio_base = ioremap(GPIO_BASE_ADDR , 0x60);
   gpfsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);
   gpfsel0 = (volatile unsigned int *)(gpio_base + GPFSEL0);

   (*gpfsel0) |= (0x1 << 29);
   (*gpfsel0) |= (0x1 << 26);

   (*gpfsel1) |= (0x1 << 5);
   (*gpfsel1) |= (0x1 << 2);

   //*spi_cs|=(1<<21);
   *spi_cs=0x00b0;
   *spi_clock&=(0<<31);
   *spi_clock|=(1<<7);

   return 0;
}

int vib_release(struct inode *inode,struct file *flip)
{
   printk(KERN_ALERT "vib driver closed!!\n");
   iounmap((void *)spi_base); 
   iounmap((void*)gpio_base);
   return 0;
}

long vib_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{
   switch (cmd)
   {
      case VIB_CMD_SEND:
         *spi_cs|=(1<<7);
         txcnt=rxcnt=0;
         while(txcnt<3 || rxcnt<3)
         {
            while(((*spi_cs) & (1<<18))&&(txcnt<3))
            {
               (*spi_fifo)|=mosi[0][txcnt++];
               *spi_fifo=0;
               break;
               if(txcnt==3)
               break;
            }
            while(1)
            {
               while(1)
               {              
                  if(((*spi_cs)& (1<<16))!=0)
                  break;
               } 
               if(((*spi_cs) & (1<<17))==0){
                  rxcnt++;
                  break;
               }
               else if(((*spi_cs) & (1<<17))!=0)
               {
                  if(rxcnt==0)
                  {                 
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
         printk(KERN_INFO "done %d %d %d\n", miso[0],miso[1],miso[2]);
         int result=miso[0];
         copy_to_user(arg,&result,sizeof(int));
         break;
      default :
         printk(KERN_ALERT "ioctl : command error\n");
   }
   return 0;
}

static struct file_operations vib_fops={
   .owner = THIS_MODULE,
   .open = vib_open,
   .release = vib_release,
   .unlocked_ioctl = vib_ioctl
};

int __init vib_init(void)
{
   if(register_chrdev(VIB_MAJOR_NUMBER,VIB_DEV_NAME,&vib_fops)<0)
      printk(KERN_ALERT "VIBRATE driver init fail\n");
   else
      printk(KERN_ALERT "VIBRATE driver init success\n");
   return 0;
}

void __exit vib_exit(void){
   unregister_chrdev(VIB_MAJOR_NUMBER,VIB_DEV_NAME);
   printk(KERN_ALERT "VIB driver exit done\n");
}

module_init(vib_init);
module_exit(vib_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JH");
MODULE_DESCRIPTION("des");
