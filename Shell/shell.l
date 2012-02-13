/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "y.tab.h"

%}

%%

\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     		}

"<"	{
		return LESS;
	}

">" 	{
		return GREAT;
	}

">&"	{
		return GAMP;
	}

">>"    {
                return GG;
        }

">>&"	{
		return GGAMP;
	}

"|"	{
		return PIPE;
	}

"&"	{
		return AMP;
	}

[`].*[`] {
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

["].*["] {
              yylval.string_val = strdup(yytext);
              return QWORD;
        }

([^ \t\n<>|&]|\\.)([^ \t\n<>|&]|\\.)*  {
		yylval.string_val = strdup(yytext);
		return WORD;
	}

.	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%