#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int *data = malloc(100*sizeof(int));
    free(data);
    //printf("%d\n", data[100]);
    
    data = malloc(100*sizeof(int));

    free(data+40);
    return 0;
}
