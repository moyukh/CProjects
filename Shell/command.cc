#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include "command.h"
#include <regexpr.h>
#include "shell.h"
#include <pwd.h>
#include <list>

extern char **environ;  //grabs invironmental variables
char *SHELLPATH;
#define MAXFILENAME 1024
std::list<char *> history;
std::list<char *>::iterator iter;

void SimpleCommand::expandWildcard(char *prefix, char *suffix)
{
  if(suffix[0] == 0)
    {
      Command::_currentSimpleCommand->insertArgument(strdup(prefix));
      return;
    }

  char *s = strchr(suffix, '/');
  char component[MAXFILENAME];
  char *original = strdup(suffix);
  
  if(s != NULL)
    {
      int length = (strlen(suffix) - strlen(s));
      strncpy(component, suffix, length);
      suffix = s + 1;
      component[length] = '\0';
    }
  else
    {
      strcpy(component, suffix);
      suffix = suffix + strlen(suffix);
    }
  
  char newPrefix[MAXFILENAME];
  
  if(!(strchr(component, '*')) && !(strchr(component, '?')))
    {
      sprintf(newPrefix, "%s%s/", prefix, component);
      expandWildcard(newPrefix, suffix);
      return;
    }

  char *reg = (char *)malloc(2 * strlen(component) + 10);
  char *a = strdup(component);
  char *r = reg;
  
  *r = '^';
  r++;
  int crap = 0;

  while(*a)
    {
      if(*a == '*')
	{
	  if(crap == 0) 
	    {
	      *r = '[';
	      r++;
	      *r = '^';
	      r++;
	      *r = '\\';
	      r++;
	      *r = '.';
	      r++;
	      *r = '*';
	      r++;
	      *r = ']';
	      r++;
	      *r = '.';
	      r++;
	      *r = '*';
	      r++;
	    }
	  else 
	    {
	      *r = '.';
	      r++;
	      *r = '*';
	      r++;
	    }
	}
      else if(*a == '?')
	{
	  *r = '.';
	  r++;
	}
      else if(*a == '.')
	{
	  *r = '\\';
	  r++;
	  *r = '.';
	  r++;
	}
      else
	{
	  *r = *a;
	  r++;
	}
      a++;
      crap = 1;
    }
  *r = '$'; 
  r++;
  *r = 0;

  char *expbuf = compile(reg, 0, 0);
  if(!expbuf)
    {
      perror("compile");
      return;
    }

  DIR *dirp;
  struct dirent *dp;  
  char *path = (char *)malloc(sizeof(char) * 256);
  
  if(prefix[0] == '\0')
    {
      path = ".";
    }
  else
    {
      path = prefix;
    }
  
    if((dirp = opendir(path)) == NULL) //open the directory and start looking
  {
    return;
  }
  while((dp = readdir(dirp)) != NULL)
    {
      if(advance(dp->d_name, expbuf))
	{
	  if(path[0] != '.')
	    {
	      if(prefix[strlen(prefix) - 1] == '/')
		{
		  sprintf(newPrefix, "%s%s", prefix, dp->d_name);
		}
	      else
		{
		  sprintf(newPrefix, "%s/%s", prefix, dp->d_name);
		}
	    }
	  else
	    {
	      sprintf(newPrefix, "%s", dp->d_name);
	    }
	  expandWildcard(newPrefix, suffix);
	}
    }
  closedir(dirp);
}
  
