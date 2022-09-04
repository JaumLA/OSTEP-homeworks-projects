#include "common_threads.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>


sem_t s; 

void *child(void *arg) {
    sleep(1);
    printf("child\n");

    if(sem_post(&s) == -1 ){
        printf("An error occurred at sem_post.\n");
        exit(1);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    printf("parent: begin\n");

    if(sem_init(&s, 0, 0) == -1){
        printf("An error occurred at sem_init\n");
        exit(1);
    }
    Pthread_create(&p, NULL, child, NULL);

    if(sem_wait(&s) == -1){
        printf("An error occurred at sem_wait.\n");
        exit(1);
    }
    printf("parent: end\n");
    return 0;
}

