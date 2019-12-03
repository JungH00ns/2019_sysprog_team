#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h> 
#include <pthread.h>

#include <sys/ioctl.h> 
#include <sys/types.h> 
#include <sys/sysmacros.h>  
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// button define section
#define BUTTON_MAJOR_NUMBER 506
#define BUTTON_MINOR_NUMBER 100
#define BUTTON_DEV_PATH		"/dev/button_dev"
#define BUTTON_MAGIC_NUMBER 		 'b'
#define IOCTL_CMD_SIMPLE     _IOW(BUTTON_MAGIC_NUMBER, 0,int)
#define IOCTL_CMD_ON_OFF     _IOW(BUTTON_MAGIC_NUMBER, 1,int)
// led define section
#define LED_MAJOR_NUMBER 507
#define LED_MINOR_NUMBER 100
#define LED_DEV_PATH_NAME "/dev/led_dev"
#define LED_MAGIC_NUMBER  'l'
#define LED_CMD_SEND     _IOW(LED_MAGIC_NUMBER, 0,int)

#define INTERVAL 		50000
#define IOCTL_CMD_SEND     _IOW(IOCTL_MAGIC_NUMBER, 0,int)
// socket define section
#define PORT 20163

#define BUFF_SIZE 100
#define LISTEN_QUEUE_SIZE 5

#define SITUATION_ON 	0 
#define SITUATION_OFF 	1
void *myFunc(void *arg);

// button variable
dev_t button_dev;
int button_fd;
// led variable
dev_t led_dev;
int led_fd;
int button_on = 0;	//simple_button
int simple_button_value = 0;
int mode = 0;		//on_off
int client_state=0;
int button_init() {
	button_dev = makedev(BUTTON_MAJOR_NUMBER, BUTTON_MINOR_NUMBER);
	mknod(BUTTON_DEV_PATH, S_IFCHR | 0666, button_dev);
	button_fd = open(BUTTON_DEV_PATH, O_RDONLY);
	if (button_fd < 0) {
		printf("failed to open button device\n");
		return -1;
	}
	return 0;
}

int button_set(int selc) {
	if (selc == 0) {	//simple
		ioctl(button_fd, IOCTL_CMD_SIMPLE, &simple_button_value);
		usleep(INTERVAL);
		if(simple_button_value){	//start
			printf("simple_button_value : %d  return 0\n",simple_button_value);
			return 0;
		}
		else{	//button on
			printf("simple_button_value : %d  return 1\n",simple_button_value);
			return 1;
		}
			
		
		printf("????\n");
		return 1;
	}
	else if (selc == 1) {	//on_off
		ioctl(button_fd, IOCTL_CMD_ON_OFF, &mode);
		//printf("mode : %d\n", mode);
		
		usleep(INTERVAL);
		if(mode==0){
			//printf("on_off mode==0\n");
			return 0;
		}else{
			//printf("on_off mode==1...\n");
			return 9;
		}
		
	}

	return 0;
}

int led_init() {

	led_dev = makedev(LED_MAJOR_NUMBER, LED_MINOR_NUMBER);
	mknod(LED_DEV_PATH_NAME, S_IFCHR | 0666, led_dev);
	led_fd = open(LED_DEV_PATH_NAME, O_RDWR);
	if (led_fd < 0) {
		printf("fail to open led\n");
		return -1;
	}
	return 0;
}

void led_status(int state) {
	int recv_bit;
	if (state == 0) {
		ioctl(led_fd, LED_CMD_SEND, &state);
		usleep(1000);
	}
	else if (state == 1) {
		ioctl(led_fd, LED_CMD_SEND, &state);
		usleep(1000);
	}
	else if (state == 2) {
		ioctl(led_fd, LED_CMD_SEND, &state);
		usleep(1000);
	}
	else if (state == 3) {
		ioctl(led_fd, LED_CMD_SEND, &state);
		usleep(1000);
	}
	else if (state == 4) {
		ioctl(led_fd, LED_CMD_SEND, &state);
		usleep(1000);
	}
	return;
}



