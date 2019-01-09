%token	<string_val> WORD

%token 	NOTOKEN GREAT LESS ERR NEWLINE PIPE GREATGREAT LESSLESS LOGIC_AND LOGIC_OR

%union	{
	char *string_val;
}

%{
extern int yylex();
#define yylex yylex
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "command.h"
 
void yyerror(const char *str) {
    fprintf(stderr,"error: %s\n",str);
}
 
int yywrap() {
    return 1;
} 

%}


%%

goal:	
	first_commands
	| first_commands logic_and
	| first_commands logic_or
	;

logic_and:
	LOGIC_AND command
	;
	
logic_or:
	LOGIC_OR command
	;

first_commands: 
	command
	| first_commands command
	;

command:
	simple_command
	;

simple_command:	
	command_and_args iomodifier_opt NEWLINE {
		execute();
	}
	| command_and_args iomodifier_opt logic_and {
		_commandQueueBack->logicAnd = 1;
	}
	| command_and_args iomodifier_opt logic_or {
		_commandQueueBack->logicOr = 1;
	}
	| NEWLINE {
		prompt();
	}
	| error NEWLINE { 
		yyerrok; 
		prompt(); 
	}
	;

command_and_args:
	command_word arg_list pipe {
		insertSimpleCommand( _currentCommand, _currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	|
	;

argument:
	WORD {
		insertArgument(_currentSimpleCommand, $1);
	}
	;

command_word:
	WORD {
		struct CommandQueue *tmp = initializeCommandQueue();
		_currentCommand = newCommand();
		tmp->command = _currentCommand;
		if (_commandQueue == NULL)
		{
			_commandQueue = tmp;
			_commandQueueBack = _commandQueue;
		}
		else
		{
			_commandQueueBack->next = tmp;
			_commandQueueBack = tmp;
		}
		_currentSimpleCommand = newSimpleCommand();
	    insertArgument(_currentSimpleCommand, $1);
	}
	;

iomodifier_opt:
	GREAT WORD {
		_currentCommand->_outFile = $2;
	}
	| LESS WORD {
		_currentCommand->_inputFile = $2;
	}
	| ERR WORD {
		_currentCommand->_errFile = $2;
	}
	| GREATGREAT WORD {
		_currentCommand->_appendOutputFile = $2;
	}
	| LESSLESS WORD {
		_currentCommand->_inputMatchWord = $2;
	}
	| 
	;

pipe: 
	PIPE command_and_args_for_pipe {
		// Nothing done yet;
	}
	| 
	;

command_and_args_for_pipe: 
	command_word_for_pipe arg_list pipe {
		// Nothing done yet;
	}
	;

command_word_for_pipe: 
	WORD {
		insertSimpleCommand( _currentCommand, _currentSimpleCommand );
		_currentSimpleCommand = newSimpleCommand();
	    insertArgument(_currentSimpleCommand, $1 );
	}
	;

%%
