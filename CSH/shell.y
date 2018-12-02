
/*
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
  */

%token	<string_val> WORD

%token 	NOTOKEN GREAT NEWLINE 

%union	{
	char *string_val;
}

%{
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
		printf("   Yacc: Execute command\n");
		//(*_currentCommand->execute)(_currentCommand);
	}
	| NEWLINE {
	}
	| error NEWLINE { yyerrok; }
	;

command_and_args:
	command_word arg_list {
		printf("	Yacc: insert simple command\n");
		//printf("%d %d", _currentCommand->_numberOfAvailableSimpleCommands, _currentCommand->_numberOfSimpleCommands);
		//(*_currentCommand->
		//	insertSimpleCommand)(_currentCommand, _currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* empty */
	;

argument:
	WORD {
			printf("AICi");
            printf("   Yacc: insert argument \"%s\"\n", $1);
	       (*_currentSimpleCommand->insertArgument)(_currentSimpleCommand, $1 );
	}
	;

command_word:
	WORD {
				printf("%s\n", $1);
               printf("   Yacc: insert command \"%s\"\n", $1);
			free(_currentSimpleCommand);
			_currentSimpleCommand = newSimpleCommand();
	       (*_currentSimpleCommand->insertArgument)(_currentSimpleCommand, $1 );
	}
	;

iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		_currentCommand->_outFile = $2;
	}
	| /* empty */ 
	;

%%