void SimpleCommand::expandWildcardIfNecessary(char *argument)
{
  if((strstr(argument, "*") == NULL) && (strstr(argument, "?") == NULL))
    {
      Command::_currentSimpleCommand->insertArgument(argument); //no wildcarding needed here!
      return;
    }

  char *prefix = (char *)malloc(sizeof(char)*256);
  int temp = _numberOfArguments;
  
  expandWildcard(prefix, argument);

  if(temp != _numberOfArguments)
    {
      char **_wildcard2 = (char **)malloc(sizeof(char *)*_numberOfArguments);
      
      for(int k = 1; k < Command::_currentSimpleCommand->_numberOfArguments; k++)
	{
	  _wildcard2[k - 1] = strdup(Command::_currentSimpleCommand->_arguments[k]);
	}
      
      qsort(_wildcard2, _numberOfArguments - 1, sizeof(char *), compare);
  
      for(int k = 1; k < Command::_currentSimpleCommand->_numberOfArguments; k++)
	{
	  _arguments[k] = strdup(_wildcard2[k - 1]);
	}
      free(_wildcard2);
    }
  else
    {
      Command::_currentSimpleCommand->insertArgument(argument); //no wildcarding needed here!
      return;
    }
}

SimpleCommand::SimpleCommand()
{
  // Creat available space for 5 arguments
  _numberOfAvailableArguments = 5;
  _numberOfArguments = 0;
  _arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void SimpleCommand::insertArgument(char * argument)
{
  if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) 
    {
    // Double the available space
      _numberOfAvailableArguments *= 2;
      _arguments = (char **) realloc( _arguments, _numberOfAvailableArguments * sizeof(char *));
    }
  
  _arguments[ _numberOfArguments ] = argument;
  
  // Add NULL argument at the end
  _arguments[ _numberOfArguments + 1] = NULL;
  
  _numberOfArguments++;
}

Command::Command()
{
  // Create available space for one simple command
  _numberOfAvailableSimpleCommands = 1;
  _simpleCommands = (SimpleCommand **)
    malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );
  
  _numberOfSimpleCommands = 0;
  _outFile = 0;
  _inputFile = 0;
  _errFile = 0;
  _background = 0;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
  if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) 
    {
      _numberOfAvailableSimpleCommands *= 2;
      _simpleCommands = (SimpleCommand **) realloc( _simpleCommands, _numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
    }
  
  _simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
  _numberOfSimpleCommands++;
}

void Command:: clear()
{
  for ( int i = 0; i < _numberOfSimpleCommands; i++ ) 
    {
      for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) 
	{
	  free ( _simpleCommands[ i ]->_arguments[ j ] );
	}
      
      free ( _simpleCommands[ i ]->_arguments );
      free ( _simpleCommands[ i ] );
    }
  
  if ( _outFile ) 
    {
      free( _outFile );
    }
  
  if ( _inputFile ) 
    {
      free( _inputFile );
    }
  
  if ( _errFile ) 
    {
      free( _errFile );
    }
  
  _numberOfSimpleCommands = 0;
  _outFile = 0;
  _inputFile = 0;
  _errFile = 0;
  _background = 0;
}

void Command::print()
{
  printf("\n\n");
  printf("              COMMAND TABLE                \n");
  printf("\n");
  printf("  #   Simple Commands\n");
  printf("  --- ----------------------------------------------------------\n");
  
  for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
    printf("  %-3d ", i );
    for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
      printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
    }
    printf("\n");
  }
  
  printf( "\n\n" );
  printf( "  Output       Input        Error        Background\n" );
  printf( "  ------------ ------------ ------------ ------------\n" );
  printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
	  _inputFile?_inputFile:"default", _errFile?_errFile:"default",
	  _background?"YES":"NO");
  printf( "\n\n" );
  
}

