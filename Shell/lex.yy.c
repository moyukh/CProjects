#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# ifndef YYLMAX 
# define YYLMAX BUFSIZ
# endif 
#ifndef __cplusplus
# define output(c) (void)putc(c,yyout)
#else
# define lex_output(c) (void)putc(c,yyout)
#endif

#if defined(__cplusplus) || defined(__STDC__)

#if defined(__cplusplus) && defined(__EXTERN_C__)
extern "C" {
#endif
	int yyback(int *, int);
	int yyinput(void);
	int yylook(void);
	void yyoutput(int);
	int yyracc(int);
	int yyreject(void);
	void yyunput(int);
	int yylex(void);
#ifdef YYLEX_E
	void yywoutput(wchar_t);
	wchar_t yywinput(void);
#endif
#ifndef yyless
	int yyless(int);
#endif
#ifndef yywrap
	int yywrap(void);
#endif
#ifdef LEXDEBUG
	void allprint(char);
	void sprint(char *);
#endif
#if defined(__cplusplus) && defined(__EXTERN_C__)
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
	void exit(int);
#ifdef __cplusplus
}
#endif

#endif
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
#ifndef __cplusplus
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#else
# define lex_input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#endif
#define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng;
#define YYISARRAY
char yytext[YYLMAX];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;

# line 2 "shell.l"
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "y.tab.h"

# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
#ifdef __cplusplus
/* to avoid CC and lint complaining yyfussy not being used ...*/
static int __lex_hack = 0;
if (__lex_hack) goto yyfussy;
#endif
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:

# line 20 "shell.l"
	{
		return NEWLINE;
	}
break;
case 2:

# line 24 "shell.l"
	{
		/* Discard spaces and tabs */
     		}
break;
case 3:

# line 28 "shell.l"
{
		return LESS;
	}
break;
case 4:

# line 32 "shell.l"
	{
		return GREAT;
	}
break;
case 5:

# line 36 "shell.l"
{
		return GAMP;
	}
break;
case 6:

# line 40 "shell.l"
   {
                return GG;
        }
break;
case 7:

# line 44 "shell.l"
{
		return GGAMP;
	}
break;
case 8:

# line 48 "shell.l"
{
		return PIPE;
	}
break;
case 9:

# line 52 "shell.l"
{
		return AMP;
	}
break;
case 10:

# line 56 "shell.l"
{
                #define BUFFSIZE 1024
                int infd = dup(0);
                int outfd = dup(1);

                char *command = strtok(yytext, "`");
                int pipeA[2];
                int pipeB[2];
                char buffer[BUFFSIZE];
                int i, r = 0;
                
                if(pipe(pipeA) == -1 || pipe(pipeB) == 1)
                {
                  perror("pipe");
                  exit(1);
                }

                dup2(pipeA[0], 0);
                dup2(pipeB[1], 1);

                close(pipeA[0]);
                close(pipeB[1]);

                r = fork();

                if(r == -1)
                  {
                    perror("fork");
                    exit(1);
                  }

                if(r == 0)
                  {
                    write(pipeA[1], command, strlen(command));
                    write(pipeA[1], "\nexit\n", 6);

	            execl(SHELLPATH, "shell", NULL);
                    perror("execl: subshell error");
                    exit(1);
                  }

                waitpid(r, NULL, NULL);

                dup2(infd, 0);
                dup2(outfd, 1);
                close(infd);
                close(outfd);

                read(pipeB[0], buffer, BUFFSIZE);

                close(pipeB[0]);

                i = (strlen(buffer) - 1);
                //fprintf(stderr, "%d", i);
                while(i >= 0)
                  {
                   if(buffer[i] != '\0')
                   {
                   //fprintf(stderr, "%s", buffer);
                    if(buffer[i] == '\n')
                      {
                        yyunput(32);
                      }
                    else
                      {
                        yyunput(buffer[i]);
                      } 
                    }
                    i--;
                  }


                close(pipeB[0]);
                close(pipeA[1]);
        }
break;
case 11:

# line 132 "shell.l"
{
              yylval.string_val = strdup(yytext);
              return QWORD;
        }
break;
case 12:

# line 137 "shell.l"
 {
		yylval.string_val = strdup(yytext);
		return WORD;
	}
break;
case 13:

# line 142 "shell.l"
{
		/* Invalid character in input */
		return NOTOKEN;
	}
