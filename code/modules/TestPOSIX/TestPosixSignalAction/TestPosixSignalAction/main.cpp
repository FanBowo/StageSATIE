#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void my_func(int sign_no,siginfo_t *info,void *Parametres){
    if(sign_no==SIGINT){//Ctrl + C
        printf("I have get SIGINT\n");
    }
    else if(sign_no==SIGQUIT){
        printf("I have get SIGQUIT\n");
    }

    int i;
    for(i=0;i<10;i++){
    printf("%d\n ",(*( (int*)((*info).si_ptr)+i)));
    }

}

int main(){
    printf("waiting for signal SIGINT or SIGQUIT\n");
    int Testdata[10]={1,2,3,4,5,6,7,8,9,10};

    struct sigaction act;
    union sigval mysigval;
    pid_t pid;
    pid=getpid();

    mysigval.sival_ptr=Testdata;
    sigemptyset(&act.sa_mask);
    act.sa_sigaction=my_func;
    act.sa_flags=SA_SIGINFO;
    if(sigaction(SIGINT,&act,NULL) < 0){
        printf("install sigal error\n");
    }
   if(sigaction(SIGQUIT,&act,NULL) < 0){
        printf("install sigal error\n");
    }
    printf("wait for the signal\n");
    sigqueue(pid,SIGINT,mysigval);//向本进程发送信号，并传递附加信息
    sigqueue(pid,SIGQUIT,mysigval);//向本进程发送信号，并传递附加信息
    pause();
    pause();
    exit(0);
}
