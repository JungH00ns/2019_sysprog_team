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

//flame define section
#define FLAME_MAJOR_NUMBER 502
#define FLAME_MINOR_NUMBER 100
#define FLAME_MAGIC_NUMBER  'f'
#define FLAME_CMD_SEND     _IOW(FLAME_MAGIC_NUMBER, 0,int)
#define FLAME_CMD_RECV    _IOR(FLAME_MAGIC_NUMBER, 1,int)

#define FLAME_DEV_PATH_NAME "/dev/flame_dev"

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

//flame variable
dev_t flame_dev;
int flame_fd;

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
int flame_init();
int flame_check();
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
        motor_init();
        flame_init();
        buzzer_init();
        
        //socket variables...
        int readBytes, writtenBytes;
        char sendBuffer[BUFFER_SIZE];
        char receiveBuffer[BUFFER_SIZE];
        int flag=0;
        int send_flag=0;
        while (1) 
        {
            int ultra_status=ultra_check();
            int flame_status=flame_check();
            
            printf("    %d\n",flame_status);
            if(send_flag != 1 && send_flag!=2)
            {
                if(flame_status==3)
                    send_flag=3;
                else if(ultra_status==4)
                    send_flag=4;
            }
                
            sprintf(sendBuffer,"%d\n", send_flag);
            write(connectFD, sendBuffer, strlen(sendBuffer));
            
            readBytes = read(connectFD, receiveBuffer, BUFF_SIZE);
           
            int recv_value=atoi(receiveBuffer);

            printf("send : %d recv : %d \n",send_flag,recv_value);
            if(recv_value==1){
                send_flag=1;
            }
            if(recv_value==2){
                send_flag=2;
            }
            if(recv_value!=0){
                flag++;
                if(flag==1)
                {
                    int temp_value;
                    ioctl(motor_fd,MOTOR_IOCTL_CMD_SET_DIRECTION,&temp_value);
                    usleep(500000);
                    ioctl(motor_fd,MOTOR_IOCTL_CMD_CLEAR_DIRECTION,&temp_value);
                    flag++;
                }
                buzzer_check(recv_value);
            }//recv 0 -> flag =0
             else if(recv_value==0){
                flag=0;
                send_flag=0;
            }
                
                receiveBuffer[readBytes] = '\0';
                sleep(1);
        }
        
    }
 
    close(connectFD);
    close(ultra_fd);
    close(flame_fd);
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
        return 4;
}

int flame_init(){
    flame_dev=makedev(FLAME_MAJOR_NUMBER, FLAME_MINOR_NUMBER);
	mknod(FLAME_DEV_PATH_NAME, S_IFCHR|0666, flame_dev);
	flame_fd=open(FLAME_DEV_PATH_NAME, O_RDWR);
	if(flame_fd < 0){
		printf("fail to open flame\n");
		return -1;
	}
    return 0;
}

int flame_check(){
    int recv_bit;
    ioctl(flame_fd,FLAME_CMD_RECV,&recv_bit);
    
    if(recv_bit==1)
        return 0;
    else if(recv_bit==0)
        return 3;
}

int motor_init(){
    motor_dev=makedev(MOTOR_MAJOR_NUMBER, MOTOR_MINOR_NUMBER);
	mknod(MOTOR_DEV_PATH_NAME, S_IFCHR|0666, motor_dev);
	
	motor_fd=open(MOTOR_DEV_PATH_NAME, O_RDWR);
	
	if(motor_fd < 0){
		printf("fail to open motor\n");
		return -1;
	}
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
    int situation=num-1;
    ioctl(buzzer_fd,BUZZER_CMD_SET_DIRECTION,&situation);
}
int SPIDataRW (int channel, unsigned char *data, int len)
 {	 
     struct spi_ioc_transfer spi ;

     channel &= 1 ;
     spi.tx_buf        = (unsigned long)data ;
     spi.rx_buf        = (unsigned long)data ;
     spi.len           = len ;
     spi.delay_usecs   = spiDelay ;
     spi.speed_hz      = 1000000;
     spi.bits_per_word = spiBPW ;
     return ioctl (spiFds [channel], SPI_IOC_MESSAGE(1), &spi) ;
 }

 int read_mcp3208_adc(unsigned char adcChannel)
{
  unsigned char buff[3];
  int adcValue = 0;

  buff[0] = 0x06 | ((adcChannel & 0x07) >> 7);
  buff[1] = ((adcChannel & 0x07) << 6);
  buff[2] = 0x00;

  digitalWrite_pin_25(0);  // Low : CS Active

  SPIDataRW(SPI_CHANNEL, buff, 3);

  buff[1] = 0x0F & buff[1];
  adcValue = ( buff[1] << 8) | buff[2];

  digitalWrite_pin_25(1);  // High : CS Inactive

  return adcValue;
}




 void pin_25_Mode(int mode)
 {
     int fSel, shift, alt ;

       fSel    = 2;
       shift   = 15;

	 if (mode == INPUT){
			*(gpio + fSel ) = (*(gpio + fSel) & ~(7 << shift)) ; // Sets bits to     zero = input
     }
	 else if (mode == OUTPUT){
			*(gpio ) = (*(gpio ) & ~(7 << shift)) | (1 << 24) ;
			printf("OUTPUT\n");
	 }
 }

 void digitalWrite_pin_25(int value)
 {
     int pin =8;
     int gpCLR = 10;
     int gpSET = 7;

     if (value == LOW)
         *(gpio + gpCLR) = 1 << (pin & 31) ;
     else
         *(gpio + gpSET) = 1 << (pin & 31) ;
}
