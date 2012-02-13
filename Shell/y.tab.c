
# line 20 "shell.y"
typedef union
#ifdef __cplusplus
	YYSTYPE
#endif
	{
		char   *string_val;
	} YYSTYPE;
# define WORD 257
# define QWORD 258
# define NOTOKEN 259
# define GREAT 260
# define NEWLINE 261
# define LESS 262
# define GAMP 263
# define GG 264
# define GGAMP 265
# define PIPE 266
# define AMP 267

# line 25 "shell.y"
extern "C" int yylex();
#define yylex yylex
#include <stdio.h>
#include "command.h"

#include <inttypes.h>

#ifdef __STDC__
#include <stdlib.h>
#include <string.h>
#define	YYCONST	const
#else
#include <malloc.h>
#include <memory.h>
#define	YYCONST
#endif

#include <values.h>

#if defined(__cplusplus) || defined(__STDC__)

#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
#ifndef yyerror
#if defined(__cplusplus)
	void yyerror(YYCONST char *);
#endif
#endif
#ifndef yylex
	int yylex(void);
#endif
	int yyparse(void);
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif

#endif

#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
YYSTYPE yylval;
YYSTYPE yyval;
typedef int yytabelem;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#if YYMAXDEPTH > 0
int yy_yys[YYMAXDEPTH], *yys = yy_yys;
YYSTYPE yy_yyv[YYMAXDEPTH], *yyv = yy_yyv;
#else	/* user does initial allocation */
int *yys;
YYSTYPE *yyv;
#endif
static int yymaxdepth = YYMAXDEPTH;
# define YYERRCODE 256

# line 181 "shell.y"


void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();

 	
}
#endif
static YYCONST yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 2,
	0, 1,
	-2, 0,
	};
# define YYNPROD 30
# define YYLAST 45
static YYCONST yytabelem yyact[]={

    22,    31,    23,    24,    25,    26,    15,    22,    38,    23,
    24,    25,    26,     7,     9,    39,    12,    37,     6,    28,
    29,    36,    35,    34,     9,    14,     3,     5,    21,    10,
    20,    19,    18,    17,    16,    27,    13,     8,    30,    11,
     4,     2,    33,    32,     1 };
static YYCONST yytabelem yypact[]={

  -243,-10000000,  -243,-10000000,-10000000,-10000000,-10000000,  -245,-10000000,-10000000,
-10000000,  -260,-10000000,  -238,  -266,  -233,  -253,-10000000,-10000000,-10000000,
-10000000,-10000000,  -234,  -235,  -236,  -240,  -249,-10000000,-10000000,-10000000,
  -246,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000,-10000000 };
static YYCONST yytabelem yypgo[]={

     0,    44,    41,    26,    40,    27,    39,    25,    38,    37,
    36,    35,    34,    33,    32,    31,    30,    28 };
static YYCONST yytabelem yyr1[]={

     0,     1,     2,     2,     3,     4,     4,     4,     5,     6,
     6,     8,     8,    10,    10,    11,    11,     7,     7,    12,
    12,    12,    12,    12,     9,    13,    14,    15,    16,    17 };
static YYCONST yytabelem yyr2[]={

     0,     2,     2,     4,     2,    11,     2,     5,     5,     6,
     0,     3,     0,     4,     0,     3,     3,     4,     0,     2,
     2,     2,     2,     2,     3,     5,     5,     5,     5,     5 };
static YYCONST yytabelem yychk[]={

-10000000,    -1,    -2,    -3,    -4,    -5,   261,   256,    -9,   257,
    -3,    -6,   261,   -10,    -7,   266,   -12,   -13,   -14,   -15,
   -16,   -17,   260,   262,   263,   264,   265,   -11,   257,   258,
    -8,   267,    -5,    -7,   257,   257,   257,   257,   257,   261 };
static YYCONST yytabelem yydef[]={

     0,    -2,    -2,     2,     4,    10,     6,     0,    14,    24,
     3,    18,     7,     8,    12,     0,    18,    19,    20,    21,
    22,    23,     0,     0,     0,     0,     0,    13,    15,    16,
     0,    11,     9,    17,    25,    26,    27,    28,    29,     5 };
typedef struct
#ifdef __cplusplus
	yytoktype
