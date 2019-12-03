#include <unistd.h> 
#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include <sys/ioctl.h> 
#include <sys/types.h> 
#include <sys/sysmacros.h>  


#define IOCTL_MAGIC_NUMBER 		 'b'
#define IOCTL_CMD_SET_LED_ON     _IO(IOCTL_MAGIC_NUMBER, 0)
#define IOCTL_CMD_SET_LED_OFF    _IO(IOCTL_MAGIC_NUMBER, 1)

#define SIMPLE_BUTTON_MAJOR_NUMBER 506
#define SIMPLE_BUTTON_MINOR_NUMBER 100
#define SIMPLE_BUTTON_DEV_PATH 	"/simple_button_dev"

#define INTERVAL 		50000

#define SITUATION_ON 	0 
#define SITUATION_OFF 	1 
#define IOCTL_CMD_SEND     _IOW(IOCTL_MAGIC_NUMBER, 0,int)
int main (int argc, char ** argv ){ 
	dev_t simple_button_dev;
	int simple_button_fd; 
	
	simple_button_dev=makedev(SIMPLE_BUTTON_MAJOR_NUMBER, SIMPLE_BUTTON_MINOR_NUMBER);
	mknod(SIMPLE_BUTTON_DEV_PATH, S_IFCHR|0666, simple_button_dev);
	simple_button_fd = open(SIMPLE_BUTTON_DEV_PATH, O_RDONLY);
	if (simple_button_fd < 0) { 
		printf("failed to open button device\n"); 
		return 0;
	} 
	
	int status = SITUATION_OFF; 
	int current_button_value = 0, prev_button_value=0; 
	
	while(1){
		usleep(INTERVAL); 
		prev_button_value = current_button_value; 
		//read(simple_button_fd, &current_button_value, sizeof(int));
		ioctl(simple_button_fd,IOCTL_CMD_SEND,&current_button_value);
		if (prev_button_value == 0 && current_button_value != 0) {
			if (status == SITUATION_ON) {
				//off
				status = SITUATION_OFF;  
				printf("status : %d\n",status);
			}
			else { 
				//on 
				status = SITUATION_ON; 
				printf("status : %d\n",status);
			} 
		}		
	}
	
	close(simple_button_fd); 
} 
