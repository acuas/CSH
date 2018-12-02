#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "command.h"

#define InitNoOfArguments 5

struct SimpleCommand *newSimpleCommand() {
    struct SimpleCommand *_tmp = (struct SimpleCommand *) malloc(sizeof(struct SimpleCommand));
    _tmp->_numberOfAvailableArguments = 5;
    _tmp->_numberOfArguments = 0;
    _tmp->_arguments = (char **) malloc(_tmp->_numberOfAvailableArguments * sizeof(char *));
    _tmp->insertArgument = &insertArgument;
    return _tmp;
}

void insertArgument(struct SimpleCommand *_tmp, char * argument ){
    printf("%d %d", _tmp->_numberOfArguments, _tmp->_numberOfAvailableArguments);
	if ( _tmp->_numberOfAvailableArguments == _tmp->_numberOfArguments  + 1 ) {
		// Double the available space
		_tmp->_numberOfAvailableArguments *= 2;
        
        char **_moreArguments = (char **) realloc( _tmp->_arguments,
				_tmp->_numberOfAvailableArguments * sizeof( char * ) );
        if (_moreArguments != NULL) {
            // Succes reallocating memory
            _tmp->_arguments = _moreArguments;
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
    printf("Am reusit sa ies.\n");
}

struct Command *newCommand() {
    // Create available space for one simple command
    struct Command *_tmp = (struct Command *) malloc(sizeof(struct Command *));
    _tmp->_numberOfAvailableSimpleCommands = 5;
    _tmp->_numberOfSimpleCommands = 0;
	_tmp->_simpleCommands = (struct SimpleCommand **)
		malloc( -_tmp->_numberOfSimpleCommands * sizeof( struct SimpleCommand * ) );

    _tmp->prompt = &prompt;
    _tmp->print = &print;
    _tmp->execute = &execute;
    _tmp->clear = &clear;
    _tmp->insertSimpleCommand = &insertSimpleCommand;
	_tmp->_numberOfSimpleCommands = 0;
	_tmp->_outFile = 0;
	_tmp->_inputFile = 0;
	_tmp->_errFile = 0;
	_tmp->_background = 0;
    return _tmp;
}

void insertSimpleCommand( struct Command *_tmp, struct SimpleCommand * simpleCommand ) {
    printf("0, 0");
    //printf("%d %d", _tmp->_numberOfAvailableSimpleCommands, _tmp->_numberOfSimpleCommands);
    /*if ( _tmp->_numberOfAvailableSimpleCommands == _tmp->_numberOfSimpleCommands ) {
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
    */
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
    printf("Number of Simple Commands %d", _tmp->_numberOfSimpleCommands);
	// Don't do anything if there are no simple commands
	if ( _tmp->_numberOfSimpleCommands == 0 ) {
		(*_tmp->prompt)();
		return;
	}

	// Print contents of Command data structure
	(*_tmp->print)(_tmp);

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec

	// Clear to prepare for next command
	/*(*_tmp->clear)(_tmp);
	// Print new prompt
	(*_tmp->prompt)();
    */
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
    (*_currentCommand->prompt)();
    yyparse();
    return 0;
}