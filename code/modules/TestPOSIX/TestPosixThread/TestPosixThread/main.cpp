#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

void * async_counter(void * pv_unused){
    int j=0;
    while(j<10){
        printf("async_counter : %d\n",j);
        sleep(2);
        j++;
    }
    return NULL;
}

int main(){
    pthread_t async_counter_t;

    int i;

    if(0!=(errno=pthread_create(&async_counter_t,NULL,async_counter,NULL))){
        perror("pthread create failed");
        return EXIT_FAILURE;
    }

    i=0;
    while(i<15){
        printf("sync_counter: %d\n",i);
        sleep(1);
        i++;
    }

    printf("Waiting for async counter to finish ...\n");

    if(0!=(errno=pthread_join(async_counter_t,NULL))){
        perror("pthread join failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
