#include <stdio.h>
#include <wiringPi.h>
#include <time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#define GPIO_BASE_ADDR 0x3F200000

#define GPFSEL1 0x04
#define GPCLR0 0x28
#define GPSET0 0x1c
#define GPLEV0 0x34
#define GPEDS0 0x40
#define GPREN0 0x4c
#define GPFEN0 0x58

/*volatile unsigned int *gpset0;
volatile unsigned int *gplev0;
volatile unsigned int *gpfsel1;
volatile unsigned int *gpclr0;
volatile unsigned int *gpeds0;
volatile unsigned int *gpren0;
volatile unsigned int *gpfen0;*/

int main(void){
    float dist,s,e;
    
    int fd = open("/dev/mem", O_RDWR|O_SYNC);
    if(fd <0 ){
        printf("fail to open vm\n");
        return -1;
    }
    
    /*char *gpio_memory_map = (char*)mmap(0,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,GPIO_BASE_ADDR);
    if(gpio_memory_map == MAP_FAILED)
    {
        printf("fail to map\n");
        return -1;
    }
    
    volatile unsigned int* gpio_base = (volatile unsigned int*)gpio_memory_map;
    
    gpfsel1 = (volatile unsigned int *)(gpio_base + GPFSEL1);
	gplev0 = (volatile unsigned int *)(gpio_base + GPLEV0);
	gpset0 = (volatile unsigned int *)(gpio_base + GPSET0);
	gpeds0 = (volatile unsigned int *)(gpio_base + GPEDS0);
	gpren0 = (volatile unsigned int *)(gpio_base + GPREN0);
	gpfen0 = (volatile unsigned int *)(gpio_base + GPFEN0);*/
    
    int tmp,tmp2,tmp3,tmp4,tmp5,tmp6;

    wiringPiSetup();
    
    pinMode(0,OUTPUT);
    pinMode(1,INPUT);
    
    while(1)
    {
        digitalWrite(0,0);
        /*tmp = ((*gplev0) & (0x01 << 17));
        tmp2 = ((*gplev0) & (0x01 << 18));
        tmp3 = ((*gpeds0) & (0x01 << 17));
        tmp4 = ((*gpeds0) & (0x01 << 18));
        tmp5 = ((*gpren0) & (0x01 << 17));
        tmp6 = ((*gpren0) & (0x01 << 18));
        printf(" 1 == lev17 : %d , lev18 : %d , eds 17 : %d , eds 18 : %d , ren 17 : %d , ren 18 : %d\n",tmp,tmp2,tmp3,tmp4,tmp5,tmp6);*/
        digitalWrite(0,1);

        /*tmp = ((*gplev0) & (0x01 << 17));
        tmp2 = ((*gplev0) & (0x01 << 18));
        tmp3 = ((*gpeds0) & (0x01 << 17));
        tmp4 = ((*gpeds0) & (0x01 << 18));
        tmp5 = ((*gpren0) & (0x01 << 17));
        tmp6 = ((*gpren0) & (0x01 << 18));
        printf(" 2 == lev17 : %d , lev18 : %d , eds 17 : %d , eds 18 : %d , ren 17 : %d , ren 18 : %d\n",tmp,tmp2,tmp3,tmp4,tmp5,tmp6);*/
        delayMicroseconds(10);
        digitalWrite(0,0);
        while(digitalRead(1)==0)
            s = micros();
        while(digitalRead(1)==1)
            e = micros();
        dist = (e-s) / 58;
        delayMicroseconds(10);
        printf("distance(cm) :%f",dist);
        
        /*tmp = ((*gplev0) & (0x01 << 17));
        tmp2 = ((*gplev0) & (0x01 << 18));
        tmp3 = ((*gpeds0) & (0x01 << 17));
        tmp4 = ((*gpeds0) & (0x01 << 18));
        tmp5 = ((*gpren0) & (0x01 << 17));
        tmp6 = ((*gpren0) & (0x01 << 18));
        printf(" 3 == lev17 : %d , lev18 : %d , eds 17 : %d , eds 18 : %d , ren 17 : %d , ren 18 : %d\n",tmp,tmp2,tmp3,tmp4,tmp5,tmp6);*/
        delay(100);
    }
    return 0;
}