void Command::execute()
{
  int pid; //RETURNED STATUS OF FORK()
  int outfd; //TEMP OUT FILE DESCRIPTOR
  int infd; //TEMP IN FILE DESCRIPTOR
  int errfd; //TEMP ERR FILE DESCRIPTOR
  int defaultin = dup(0); //INPUT: DEFAULTIN
  int defaultout = dup(1); //OUTPUT: DEFAULTOUT
  int defaulterr = dup(2); //ERROR: DEFAULTERR
  
  if(!strcmp(_simpleCommands[0]->_arguments[0],"exit")) //FINISHED: EXIT
    {
      exit(0);
    }

  if(!strcmp(_simpleCommands[0]->_arguments[0], "cd")) //FINISHED: CD
    {
      int status;
      char *path;
      if(_simpleCommands[0]->_arguments[1] == NULL)
	{
	  path = getenv("HOME");
	  status = chdir(path);
	  clear();
	  prompt();
	  
	  return;
	}
      
      status = chdir(_simpleCommands[0]->_arguments[1]);
      
      if(status != 0)
	{
	  printf("%s: No such file or directory.\n", _simpleCommands[0]->_arguments[1]);
	}
      
      clear();
      prompt();
      
      return;
      }
  
  /*if(strstr(_simpleCommands[0]->_arguments[0], "!")) //BANG COMPLETION
    {
      char *command = strtok(_simpleCommands[0]->_arguments[0], "!");
      for(iter = history.begin(); iter != history.end(); iter++)
	{
	  if(strchr(command[0], *(iter)))
	    {
	      break;
	      }
	}

      clear();
      prompt();
      return;
    }*/

  if(!strcmp(_simpleCommands[0]->_arguments[0], "setenv")) //FINISHED: SETENV
    {
      int status;
      char *name; 
      char *value;
      char *path;

      if((_simpleCommands[0]->_arguments[1] == NULL) || (_simpleCommands[0]->_arguments[1] == NULL))
	{
	  perror("setenv requires two arguments.");
	}

      name = strdup(_simpleCommands[0]->_arguments[1]);
      value = strdup(_simpleCommands[0]->_arguments[2]);
      path = strcat(name, "=");
      path = strcat(path, value);
            
      if((status = putenv(path)) != 0)
	{
	  perror("setenv out of memory");
	  exit(1);
	}
 
      clear();
      prompt();

      return;
    }
  
  if(!strcmp(_simpleCommands[0]->_arguments[0], "unsetenv")) //FINISHED: UNSETENV
    {
      char **p = environ;
      int i = 0;

      if(_simpleCommands[0]->_arguments[1] == NULL)
	{
	  perror("unsetenv requires an argument.");
	  exit(1);
	}
      

      char *path = strdup(_simpleCommands[0]->_arguments[1]);
      path = strcat(path, "=");
      
      while((*p != NULL) && (!strstr(*p, path)))
	{
	  p++;
	}
      
      if(*p)
        {
          while(p[i] != NULL)
      	    { 
	      p[i] = p[i + 1];
  	      p++;
	    }
	  
	  clear();
	  prompt();

	  return;
	}

      return;
    }
  
  // Don't do anything if there are no simple commands
  if ( _numberOfSimpleCommands == 0 ) {
    prompt();
    return;
  }

  /*if(!strcmp(_simpleCommands[0]->_arguments[0], "history")) //FINISHED: HISTORY
    {
      if(history.size() == 0)
	{
	  printf("\n");
	}
      else
	{
	  int g = 1;
	  for(iter = history.begin(); iter != history.end(); iter++)
	    {
	      printf("\t%d.\t%s\n", g, (*iter));
	      g++;
	    }
	}
      history.push_back(strdup("history"));
      clear();
      prompt();
      return;
    }*/
  
  if(_amb)
    {
      perror("Ambiguous output redirect\n");
      exit(2);
    }
  
  // Print contents of Command data structure
  //print();

  if(_inputFile != 0) //redirect input
    {
      infd = open(_inputFile, O_RDONLY, 0664); //REDIRECTING READ FROM FILE
      
      if(infd < 0)
	{
	  printf("%s: No such file or directory", _inputFile);
	}
    }
  else //use default input
    {
      infd = dup(defaultin);
    }
  
  /*if(history.size() > 100)
    {
      while(history.size() > 100)
	{
	  history.pop_front();
	}
    }
  
  char *shiz = (char *)malloc(sizeof(char)*256);
  for(int l = 0; l < _simpleCommands[0]->_numberOfArguments; l++) //FINISHED: ADDING HISTORY
    {
      sprintf(shiz, "%s %s", shiz, _simpleCommands[0]->_arguments[l]);
    }
  history.push_back(strdup(shiz));*/
  
  for(int i = 0; i < _numberOfSimpleCommands; i++) //for each command, run execvp, getting the fork()
    {
      char *final = (char *)malloc(sizeof(char)*1000);
      bool found = false;

      if(!(strcmp(_simpleCommands[i]->_arguments[0], "echo"))) //FINISHED: ENVIRON VARIABLE EXPANSION
	{
	  for(int c = 1; c < _simpleCommands[i]->_numberOfArguments; c++)
	    {
	      char *rough = (char *)malloc(sizeof(char)*1000);
	      char *weakSauce = rough;

	      if((strstr(_simpleCommands[i]->_arguments[c], "${")))
		{
		  char *search = strdup(_simpleCommands[i]->_arguments[c]);

		  while(*search)
		    {
		      if(*search == '$')
			{
			  search += 2;
			  while(*search != '}')
			    {
			      *rough = *search;
			      rough++; 
			      search++;
			    }

			  char *envExpanded = getenv(weakSauce);
			  weakSauce += strlen(envExpanded);
			  rough = strcat(rough, envExpanded);
			  final = strdup(strcat(final, rough));
			  search++;
			}
		      else
			{
			  sprintf(final, "%s%c", final, *search);
			  rough++;
			  search++;
			}
		    }
		  found = true;
		  //_simpleCommands[i]->_arguments[c] = strdup(final);
		}
	      if(found)
		{
		  _simpleCommands[i]->_arguments[c] = strdup(final);
		  final = (char *)malloc(sizeof(char)*1000);
		  found = false;
		}
	    }
	}
      
      char *path = (char *)malloc(sizeof(char)*1000);
      bool tildoOnly = false;
      bool tildoSlash = false;
      struct passwd *user;
      
      if((_simpleCommands[i]->_arguments[1] != NULL) && strstr(_simpleCommands[i]->_arguments[1], "~")) //FINISHED: TILDE EXPANSION
	{  
	  for(int j = 1; j < _simpleCommands[i]->_numberOfArguments; j++)
	    {
	      if((strstr(_simpleCommands[i]->_arguments[j], "~/")))
		{
		  tildoSlash = true;
		}
	      if(!(strcmp(_simpleCommands[i]->_arguments[j], "~")) && (_simpleCommands[i]->_arguments[j][1] == '\0'))
		{
		  tildoOnly = true;
		}
	      
	      if(tildoOnly)
		{
		  user = getpwnam(getenv("USER"));
		  _simpleCommands[i]->_arguments[j] = strdup(user->pw_dir);
		}
	      else if(tildoSlash)
		{
		  char *extra = strdup(strpbrk(_simpleCommands[i]->_arguments[j], "/"));
		  user = getpwnam(getenv("USER"));
		  path = strcat(path, user->pw_dir);
		  path = strcat(path, extra);
		  _simpleCommands[i]->_arguments[j] = strdup(path);
		}
	      else
		{
		  if(strstr(_simpleCommands[i]->_arguments[j], "/"))
		    {
		      char *extra = strdup(strpbrk(_simpleCommands[i]->_arguments[j], "/"));
		      char *temp = strtok(_simpleCommands[i]->_arguments[j], "/");
		      temp = strtok(_simpleCommands[i]->_arguments[j], "~");
		      user = getpwnam(temp);

		      path = strcat(path, user->pw_dir);
		      path = strcat(path, extra);
		      _simpleCommands[i]->_arguments[j] = strdup(path);
		    }
		  else
		    {
		      char *temp = strtok(_simpleCommands[i]->_arguments[j], "~/");
		      user = getpwnam(temp);
		      path = strdup(_simpleCommands[i]->_arguments[j]);
		      _simpleCommands[i]->_arguments[j] = strdup(user->pw_dir);
		      
		    }
		}
	      
	    }
	}
      
      dup2(infd, 0);
      close(infd);
      
      if(i == _numberOfSimpleCommands - 1)
	{
	  if(_outFile != 0)
	    {
	      if(_append == 1)   
		{ 
		  outfd = open(_outFile, O_CREAT | O_APPEND | O_WRONLY, 0664);
		}
	      else
		{
		  outfd = open(_outFile, O_CREAT | O_WRONLY | O_TRUNC, 0664); //REDIRECTING OUTPUT TO FILE
		}
	    }
	  else
	    {
	      outfd = dup(defaultout);
	    }
	  if(_errFile != 0)
	    {
	      if(_append == 1)
		{
		  errfd = open(_errFile, O_CREAT | O_APPEND | O_WRONLY, 0664); //REDIRECTING ERR TO FILE
		} 
	      else
		{
		  errfd = open(_errFile, O_CREAT | O_APPEND | O_WRONLY, 0664);
		}
	      dup2(errfd, 2);
     	    }
	}
      else
	{
	  int fdpipe[2];

	  if(pipe(fdpipe) == -1)
	    {
	      perror("Pipe Error");
	      exit(2);
	    }

	  infd = fdpipe[0];
	  outfd = fdpipe[1];  	  
       	}

      dup2(outfd, 1);
      close(outfd);

      pid = fork();
      
      if(pid == 0)
	{
	  if(!strcmp(_simpleCommands[i]->_arguments[0], "printenv"))
	    {
	      printEnv();
	    }
	  
	  execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments); //execvp for each argumennt
	  perror("execvp");
	  exit(1);
	}
      
      else if(pid < 0) //error out
	{
	  perror("fork");
	  exit(2);
	}
    } 
  
  if(!_background) //wait for the shell to complete command
    {
      waitpid(pid, NULL, 0);	
    }

  dup2(defaultin, 0);
  dup2(defaultout, 1);
  dup2(defaulterr, 2);
  
  // Clear to prepare for next command
  clear();
  
  // Print new prompt
  prompt();
}

