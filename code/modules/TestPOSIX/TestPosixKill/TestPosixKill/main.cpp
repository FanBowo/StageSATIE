#define _POSIX_C_SOURCE 1
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

int main(void){
    pid_t pid =getpid();
    kill(pid,SIGKILL);
    puts("Signal delivery initiated.");

    pause();

    puts("This never get printed");
}
