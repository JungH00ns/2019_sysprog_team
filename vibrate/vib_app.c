#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#define VIB_MAJOR_NUMBER 503
#define VIB_MINOR_NUMBER 100

#define VIB_MAGIC_NUMBER  'v'
#define VIB_CMD_SEND     _IOW(VIB_MAGIC_NUMBER, 0,int)
#define VIB_CMD_RECV     _IOW(VIB_MAGIC_NUMBER, 1,int)
#define VIB_DEV_PATH_NAME "/dev/vib_dev"

int main(void)
{
	dev_t vib_dev;
	char buf[1024];
	int vibfd;

	vib_dev = makedev(VIB_MAJOR_NUMBER , VIB_MINOR_NUMBER);
	
	mknod(VIB_DEV_PATH_NAME ,S_IFCHR|0666 , vib_dev);
	
	vibfd=open(VIB_DEV_PATH_NAME,O_RDWR);
	
	if(vibfd<0)
	{
		printf("fail to open vib\n");
		return -1;
	}
	int recv_bit;
	while(1){
		recv_bit=1;
		ioctl(vibfd,VIB_CMD_SEND,&recv_bit);
		printf(" now = %d\n",recv_bit);
		usleep(500000);
	}
	
	close(vibfd);
	
	return 0;
}
