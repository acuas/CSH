
/*
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
  */

%token	<string_val> WORD

%token 	NOTOKEN GREAT LESS NEWLINE PIPE

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
		//printf("   Yacc: insert output \"%s\"\n", $2);
		//strcpy(_currentCommand -> _outFile, $2);
		_currentCommand->_outFile = $2;
	}
	| LESS WORD {
		_currentCommand->_inputFile = $2;
	}
	|
	;

pipe: 
	PIPE command_and_args_for_pipe {
		// printf("Am intrat pe pipe");
		//insertSimpleCommand( _currentCommand, _currentSimpleCommand );
	}
	| 
	;

command_and_args_for_pipe: 
	command_word_for_pipe arg_list pipe {
		
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
