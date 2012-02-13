#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

int
main( int argc, char **argv )
{
  unsigned long sixteenk = 16*1024;
  char* one = (char*) malloc(sixteenk - 32);
  free(one);
  char* two = (char*)sbrk(sixteenk);
  char* three = (char*)malloc(sixteenk * 2 - 64);
  assert (three > two + sixteenk);

  pid_t pid = getpid();
  char command[1024];
  sprintf(command, "pmap %d\n", pid);
  system(command);
  
  return 0;
}
