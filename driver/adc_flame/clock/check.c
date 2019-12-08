#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#define GPIO_BASE_ADDR 0x3F200000
#define GPLEV0 0x34
#define GPSET0 0x1C
#define GPCLR0 0x28

#define CLOCK_MAJOR_NUMBER 508
#define CLOCK_MINOR_NUMBER 100
#define CLOCK_DEV_PATH_NAME "/dev/clock_dev"

#define CLOCK_MAGIC_NUMBER  'c'
#define CLOCK_CMD_SEND     _IOWR(CLOCK_MAGIC_NUMBER, 0,int)
#define CLOCK_CMD_RECV    _IOWR(CLOCK_MAGIC_NUMBER, 1,int)
#define CLOCK_CMD_CHECK   _IOWR(CLOCK_MAGIC_NUMBER, 2,int)
#define CLOCK_CMD_CLEAR    _IOWR(CLOCK_MAGIC_NUMBER, 3,int)

#define FLAME_MAJOR_NUMBER 502
#define FLAME_MINOR_NUMBER 100
#define FLAME_DEV_PATH_NAME "/dev/flame_dev"

#define FLAME_MAGIC_NUMBER  'f'
#define FLAME_CMD_SEND     _IOWR(FLAME_MAGIC_NUMBER, 0,int)
#define FLAME_CMD_RECV    _IOWR(FLAME_MAGIC_NUMBER, 1,int)
#define FLAME_CMD_CHECK    _IOWR(FLAME_MAGIC_NUMBER, 2,int)

void* clock_msg(void* arg);
void* flame_msg(void* arg);
dev_t clock_dev;
int clock_fd;

dev_t flame_dev;
int flame_fd;

int bit1[8]={0,0,0,0,0,0,0,1};
int bit2[8]={1,0,0,0,0,0,0,0};
int bit3[8]={0,0,0,0,0,0,0,0};
int check_cs=-1;

int main(void){
	
	pthread_t clock_thread, flame_thread;
	void* thread_return;


	flame_dev=makedev(FLAME_MAJOR_NUMBER, FLAME_MINOR_NUMBER);
	mknod(FLAME_DEV_PATH_NAME, S_IFCHR|0666, flame_dev);
	flame_fd=open(FLAME_DEV_PATH_NAME, O_RDWR);
	if(flame_fd < 0){
		printf("fail to open flame\n");
		return -1;
	}

	clock_dev=makedev(CLOCK_MAJOR_NUMBER, CLOCK_MINOR_NUMBER);
	mknod(CLOCK_DEV_PATH_NAME, S_IFCHR|0666, clock_dev);
	clock_fd=open(CLOCK_DEV_PATH_NAME, O_RDWR);
	if(clock_fd < 0){
		printf("fail to open clock\n");
		return -1;
	}

	
	
	pthread_create(&clock_thread, NULL, clock_msg, (void*)&clock_fd);
    pthread_create(&flame_thread, NULL, flame_msg, (void*)&flame_fd);
	
    pthread_join(clock_thread, &thread_return);
    pthread_join(flame_thread, &thread_return);
   
    close(clock_fd);
    close(flame_fd);

	//printf("*****************WELCOME TO FLAME PROGRAM!*****************\n");
	//int time=0;
	
	return 0;
}

void* clock_msg(void* arg){
		int recv_bit=0;
	
	while(1){
		//if(check_cs==1){
			recv_bit=1;
			ioctl(clock_fd,CLOCK_CMD_RECV,&recv_bit);
			usleep(1);
			recv_bit=0;
			ioctl(clock_fd,CLOCK_CMD_RECV,&recv_bit);
			usleep(1);
		//}
	}
}

void* flame_msg(void* arg){
	int count=1;
	check_cs=1;
	//ioctl(clock_fd,CLOCK_CMD_CHECK,&check_cs);
	printf("-----%d\n",check_cs);

	//ioctl(flame_fd,FLAME_CMD_RECV,&check_cs);

	//while(1){
		int clock_fulse=0;
		count=1;
		ioctl(flame_fd,FLAME_CMD_RECV,&clock_fulse);
		printf("%d\n",check_cs);
		//if(clock_fulse==1){
			while(1){
				for(int i=0;i<8;i++){
					ioctl(flame_fd,FLAME_CMD_SEND,bit1[i]);
					count++;
					usleep(1);
					
				}
				for(int i=0;i<8;i++){
					ioctl(flame_fd,FLAME_CMD_SEND,bit2[i]);
					count++;
					usleep(1);
				}
				for(int i=0;i<8;i++){
					ioctl(flame_fd,FLAME_CMD_SEND,bit3[i]);
					count++;
					usleep(1);
				}
				break;
			}
		//}else{
			//if(count==24){
			//	check_cs=-1;
					//ioctl(clock_fd,CLOCK_CMD_CLEAR,&check_cs);

			//	break;
			//	}
		//}
	//	usleep(1);
	//}
	

}

