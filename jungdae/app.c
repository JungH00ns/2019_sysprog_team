#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

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

//ultra variable
dev_t ultra_dev;
int ultra_fd;

struct sockaddr_in connectSocket;

int ultra_init();
int ultra_check();

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
    {   ultra_init();
        int readBytes, writtenBytes;
        char sendBuffer[BUFFER_SIZE];
        char receiveBuffer[BUFFER_SIZE];
 
        while (1) 
        {
            //ioctl
            int ultra_status=ultra_check();
            sprintf(sendBuffer,"%d\n", ultra_status);
//            itoa(ultra_status, sendBuffer,10);
            //printf("Please input\n");
            //fgets(sendBuffer,BUFF_SIZE,stdin);
 
            write(connectFD, sendBuffer, strlen(sendBuffer));
            
            //readBytes = read(connectFD, receiveBuffer, BUFF_SIZE);
            //printf("%d bytes read\n", readBytes);
           // receiveBuffer[readBytes] = '\0';
           // fputs(receiveBuffer, stdout);
           // fflush(stdout);
            sleep(1);
        }
    }
 
    close(connectFD);
    close(ultra_fd);
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
