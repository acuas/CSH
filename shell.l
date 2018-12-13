%{

#include <string.h>
#include "y.tab.h"

%}

%%

\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard Spaces and Tabs */
    }	

"\e[A"	{
		return UP_ARROW;
}

"\e[B"	{
		return DOWN_ARROW;
}

">" 	{
		return GREAT;
	}

"<" 	{
		return LESS;
	}

	
"|" 	{
		return PIPE;
	}


[\./a-zA-Z\-][\./a-zA-Z0-9\-]*  {
		/* Assume that file names only have alpha chars */
		yylval.string_val=strdup(yytext);
		return WORD;
	}

.	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