int main() {

	struct sockaddr_in listenSocket;
	pthread_t thread_t;
	int th_id;
	memset(&listenSocket, 0, sizeof(listenSocket));

	listenSocket.sin_family = AF_INET;
	listenSocket.sin_addr.s_addr = htonl(INADDR_ANY);
	listenSocket.sin_port = htons(PORT);

	int listenFD = socket(AF_INET, SOCK_STREAM, 0);
	int connectFD;
	if (bind(listenFD, (struct sockaddr *) &listenSocket, sizeof(listenSocket)) == -1) {
		printf("Can not bind.\n");
		return -1;
	}

	if (listen(listenFD, LISTEN_QUEUE_SIZE) == -1) {
		printf("Listen fail.\n");
		return -1;
	}

	printf("Waiting for clients...\n");
	if (led_init() < 0) {
		printf("led init error\n");
	}
	if (button_init() < 0) {
		printf("button init error\n");
	}
	while (1)
	{
		struct sockaddr_in connectSocket, peerSocket;

		socklen_t connectSocketLength = sizeof(connectSocket);

		while ((connectFD = accept(listenFD, (struct sockaddr*)&connectSocket, (socklen_t *)&connectSocketLength)) >= 0)
		{
			th_id = pthread_create(&thread_t, NULL, myFunc, (void *)&connectFD);
			if (th_id != 0) {
				perror("Thread Create Error");
				return 1;
			}
		}
	}
	close(listenFD);
	return 0;
}
void *myFunc(void *arg)
{
	char readBuff[BUFF_SIZE];
	char sendBuff[BUFF_SIZE];
	int connectFD;
	int receivedBytes;
	int situation_off=0;
	char arr[1024] = { 0, };    //
	int end;
	connectFD = *((int *)arg);

	printf("enter client :  %d\n", connectFD);

	while ((receivedBytes = read(connectFD, readBuff, BUFF_SIZE)) > 0)
	{
		readBuff[receivedBytes] = '\0';
		//fputs(readBuff, stdout);
		printf("readBuff : %s",readBuff);
		fflush(stdout);
		
		end=button_set(0);	//simple_button check
		situation_off=button_set(1);	//on_off check
		
		if(end==1){		//or 0
			printf("button_set %d\n",end);
			client_state=0;
			led_status(0);      //black
			write(connectFD, "0", 1);
		}
		else if(situation_off==9){ 
			printf("situation_off\n");
			write(connectFD, "0", 1);
			led_status(0);      //black
		}
		else if (!strncmp(readBuff, "0", 1)) {
			//alert event not occuerd
			led_status(0);      //black
			printf("not occuerd\n");
			if(client_state!=0){
                 sprintf(arr, "%d\n", client_state);
                 write(connectFD,arr,1);
            }
            else if(client_state==0){
                 write(connectFD,"0",1);
            }
		}
		else if (!strncmp(readBuff, "1", 1)) {
			//tanyack susang! - ultra
			led_status(1);  //red
			printf("tanyack susang! - ultra\n");
			if(client_state==0){ //
                 write(connectFD,"1",1);
                 client_state=1;
            }
            else if(client_state!=0){ //
				 sprintf(arr, "%d\n", client_state);
                 write(connectFD,arr,1);
            }
		}
		else if (!strncmp(readBuff, "2", 1)) {
			//tanyack susang! - vibe
			led_status(2);  //green
			printf("tanyack susang! - vibe\n");
			if(client_state==0){ //
                 write(connectFD,"2",1);
                 client_state=2;
            }
            else if(client_state!=0){ 
				 sprintf(arr, "%d\n", client_state);
                 write(connectFD,arr,1);
            }
		}
		else if (!strncmp(readBuff, "3", 1)) {
			//jungdae susang! - fire
			led_status(3);  //blue
			printf("jungdae susang! - fire\n");
			if(client_state==0){ //
                 write(connectFD,"3",1);
                 client_state=3;
            }
            else if(client_state!=0){ 
				 sprintf(arr, "%d\n", client_state);
                 write(connectFD,arr,1);
            }
		}
		else if (!strncmp(readBuff, "4", 1)) {
			//jungdae susang! - ultra
			led_status(4);  //yellow (red,green)
			printf("jungdae susang! - ultra\n");
			if(client_state==0){ //
                 write(connectFD,"4",1);
                 client_state=4;
            }
            else if(client_state!=0){ 
				 sprintf(arr, "%d\n", client_state);
                 write(connectFD,arr,1);
            }
		}
		else if (!strncmp(readBuff, "5", 1)) {
			//temp temp
			write(connectFD, "5", 1);
		}
	}
}

