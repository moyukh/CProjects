#ifndef command_h
#define command_h
// Command Data Structure

struct SimpleCommand {
	// Available space for arguments currently preallocated
  int _numberOfAvailableArguments;
  
  // Number of arguments
  int _numberOfArguments;
  char ** _arguments;

  SimpleCommand();
  
  void expandWildcardIfNecessary(char *argument);
  void insertArgument(char *argument);
  void expandWildcard(char *prefix, char *suffix);
};

struct Command {
  int _numberOfAvailableSimpleCommands;
  int _numberOfSimpleCommands;
  SimpleCommand ** _simpleCommands;
  char * _outFile;
  char * _inputFile;
  char * _errFile;
  int _background;
  int _append;
  int _amb;
  
  void prompt();
  void print();
  void execute();
  void clear();
  
  Command();
  void insertSimpleCommand( SimpleCommand * simpleCommand );
  static Command _currentCommand;
  static SimpleCommand *_currentSimpleCommand;
};

struct History {
  char *command;
  int num;
  int currentCommand;
  int totalCommands;
  struct History *next;
};

void printEnv();
extern void zombie(int);
extern "C" void ignore(int);
int compare(const void *, const void *);

#endif
