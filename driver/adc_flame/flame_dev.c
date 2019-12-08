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
#define GPREN0  0x4C
#define GPFEN0  0x58

#define SPI_BASE_ADDRESS 0x3F204000

#define SPI_CS 0x00
#define SPI_FIFO 0x04
#define SPI_CLOCK 0x08
#define SPI_DLEN 0x0c
#define SPI_LTOH 0x10
#define SPI_DC 0x14

#define FLAME_MAGIC_NUMBER  'f'
#define FLAME_CMD_SEND     _IOWR(FLAME_MAGIC_NUMBER, 0,int)
#define FLAME_CMD_RECV    _IOWR(FLAME_MAGIC_NUMBER, 1,int)
#define FLAME_CMD_CHECK    _IOWR(FLAME_MAGIC_NUMBER, 2,int)

#define BIT_1 0x01
#define BIT_2 0x01
#define BIT_3 0x00


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




// 8 ce0, 9 miso, 10 mosi, 11 sclk (all alt1)
int flame_open(struct inode * inode, struct file * filp){
    printk(KERN_ALERT "flame driver open\n"); 
  
    spi_base = ioremap(SPI_BASE_ADDRESS, 0x60); 
    spi_cs = (volatile unsigned int *)(spi_base + SPI_CS);
    spi_fifo = (volatile unsigned int *)(spi_base + SPI_FIFO);
    spi_clock = (volatile unsigned int *)(spi_base + SPI_CLOCK);
    spi_dlen = (volatile unsigned int *)(spi_base + SPI_DLEN);
    spi_ltoh = (volatile unsigned int *)(spi_base + SPI_LTOH);
    spi_dc = (volatile unsigned int *)(spi_base + SPI_DC);
    
    //*spi_cs|=(1<<21);
    *spi_cs=0x00b0;
    *spi_clock&=(0<<31);
    *spi_clock|=(1<<7);
    
   //s printk(KERN_INFO "sel0 :%d %d sel1 ;%d %d %d %d %d \n", spi_cs,*spi_cs,*spi_fifo,  *spi_clock,*spi_dlen, *spi_ltoh, *spi_dc);
    

    return 0; 
}

int flame_release(struct inode * inode, struct file * filp) { 
    printk(KERN_ALERT "flame driver close\n"); 
    //.//iounmap((void *)gpio_base); 
    iounmap((void *)spi_base); 
    return 0; 
}
   uint8_t mosi[2][3]={{0x01, 0x80, 0x00},{0x01,0x90,0x00}};
   uint8_t miso[3]={0};
   uint32_t txcnt=0,rxcnt=0;
   int i=0;

