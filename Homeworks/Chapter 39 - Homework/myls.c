#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void list_files(DIR *, int);
void print_stat(struct stat *);

int main(int argc, char const *argv[])
{
  int i = 1;	//index for the first arg
  int show_info = 0;	//1 to show the file  infos, else 0
  char *path = NULL;
  DIR *dir;

  //if no argument is given just read the current directory
  if (argc <= 1)
  {
    dir = opendir(getcwd(NULL, 0));
    if (dir == NULL)
    {
      printf("Cannot open the current directory.\n");
      exit(1);
    }
    list_files(dir, 0);
  }

  //search for the  -l flag and path to the directory
  else
  {
    while (i < argc)
    {
      if (strcmp("-l", argv[i]) == 0)
        show_info = 1;
      else
        strcpy(path, argv[i]);
      i++;
    }
    if (path == NULL)
      dir = opendir(getcwd(NULL, 0));
    if (dir == NULL)
    {
      printf("Cannot open the current directory.\n");
      exit(1);
    }

    list_files(dir, show_info);
  }
  return 0;
}

/*list_files: give a DIR *dir and a true/false show_info,
 * print the files names into the stdout, with some informations
 * based on show_info value.*/
void list_files(DIR *dir, int show_info)
{
  struct stat *fs = malloc(sizeof(struct stat));
  struct dirent *files;
  while ((files = readdir(dir)) != NULL)
  {
    stat(files->d_name, fs);
    printf("Name: %s\n", files->d_name);
    if (show_info)
      print_stat(fs);
  }
}

/*print_stat: given a struct stat *fs, print into stdout
 * the informations aobut the file.*/
void print_stat(struct stat *fs)
{
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
}
