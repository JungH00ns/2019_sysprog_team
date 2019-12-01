#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#define BUZZER_MAJOR_NUMBER 505
#define BUZZER_MINOR_NUMBER 100
#define BUZZER_DEV_PATH_NAME "/dev/buzzer_dev"

#define IOCTL_MAGIC_NUMBER 'b'
#define IOCTL_CMD_SET_DIRECTION _IOWR(IOCTL_MAGIC_NUMBER, 0, int)
#define IOCTL_CMD_CLEAR_DIRECTION _IOWR(IOCTL_MAGIC_NUMBER,1,int)
int main(void){
	dev_t buzzer_dev;
	int buzzer_fd;
	int pin_direction =-1;
	int situation=0;
	buzzer_dev=makedev(BUZZER_MAJOR_NUMBER, BUZZER_MINOR_NUMBER);
	mknod(BUZZER_DEV_PATH_NAME, S_IFCHR|0666, buzzer_dev);
	
	buzzer_fd=open(BUZZER_DEV_PATH_NAME, O_RDWR);
	
	if(buzzer_fd < 0){
		printf("fail to open buzzer\n");
		return -1;
	}
	ioctl(buzzer_fd,IOCTL_CMD_SET_DIRECTION,&situation);
	situation=1;
	sleep(1);
	ioctl(buzzer_fd,IOCTL_CMD_SET_DIRECTION,&situation);
	close(buzzer_fd);
	
	return 0;
}
