#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#define MOTOR_MAJOR_NUMBER 505
#define MOTOR_MINOR_NUMBER 100
#define MOTOR_DEV_PATH_NAME "/dev/buzzer_dev"

#define IOCTL_MAGIC_NUMBER 'b'
#define IOCTL_CMD_SET_DIRECTION _IOWR(IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_CLEAR_DIRECTION _IOWR(IOCTL_MAGIC_NUMBER,1,int)
int main(void){
	dev_t motor_dev;
	int fd;
	int pin_direction =-1;
	int temp=0;
	motor_dev=makedev(MOTOR_MAJOR_NUMBER, MOTOR_MINOR_NUMBER);
	mknod(MOTOR_DEV_PATH_NAME, S_IFCHR|0666, motor_dev);
	
	fd=open(MOTOR_DEV_PATH_NAME, O_RDWR);
	
	if(fd < 0){
		printf("fail to open buzzer\n");
		return -1;
	}
	usleep(500000);
	ioctl(fd,IOCTL_CMD_SET_DIRECTION,&temp);
    usleep(500000);
	ioctl(fd,IOCTL_CMD_CLEAR_DIRECTION,&temp);
	usleep(500000);
	close(fd);
	
	return 0;
}
