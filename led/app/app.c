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

#define LED_MAJOR_NUMBER	507
#define LED_MINOR_NUMBER 100
#define LED_DEV_PATH_NAME "/dev/led_dev"


#define LED_MAGIC_NUMBER  'l'
#define LED_CMD_SEND     _IOW(LED_MAGIC_NUMBER, 0,int)

int main(void){
	dev_t led_dev;

	led_dev=makedev(LED_MAJOR_NUMBER, LED_MINOR_NUMBER);
	int fd;
	mknod(LED_DEV_PATH_NAME, S_IFCHR|0666, led_dev);
	fd=open(LED_DEV_PATH_NAME, O_RDWR);
	if(fd < 0){
		printf("fail to open led\n");
		return -1;
	}
	printf("*****************WELCOME TO FLAME PROGRAM!*****************\n");
	int recv_bit=1;
	
		recv_bit=1;
		ioctl(fd,LED_CMD_SEND,&recv_bit);
		printf("%d\n",recv_bit);
		//sleep(1);
		recv_bit=2;
		ioctl(fd,LED_CMD_SEND,&recv_bit);
		printf("%d\n",recv_bit);
		//sleep(1);
		recv_bit=4;
		ioctl(fd,LED_CMD_SEND,&recv_bit);
		printf("%d\n",recv_bit);
		//sleep(1);

	return 0;
}
