#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

int
main( int argc, char **argv )
{
  unsigned long gb2 = 0x7FFFFFE0;
  unsigned long gb4 = 0xFFFFFFFF;
  char* ret = (char*)malloc(gb4);
  assert (ret == (char*)0);
  ret = (char*)malloc(gb2);
  assert (ret != (char*)0);

  pid_t pid = getpid();
  char command[1024];
  sprintf(command, "pmap %d\n", pid);
  system(command);

  exit(0);
}
