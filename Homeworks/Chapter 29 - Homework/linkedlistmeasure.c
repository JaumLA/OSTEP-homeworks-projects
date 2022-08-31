#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// basic node structure
typedef struct node_t
{
    int key;
    struct node_t *next;
} node_t;

// basic list structure (one used per list)
typedef struct __list_t
{
    node_t *head;
    pthread_mutex_t lock;
} list_t;

void List_Init(list_t *L)
{
    L->head = NULL;
    pthread_mutex_init(&L->lock, NULL);
}

void List_Insert(list_t *L, int key)
{
    // synchronization not needed
    node_t *new = malloc(sizeof(node_t));
    if (new == NULL)
    {
        perror("malloc");
        return;
    }
    new->key = key;

    // just lock critical section
    pthread_mutex_lock(&L->lock);
    new->next = L->head;
    L->head = new;
    pthread_mutex_unlock(&L->lock);
}

int List_Lookup(list_t *L, int key)
{
    int rv = -1;
    pthread_mutex_lock(&L->lock);
    node_t *curr = L->head;
    while (curr)
    {
        if (curr->key == key)
        {
            rv = 0;
            break;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&L->lock);
    return rv;
}

typedef struct targs
{
    list_t *ls;
    int totalnodes;
    int startkey;
} targs;

// created threads start executing this function.
void *worker(void *arg)
{

    int i;
    targs *targ = (targs *)arg;

    printf("Started at key %d \n", targ->startkey);
    for (i = targ->startkey; i < i + targ->totalnodes; i++)
    {
        List_Insert(targ->ls, i);
    }
    printf("Finished, started at key %d", targ->startkey);
    return NULL;
}

// Insert n nodes into a concurrent list and
// lookup all of the nodes.
int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("Usage: linkedlistmeasure totalnode threadnum\n");
        exit(1);
    }
    int totalnodes = atoi(argv[1]);
    int totalthreads = atoi(argv[2]);

    list_t *ls = malloc(sizeof(list_t));
    List_Init(ls);

    pthread_t pid[totalthreads];

    for (int i = 0; i < totalthreads; i++)
    {
        targs *targ = malloc(sizeof(targs));
        targ->ls = ls;
        targ->startkey = i * totalnodes;
        targ->totalnodes = totalnodes;

        if (pthread_create(&pid[i], NULL, worker, targ) != 0)
            printf("Error in creating a thread.\n");
    }
    clock_t bef = clock();

    for (int i = 0; i < totalnodes; i++)
        List_Insert(ls, i);

    for (int i = 0; i < totalnodes; i++)
        List_Lookup(ls, i);

    clock_t aft = clock();

    double totalsec = (double)(aft - bef) / CLOCKS_PER_SEC;
    printf("time to insert %d nodes and lookup all of them: %f seconds\n", totalnodes*totalthreads, totalsec);
    return 0;
}
