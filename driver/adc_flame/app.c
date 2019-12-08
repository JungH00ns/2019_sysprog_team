#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#define FLAME_MAJOR_NUMBER 502
#define FLAME_MINOR_NUMBER 100
#define FLAME_DEV_PATH_NAME "/dev/flame_dev"
#define SPI_DEV_PATH_NAME "/dev/spidev0.0"


#define FLAME_MAGIC_NUMBER  'f'
#define FLAME_CMD_SEND     _IOWR(FLAME_MAGIC_NUMBER, 0,int)
#define FLAME_CMD_RECV    _IOWR(FLAME_MAGIC_NUMBER, 1,int)

int main(void){
	dev_t flame_dev;
	dev_t spi_dev;
	flame_dev=makedev(FLAME_MAJOR_NUMBER, FLAME_MINOR_NUMBER);
	
	int flame_fd;
	int spi_fd;
	mknod(FLAME_DEV_PATH_NAME, S_IFCHR|0666, flame_dev);
	flame_fd=open(FLAME_DEV_PATH_NAME, O_RDWR);
	if(flame_fd < 0){
		printf("fail to open flame\n");
		return -1;
	}
	printf("*****************WELCOME TO FLAME PROGRAM!*****************\n");
	int recv_bit=0;
	int time=0;
	ioctl(flame_fd,FLAME_CMD_SEND,&recv_bit);
	int num=0;
/*	
	while(1){
		ioctl(flame_fd,FLAME_CMD_RECV,&recv_bit);
		printf("%d\n",recv_bit);
		if(recv_bit==7){
		recv_bit=0;
		sleep(1);
			ioctl(flame_fd,FLAME_CMD_SEND,&recv_bit);

		continue;
		}
		nanosleep(1);
	}
	* */
	return 0;
}
