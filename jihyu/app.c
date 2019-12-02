#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h> 

#include <sys/ioctl.h> 
#include <sys/types.h> 
#include <sys/sysmacros.h>  
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// button define section
#define BUTTON_MAJOR_NUMBER 506
#define BUTTON_MINOR_NUMBER 100
#define BUTTON_DEV_PATH 	"/button/simple_button_dev"
#define IOCTL_MAGIC_NUMBER 		 'b'
#define IOCTL_CMD_SET_LED_ON     _IO(IOCTL_MAGIC_NUMBER, 0)
#define IOCTL_CMD_SET_LED_OFF    _IO(IOCTL_MAGIC_NUMBER, 1)
#define INTERVAL 		50000
#define SITUATION_ON 	0 
#define SITUATION_OFF 	1
#define IOCTL_CMD_SEND     _IOW(IOCTL_MAGIC_NUMBER, 0,int)
// socket define section
#define PORT 20163


#define BUFFER_SIZE 4096
#define BUFF_SIZE 100
#define LISTEN_QUEUE_SIZE 5

// button variable
dev_t button_dev;
int button_fd; 
int status = SITUATION_OFF; 
int current_button_value = 0, prev_button_value=0; 

int button_init(){
    button_dev=makedev(BUTTON_MAJOR_NUMBER, BUTTON_MINOR_NUMBER);
	mknod(BUTTON_DEV_PATH, S_IFCHR|0666, button_dev);
	button_fd = open(BUTTON_DEV_PATH, O_RDONLY);
	if (button_fd < 0) { 
		printf("failed to open button device\n"); 
		return -1;
	} 
    return 0;
}

int button_status(int situ){
    if(situ==0){
        return 0;
    }
    
	
    usleep(INTERVAL); 
    prev_button_value = current_button_value; 
    //read(button_fd, &current_button_value, sizeof(int));
    ioctl(button_fd,IOCTL_CMD_SEND,&current_button_value);
    if (prev_button_value == 0 && current_button_value != 0) {
        if (status == SITUATION_ON) {
            //off
            status = SITUATION_OFF;  //1
            printf("status : %d\n",status);
            return status;
        }
        else { 
            //on 
            status = SITUATION_ON;     //0
            printf("status : %d\n",status);
            return status;
        } 
    }		
	return 0;
}
void childHandler(int signal)
{
    
    int status;
    pid_t spid;
    while((spid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        printf("================================\n");
        printf("PID         : %d\n", spid);
        printf("Exit Value  : %d\n", WEXITSTATUS(status));
        printf("Exit Stat   : %d\n", WIFEXITED(status));
    }
}
 
int main() {
 
    signal(SIGCHLD, (void *)childHandler);    
 
    struct sockaddr_in listenSocket;
 
    memset(&listenSocket, 0, sizeof(listenSocket));
 
    listenSocket.sin_family = AF_INET;
    listenSocket.sin_addr.s_addr = htonl(INADDR_ANY);
    listenSocket.sin_port = htons(PORT);
 
    int listenFD = socket(AF_INET, SOCK_STREAM, 0);
    int connectFD;
 
    ssize_t receivedBytes;
    char readBuff[BUFFER_SIZE];
    char sendBuff[BUFFER_SIZE];
    pid_t pid;
 
 
    if (bind(listenFD, (struct sockaddr *) &listenSocket, sizeof(listenSocket)) == -1) {
        printf("Can not bind.\n");
        return -1;
    }
 
    if (listen(listenFD, LISTEN_QUEUE_SIZE) == -1) {
        printf("Listen fail.\n");
        return -1;
    }
 
    printf("Waiting for clients...\n");
 
    while (1) 
    {
        struct sockaddr_in connectSocket, peerSocket;
 
        socklen_t connectSocketLength = sizeof(connectSocket);
 
        while((connectFD = accept(listenFD, (struct sockaddr*)&connectSocket, (socklen_t *)&connectSocketLength)) >= 0)
        {
            getpeername(connectFD, (struct sockaddr*)&peerSocket, &connectSocketLength);
 
            char peerName[sizeof(peerSocket.sin_addr) + 1] = { 0 };
            sprintf(peerName, "%s", inet_ntoa(peerSocket.sin_addr));
 
            if(strcmp(peerName,"0.0.0.0") != 0)
                printf("Client : %s\n", peerName);
        
 
            if (connectFD < 0)
            {
                printf("Server: accept failed\n");
                exit(0);
            }
            pid = fork();
 
            if(pid == 0)
            {    
                close(listenFD);
 
                ssize_t receivedBytes;
                int client_state=0;
                char arr[1024]={0,};    //
                while((receivedBytes = read(connectFD, readBuff, BUFF_SIZE)) > 0)
                {                
                    printf("%lu bytes read\n", receivedBytes);
                    readBuff[receivedBytes] = '\0';
                    fputs(readBuff, stdout);
                    fflush(stdout);
                    int button_state=0; //
                  //  button_state=atoi(readBuff);    //
                    
                   // button_state=button_status(button_state); //
                      printf("client : %d\n", client_state);

                    if(!strncmp(readBuff,"0",1)){
                        //alert event not occuerd
                        printf("read0\n");
                        if(client_state!=0){
                            sprintf(arr, "%d\n", client_state);
                            //sprintf()
                            write(connectFD,arr,1);
                        }
                        else if(client_state==0){

                            write(connectFD,"0",1);
                        }
                    }
                    else if(!strncmp(readBuff,"1",1)){


                        //tanyack susang! - ultra
                        if(client_state==0){ //

                            //if(button_state==1){ //button_status-> button_state
                           //     write(connectFD,"0",1);
                           // }
                           // else if(button_state==0){
                                write(connectFD,"1",1);
                                client_state=1;
                           // }
                        }
                        else if(client_state!=0){ //
sprintf(arr, "%d\n", client_state);
                            //sprintf()
                            write(connectFD,arr,1);
                           /* if(button_state==1){
                                write(connectFD,"0",1);
                                client_state=0;
                            }
                            else if(button_state==0){
                                write(connectFD,"1",1);
                                client_state=1;
                            }*/
                          //  write(connectFD,"1",1);
                        }
                    }
                    else if(!strncmp(readBuff,"2",1)){
                         if(client_state==0){ //
                            if(button_state==1){ //button_status-> button_state
                                write(connectFD,"0",1);
                            }
                            else if(button_state==0){
                                write(connectFD,"2",1);
                                client_state=2;
                            }
                        }
                        else if(client_state!=0){ //
                            if(button_state==1){
                                write(connectFD,"0",1);
                                client_state=0;
                            }
                            else if(button_state==0){
                                write(connectFD,"2",1);
                                client_state=2;
                            }
                          //  write(connectFD,"1",1);
                        }
                    }
                    else if(!strncmp(readBuff,"3",1)){
                        //jungdae susang! - fire
                        write(connectFD,"3",1);
                        client_state=3;
                    }
                    else if(!strncmp(readBuff,"4",1)){
                        //jungdae susang! - ultra
                        write(connectFD,"4",1);
                        client_state=4;
                    }
                    else if(!strncmp(readBuff,"5",1)){
                        //temp temp
                        write(connectFD,"5",1);
                        client_state=5;
                    }
                }
                
                close(connectFD); 
                return 0; 
    
            }
 
            else
                close(connectFD);
        }
        
    }
    close(listenFD);
    close(button_fd); 
    return 0;
}

