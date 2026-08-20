%{
#include "calculadora.tab.h"
#include <stdlib.h>
%}

%option noyywrap

%%
[0-9]+ {
    yylval = atoi(yytext);
    return NUMBER;
}

"+"     { return ADD; }
"-"     { return SUB; }
"*"     { return MUL; }
"/"     { return DIV; }
"|"     { return ABS; }
"\n"    { return EOL; }

[ \t]   { /* ignorar espacios y tabulaciones */ }

.       { printf("Caracter desconocido: %s\n", yytext); }
%%