#endif
{ char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"WORD",	257,
	"QWORD",	258,
	"NOTOKEN",	259,
	"GREAT",	260,
	"NEWLINE",	261,
	"LESS",	262,
	"GAMP",	263,
	"GG",	264,
	"GGAMP",	265,
	"PIPE",	266,
	"AMP",	267,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"goal : commands",
	"commands : command",
	"commands : commands command",
	"command : simple_command",
	"simple_command : command_and_args pipe_list iomodifier_list background_opt NEWLINE",
	"simple_command : NEWLINE",
	"simple_command : error NEWLINE",
	"command_and_args : command_word arg_list",
	"pipe_list : pipe_list PIPE command_and_args",
	"pipe_list : /* empty */",
	"background_opt : AMP",
	"background_opt : /* empty */",
	"arg_list : arg_list argument",
	"arg_list : /* empty */",
	"argument : WORD",
	"argument : QWORD",
	"iomodifier_list : iomodifier iomodifier_list",
	"iomodifier_list : /* empty */",
	"iomodifier : iomodifier_great",
	"iomodifier : iomodifier_less",
	"iomodifier : iomodifier_gamp",
	"iomodifier : iomodifier_gg",
	"iomodifier : iomodifier_ggamp",
	"command_word : WORD",
	"iomodifier_great : GREAT WORD",
	"iomodifier_less : LESS WORD",
	"iomodifier_gamp : GAMP WORD",
	"iomodifier_gg : GG WORD",
	"iomodifier_ggamp : GGAMP WORD",
};
#endif /* YYDEBUG */
# line	1 "/usr/ccs/bin/yaccpar"
/*
 * Copyright (c) 1993 by Sun Microsystems, Inc.
 */

#pragma ident	"@(#)yaccpar	6.16	99/01/20 SMI"

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#define YYNEW(type)	malloc(sizeof(type) * yynewmax)
#define YYCOPY(to, from, type) \
	(type *) memcpy(to, (char *) from, yymaxdepth * sizeof (type))
#define YYENLARGE( from, type) \
	(type *) realloc((char *) from, yynewmax * sizeof(type))
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-10000000)

/*
** global variables used by the parser
*/
YYSTYPE *yypv;			/* top of value stack */
int *yyps;			/* top of state stack */

int yystate;			/* current state */
int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */



#ifdef YYNMBCHARS
#define YYLEX()		yycvtok(yylex())
/*
** yycvtok - return a token if i is a wchar_t value that exceeds 255.
**	If i<255, i itself is the token.  If i>255 but the neither 
**	of the 30th or 31st bit is on, i is already a token.
*/
#if defined(__STDC__) || defined(__cplusplus)
int yycvtok(int i)
#else
int yycvtok(i) int i;
#endif
{
	int first = 0;
	int last = YYNMBCHARS - 1;
	int mid;
	wchar_t j;

	if(i&0x60000000){/*Must convert to a token. */
		if( yymbchars[last].character < i ){
			return i;/*Giving up*/
		}
		while ((last>=first)&&(first>=0)) {/*Binary search loop*/
			mid = (first+last)/2;
			j = yymbchars[mid].character;
			if( j==i ){/*Found*/ 
				return yymbchars[mid].tvalue;
			}else if( j<i ){
				first = mid + 1;
			}else{
				last = mid -1;
			}
		}
		/*No entry in the table.*/
		return i;/* Giving up.*/
	}else{/* i is already a token. */
		return i;
	}
}
#else/*!YYNMBCHARS*/
#define YYLEX()		yylex()
#endif/*!YYNMBCHARS*/

