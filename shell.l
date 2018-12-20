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
	