long flame_ioctl(struct file * filp, unsigned int cmd, unsigned long arg)
{   
    int kbuf=0;
    //printk(KERN_INFO "0: %d sel1 ;%d %d %d %d %d \n", *spi_cs,*spi_fifo,  *spi_clock,*spi_dlen, *spi_ltoh, *spi_dc);
    switch (cmd){
        case FLAME_CMD_SEND:
         *spi_cs|=(1<<7);
         
           txcnt=rxcnt=0;
           // peri_set_bits(addr_cs,SPI0_CS_CLEAR,SPI0_CS_CLEAR);
            //peri_set_bits(addr_cs,SPI0_CS_TA,SPI0_CS_TA);
            while(txcnt<3 || rxcnt<3){
                while(((*spi_cs) & (1<<18))&&(txcnt<3)){
                    (*spi_fifo)|=mosi[0][txcnt++];
                    printk(KERN_INFO "send done %d %d\n",txcnt, *spi_cs);
                    *spi_fifo=0;
                    break;
                    if(txcnt==3)
                        break;
                }
                
                while(1){
                    printk(KERN_INFO "recv done %d %d\n",rxcnt,*spi_cs);
                    while(1){
                    printk(KERN_INFO "while%d %d\n",rxcnt,*spi_cs);
                        
                        if(((*spi_cs)& (1<<16))!=0)
                            break;
                    } 
                    if(((*spi_cs) & (1<<17))==0){
                    printk(KERN_INFO "recv-1 done %d %d\n",rxcnt,*spi_cs);

                        rxcnt++;
                        break;
                    }
                    else if(((*spi_cs) & (1<<17))!=0){
                        printk(KERN_INFO "recv-2 done %d %d\n",rxcnt,*spi_cs);
                        if(rxcnt==0){
                            
                            miso[rxcnt]=*spi_fifo;
                            printk(KERN_INFO "recv-2-0 done %d %d\n",rxcnt,*spi_cs);

                            
                        }else if(rxcnt==1){
                            miso[rxcnt]=*spi_fifo;
                            printk(KERN_INFO "recv-2-1 done %d %d\n",rxcnt,*spi_cs);

                           // break;
                        }else if(rxcnt==2){
                            miso[rxcnt]=*spi_fifo;
                            printk(KERN_INFO "recv-2-2 done %d %d\n",rxcnt,*spi_cs);

                            //break;
                        }else{
                            break;
                        }
                    }
                }
            }
            *spi_cs&=(0<<7);
             printk(KERN_INFO "done %d %d %d\n", miso[0],miso[1],miso[2]);
            
            break;
            
        case FLAME_CMD_RECV:
            copy_from_user(&kbuf, (const void*)arg, 4);
            int tx_check, rx_check, done_check;
            printk(KERN_INFO "%d\n", *spi_cs);

            //printk(KERN_INFO "\n%d %d %d %d\n", *spi_cs,*spi_fifo,  rx_check, done_check);
            rx_check=(*spi_cs) & (1<<17);
            done_check=(*spi_cs) & (1<<16);
          //  printk(KERN_INFO "%d %d %d %d\n", *spi_cs,*spi_fifo,  rx_check, done_check);
 
            if(kbuf==0){
                *spi_fifo|=mosi[0][0];
                kbuf+=2;
                copy_to_user(arg,&kbuf,sizeof(int));
                printk(KERN_INFO "1 done %d \n",*spi_cs);
            }else if(kbuf==1){
                if(done_check!=0){
                    if(rx_check!=0)
                    {
                        copy_to_user(arg,&kbuf,sizeof(int));
                        printk(KERN_INFO "2-1 done %d\n", *spi_cs);

                        int *temp;
                        temp=spi_fifo;
                        printk(KERN_INFO "2-1-1 done %d %d\n", *spi_cs,*temp);
                        kbuf+=1;

                        
                    }else{
                        kbuf+=1;
                        copy_to_user(arg,&kbuf,sizeof(int));
                        int temp=*spi_fifo;

                        printk(KERN_INFO "2-1-2 done %d\n", *spi_cs);
                    }  
                }else{
                    if(rx_check!=0)
                    {   
                        copy_to_user(arg,&kbuf,sizeof(int));
                        int temp=*spi_fifo;
                        printk(KERN_INFO "2-2-1 done %d %c\n", *spi_cs,temp);
                    }else{
                        
                        copy_to_user(arg,&kbuf,sizeof(int));
                        int temp=*spi_fifo;

                        printk(KERN_INFO "2-2-2 done %d %d\n", *spi_cs,temp);
                    }  
                }
                  
            }else if(kbuf==2){
                *spi_fifo|=mosi[0][1];
                kbuf+=2;
                copy_to_user(arg,&kbuf,sizeof(int));
                printk(KERN_INFO "3 done %d \n",*spi_cs);
            }else if(kbuf==3){
                if(done_check!=0){
                    if(rx_check!=0)
                    {   
                        copy_to_user(arg,&kbuf,sizeof(int));
                        printk(KERN_INFO "4-1 done %d\n", *spi_cs);

                        char temp=*spi_fifo;
                        printk(KERN_INFO "4-1-1 done %d %d\n", *spi_cs,temp);
                        kbuf+=1;

                    }else{
                        kbuf+=1;
                        copy_to_user(arg,&kbuf,sizeof(int));
                        printk(KERN_INFO "4-1-2 done \n");
                    }  
                }else{
                    if(rx_check!=0)
                    {   
                        copy_to_user(arg,&kbuf,sizeof(int));
                        printk(KERN_INFO "4-2-1 done \n");
                    }else{
                        
                        copy_to_user(arg,&kbuf,sizeof(int));
                        printk(KERN_INFO "4-2-2 done \n");
                    }  
                }
            }else if(kbuf==4){
                *spi_fifo|=mosi[0][2];
                kbuf+=1;
                copy_to_user(arg,&kbuf,sizeof(int));
                printk(KERN_INFO "5 done %d \n",*spi_cs);
            }else if(kbuf==5){
                if(done_check!=0){
                    if(rx_check!=0)
                    {   
                        copy_to_user(arg,&kbuf,sizeof(int));
                        printk(KERN_INFO "6-1 done %d\n", *spi_cs);

                       int temp;
                        temp=*spi_fifo;
                        printk(KERN_INFO "6-1-1 done %d %c\n", *spi_cs,temp);
                        kbuf+=1;

                    }else{
                        kbuf+=1;
                        copy_to_user(arg,&kbuf,sizeof(int));
                        printk(KERN_INFO "6-1-2 done \n");
                    }  
                }else{
                    if(rx_check!=0)
                    {   
                        copy_to_user(arg,&kbuf,sizeof(int));
                        printk(KERN_INFO "6-2-1 done \n");
                    }else{
                        
                        copy_to_user(arg,&kbuf,sizeof(int));
                        printk(KERN_INFO "6-2-2 done \n");
                    }  
                }
            }else if(kbuf==6){
                if(done_check==0){
                    copy_to_user(arg,&kbuf,sizeof(int));
                    printk(KERN_INFO "7-1 done \n");

                }else{
                    *spi_cs&=(0<<7);
                    kbuf+=1;
                    copy_to_user(arg,&kbuf,sizeof(int));
                    printk(KERN_INFO "7-2 done \n");
                }
            }
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
