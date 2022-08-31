#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


/* The variable in the child become independent to the parent */
int main(int argc, char const *argv[])
{
    int c = 13;
    int rc = fork();

    if (rc == -1)
    {
        printf("Something bad occurred.");
        exit(1);
    }
    else if (rc == 0)
    {
        printf("The child variable is 'c' = %d\n", c);
        c = 10;
        printf("After the child change it to 10 'c' become: %c\n", c);
    }
    else
    {
        printf("The parent variable is 'c' = %d\n", c);
        c = 20;
        printf("After the parent change it to 20 'c' become: %c\n", c);
    }
    return 0;
}
