#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#include <linux/spi/spidev.h>

#define VIB_MAJOR_NUMBER 503
#define VIB_MINOR_NUMBER 100

#define SPI_CHANNEL 0
#define SPI_SPEED 1000000

#define VIB_MAGIC_NUMBER  'v'

#define SPI_CHANNEL 0
#define SPI_SPEED 1000000

#define INPUT 0
#define OUTPUT 1
#define LOW 0
#define HIGH 1

#define VIB_CMD_IN     _IOW(VIB_MAGIC_NUMBER, 0,int)
#define VIB_CMD_OUT     _IOW(VIB_MAGIC_NUMBER, 1,int)

static unsigned char spiMode = 0;
static unsigned char spiBPW = 8 ;
static unsigned char spiDelay = 0;
static unsigned int spiSpeeds[2];

static int spiFds[2];
int vibfd;
int read_mcp3008_adc(unsigned char adcChannel);

int SPIDataRW(int channel , unsigned char *data , int len);

#define VIB_DEV_PATH_NAME "/dev/vib_dev"
#define SPI_DEV_PATH_NAME "/dev/spidev0.0"

int SPIDataRW(int channel , unsigned char *data ,int len)
{
	struct spi_ioc_transfer spi;
	
	channel &=1;
	
	spi.tx_buf = (unsigned long)data;
	spi.rx_buf = (unsigned long)data;
	spi.len = len;
	spi.delay_usecs = spiDelay;
	spi.speed_hz = 1000000;
	spi.bits_per_word = spiBPW;    
	//sleep(1000);
	
	return ioctl(spiFds[channel],SPI_IOC_MESSAGE(1),&spi);
}

int read_mcp3008_adc(unsigned char adcChannel)
{
	unsigned char buff[3];
	int adcValue = 0;
	int send_bit;
	buff[0] = 0x06 | ((adcChannel & 0x07) >> 7 );
	buff[1] = ((adcChannel & 0x07) << 6 );
	buff[2] = 0x00;
	
	send_bit = 0;
	ioctl(vibfd,VIB_CMD_OUT,&send_bit);
	
	SPIDataRW(SPI_CHANNEL,buff,3);
	
	printf("buff 1 : %s\n buff 2 : %s\n buff 3 : %s\n",buff[1],buff[2],buff[3]);
	
	buff[1] = 0x0F & buff[1];
	adcValue = (( buff[1]<<8) | buff[2]);
	
	send_bit = 1;
	ioctl(vibfd,VIB_CMD_OUT,&send_bit);
	
	return adcValue;
}

int main(void)
{
	dev_t vib_dev;
	char buf[1024];
	int adcValue =0;	
	int adcChannel=0;
	int spifd;
	
	vib_dev = makedev(VIB_MAJOR_NUMBER , VIB_MINOR_NUMBER);
	
	mknod(VIB_DEV_PATH_NAME ,S_IFCHR|0666 , vib_dev);
	
    spifd = open(SPI_DEV_PATH_NAME,O_RDWR);
	if(spifd <0 )
	{
		printf("fail to open spi\n");
		return -1;
	}
   
	vibfd=open(VIB_DEV_PATH_NAME,O_RDWR);
	
	if(vibfd<0)
	{
		printf("fail to open vib\n");
		return -1;
	}
	spiFds[0]=spifd;
	printf("spifd = %d\n",spiFds[0]);
	while(1)
	{
		adcValue = read_mcp3008_adc(adcChannel);
		printf("adc0 value = %d\n",adcValue);
		sleep(1);
	}
	
	close(vibfd);
	
	return 0;
}
