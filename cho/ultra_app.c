#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#define ULTRA_MAJOR_NUMBER 501
#define ULTRA_MINOR_NUMBER 100

#define ULTRA_MAGIC_NUMBER  'u'
#define ULTRA_CMD_SEND     _IOW(ULTRA_MAGIC_NUMBER, 0,int)
#define ULTRA_CMD_RECV     _IOW(ULTRA_MAGIC_NUMBER, 1,int)
#define ULTRA_DEV_PATH_NAME "/dev/ultra_dev"

int main(void)
{
	dev_t ultra_dev;
	char buf[1024];
	int fd;
	int pin_direction = -1,numLoop =0;

	ultra_dev = makedev(ULTRA_MAJOR_NUMBER , ULTRA_MINOR_NUMBER);
	
	mknod(ULTRA_DEV_PATH_NAME ,S_IFCHR|0666 , ultra_dev);
	
	fd=open(ULTRA_DEV_PATH_NAME,O_RDWR);
	
	if(fd<0)
	{
		printf("fail to open ultra\n");
		return -1;
	}
	int recv_bit;
	while(1){
		recv_bit=1;
		ioctl(fd,ULTRA_CMD_SEND,&recv_bit);
		usleep(10);
		recv_bit = 0;
		ioctl(fd,ULTRA_CMD_SEND,&recv_bit);
		int time = 0 ;
		int flag=0;
		int time_bit = 0;
		while(1)
		{
			//printf("time = %d \n",time);
			ioctl(fd,ULTRA_CMD_RECV,&time_bit);
			
			if(time_bit!=0)
			{
				printf("time bit : %d\n",time_bit);
				break;
			}
			usleep(1);
		}
		sleep(1);
	}
	
	close(fd);
	
	return 0;
}
