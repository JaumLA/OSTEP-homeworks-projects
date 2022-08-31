#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Wrong number of arguments.\n");
        exit(1);
    }
    int sizemega = atoi(argv[1]);
    int sizebyt = sizemega * 1000000;
    int *ptr = malloc(sizebyt);
    int n = sizebyt/(sizeof(int));
    if (ptr == NULL)
    {
        printf("Cannot alloc memory.\n");
        exit(1);
    }
    for (int i = 0; i < n; i++)
    {
        ptr[i] = 1;
    }
    
    printf("%d\n", getpid());
    sleep(10);
    free(ptr);

    return 0;
}
