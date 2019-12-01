#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>

#define PORT 20163
#define BUFFER_SIZE 4096
#define BUFF_SIZE 100

//ultra define section
#define ULTRA_MAJOR_NUMBER 501
#define ULTRA_MINOR_NUMBER 100
#define ULTRA_MAGIC_NUMBER  'u'
#define ULTRA_CMD_SEND     _IOW(ULTRA_MAGIC_NUMBER, 0,int)
#define ULTRA_CMD_RECV     _IOW(ULTRA_MAGIC_NUMBER, 1,int)
#define ULTRA_DEV_PATH_NAME "/dev/ultra_dev"

//vib define section
#define VIB_MAJOR_NUMBER 503
#define VIB_MINOR_NUMBER 100
#define VIB_MAGIC_NUMBER  'v'
#define VIB_CMD_SEND     _IOW(VIB_MAGIC_NUMBER, 0,int)
#define VIB_CMD_RECV     _IOW(VIB_MAGIC_NUMBER, 1,int)
#define VIB_DEV_PATH_NAME "/dev/vib_dev"

//motor define section
#define MOTOR_MAJOR_NUMBER 504
#define MOTOR_MINOR_NUMBER 100
#define MOTOR_MAGIC_NUMBER 's'
#define MOTOR_DEV_PATH_NAME "/dev/motor_dev"
#define MOTOR_IOCTL_CMD_SET_DIRECTION _IOWR(MOTOR_MAGIC_NUMBER, 0, int)
#define MOTOR_IOCTL_CMD_CLEAR_DIRECTION _IOWR(MOTOR_MAGIC_NUMBER,1,int)

//buzzer define section
#define BUZZER_MAJOR_NUMBER 505
#define BUZZER_MINOR_NUMBER 100
#define BUZZER_DEV_PATH_NAME "/dev/buzzer_dev"

#define BUZZER_MAGIC_NUMBER 'b'
#define BUZZER_CMD_SET_DIRECTION _IOWR(BUZZER_MAGIC_NUMBER, 0, int)
#define BUZZER_CMD_CLEAR_DIRECTION _IOWR(BUZZER_MAGIC_NUMBER,1,int)

//ultra variable
dev_t ultra_dev;
int ultra_fd;

//vib variable
dev_t vib_dev;
int vib_fd;

//motor variable
dev_t motor_dev;
int motor_fd;

//buzzer variable
dev_t buzzer_dev;
int buzzer_fd;

//socket variable
struct sockaddr_in connectSocket;

int ultra_init();
int ultra_check();
int vib_init();
int vib_check();
int motor_init();
int buzzer_init();
int buzzer_check(int num);

int main(int argc, char** argv)
{
    if (argc != 2) 
    {
        printf("Usage: %s IPv4-address\n", argv[0]);
        return -1;
    }
 
    memset(&connectSocket, 0, sizeof(connectSocket));
 
    connectSocket.sin_family = AF_INET;
    inet_aton(argv[1], (struct in_addr*) &connectSocket.sin_addr.s_addr);
    connectSocket.sin_port = htons(PORT);
 
    int connectFD = socket(AF_INET, SOCK_STREAM, 0);
 
    if (connect(connectFD, (struct sockaddr*) &connectSocket, sizeof(connectSocket)) == -1) 
    {
        printf("Can not connect.\n");
        return -1;
    }
    else 
    {   
        //initialize device drivers
        ultra_init();
        vib_init();
        motor_init();
        buzzer_init();
	
        //socket variables...
        int readBytes, writtenBytes;
        char sendBuffer[BUFFER_SIZE];
        char receiveBuffer[BUFFER_SIZE];
 
        while (1) 
        {
            int send_flag=0;
            int ultra_status=ultra_check();
            int vib_status=vib_check();
            
            if(ultra_status==1)
                send_flag=1;
            else if(vib_status==2)
                send_flag=2; //2
	    
            sprintf(sendBuffer,"%d\n", send_flag);
            write(connectFD, sendBuffer, strlen(sendBuffer));
            
            readBytes = read(connectFD, receiveBuffer, BUFF_SIZE);
           
            int recv_value=atoi(receiveBuffer);
          //  printf("send : %d recv : %d \n",vib_status,recv_value);
            
            if(recv_value!=0&&(ultra_status!=0||vib_status!=0)){
                int temp_value;
                ioctl(motor_fd,MOTOR_IOCTL_CMD_SET_DIRECTION,&temp_value);
                usleep(500000);
                ioctl(motor_fd,MOTOR_IOCTL_CMD_CLEAR_DIRECTION,&temp_value);
		buzzer_check(recv_value);
            }
            
            receiveBuffer[readBytes] = '\0';
            sleep(1);
        }
    }
 
    close(connectFD);
    close(ultra_fd);
    close(vib_fd);
    close(motor_fd);
    close(buzzer_fd);

    return 0;
}  

