#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define BUFSIZE 512
using namespace std;

int main(){
    #define CKERR(msg) {if(ret<0){perror(msg);\
                        close(sockp[0]);close(sockp[1]);exit(EXIT_FAILURE);}}

    const char * strs_to_write[]={"hello","form","other","side","test1","test2","test3"};

    int sockp[2]={-1,-1};
    ssize_t ret=socketpair(AF_UNIX,SOCK_STREAM,0,sockp);
    CKERR("Socket pair creation error")

    struct pollfd pfds[2];
    for(int i=0;i<2;i++){
        pfds[i]=(struct pollfd){sockp[i],POLLIN|POLLOUT,0};
        fcntl(sockp[i],F_SETFL|O_NONBLOCK);
    }
    char buf[BUFSIZE];

    size_t snt=0,msgs=sizeof(strs_to_write)/sizeof(char*);
    printf("Total messages: %d\n",msgs);
    while(1){
        int ret=poll(pfds,2,5);//2:length of pollfd array 5:milliseconds to wait
        CKERR("Poll Error")

        if(pfds[0].revents&POLLOUT&&snt<msgs){
            ssize_t ret=send(sockp[0],strs_to_write[snt],strlen(strs_to_write[snt]),0);
            printf("sended str: %.*s\n",ret,strs_to_write[snt]);
            if(++snt>=msgs) close(sockp[0]);

            CKERR("Send error")
            if(ret==0){
                puts("connection closed");
                break;
            }
            if(ret>0){

            }

        }

        if(pfds[1].revents&POLLIN){
            ssize_t ret=recv(sockp[1],buf,BUFSIZE,0);
            CKERR("receive error")
            if(ret==0){
                puts("Connection closed");
                break;
            }
            if(ret>0){
                printf("received str: %.*s\n",ret,(char*)buf);
                //printf("received str: %s\n",(char*)buf);
            }
        }
    }
    close(sockp[1]);
    return EXIT_SUCCESS;
}
