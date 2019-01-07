
#ifndef command_h
#define command_h

// Command Data Structure
struct SimpleCommand {
	// Available space for arguments currently preallocated
	int _numberOfAvailableArguments;

	// Number of arguments
	int _numberOfArguments;
	char ** _arguments;
};

void insertArgument(struct SimpleCommand *, char * argument);
struct SimpleCommand *newSimpleCommand();

struct Command {
	int _numberOfAvailableSimpleCommands;
	int _numberOfSimpleCommands;
	struct SimpleCommand ** _simpleCommands;
	char * _outFile;
	char * _inputFile;
	char * _errFile;
	char * _appendOutputFile;
	char * _inputMatchWord;
	char * _doubleErrFile;
	int _background;
};

struct CommandQueue {
	int succesExit;
	int logicAnd;
	int logicOr;
	struct Command *command;
	struct CommandQueue *next;
};

// Static variables 
struct CommandQueue *_commandQueue, *_commandQueueBack;
struct Command *_currentCommand;
struct SimpleCommand *_currentSimpleCommand;

void prompt();
void print(struct Command *_tmp);
void printCommandQueue();
void execute();
void executeCommand(struct Command *, struct CommandQueue *);
void clearCommandQueue();
void clearCommand();
void insertSimpleCommand(struct Command *,struct SimpleCommand * simpleCommand );
struct Command *newCommand();
struct CommandQueue *initializeCommandQueue();

void getTheRightUser();
extern char *userName;
extern char *machineName;


#endif
