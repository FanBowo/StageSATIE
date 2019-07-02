#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void my_func(int sign_no){
    if(sign_no==SIGINT){//Ctrl + C
        printf("I have get SIGINT\n");
    }
    else if(sign_no==SIGQUIT){
        printf("I have get SIGQUIT\n");
    }
}

int main(){
    printf("waiting for signal SIGINT or SIGQUIT\n");

    signal(SIGINT,my_func);
    signal(SIGQUIT,my_func);

    pause();
    pause();
    exit(0);
}
