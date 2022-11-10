#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void seek_last_n_lines(FILE *, int);

int main(int argc, char const *argv[])
{
  if(argc != 3){
    printf("Usage: /mytail -n fileName. Where n is the number of lines\n");
    exit(1);
  }
  int ln = atoi(argv[1]+1);
  FILE *f = fopen(argv[2], "r");

  if(fseek(f, 0, SEEK_END) <= -1)
    exit(1);

  seek_last_n_lines(f, ln);

  char *ptr = NULL;
  size_t size = 0;
  int n_char = 0;
  while((n_char = getline(&ptr, &size, f)) != -1){
    if(n_char == 1)
      continue;
    printf("line %ld: %s", size, ptr);
    ptr = NULL;
  }
  free(ptr);
  fclose(f);
  return 0;
}

void seek_last_n_lines(FILE *f, int ln){
  long len = ftell(f);
  int n = 0;
  while(n < ln + 1){
    if(fseek(f, -1, SEEK_CUR) <= -1)
      exit(1);
    if(fgetc(f) == '\n')
      n++;
    if(fseek(f, -1, SEEK_CUR) <= -1)
      exit(1);
    if(len - ftell(f) >= len){
      fseek(f, 0, SEEK_SET);
      break;
    }
  }
}
