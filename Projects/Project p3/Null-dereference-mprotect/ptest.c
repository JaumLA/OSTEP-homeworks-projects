#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char *argv[])
{
  int *c;
  c = (int *)0x1000;
  printf(1,"%x\n", (unsigned int)*c);
  mprotect(c, 1);
  printf(1, "%x\n", (unsigned int)*c);
  munprotect(c, 1);
  c = (int *)0x1005;
  *c = 0x0;
  exit();
}