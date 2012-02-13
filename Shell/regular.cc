
#include <regexpr.h>
#include <stdio.h>
#include <stdlib.h>

const char * usage = ""
"Usage:\n"
"      regular regular-expresion string\n"
"\n"
"      Tells if \"string\" matches \"regular-expresion\".\n"
"\n"
"      '^' and '$' characters are added at the beginning and\n"
"      end of \"regular-expresion\" to force match of the entire\n"
"      \"string\"\n"
"\n"
"	To know more about regular expresions type \"man ed\"\n"
"Try:\n"
"      csh> regular aaa aaa\n"
"      csh> regular \"a*\" aaaa\n"
"      csh> regular \"a*\" aaa\n"
"      csh> regular \"a*\" aaaf\n"
"      csh> regular \"a.*\" akjhkljh \n"
"      csh> regular \"a.*\" jkjhkj\n"
"      csh> regular \"a.*\" aaalklkjlk\n"
"      csh> regular \".*\\..*\" kljhkljhlj.lkjhlkj\n"
"      csh> regular \".*\\..*\" kljhkljhlj\n\n";

int
main(int argc, char ** argv)
{
	char * expbuf;
	int result;
	const char * match;
	char buf[ 1024 ];

	if ( argc < 3 ) {
		fprintf( stderr, "%s", usage );
		exit( -1 );
	}

	/*
	 *  Add ^ and $ at the beginning and end of regular expression
	 *  to force match of the entire string. See "man ed".
	 */
	sprintf( buf, "^%s$", argv[ 1 ] );
	
	expbuf = compile( buf, 0, 0 );
	if( expbuf == 0 ) {
		fprintf( stderr, "%s: Bad regular expresion \"%s\"\n",
			 argv[ 0 ], buf );
		exit( -1 );
      	}

	result = advance( argv[ 2 ], expbuf );

	match = "MATCHES";
	if ( result == 0 ) {
		match = "DOES NOT MATCH";
	}

	fprintf( stderr, "\t\"%s\" %s \"%s\"\n", buf, match, argv[ 2 ] );

	return 0;
}

