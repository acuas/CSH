
/*
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
  */

%token	<string_val> WORD

%token 	NOTOKEN GREAT LESS ERR NEWLINE PIPE GREATGREAT LESSLESS ERRERR

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
	commands
	;

commands: 
	command
	| commands command 
	;

command: simple_command
        ;

simple_command:	
	command_and_args iomodifier_opt NEWLINE {
		//printf("   Yacc: Execute command\n");
		execute();
		
	}
	| NEWLINE {
		prompt();
	}
	| error NEWLINE { yyerrok; prompt(); }
	;

command_and_args:
	command_word arg_list pipe {
		//printf("	Yacc: insert simple command\n");
		insertSimpleCommand( _currentCommand, _currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument 
	|
	;

argument:
	WORD {
		//printf("AICi");
        //printf("   Yacc: insert argument \"%s\"\n", $1);
		insertArgument(_currentSimpleCommand, $1 );
	}

	;

command_word:
	WORD {
		//printf("%s\n", $1);
        //printf("   Yacc: insert command \"%s\"\n", $1);
		_currentSimpleCommand = newSimpleCommand();
	    insertArgument(_currentSimpleCommand, $1 );
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
	| ERRERR WORD {
		_currentCommand->_doubleErrFile = $2;
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
