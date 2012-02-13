
#include <stdlib.h>
#include <stdio.h>

int
main( int argc, char **argv )
{
  printf("\n---- Running test1 ---\n");

  int * p = (int *) malloc( 20 );
  *p = 10;
  printf("p1 = %d\n", p);
  free( p );

  p = (int *) malloc( 20 );
  if (p != NULL)
    {
  *p = 10;
  printf("p2 = %d\n", p);
    }
  else
    {
      printf("fail");
    }
  free( p );
  
  printf(">>>> test1 passed\n\n");

  exit(0);
}
