%{

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

%}

%%

\n 	{
		return NEWLINE;
	}

"exit" 	{
		exit(EXIT_SUCCESS);
	}

"&" {
		return BACKGROUND;
	}

"&&"	{
		return LOGIC_AND;
	}

"||"	{
		return LOGIC_OR;
	}

[ \t] 	{
		/* Discard Spaces and Tabs */
    }	

">" 	{
		return GREAT;
	}

"<" 	{
		return LESS;
	}

"2>"    {
		return ERR;
	}

">>" 	{
		return GREATGREAT;
	}

"<<"	{
		return LESSLESS;
	}

"|" 	{
		return PIPE;
	}

[\./a-zA-Z\-!0-9][\./a-zA-Z!0-9\-]*  {
		/* Assume that file names only have alpha chars */
		yylval.string_val=strdup(yytext);
		return WORD;
	}

.	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
