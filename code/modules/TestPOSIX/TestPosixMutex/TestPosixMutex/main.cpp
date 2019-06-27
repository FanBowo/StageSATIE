#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int global_resource;

//pthread_mutex_t mutex;

void * thread_inc(void *arg){
    pthread_mutex_t *mutex=(pthread_mutex_t *) (arg);
    if(0!=(errno=pthread_mutex_lock(mutex))){
        perror("pthread mutex lock failed");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<10;i++){
        global_resource++;
        printf("Increment:%d\n",global_resource);
        sleep(1);
    }

    printf("Thread inc finished\n");

    if(0!=(errno=pthread_mutex_unlock(mutex))){
        perror("pthread unlock mutex failed");
        exit(EXIT_FAILURE);
    }
    return NULL;
}

void * thread_dec(void *arg){
    pthread_mutex_t *mutex=(pthread_mutex_t *) (arg);
    if(0!=(errno=pthread_mutex_lock(mutex))){
        perror("pthread mutex lock failed");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<10;i++){
        global_resource--;
        printf("Increment:%d\n",global_resource);
        sleep(1);
    }

    printf("Thread dec finished\n");

    if(0!=(errno=pthread_mutex_unlock(mutex))){
        perror("pthread unlock mutex failed");
        exit(EXIT_FAILURE);
    }
    return NULL;
}

int main(){
    pthread_t threads[2];
    pthread_mutex_t mutex;

    if(0!=(errno=pthread_mutex_init(&mutex,NULL))){
        perror("pthrea  mutex init failed");
        return EXIT_FAILURE;
    }

    if(0!=(errno=pthread_create(&threads[0],NULL,thread_inc,&mutex))){
        perror("pthread create failed\n");
        return EXIT_FAILURE;
    }
    if(0!=(errno=pthread_create(&threads[1],NULL,thread_dec,&mutex))){
        perror("pthread create failed\n");
        return EXIT_FAILURE;
    }

    printf("wait for two threads finished\n");

    for(int i=0;i<2;i++){
        if(0!=(errno=pthread_join(threads[i],NULL))){
            perror("pthread join failed");
            return EXIT_FAILURE;
        }
    }

    if(0!=(errno=pthread_mutex_destroy(&mutex))){
        perror("pthread mutex destroy failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
