#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* get a file path and print into stdout informations
 * about the file.*/
int main(int argc, char const *argv[])
{
  if (argc != 2)
  {
    printf("Give a file path to get its stats.\n");
    exit(1);
  }

  struct stat *fs = malloc(sizeof(struct stat));
  if (stat(argv[1], fs) <= -1)
  {
    printf("Cannot retrieve the given file %s stats.\n", argv[1]);
    exit(1);
  }

  printf("File type:  ");

  switch (fs->st_mode & S_IFMT)
  {
  case S_IFBLK:
    printf("block device\n");
    break;
  case S_IFCHR:
    printf("character device\n");
    break;
  case S_IFDIR:
    printf("directory\n");
    break;
  case S_IFIFO:
    printf("FIFO/pipe\n");
    break;
  case S_IFLNK:
    printf("symlink\n");
    break;
  case S_IFREG:
    printf("regular file\n");
    break;
  case S_IFSOCK:
    printf("socket\n");
    break;
  default:
    printf("unknown?\n");
  }
  printf("Size: %ld\n", fs->st_size);
  printf("# of blocks: %ld\n", fs->st_blocks);
  printf("# of links: %ld\n", fs->st_nlink);
  printf("Block size: %ld\n", fs->st_blksize);

  return 0;
}
