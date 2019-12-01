#include <unistd.h> 
#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include <sys/ioctl.h> 
#include <sys/types.h> 
#include <sys/sysmacros.h>  


#define IOCTL_MAGIC_NUMBER 		 'o'

#define ON_OFF_MAJOR_NUMBER 508
#define ON_OFF_MINOR_NUMBER 100
#define ON_OFF_DEV_PATH 	"/on_off_dev"
#define IOCTL_CMD_SEND     _IOW(IOCTL_MAGIC_NUMBER, 0,int)
//#define INTERVAL 		50000

#define MODE_ON 	0 
#define MODE_OFF 	1 

int main (int argc, char ** argv ){ 
	dev_t on_off_dev;
	int on_off_fd; 
	int num;
	on_off_dev=makedev(ON_OFF_MAJOR_NUMBER, ON_OFF_MINOR_NUMBER);
	mknod(ON_OFF_DEV_PATH, S_IFCHR|0666, on_off_dev);
	
	on_off_fd= open(ON_OFF_DEV_PATH, O_RDONLY);
	if (on_off_fd < 0) { 
		printf("failed to open button device\n"); 
		return 0;
	} 
	int mode = MODE_OFF; 
	while(1){
		ioctl(on_off_fd,IOCTL_CMD_SEND,&num);
		//printf("%d",num);
		usleep(50000);
	}
	
	
	close(on_off_fd); 
} 
