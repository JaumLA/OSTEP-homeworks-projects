#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

/* If wait is used in the child process it return -1. and
    terminate the child process, if used in the parent wait for,
    the child to exit and return the child pid. */
int main(int argc, char const *argv[])
{
    struct timeval tv = {0};
    int argp1[2], argp2[2];
    int n = 10000;
    if (pipe(argp1) < 0)
    {
        printf("Error on pipe function.");
        exit(1);
    }
    if (pipe(argp2) < 0)
    {
        printf("Error on pipe function.");
        exit(1);
    }
    int rc1 = fork();

    if (rc1 == -1)
    {
        printf("Something bad occurred.\n");
        exit(1);
    }
    else if (rc1 == 0)
    {
        sched_setaffinity(0, sizeof(cpu_set_t), 0);
        for (int i = 0; i < n; i++)
        {
            read(argp1[0], NULL, 0);
            write(argp2[1], NULL, 0);
        }
    }
    else
    {
        sched_setaffinity(0, sizeof(cpu_set_t), 0);

        gettimeofday(&tv, NULL);
        double microbefore = (double)tv.tv_usec;

        for (int i = 0; i < n; i++)
        {
            write(argp1[1], NULL, 0);
            read(argp2[0], NULL, 0);
        }

        gettimeofday(&tv, NULL);
        printf("Seconds: %f\n", (double)(tv.tv_usec - microbefore) / (double)n);
    }
    return 0;
}
