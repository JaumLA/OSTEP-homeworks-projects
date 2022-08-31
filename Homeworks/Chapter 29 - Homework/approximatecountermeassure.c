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
    long global;                    // global count
    pthread_mutex_t glock;          // global lock
    long local[NUMCPUS];            // per-CPU count
    pthread_mutex_t llock[NUMCPUS]; // ... and locks
    int threshold;                  // update frequency

} counter_t;

// init: record threshold, init locks, init values
// of all local counts and global count
void init(counter_t *c, int threshold)
{
    c->threshold = threshold;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);
    int i;
    for (i = 0; i < NUMCPUS; i++)
    {
        c->local[i] = 0;
        pthread_mutex_init(&c->llock[i], NULL);
    }
}

// update: usually, just grab local lock and update
// local amount; once local count has risen ’threshold’,
// grab global lock and transfer local values to it
void update(counter_t *c, int threadID, int amt)
{
    int cpu = threadID % NUMCPUS;
    pthread_mutex_lock(&c->llock[cpu]);
    c->local[cpu] += amt;
    if (c->local[cpu] >= c->threshold)
    {
        // transfer to global (assumes amt>0)
        pthread_mutex_lock(&c->glock);
        c->global += c->local[cpu];
        pthread_mutex_unlock(&c->glock);
        c->local[cpu] = 0;
    }
    pthread_mutex_unlock(&c->llock[cpu]);
}

// get: just return global amount (approximate)
int get(counter_t *c)
{
    pthread_mutex_lock(&c->glock);
    int val = c->global;
    pthread_mutex_unlock(&c->glock);
    return val; // only approximate!
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

    int cpunum = sched_getcpu();

    if (cpunum == -1)
        exit(1);
    for (i = 0; i < targ->lim; i++)
    {
        update(targ->c, cpunum, 1);
    }
    return NULL;
}

//
counter_t *incnthreads(int tnum, int interval, int limitcount)
{
    targs *targ = malloc(sizeof(targs));
    targ->c = malloc(sizeof(counter_t));
    targ->lim = limitcount;
    init(targ->c, interval);

    pthread_t pid[tnum];
    cpu_set_t set;

    // set threads to each cpu, alternating between them.
    for (int i = 0; i < tnum; i++)
    {
        if (pthread_create(&pid[i], NULL, pcounter, targ) != 0)
            printf("Error in creating a thread.\n");
        CPU_ZERO(&set);
        CPU_SET(i % NUMCPUS, &set);
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

    int interval = 1024;
    int tnum = 10;
    clock_t before = clock();
    c = incnthreads(tnum, interval, 1e+6);
    clock_t after = clock();

    printf("Global counter is: %d\n", get(c));
    printf("Finished in: %f seconds.\n", (double)(after - before) / (CLOCKS_PER_SEC*tnum));
    return 0;
}