break;
case -1:
break;
default:
(void)fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */
int yyvstop[] = {
0,

12,
13,
0, 

2,
13,
0, 

1,
0, 

12,
13,
0, 

9,
13,
0, 

3,
13,
0, 

4,
13,
0, 

12,
13,
0, 

12,
13,
0, 

8,
13,
0, 

12,
0, 

12,
0, 

12,
0, 

11,
12,
0, 

12,
0, 

5,
0, 

6,
0, 

12,
0, 

12,
0, 

10,
12,
0, 

11,
0, 

7,
0, 

10,
0, 
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
6,15,	0,0,	0,0,	3,13,	
0,0,	10,13,	1,4,	1,5,	
6,16,	6,0,	11,21,	3,0,	
3,0,	10,13,	10,0,	14,0,	
16,16,	15,0,	11,22,	11,0,	
17,0,	13,0,	13,0,	25,0,	
16,16,	16,0,	18,15,	18,0,	
21,0,	27,0,	24,0,	1,6,	
0,0,	6,17,	3,0,	1,7,	
3,13,	6,16,	10,13,	9,19,	
3,0,	20,26,	10,13,	11,21,	
13,0,	0,0,	0,0,	11,22,	
0,0,	16,25,	13,0,	23,21,	
23,0,	16,16,	0,0,	18,15,	
0,0,	1,8,	2,8,	1,9,	
2,9,	0,0,	3,0,	9,20,	
3,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	22,22,	0,0,	
13,0,	0,0,	13,0,	0,0,	
0,0,	0,0,	22,22,	22,0,	
23,21,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,10,	2,10,	6,18,	
0,0,	1,11,	3,14,	6,15,	
10,14,	14,14,	3,13,	15,18,	
10,13,	11,23,	17,18,	22,22,	
13,14,	11,24,	0,0,	22,22,	
0,0,	18,18,	21,23,	16,16,	
24,23,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,12,	2,12,	0,0,	
0,0,	0,0,	3,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	23,23,	0,0,	
13,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	22,27,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-2,	yysvec+1,	0,	
yycrank+-6,	0,		yyvstop+1,
yycrank+0,	0,		yyvstop+4,
yycrank+0,	0,		yyvstop+7,
yycrank+-3,	0,		yyvstop+9,
yycrank+0,	0,		yyvstop+12,
yycrank+0,	0,		yyvstop+15,
yycrank+5,	0,		yyvstop+18,
yycrank+-8,	0,		yyvstop+21,
yycrank+-13,	0,		yyvstop+24,
yycrank+0,	0,		yyvstop+27,
yycrank+-16,	yysvec+3,	yyvstop+30,
yycrank+-9,	yysvec+10,	yyvstop+32,
yycrank+-11,	yysvec+6,	yyvstop+34,
yycrank+-19,	0,		0,	
yycrank+-14,	yysvec+6,	yyvstop+36,
yycrank+-21,	yysvec+6,	yyvstop+39,
yycrank+0,	0,		yyvstop+41,
yycrank+7,	0,		yyvstop+43,
yycrank+-22,	yysvec+11,	yyvstop+45,
yycrank+-73,	0,		0,	
yycrank+-46,	yysvec+11,	yyvstop+47,
yycrank+-24,	yysvec+11,	yyvstop+49,
yycrank+-17,	yysvec+16,	yyvstop+52,
yycrank+0,	0,		yyvstop+54,
yycrank+-23,	yysvec+22,	yyvstop+56,
0,	0,	0};
struct yywork *yytop = yycrank+169;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
  0,   1,   1,   1,   1,   1,   1,   1, 
  1,   9,  10,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  9,   1,  34,   1,   1,   1,  38,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,  38,   1,  38,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
 96,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,  38,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*	Copyright (c) 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#pragma ident	"@(#)ncform	6.12	97/12/08 SMI"

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
#if defined(__cplusplus) || defined(__STDC__)
int yylook(void)
#else
yylook()
#endif
{
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
#ifndef __cplusplus
			*yylastch++ = yych = input();
#else
			*yylastch++ = yych = lex_input();
#endif
#ifdef YYISARRAY
			if(yylastch > &yytext[YYLMAX]) {
				fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
				exit(1);
			}
#else
			if (yylastch >= &yytext[ yytextsz ]) {
				int	x = yylastch - yytext;

				yytextsz += YYTEXTSZINC;
				if (yytext == yy_tbuf) {
				    yytext = (char *) malloc(yytextsz);
				    memcpy(yytext, yy_tbuf, sizeof (yy_tbuf));
				}
				else
				    yytext = (char *) realloc(yytext, yytextsz);
				if (!yytext) {
				    fprintf(yyout,
					"Cannot realloc yytext\n");
				    exit(1);
				}
				yylastch = yytext + x;
			}
#endif
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (uintptr_t)yyt > (uintptr_t)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((uintptr_t)yyt < (uintptr_t)yycrank) {	/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
#ifndef __cplusplus
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
#else
		yyprevious = yytext[0] = lex_input();
		if (yyprevious>0)
			lex_output(yyprevious);
#endif
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
#if defined(__cplusplus) || defined(__STDC__)
int yyback(int *p, int m)
#else
yyback(p, m)
	int *p;
#endif
{
	if (p==0) return(0);
	while (*p) {
		if (*p++ == m)
			return(1);
	}
	return(0);
}
	/* the following are only used in the lex library */
#if defined(__cplusplus) || defined(__STDC__)
int yyinput(void)
#else
yyinput()
#endif
{
#ifndef __cplusplus
	return(input());
#else
	return(lex_input());
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyoutput(int c)
#else
yyoutput(c)
  int c; 
#endif
{
#ifndef __cplusplus
	output(c);
#else
	lex_output(c);
#endif
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyunput(int c)
#else
yyunput(c)
   int c; 
#endif
{
	unput(c);
	}
