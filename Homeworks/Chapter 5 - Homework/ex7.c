#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

/* If wait is used in the child process it return -1. and
    terminate the child process, if used in the parent wait for,
    the child to exit and return the child pid. */
int main(int argc, char const *argv[])
{
    int rc = fork();
    if (rc == -1)
    {
        printf("Something bad occurred.\n");
        exit(1);
    }
    else if (rc == 0)
    {
        printf("Child say: Hello. My pid is: %d\n", getpid());
        close(STDOUT_FILENO);
        printf("Am i open?\n");
    }
    else
    {
        printf("Parent say: Goodbye.\n");
    }
    return 0;
}
