#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    if (argc != 3)
        exit(1);

    //int numpages = atoi(argv[1]);
    //int trials = atoi(argv[2]);

   cpu_set_t set;
   CPU_ZERO(&set);                                // clear cpu mask
   CPU_SET(0, &set);                              // set cpu 0
   if(sched_setaffinity(0, sizeof(cpu_set_t), &set) != 0) // 0 is the calling process
       exit(1);

    int maxnumpages = 1024;
    long pagesize = sysconf(_SC_PAGESIZE); // 4096
    long jump = pagesize / sizeof(int);    // 1024
    struct timeval tvbefore;
    struct timeval tvafter;

    FILE *fp = fopen("dataset", "w+");
    if (fp == NULL)
        exit(1);

    int trials = 10;
    double mean = 0;
    int j = 0;
    int l = 1;

    for (int numpages = 1; numpages <= maxnumpages; numpages *= 2)
    {
        trials = 10;
        mean = 0;
        int *a = calloc(numpages, pagesize);
        j = 0;
        l = 1;
        while (l <= trials)
        {
            if (gettimeofday(&tvbefore, NULL) == -1)
                exit(1);

            for (j = 0; j < 1e+3; j++)
            {
                for (int i = 0; i < numpages * jump; i += jump)
                {
                    a[i] += 1;
                }
            }

            if (gettimeofday(&tvafter, NULL) == -1)
                exit(1);

            double micro = ((double)(tvafter.tv_usec - tvbefore.tv_usec))*1e-3;
            double msec = (double)(tvafter.tv_sec - tvbefore.tv_sec)*1e-9;
            //printf("trial %d: %f\n", trials, micro + msec);
            mean += micro + msec;
            l++;
        }
        mean = mean/(double)numpages;
        if(fprintf(fp, "%d \t %f\n", numpages, mean) < 0)
                exit(1);
    }

    trials = 10;
    mean = 0;
    j = 0;
    l = 1;
    
    // printf("Mean for %d pages: %f\n", numpages, mean);
    return 0;
}
