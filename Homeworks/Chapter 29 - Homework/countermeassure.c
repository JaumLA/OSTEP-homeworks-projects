#define _GNU_SOURCE

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <utmpx.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/time.h>

#define NUMCPUS 4

int sig = 0;

typedef struct counter_t
{
    int value;                    // global count
    pthread_mutex_t glock;          // global lock
} counter_t;

// init: record threshold, init locks, init values
// of all local counts and global count
void init(counter_t *c)
{
    c->value = 0;
    pthread_mutex_init(&c->glock, NULL);
}

// update: usually, just grab local lock and update
// local amount; once local count has risen ’threshold’,
// grab global lock and transfer local values to it
void increment(counter_t *c)
{
    pthread_mutex_lock(&c->glock);
    c->value++;
    pthread_mutex_unlock(&c->glock);
}

void decrement(counter_t *c)
{
    pthread_mutex_lock(&c->glock);
    c->value--;
    pthread_mutex_unlock(&c->glock);
}

int get(counter_t *c)
{
    pthread_mutex_lock(&c->glock);
    int rc = c->value;
    pthread_mutex_unlock(&c->glock);
    return rc;
}

typedef struct targs
{
    counter_t *c;
    int lim;
} targs;

// created threads start executing this function.
void *pcounter(void *arg)
{
    int i;
    targs *targ = (targs *)arg;

    for (i = 0; i < targ->lim; i++)
    {
        increment(targ->c);
    }
    return NULL;
}

//
counter_t *incnthreads(int tnum, int limitcount)
{
    targs *targ = malloc(sizeof(targs));
    targ->c = malloc(sizeof(counter_t));
    targ->lim = (int)ceil(limitcount / tnum);
    init(targ->c);

    pthread_t pid[tnum];
    cpu_set_t set;

    // set threads to each cpu, alternating between them.
    for (int i = 0; i < tnum; i++)
    {
        if (pthread_create(&pid[i], NULL, pcounter, targ) != 0)
            printf("Error in creating a thread.\n");
        CPU_ZERO(&set);
        CPU_SET(i, &set);
        pthread_setaffinity_np(pid[i], sizeof(cpu_set_t), &set);
    }
    sig = 1;

    
    for (int i = 0; i < tnum; i++)
    {
        if (pthread_join(pid[i], NULL) != 0)
            printf("Pthread join error.\n");
    }
    
    return targ->c;
}

int main(int argc, char const *argv[])
{
    counter_t *c;
    clock_t before = clock();
    c = incnthreads(30, 1e+6);
    printf("Global counter is: %d\n", get(c));
    clock_t after = clock();
    printf("Finished in: %f seconds.\n", (double)(after - before) / CLOCKS_PER_SEC);
    return 0;
}
