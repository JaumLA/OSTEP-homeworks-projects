#include "types.h"
#include "user.h"

int 
main(int argc, char *argv[])
{
    int count = getreadcount();
    printf(1, "%d\n", count, argv[0]);
    exit();
}