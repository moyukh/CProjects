
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token  <string_val> QWORD

%token 	NOTOKEN, GREAT, NEWLINE, LESS, GAMP, GG, GGAMP, PIPE, AMP

%union	{
		char   *string_val;
	}

%{
extern "C" int yylex();
#define yylex yylex
#include <stdio.h>
#include "command.h"
%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command 
	;

command: simple_command
        ;

simple_command:	
	command_and_args pipe_list iomodifier_list background_opt NEWLINE {
		//printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	
	| NEWLINE
	| error NEWLINE { yyerrok; }
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

pipe_list:
	pipe_list PIPE command_and_args
	| /*can be empty*/
	;

background_opt:
	AMP {
	  Command::_currentCommand._background = 1; //set background task
	}
	| /*can be empty*/
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
	  char *tok;
	  if(!strstr("`", $1))
	    {
	      tok = strtok($1, "`");
	      while(*tok)
		{
		  tok = (tok + 1);
		  tok++;
		}
	      printf("%s", tok);

	    }
	  for(int i = 0; i < strlen($1) - 1; i++)
	    {
	      if($1[i] == '\\')
		{
		  int j;
		  for(j = i; j < strlen($1) - 1; j++)
		    {
		      $1[j] = $1[j + 1];
		    }
		  $1[j] = '\0';
		}
	    }
	  Command::_currentSimpleCommand->expandWildcardIfNecessary(strdup($1));
	}
        | QWORD {
	  $1++;
          $1[strlen($1) - 1] = '\0';
	  Command::_currentSimpleCommand->expandWildcardIfNecessary(strdup($1));
	}
	;
iomodifier_list:
	iomodifier iomodifier_list
	| /*can be empty*/
	;

iomodifier:	/*different types of IO modifiers*/
	iomodifier_great
	| iomodifier_less
	| iomodifier_gamp
	| iomodifier_gg
	| iomodifier_ggamp
	;

command_word:
	WORD {
	  Command::_currentSimpleCommand = new SimpleCommand();
	  Command::_currentSimpleCommand->insertArgument( $1 );
	}
        ;

iomodifier_great:
	GREAT WORD {
	  if(Command::_currentCommand._outFile == NULL)
	    {
	      Command::_currentCommand._outFile = $2;
	    }
	  else
 	    {
	      Command::_currentCommand._amb = 1;
            }
	} 
        ;

iomodifier_less:
	LESS WORD {
	  if(Command::_currentCommand._inputFile == NULL)
	    {
	      Command::_currentCommand._inputFile = $2;
	    }
	  else
	    {
	      Command::_currentCommand._amb = 1;
	    }
	}
        ;

iomodifier_gamp:
	GAMP WORD {
	  Command::_currentCommand._outFile = $2;
	  Command::_currentCommand._errFile = $2;
	}
	;

iomodifier_gg:
	GG WORD {
	  Command::_currentCommand._outFile = $2;
	  Command::_currentCommand._append = 1;
	    
	}
	;

iomodifier_ggamp:
	GGAMP WORD {
	  Command::_currentCommand._outFile = $2;
	  Command::_currentCommand._errFile = $2;
	  Command::_currentCommand._append = 1;
	}
	;

%%

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
