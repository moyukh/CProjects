
#include <assert.h>
#include <string.h>
#include <stdlib.h>

char * mystrcpy( char * dest, const char * src );
int mystrcmp( const char * a, const char * b );
char * mystrstr( const char * dest, const char * src );
char * mystrdup( const char * s1);

int main()
{
  /* test mystrcpy */
  char s1[30];
  char * b;
  int i, j;
  char *s2[100];

//  printf("test1 start\n");
  b = mystrcpy( s1, "Hello World" );
 // printf("test1 end\n");
  assert( strcmp( s1, "Hello World" ) == 0 );
  assert( b == s1 );
  
  b = mystrcpy( s1, "Hi!" );
  assert( strcmp( s1, "Hi!" ) == 0 );
  assert( b == s1 );
  
  /* test mystrcmp */

#define SIGN(a) (((a)>0)?1:((a)==0)?0:-1)

  assert( SIGN(mystrcmp( "hello", "hello")) == SIGN(strcmp( "hello", "hello")) );
  assert( SIGN(mystrcmp( "hello", "he")) == SIGN(strcmp( "hello", "he")) );
  assert( SIGN(mystrcmp( "he", "hello")) == SIGN(strcmp( "he", "hello")) );
  assert( SIGN(mystrcmp( "seven", "apple")) == SIGN(strcmp( "seven", "apple")) );
  assert( SIGN(mystrcmp( "apple", "seven")) == SIGN(strcmp( "apple", "seven")) );
  assert( SIGN(mystrcmp( "first", "second")) == SIGN(strcmp( "first", "second")) );
  assert( SIGN(mystrcmp( "second", "first")) == SIGN(strcmp( "second", "first")) );

  /* test mystrstr */
  b = "Hello world, Hello world.";
  assert( mystrstr( b, "ld") == strstr( b, "ld") );
  assert( mystrstr( b, "He") == strstr( b, "He") );
  assert( mystrstr( b, "d.") == strstr( b, "d.") );
  assert( mystrstr( b, "jj") == strstr( b, "jj") );
  assert( mystrstr( b, "lloo") == strstr( b, "lloo") );

  /* test mystrdup */
  printf("test mystrdup\n");
  for ( j = 0; j < 100; j++ ) {
    for ( i = 0; i < 100; i++ ) {
      const char * hello = "Hello world hello world hello";
      s2[ i ] = mystrdup( hello );
      assert( s2[ i ] != NULL );
      assert( s2[ i ] != hello );
      assert( strcmp( s2[ i ], hello ) == 0 );
    }
    for ( i = 0; i < 100; i++ ) {
      free( s2[ i ] );
    }
  }
  
  printf("%s\n", mystrdup("nigg"));
  
  printf(">>> test_mystring succeeded...\n");

  exit(0);
}

