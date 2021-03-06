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
#include <signal.h>
#include <setjmp.h>
#include <limits.h>
#include "command.h"

#define noOfChars 50

char *userName;
char *machineName;

void getTheRightUser() {
	userName = (char *)malloc(4096 * sizeof(char));
    machineName = (char *)malloc(4096 * sizeof(char));
	getlogin_r(userName, 4096);	// the userName
	gethostname(machineName, 4096); // the MachineName
}

struct SimpleCommand *newSimpleCommand() {
    struct SimpleCommand *_tmp;
	_tmp = (struct SimpleCommand *) malloc(sizeof(struct SimpleCommand));
    _tmp->_numberOfAvailableArguments = 10;
    _tmp->_numberOfArguments = 0;
    _tmp->_arguments = (char **) malloc(_tmp->_numberOfAvailableArguments * sizeof(char *));
	
	int i;
    for (i = 0; i < _tmp->_numberOfAvailableArguments; ++i) {
        _tmp->_arguments[i] = (char *) calloc(noOfChars, sizeof(char));
    }
    
	return _tmp;
}

void insertArgument(struct SimpleCommand *_tmp, char * argument ) {
	if (_tmp->_numberOfAvailableArguments == _tmp->_numberOfArguments  + 1) {
		// Double the available space
		_tmp->_numberOfAvailableArguments *= 2;
        char **_moreArguments = (char **)realloc(_tmp->_arguments, _tmp->_numberOfAvailableArguments * sizeof(char *));
        if (_moreArguments != NULL) {
            // Succes reallocating memory
            _tmp->_arguments = _moreArguments;
            int i;
            for (i = _tmp->_numberOfAvailableArguments / 2; i < _tmp->_numberOfAvailableArguments; ++i) {
                _tmp->_arguments[i] = (char *)calloc(noOfChars, sizeof(char));
            }
        }
        else {
            // Error reallocating memory
            free(_tmp->_arguments);
            printf("Error reallocating memory");
            exit(1);
        }
	}

	strcpy(_tmp->_arguments[_tmp->_numberOfArguments], argument);
	_currentSimpleCommand->_numberOfArguments++;
}

struct Command *newCommand() {
    // Create available space for one simple command
    struct Command *_tmp = (struct Command *) malloc(sizeof(struct Command));
    _tmp->_numberOfAvailableSimpleCommands = 10;
    _tmp->_numberOfSimpleCommands = 0;
	_tmp->_simpleCommands = (struct SimpleCommand **) malloc( _tmp->_numberOfAvailableSimpleCommands * sizeof(struct SimpleCommand *));
	_tmp->_outFile = NULL;
	_tmp->_inputFile = NULL;
	_tmp->_errFile = NULL;
	_tmp->_appendOutputFile = NULL;
	_tmp->_inputMatchWord = NULL;
	_tmp->_doubleErrFile = NULL;
	_tmp->_background = 0;
    return _tmp;
}

void insertSimpleCommand(struct Command *_tmp, struct SimpleCommand * simpleCommand) {
    if (_tmp->_numberOfAvailableSimpleCommands == _tmp->_numberOfSimpleCommands) {
		_tmp->_numberOfAvailableSimpleCommands *= 2;
        struct SimpleCommand **moreSimpleCommand = (struct SimpleCommand **) realloc(_tmp->_simpleCommands,
			 _tmp->_numberOfAvailableSimpleCommands * sizeof(struct SimpleCommand *));
        if (moreSimpleCommand != NULL) {
            // Succes reallocating memory
            _tmp->_simpleCommands = moreSimpleCommand;
			for (int i = _tmp->_numberOfAvailableSimpleCommands / 2; i < _tmp->_numberOfAvailableSimpleCommands; i++)
				_tmp->_simpleCommands[i] = NULL;
        }
        else {
            // Error reallocating memory
            printf("Error reallocating memory.\n");
            free(_tmp->_simpleCommands);
            exit(1);
        }
	}
	
	_tmp->_simpleCommands[_tmp->_numberOfSimpleCommands] = simpleCommand;
	_tmp->_numberOfSimpleCommands++;
}

void clearCommandQueue() {
	deleted = 0;
	struct CommandQueue *tmp1, *tmp2;
	tmp1 = _commandQueue;
	tmp2 = tmp1;
	while (tmp1 != NULL) {
		tmp1 = tmp1->next;
		//clearCommand(tmp2->command);
		//free(tmp2);
		tmp2 = tmp1;
	}
	_commandQueue = NULL;
}

void clearCommand(struct Command *command) {
	int i;
    for (i = 0; i < command->_numberOfAvailableSimpleCommands; ++i) {
        int j;
		if (command->_simpleCommands[i] != NULL) {
			for (j = 0; j < command->_simpleCommands[i]->_numberOfAvailableArguments; ++j) {
				free(command->_simpleCommands[i]->_arguments[j]);
			}
			free(command->_simpleCommands[i]->_arguments);
			free(command->_simpleCommands[i]);
		}
    }
	free(command->_simpleCommands);
    free(command);
}

