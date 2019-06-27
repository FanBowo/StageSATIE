#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
int j=0;
pthread_mutex_t mutex_j;

void * async_counter(void * pv_unused){
    // the shared variableshouldn't be variable static or global
    printf("Enter thread async_counter: %ld\n",pthread_self());
    while(1){
        if(0!=(errno=pthread_mutex_lock(&mutex_j))){
            perror("pthread mutex lock failed");
            exit(EXIT_FAILURE);
        }
        j++;
        if(0!=(errno=pthread_mutex_unlock(&mutex_j))){
            perror("pthread unlock mutex failed");
            exit(EXIT_FAILURE);
        }
        printf("Thread ID: %ld , counter: %d \n",pthread_self(),j);
        sleep((rand()%5));
    }
    return NULL;
}

int main(){
    pthread_t async_counter_t,async_counter_t1;
    if(0!=(errno=pthread_mutex_init(&mutex_j,NULL))){
        perror("pthrea  mutex init failed");
        return EXIT_FAILURE;
    }
    //int i;

    if(0!=(errno=pthread_create(&async_counter_t,NULL,async_counter,NULL))){
        perror("pthread create failed");
        return EXIT_FAILURE;
    }

    if(0!=(errno=pthread_create(&async_counter_t1,NULL,async_counter,NULL))){
        perror("pthread create failed");
        return EXIT_FAILURE;
    }


    //i=0;
//    while(1){
//        printf("main thread\n");
//        sleep(3);
//        //i++;
//    }

    printf("Waiting for async counter to finish ...\n");

    if(0!=(errno=pthread_join(async_counter_t,NULL))){
        perror("pthread join failed");
        return EXIT_FAILURE;
    }
    if(0!=(errno=pthread_join(async_counter_t1,NULL))){
        perror("pthread join failed");
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
