#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "command.h"

#define noOfChars 50

struct SimpleCommand *newSimpleCommand() {
    struct SimpleCommand *_tmp = (struct SimpleCommand *) malloc(sizeof(struct SimpleCommand));
    _tmp->_numberOfAvailableArguments = 5;
    _tmp->_numberOfArguments = 0;
    _tmp->_arguments = (char **) malloc(_tmp->_numberOfAvailableArguments * sizeof(char *));
    int i;
    for (i = 0; i < _tmp->_numberOfAvailableArguments; ++i) {
        _tmp->_arguments[i] = (char *) malloc(noOfChars * sizeof(char));
    }
    return _tmp;
}

void insertArgument(struct SimpleCommand *_tmp, char * argument ){
	if ( _tmp->_numberOfAvailableArguments == _tmp->_numberOfArguments  + 1 ) {
		// Double the available space
		_tmp->_numberOfAvailableArguments *= 2;
        
        char **_moreArguments = (char **) realloc( _tmp->_arguments,
				_tmp->_numberOfAvailableArguments * sizeof( char * ) );
        if (_moreArguments != NULL) {
            // Succes reallocating memory
            _tmp->_arguments = _moreArguments;
            int i;
            for (i = _tmp->_numberOfAvailableArguments / 2; i < _tmp->_numberOfAvailableArguments; ++i) {
                _tmp->_arguments[i] = (char *) malloc(noOfChars * sizeof(char));
            }
        }
        else {
            // Error reallocating memory
            free(_tmp->_arguments);
            printf("Error reallocating memory");
            exit (1);
        }
	}
	
	_tmp->_arguments[ _tmp->_numberOfArguments ] = argument;

	// Add NULL argument at the end
	_tmp->_arguments[ _tmp->_numberOfArguments + 1] = NULL;
	
	_tmp->_numberOfArguments++;
}

struct Command *newCommand() {
    // Create available space for one simple command
    struct Command *_tmp = (struct Command *) malloc(sizeof(struct Command *));
    _tmp->_numberOfAvailableSimpleCommands = 5;
    _tmp->_numberOfSimpleCommands = 0;
	_tmp->_simpleCommands = (struct SimpleCommand **)
		malloc( _tmp->_numberOfAvailableSimpleCommands * sizeof( struct SimpleCommand * ) );

	_tmp->_numberOfSimpleCommands = 0;
	_tmp->_outFile = 0;
	_tmp->_inputFile = 0;
	_tmp->_errFile = 0;
	_tmp->_background = 0;
    return _tmp;
}

void insertSimpleCommand(struct Command *_tmp, struct SimpleCommand * simpleCommand ) {
    printf("%d %d", _tmp->_numberOfAvailableSimpleCommands, _tmp->_numberOfSimpleCommands);
    if ( _tmp->_numberOfAvailableSimpleCommands == _tmp->_numberOfSimpleCommands ) {
		_tmp->_numberOfAvailableSimpleCommands *= 2;
        struct SimpleCommand **moreSimpleCommand = (struct SimpleCommand **) realloc( _tmp->_simpleCommands,
			 _tmp->_numberOfAvailableSimpleCommands * sizeof(struct SimpleCommand * ) );
        if (moreSimpleCommand != NULL) {
            // Succes reallocating memory
            _tmp->_simpleCommands = moreSimpleCommand;
        }
        else {
            // Error reallocating memory
            free(_tmp->_simpleCommands);
            printf("Error reallocating memory.\n");
            exit (1);
        }
	}
	
	_tmp->_simpleCommands[ _tmp->_numberOfSimpleCommands ] = simpleCommand;
	_tmp->_numberOfSimpleCommands++;
}

void clear(struct Command *_tmp)
{
	for ( int i = 0; i < _tmp->_numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _tmp->_simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _tmp->_simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _tmp->_simpleCommands[ i ]->_arguments );
		free ( _tmp->_simpleCommands[ i ] );
	}

	if ( _tmp->_outFile ) {
		free( _tmp->_outFile );
	}

	if ( _tmp->_inputFile ) {
		free( _tmp->_inputFile );
	}

	if ( _tmp->_errFile ) {
		free( _tmp->_errFile );
	}

	_tmp->_numberOfSimpleCommands = 0;
	_tmp->_outFile = 0;
	_tmp->_inputFile = 0;
	_tmp->_errFile = 0;
	_tmp->_background = 0;
}
void print(struct Command *_tmp)
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _tmp->_numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _tmp->_simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _tmp->_simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _tmp->_outFile?_tmp->_outFile:"default",
		_tmp->_inputFile?_tmp->_inputFile:"default", _tmp->_errFile?_tmp->_errFile:"default",
		_tmp->_background?"YES":"NO");
	printf( "\n\n" );
	
}

void execute(struct Command *_tmp)
{
	// Don't do anything if there are no simple commands
	if ( _tmp->_numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	// Print contents of Command data structure
	print(_tmp);

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec

	// Clear to prepare for next command
	clear(_tmp);
	// Print new prompt and allocate memory for _currentCommand
	free(_currentCommand);
    _currentCommand = newCommand();
    prompt();
}

void prompt()
{
	printf("myshell>");
	fflush(stdout);
}

int yyparse(void);

int main() {
    _currentCommand = newCommand();
    //_currentCommand->_simpleCommands = newSimpleCommand();
    prompt();
    yyparse();
    return 0;
}