int ultra_init(){
    
    ultra_dev = makedev(ULTRA_MAJOR_NUMBER , ULTRA_MINOR_NUMBER);
	
    mknod(ULTRA_DEV_PATH_NAME ,S_IFCHR|0666 , ultra_dev);
    
    ultra_fd=open(ULTRA_DEV_PATH_NAME,O_RDWR);
    if(ultra_fd<0)
	{
		printf("fail to open ultra\n");
		return -1;
	}
    return 0;
}

int ultra_check(){
    int send_bit=1;
    ioctl(ultra_fd,ULTRA_CMD_SEND,&send_bit); 
    usleep(10); //send 1 while 10us
    send_bit = 0;
    ioctl(ultra_fd,ULTRA_CMD_SEND,&send_bit); //send 0
    float time = 0;
    int recv_bit = 0;
    while(1)
    {
        ioctl(ultra_fd,ULTRA_CMD_RECV,&recv_bit);
        if(recv_bit!=0)
        {
            time=recv_bit*1.4;
            printf("time bit : %3f\n",recv_bit*1.4);
            break;
        }
        usleep(1);
    }
    if(time>=30.0)
        return 0;
    else
        return 1;
}

int vib_init(){
    vib_dev = makedev(VIB_MAJOR_NUMBER , VIB_MINOR_NUMBER);
	
    mknod(VIB_DEV_PATH_NAME ,S_IFCHR|0666 , vib_dev);
    
    vib_fd=open(VIB_DEV_PATH_NAME,O_RDWR);
    
    if(vib_fd<0)
    {
	    printf("fail to open vib\n");
	    return -1;
    }
}

int vib_check(){
    int recv_bit;

    //recv_bit=1;
    ioctl(vib_fd,VIB_CMD_SEND,&recv_bit);
    printf("now = %d\n",recv_bit);
   // usleep(500000);
    return recv_bit/4194304;
}

int motor_init(){
    
    motor_dev=makedev(MOTOR_MAJOR_NUMBER, MOTOR_MINOR_NUMBER);
    mknod(MOTOR_DEV_PATH_NAME, S_IFCHR|0666, motor_dev);
    
    motor_fd=open(MOTOR_DEV_PATH_NAME, O_RDWR);
    
    if(motor_fd < 0){
	    printf("fail to open motor\n");
	    return -1;
    }
    return 0;
}

int buzzer_init(){
    buzzer_dev=makedev(BUZZER_MAJOR_NUMBER, BUZZER_MINOR_NUMBER);
    mknod(BUZZER_DEV_PATH_NAME, S_IFCHR|0666, buzzer_dev);
    
    buzzer_fd=open(BUZZER_DEV_PATH_NAME, O_RDWR);
    
    if(buzzer_fd < 0){
	    printf("fail to open buzzer\n");
	    return -1;
    }
    return 0;
}

int buzzer_check(int num){
    int situation=num;
    ioctl(buzzer_fd,BUZZER_CMD_SET_DIRECTION,&situation);
}