// Shell implementation

void Command::prompt()
{
  if(isatty(0))
    {
      printf("myshell>");
      fflush(stdout);
    }
  fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

main(int argc, char **argv)
{
  SHELLPATH = (char *)malloc(sizeof(char)*strlen(argv[0]));
  SHELLPATH = strcpy(SHELLPATH, argv[0]);
  
  struct sigaction signalAction;
  signalAction.sa_handler = ignore;
  sigemptyset(&signalAction.sa_mask);
  signalAction.sa_flags = SA_RESTART;

  int error = sigaction(SIGINT, &signalAction, NULL);
  if (error) 
    {
      perror("sigaction");
      exit(-1);
    }

  struct sigaction signalAction2;
  signalAction2.sa_handler = zombie;
  sigemptyset(&signalAction2.sa_mask);
  signalAction2.sa_flags = SA_NOCLDWAIT;

  int error2 = sigaction(SIGCHLD, &signalAction2, NULL);
  if (error2) 
    {
      perror("sigaction2");
      exit(-1);
    }
  
  Command::_currentCommand.prompt();
  yyparse();
}

extern void zombie(int sig) //FINISHED: ZOMBIES
{
  int pid = wait3(&pid, WNOHANG, NULL);

  printf("%d exited.", pid);
}

extern "C" void ignore(int sig) //FINISHED: CTRL-C IGNORE
{
  fprintf( stderr, "\n");
  Command::_currentCommand.clear();
  Command::_currentCommand.prompt();
}

void printEnv() //FINISHED: PRINTENV
{
  char **p = environ;
  
  while(*p)
    {
      printf("%s\n", *p);
      p++;
    }

  exit(0);
}

int compare(const void *a, const void *b) //FINISHED: QSORT
{
  char *sa = *(char **)a;
  char *sb = *(char **)b;

  return strcmp(sa, sb);
}
