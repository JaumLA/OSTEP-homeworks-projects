#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int n = 100;
    int *data = malloc(n*sizeof(int));

    int *plusptr = realloc(data, n*(sizeof(int)) + sizeof(int));

    plusptr[100] = 1;

    return 0;
}
