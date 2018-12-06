#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "command.h"

#define noOfChars 50

struct SimpleCommand *newSimpleCommand() {
    struct SimpleCommand *_tmp = (struct SimpleCommand *) malloc(sizeof(struct SimpleCommand));
    _tmp->_numberOfAvailableArguments = 5;
    _tmp->_numberOfArguments = 0;
    _tmp->_arguments = (char **) malloc(_tmp->_numberOfAvailableArguments * sizeof(char *));
    int i;
    for (i = 0; i < _tmp->_numberOfAvailableArguments; ++i) {
        _tmp->_arguments[i] = (char *) calloc(noOfChars, sizeof(char));
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
                _tmp->_arguments[i] = (char *) calloc(noOfChars , sizeof(char));
            }
        }
        else {
            // Error reallocating memory
            free(_tmp->_arguments);
            printf("Error reallocating memory");
            exit (1);
        }
	}
	strcpy(_tmp->_arguments[_tmp->_numberOfArguments], argument);
	
	_currentSimpleCommand->_numberOfArguments++;
}

struct Command *newCommand() {
    // Create available space for one simple command
    struct Command *_tmp = (struct Command *) malloc(sizeof(struct Command *));
    _tmp->_numberOfAvailableSimpleCommands = 5;
    _tmp->_numberOfSimpleCommands = 0;
	_tmp->_simpleCommands = (struct SimpleCommand **)
		malloc( _tmp->_numberOfAvailableSimpleCommands * sizeof( struct SimpleCommand * ) );

	_tmp->_outFile = NULL;
	_tmp->_inputFile = NULL;
	_tmp->_errFile = NULL;
	_tmp->_background = 0;
    return _tmp;
}

void insertSimpleCommand(struct Command *_tmp, struct SimpleCommand * simpleCommand ) {
    //printf("%d %d", _tmp->_numberOfAvailableSimpleCommands, _tmp->_numberOfSimpleCommands);
    if ( _tmp->_numberOfAvailableSimpleCommands == _tmp->_numberOfSimpleCommands ) {
		_tmp->_numberOfAvailableSimpleCommands *= 2;
        struct SimpleCommand **moreSimpleCommand = (struct SimpleCommand **) realloc( _tmp->_simpleCommands,
			 _tmp->_numberOfAvailableSimpleCommands * sizeof(struct SimpleCommand * ) );
        if (moreSimpleCommand != NULL) {
            // Succes reallocating memory
            //_tmp->_simpleCommands = moreSimpleCommand;
        }
        else {
            // Error reallocating memory
            printf("Error reallocating memory.\n");
            free(_tmp->_simpleCommands);
            exit (1);
        }
	}
	
	_tmp->_simpleCommands[ _tmp->_numberOfSimpleCommands ] = simpleCommand;
	_tmp->_numberOfSimpleCommands++;
}

void clear()
{
	int i, j;
	for (i = 0; i < _currentCommand -> _numberOfSimpleCommands; ++i) {
			for (j = 0; j < _currentCommand -> _simpleCommands[i] -> _numberOfAvailableArguments; ++j) {
				//printf("%s ", _currentCommand-> _simpleCommands[i] -> _arguments[j]);
				if (_currentCommand -> _simpleCommands[i] -> _arguments[j] != NULL)
					free(_currentCommand -> _simpleCommands[i] -> _arguments[j]);
			}
			free(_currentCommand -> _simpleCommands[i] -> _arguments);
			free(_currentCommand -> _simpleCommands[i]);
	}

	//free(_currentSimpleCommand);
	free(_currentCommand);
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

void execute()
{

	// Don't do anything if there are no simple commands
	if ( _currentCommand->_numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	// Print contents of Command data structure
	print(_currentCommand);

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec

	// save IN/OUT/ERR

	int tmpin = dup(STDIN_FILENO);
	int tmpout = dup(STDOUT_FILENO);
	int tmperr = dup(STDERR_FILENO);

	// Set the initial Input
	int fdin;
	if(_currentCommand->_inputFile) {
		fdin = open(_currentCommand->_inputFile, O_RDONLY);
	}
	else{
		// Use default input
		fdin = dup(tmpin);
	}

	pid_t pid;
	int fdout, fderr, i;
	for(i = 0; i < _currentCommand->_numberOfSimpleCommands; i++){
		//redirect in out err
		dup2(fdin, 0);
		
		close(fdin);

		//setup output
		if( i == _currentCommand->_numberOfSimpleCommands - 1){
			// Last simple command
			if(_currentCommand->_outFile){
				fdout = open(_currentCommand->_outFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP |  S_IROTH);
			}
			else{
				// Use default output
				fdout = dup(tmpout);
			}
			
			if(_currentCommand->_errFile){
				fderr = open(_currentCommand->_errFile, O_WRONLY | O_CREAT,  S_IRUSR | S_IWUSR | S_IRGRP |  S_IROTH);
			}
			else{
				fderr = dup(tmperr);
			}
			
		}

		else{
			// Not last simple command create pipe
			int fdpipe[2];
			pipe(fdpipe);
			fdin = fdpipe[0];
			fdout = fdpipe[1];
		}

		// Redirect output

		dup2(fdout, STDOUT_FILENO);
		dup2(fderr, STDERR_FILENO);
		close(fdout);
		close(fderr);

		// Create child process

		pid = fork();
		if(pid < 0){
			perror(NULL);
			return;
		}
		else if(pid ==0){
			int j = 0;
			char ** argv = (char **) malloc((_currentCommand->_simpleCommands[i]->_numberOfArguments + 1)* (sizeof(char*)));
			// printf("Numarul de argumente este %d\n", _currentCommand->_simpleCommands[i]->_numberOfArguments);
			for(j = 0; j < _currentCommand->_simpleCommands[i]->_numberOfArguments; j++){
				argv[j] =  _currentCommand->_simpleCommands[i]->_arguments[j];
			}
			// Add NULL argument at the end
			argv[_currentCommand->_simpleCommands[i]->_numberOfArguments] = NULL;
			//printf("%s\n", _currentCommand->_simpleCommands[i]->_arguments[0]);
		
			if(strcmp(_currentCommand->_simpleCommands[i]->_arguments[0], "cd") == 0){
				chdir(argv[1]);

				//printf("%s\n", argv[1]);
			}
			else{
				execvp(_currentCommand->_simpleCommands[i]->_arguments[0], argv);
				perror(NULL);
				_exit(1);
			}
		}
		else{
			
				wait(NULL);
		}
		
	}

	// Restore in/out defaults

	dup2(tmpin, STDIN_FILENO);
	dup2(tmpout, STDOUT_FILENO);
	dup2(tmperr, STDERR_FILENO);
	close(tmpin);
	close(tmpout);
	close(tmperr);

	// Clear to prepare for next command
	//_currentCommand = clear(_tmp);
	// Print new prompt and allocate memory for _currentCommand
	
	clear();
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
    prompt();
    yyparse();
    return 0;
}
