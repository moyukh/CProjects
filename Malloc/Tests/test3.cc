#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread.h>
#include <unistd.h>

int allocations = 1000;

void
allocationThread(void *)
{
  for ( int j = 0; j < 10; j++ ) {
    for ( int i = 1; i < allocations; i++ ) {
      char * p1 = (char *) malloc( i );
      *p1 = 100;
      char * p2 = (char *) malloc( 100 );
      free( p1 );
      free( p2 );
    }
  }
}

extern "C" void atExitHandlerInC();

main()
{
  printf("\n---- Running test3 ---\n");

  int numberOfThreads = 5;
 // int numberOfThreads = 1 ;
  malloc(20);

  thread_t * t = new thread_t[ numberOfThreads ];
  int i;
  for ( i = 0; i < numberOfThreads; i++ ) {
    thr_create( 0, 0, (void *(*)(void *)) allocationThread, 
		(void *) 0, THR_BOUND, &t[ i ] );
  }

  for (i = 0; i < numberOfThreads; i++ ) {
    printf("Wait for %d\n", t[ i ]);
    thr_join( t[ i ], 0, 0 );
  }


  pid_t pid = getpid();
  char command[1024];
  sprintf(command, "pmap %d\n", pid);
  system(command);


  exit( 0 );
}

