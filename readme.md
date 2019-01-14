# CSHELL
A shell project, written in C99.

## Dependencies
* [gcc](https://www.gnu.org/software/gcc/)
* [Lex & Yacc](http://dinosaur.compilertools.net/)
* Any CLI
## Instalation
If you don't have the ```lex``` and ```yacc``` installed, just copy this line of code into the terminal:
```bash
sudo apt-get install flex bison
```

To install the shell, just run the code:
```bash
gcc command.c last.lex.c y.tab.c -o shell
```
And to start the shell:
```bash
./shell
```

## Usage
CSHELL uses ```lex```  and ```yacc``` to parse the input and store it in the ```struct Command```. In the ```shell.l``` is the ```lex``` and in the ```shell.y``` is the ```yac``` part. 

**DO NOT COMPILE THE ```shell.y``` file!** It will rewrite some parts of ```the y.tab.c```.

The most important part of the shell is done by the```int yyparse(void)``` function which collects all the data from the parser.

Generally, the shell makes a ```fork()``` for every command and then execute ```execvp``` to run the actual command with the passed arguments.

It uses ```pipe()``` for making pipes and redirecting the ```STDOUT```, the ```STDIN``` and the ```STDERR```.

The CSHELL implements ```&&``` and ```||``` for logical expression using the function  
```C
struct CommandQueue* initializeCommandQueue()
```

The history is kept into a file which can be read later to see all the commands you typed, handled by the ```void history()``` function.

Getting the username and the machine name is done by the ```void getTheRightUser()``` and then passed to the ```void prompt()```.

The following part
```C
struct sigaction s_child;
s_child.sa_handler = sigHandler;
sigemptyset(&s_child.sa_mask);
s_child.sa_flags = SA_RESTART;
sigaction(SIGINT, &s_child, NULL);
```
handles the signals caught by ```CTRL + C``` the way that it will not reset a previous signal.```sigemptyset(&s_child.sa_mask)``` sends those signals to the kernel and them to a list. ```s_child.sa_handler = sigHandler``` is the handler itself and is called with the ```sigaction(SIGINT, &s_child, NULL)```

## Authors
* Benescu Pavel
* Morosan Ionut-Mihai
* Popescu Nicolae-Aurelian

## TODO
- [X]  commands running in background
- [ ]  handle CTRL+Z signals
- [X]  working with more than 5 commands 
- [ ]  Bash Scripts :muscle:
## License
[MIT](https://choosealicense.com/licenses/mit/)
