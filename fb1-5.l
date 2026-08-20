%option noyywrap

%{
#include <stdio.h>
#include <stdlib.h>
#include "fb1-5.tab.h"
%}

%%

"+"             { return ADD; }
"-"             { return SUB; }
"*"             { return MUL; }
"/"             { return DIV; }
"|"             { return ABS; }

0x[a-f0-9]+     {
                    yylval = strtol(yytext, NULL, 16);
                    return NUMBER;
                }

[0-9]+          {
                    yylval = atoi(yytext);
                    return NUMBER;
                }

"//".*          { }

\n              { return EOL; }

[ \t]           { }

.               { printf("Mystery character %c\n", *yytext); }

%%