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

#define GPIO_BASE_ADDR 0x3F200000
#define GPLEV0 0x34
#define GPSET0 0x1C
#define GPCLR0 0x28

#define FLAME_MAGIC_NUMBER  'j'
#define FLAME_CMD_SEND     _IOW(FLAME_MAGIC_NUMBER, 0,int)
#define FLAME_CMD_RECV    _IOR(FLAME_MAGIC_NUMBER, 1,int)

int main(void){
	dev_t flame_dev;

	flame_dev=makedev(FLAME_MAJOR_NUMBER, FLAME_MINOR_NUMBER);
	int fd;
	mknod(FLAME_DEV_PATH_NAME, S_IFCHR|0666, flame_dev);
	fd=open(FLAME_DEV_PATH_NAME, O_RDWR);
	if(fd < 0){
		printf("fail to open flame\n");
		return -1;
	}
	printf("*****************WELCOME TO FLAME PROGRAM!*****************\n");
	int recv_bit;
	int time=0;
	while(1){
		ioctl(fd,FLAME_CMD_RECV,&recv_bit);
		printf("%d ",recv_bit);
		sleep(1);
	}
	return 0;
}
