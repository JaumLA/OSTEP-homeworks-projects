#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

/* waitpid() world be useful when are multiples childs process
    that need to control its execution and states. */
int main(int argc, char const *argv[])
{
    int rc = fork();
    int pi;
    if (rc == -1)
    {
        printf("Something bad occurred.\n");
        exit(1);
    }
    else if (rc == 0)
    {
        printf("Child say: Hello. My pid is: %d\n", getpid());
    }
    else
    {
        if ((pi = waitpid(-1, NULL, 0)) == -1)
        {
            printf("Error in the wait function.");
            exit(1);
        }
        printf("Parent say: Goodbye.\n");
        printf("pid of the terminated child: %d\n", pi);
    }
    return 0;
}