void print(struct Command *_tmp) {
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for (int i = 0; i < _tmp->_numberOfSimpleCommands; i++) {
		printf("  %-3d ", i );
		for (int j = 0; j < _tmp->_simpleCommands[i]->_numberOfArguments; j++) {
			printf("\"%s\" \t", _tmp->_simpleCommands[i]->_arguments[j]);
		}
		printf("\n");
	}
	printf("\n\n");
	printf("  Output       Input        Error        Background\n");
	printf("  ------------ ------------ ------------ ------------\n");
	printf("  %-12s %-12s %-12s %-12s\n", _tmp->_outFile?_tmp->_outFile:"default",
		_tmp->_inputFile?_tmp->_inputFile:"default", _tmp->_errFile?_tmp->_errFile:"default",
		_tmp->_background?"YES":"NO");
	printf("\n\n");
	
}

void history() {
	char *home = getenv("HOME"); 
	char *fileName = "/.csh_history"; 
	char *filePath = (char *) calloc(PATH_MAX, sizeof(char)); 
	strcat(filePath, home); 
	strcat(filePath, fileName); 
	FILE *historyStream = fopen(filePath, "r");
	
	if (historyStream == NULL) {
		printf("The file .csh_history can't be opened!\n");
		exit(EXIT_FAILURE);
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	while ((nread = getline(&line, &len, historyStream)) != -1) {
		fwrite(line, nread, 1, stdout);
	}

	fclose(historyStream);
}

void execute() {
	struct CommandQueue *tmp = _commandQueue;
	while (tmp != NULL) {
		if (tmp->logicAnd == 1 && tmp->succesExit == 0)
			break;
		executeCommand(tmp->command, tmp);
		if (tmp->logicAnd == 1 && tmp->succesExit == 0)
			break;
		tmp = tmp->next;
		
	}
	clearCommandQueue();
	prompt();
}

void executeCommand(struct Command *command, struct CommandQueue * commandQueue) {
	// Initialize the sigaction struct for signal handler
	struct sigaction s;
	s.sa_handler = sigHandler;
	sigemptyset(&s.sa_mask);
	s.sa_flags = SA_RESTART;
	sigaction(SIGINT, &s, NULL);

	
	// Don't do anything if there are no simple command
	if (command->_numberOfSimpleCommands == 0) {
		prompt();
		return;
	}
	exitStatus = 0;

	// save IN/OUT/ERR
	int tmpin = dup(STDIN_FILENO);
	int tmpout = dup(STDOUT_FILENO);
	int tmperr = dup(STDERR_FILENO);

	// Set the initial Input
	int fdin;
	if (command->_inputFile) {
		fdin = open(command->_inputFile, O_RDONLY);
	}
	else {
		if (command->_inputMatchWord) {
			char *firstStr = (char*)malloc(4096*sizeof(char));
			char *lastStr = (char*)malloc(4096*sizeof(char));
			int tmp = open("matchWord.txt", O_WRONLY | O_CREAT | O_TRUNC);
			size_t size = 0;
			int ok = 1;
			while(ok){
				write(1, "> ", strlen("> "));
				getline(&lastStr, &size, stdin);
				strcpy(firstStr, lastStr);
				lastStr[strlen(lastStr)-1] = '\0';
				if(strcmp(lastStr, command->_inputMatchWord) == 0){
					
					fdin = open("matchWord.txt", O_RDONLY);
					ok = 0;
				}else{
					write(tmp, firstStr, strlen(firstStr));
					
				}
				
			}
			close(tmp);
			free(lastStr);
			free(firstStr);
			
		}else{
			fdin = dup(tmpin);
		}
		// Use default input	
	}

	pid_t pid;
	int fdout, fderr, i, stat_loc, standardError = 0;
	for (i = 0; i < command->_numberOfSimpleCommands; i++) {
		//redirect in out err
		dup2(fdin, 0);
		close(fdin);

		//setup output
		if (i == command->_numberOfSimpleCommands - 1) {
			// Last simple command
			if (command->_outFile) {
				fdout = open(command->_outFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP |  S_IROTH);
				ftruncate(fdout, 0);
			}
			else {
				if (command->_appendOutputFile) { // check for append
					fdout = open(command->_appendOutputFile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP |  S_IROTH);
				
				}
				else {	//use default Output
					fdout = dup(tmpout);
				}
			}
			
			if (command->_errFile) {
				fderr = open(command->_errFile, O_WRONLY | O_CREAT,  S_IRUSR | S_IWUSR | S_IRGRP |  S_IROTH);
			}
			else {
				fderr = open("err", O_WRONLY | O_CREAT,  S_IRUSR | S_IWUSR | S_IRGRP |  S_IROTH);
				standardError = 1;
			}
			
		}
		else {
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

		if (strcmp(command->_simpleCommands[i]->_arguments[0], "cd") == 0) {
			int retChdir;
			if (strlen(command->_simpleCommands[i]->_arguments[1]) == 0) {
				retChdir = chdir(getenv("HOME"));
			}
			else {
				char *tok = command->_simpleCommands[i]->_arguments[1];
				if (tok == "~" || tok == " " || tok == "") {
					char *home = getenv("HOME");
					retChdir = chdir(home);
				}
				else {
					retChdir = chdir(tok);
				}

			}

			dup2(tmpin, STDIN_FILENO);
			dup2(tmpout, STDOUT_FILENO);
			dup2(tmperr, STDERR_FILENO);
			close(tmpin);
			close(tmpout);
			close(tmperr);

			continue;
				
		}
		
		// Create child process
		pid = fork();

		if (pid < 0) {
			perror(NULL);
			return;
		}
		else if(pid == 0) {
			struct sigaction s_child;
			s_child.sa_handler = sigHandler;
			sigemptyset(&s_child.sa_mask);
			s_child.sa_flags = SA_RESTART;
			sigaction(SIGINT, &s_child, NULL);
			
			int j = 0;
			char **argv = (char **)malloc((command->_simpleCommands[i]->_numberOfArguments + 1) * (sizeof(char*)));
			for (j = 0; j < command->_simpleCommands[i]->_numberOfArguments; j++) {
				argv[j] =  command->_simpleCommands[i]->_arguments[j];
			}
			// Add NULL argument at the end
			argv[command->_simpleCommands[i]->_numberOfArguments] = NULL;
			if (strcmp(command->_simpleCommands[i]->_arguments[0], "history") == 0) {
				history();
			} 
			else {
				exitStatus = execvp(command->_simpleCommands[i]->_arguments[0], argv);
				perror(NULL);
				_exit(1);
			}
		}
		else {

			if(!command->_background) {
				waitpid(pid, &stat_loc, WUNTRACED);
				// write to stderr
				if (standardError == 1) {
					FILE * fin = fopen("err", "r");
					if (fin == NULL) {
						printf("The file can't be opened!\n");
					}
					else {
						char *line = NULL;
						int entered = 0;
           				size_t len = 0;
           				ssize_t nread;
						while ((nread = getline(&line, &len, fin)) != -1) {
               				fwrite(line, nread, 1, stdout);
							entered = 1;
           				}

						fclose(fin);
						remove("err");
						
						if (entered == 1) {
							if(commandQueue->logicAnd == 1){
								commandQueue->succesExit = 0;
							    break;
							}
								
						}
						else {
							commandQueue->succesExit = 1;
						}
					}
				}
			}
		}
	}
	
	// Restore in/out defaults

	dup2(tmpin, STDIN_FILENO);
	dup2(tmpout, STDOUT_FILENO);
	dup2(tmperr, STDERR_FILENO);
	close(tmpin);
	close(tmpout);
	close(tmperr);
}

struct CommandQueue *initializeCommandQueue() {
	struct CommandQueue *tmp = (struct CommandQueue*)malloc(sizeof(struct CommandQueue));
	tmp->succesExit = 0;
	tmp->logicAnd = 0;
	tmp->logicOr = 0;
	tmp->next = NULL;
	return (tmp);
}

void printCommandQueue() {
	printf("\n\n");
	printf("              COMMAND Queue                \n");
	printf("\n");
	
	for (struct CommandQueue *temp = _commandQueue; temp != NULL; temp = temp->next) {
		printf("  #   Commands\n");
	printf("Logic  AND = %d         Logic  OR = %d\n", temp->logicAnd, temp->logicOr);
	printf("  -------------------------------------------------------------\n");
	for (int i = 0; i < temp->command->_numberOfSimpleCommands; i++) {
		printf("  %-3d ", i );
		for (int j = 0; j < temp->command->_simpleCommands[i]->_numberOfArguments; j++) {
			printf("\"%s\" \t", temp->command->_simpleCommands[i]->_arguments[j]);
		}
		printf("\n");
	}
	printf("\n\n");
	printf("  Output       Input        Error        Background\n");
	printf("  ------------ ------------ ------------ ------------\n");
	printf("  %-12s %-12s %-12s %-12s\n", temp->command->_outFile?temp->command->_outFile:"default",
		temp->command->_inputFile?temp->command->_inputFile:"default", temp->command->_errFile?temp->command->_errFile:"default",
		temp->command->_background?"YES":"NO");
	printf("\n\n");
	}
}

void prompt() {
	getTheRightUser();
	printf("%s@%s: ", userName, machineName);
	fflush(stdout);
}

void sigHandler(int nr){
	printf("\n");
	prompt();
}

int yyparse(void);

int main() {
    prompt();
    yyparse();
    return 0;
}