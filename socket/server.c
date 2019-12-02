#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define PORT 20163
#define BUFFER_SIZE 4096
#define BUFF_SIZE 100
# define LISTEN_QUEUE_SIZE 5
void *myFunc(void *arg); 

int main() {
 	pthread_t thread_t;
	int th_id;  
 
    struct sockaddr_in listenSocket;
 
    memset(&listenSocket, 0, sizeof(listenSocket));
 
    listenSocket.sin_family = AF_INET;
    listenSocket.sin_addr.s_addr = htonl(INADDR_ANY);
    listenSocket.sin_port = htons(PORT);
 
    int listenFD = socket(AF_INET, SOCK_STREAM, 0);
    int connectFD;
 int result;
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
		th_id = pthread_create(&thread_t, NULL, myFunc, (void *)&connectFD);
		if(th_id != 0){
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
	char readBuff[BUFFER_SIZE];
    	char sendBuff[BUFFER_SIZE];
	int connectFD;
	int receivedBytes;
	connectFD = *((int *)arg);
 	printf("enter client :  %d\n",connectFD);
                while((receivedBytes = read(connectFD, readBuff, BUFF_SIZE)) > 0)
                {                
 
                    printf("%lu bytes read\n", receivedBytes);
                    readBuff[receivedBytes] = '\0';
                    fputs(readBuff, stdout);
                    fflush(stdout);
                    if(!strncmp(readBuff,"0",1)){
                        write(connectFD,"0",1);
                    }
                    else if(!strncmp(readBuff,"1",1)){
                        write(connectFD,"1",1);
                    }
                    
                }
}
