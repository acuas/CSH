
#ifndef command_h
#define command_h

// Command Data Structure
struct SimpleCommand {
	// Available space for arguments currently preallocated
	int _numberOfAvailableArguments;

	// Number of arguments
	int _numberOfArguments;
	char ** _arguments;
	
	void (*insertArgument) (struct SimpleCommand *, char *);
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
	int _background;

	void (*prompt) ();
	void (*print) (struct Command *);
	void (*execute) (struct Command *);
	void (*clear) (struct Command *);
	void (*insertSimpleCommand) (struct Command *, struct SimpleCommand *);

};

// Static variables 
struct Command *_currentCommand;
struct SimpleCommand *_currentSimpleCommand;

void prompt();
void print(struct Command *_tmp);
void execute(struct Command *_tmp);
void clear(struct Command *_tmp);
void insertSimpleCommand( struct Command *_tmp, struct SimpleCommand * simpleCommand );
struct Command *newCommand();

#endif