/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
#if defined(__STDC__) || defined(__cplusplus)
int yyparse(void)
#else
int yyparse()
#endif
{
	register YYSTYPE *yypvt = 0;	/* top of value stack for $vars */

#if defined(__cplusplus) || defined(lint)
/*
	hacks to please C++ and lint - goto's inside
	switch should never be executed
*/
	static int __yaccpar_lint_hack__ = 0;
	switch (__yaccpar_lint_hack__)
	{
		case 1: goto yyerrlab;
		case 2: goto yynewstate;
	}
#endif

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

#if YYMAXDEPTH <= 0
	if (yymaxdepth <= 0)
	{
		if ((yymaxdepth = YYEXPAND(0)) <= 0)
		{
			yyerror("yacc initialization error");
			YYABORT;
		}
	}
#endif

	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */
	goto yystack;	/* moved from 6 lines above to here to please C++ */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			long yyps_index = (yy_ps - yys);
			long yypv_index = (yy_pv - yyv);
			long yypvt_index = (yypvt - yyv);
			int yynewmax;
#ifdef YYEXPAND
			yynewmax = YYEXPAND(yymaxdepth);
#else
			yynewmax = 2 * yymaxdepth;	/* double table size */
			if (yymaxdepth == YYMAXDEPTH)	/* first time growth */
			{
				char *newyys = (char *)YYNEW(int);
				char *newyyv = (char *)YYNEW(YYSTYPE);
				if (newyys != 0 && newyyv != 0)
				{
					yys = YYCOPY(newyys, yys, int);
					yyv = YYCOPY(newyyv, yyv, YYSTYPE);
				}
				else
					yynewmax = 0;	/* failed */
			}
			else				/* not first time */
			{
				yys = YYENLARGE(yys, int);
				yyv = YYENLARGE(yyv, YYSTYPE);
				if (yys == 0 || yyv == 0)
					yynewmax = 0;	/* failed */
			}
#endif
			if (yynewmax <= yymaxdepth)	/* tables not expanded */
			{
				yyerror( "yacc stack overflow" );
				YYABORT;
			}
			yymaxdepth = yynewmax;

			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = YYLEX() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register YYCONST int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
			skip_init:
				yynerrs++;
				/* FALLTHRU */
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 5:
# line 46 "shell.y"
{
		//printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	} break;
case 7:
# line 52 "shell.y"
{ yyerrok; } break;
case 8:
# line 56 "shell.y"
{
		Command::_currentCommand.insertSimpleCommand( Command::_currentSimpleCommand );
	} break;
case 11:
# line 67 "shell.y"
{
	  Command::_currentCommand._background = 1; //set background task
	} break;
case 15:
# line 79 "shell.y"
{
	  char *tok;
	  if(!strstr("`", yypvt[-0].string_val))
	    {
	      tok = strtok(yypvt[-0].string_val, "`");
	      while(*tok)
		{
		  tok = (tok + 1);
		  tok++;
		}
	      printf("%s", tok);

	    }
	  for(int i = 0; i < strlen(yypvt[-0].string_val) - 1; i++)
	    {
	      if(yypvt[-0].string_val[i] == '\\')
		{
		  int j;
		  for(j = i; j < strlen(yypvt[-0].string_val) - 1; j++)
		    {
		      yypvt[-0].string_val[j] = yypvt[-0].string_val[j + 1];
		    }
		  yypvt[-0].string_val[j] = '\0';
		}
	    }
	  Command::_currentSimpleCommand->expandWildcardIfNecessary(strdup(yypvt[-0].string_val));
	} break;
case 16:
# line 106 "shell.y"
{
	  yypvt[-0].string_val++;
          yypvt[-0].string_val[strlen(yypvt[-0].string_val) - 1] = '\0';
	  Command::_currentSimpleCommand->expandWildcardIfNecessary(strdup(yypvt[-0].string_val));
	} break;
case 24:
# line 126 "shell.y"
{
	  Command::_currentSimpleCommand = new SimpleCommand();
	  Command::_currentSimpleCommand->insertArgument( yypvt[-0].string_val );
	} break;
case 25:
# line 133 "shell.y"
{
	  if(Command::_currentCommand._outFile == NULL)
	    {
	      Command::_currentCommand._outFile = yypvt[-0].string_val;
	    }
	  else
 	    {
	      Command::_currentCommand._amb = 1;
            }
	} break;
case 26:
# line 146 "shell.y"
{
	  if(Command::_currentCommand._inputFile == NULL)
	    {
	      Command::_currentCommand._inputFile = yypvt[-0].string_val;
	    }
	  else
	    {
	      Command::_currentCommand._amb = 1;
	    }
	} break;
case 27:
# line 159 "shell.y"
{
	  Command::_currentCommand._outFile = yypvt[-0].string_val;
	  Command::_currentCommand._errFile = yypvt[-0].string_val;
	} break;
case 28:
# line 166 "shell.y"
{
	  Command::_currentCommand._outFile = yypvt[-0].string_val;
	  Command::_currentCommand._append = 1;
	    
	} break;
case 29:
# line 174 "shell.y"
{
	  Command::_currentCommand._outFile = yypvt[-0].string_val;
	  Command::_currentCommand._errFile = yypvt[-0].string_val;
	  Command::_currentCommand._append = 1;
	} break;
# line	531 "/usr/ccs/bin/yaccpar"
	}
	goto yystack;		/* reset registers in driver code */
}

