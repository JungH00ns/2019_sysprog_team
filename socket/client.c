#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h> // AF_INET 외부 네트워크 도메인
#include <sys/types.h>
#include <sys/socket.h>

#define MAXLINE 511

int main(int argc,char *argv[]){
    int cli_sock;
    struct sockaddr_in serv_addr;
    int datalen;
    pid_t pid;

    char buf[MAXLINE+1];
    int nbytes;

    if(argc != 3){
        printf("Usage : %s <IP> <Port> \n", argv[0]);
        exit(0);
    }

    cli_sock = socket(PF_INET, SOCK_STREAM, 0); 

    if(cli_sock == -1){
        perror("socket() error\n");
        exit(0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); 
    serv_addr.sin_port = htons(atoi(argv[2])); 

    if(connect(cli_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){ 
        perror("connect() error\n");
        exit(0);
    }

    if((pid=fork()) ==1){ 
        perror("fork() error\n");
        exit(0);
    }
    else if(pid == 0) { 
        while(1){        
            fgets(buf,sizeof(buf),stdin);
            nbytes = strlen(buf);
            
            if((strncmp,"exit",4) == 0){
                puts("Good Bye.");
                exit(0);
            }
            write(cli_sock,buf,MAXLINE);
        }
        exit(0);

    }
    else if(pid>0){ 
        while(1){    
            if(nbytes = read(cli_sock,buf,MAXLINE) <0){
                perror("read() error\n");
                exit(0);
            }
            printf("%s",buf);
            if(strncmp(buf,"exit",4) == 0)
                puts("Good Bye.");
                exit(0);
            }
    }

    close(cli_sock);
    return 0;
}

