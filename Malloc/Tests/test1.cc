#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int
main( int argc, char **argv )
{
  printf("\n---- Running test1 ---\n");

  int * p = (int *) malloc( 20 );
  *p = 10;
  free( p );

  p = (int *) malloc( 20 );
  *p = 10;
  free( p );

  pid_t pid = getpid();
  char command[1024];
  sprintf(command, "pmap %d\n", pid);
  system(command);

  exit(0);
}
