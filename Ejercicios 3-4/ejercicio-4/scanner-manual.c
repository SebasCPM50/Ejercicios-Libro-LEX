/*
 * Handwritten version of scanner for calculator
 */

#include <stdio.h>
#include <ctype.h>
#include "scanner.tab.h"

int yylval;

FILE *yyin;
static int seeneof = 0;

void yyerror(const char *format, int c)
{
    fprintf(stderr, format, c);
}

int yylex(void)
{
    if (!yyin)
        yyin = stdin;

    if (seeneof)
        return 0;

    while (1) {
        int c = getc(yyin);

        if (isdigit(c)) {
            int i = c - '0';

            while (isdigit(c = getc(yyin)))
                i = (10 * i) + c - '0';

            yylval = i;

            if (c == EOF)
                seeneof = 1;
            else
                ungetc(c, yyin);

            return NUMBER;
        }

        switch (c) {
            case '+': return ADD;
            case '-': return SUB;
            case '*': return MUL;
            case '|': return ABS;
            case '(': return OP;
            case ')': return CP;

            case '\n':
                return EOL;

            case ' ':
            case '\t':
                break;

            case EOF:
                return 0;

            case '/':
                c = getc(yyin);

                if (c == '/') {
                    while ((c = getc(yyin)) != '\n')
                        if (c == EOF)
                            return 0;

                    break;
                }

                if (c == EOF)
                    seeneof = 1;
                else
                    ungetc(c, yyin);

                return DIV;

            default:
                yyerror("Mystery character %c\n", c);
                break;
        }
    }
}

int main(void)
{
    int tok;

    while ((tok = yylex())) {
        printf("%d", tok);

        if (tok == NUMBER)
            printf(" = %d\n", yylval);
        else
            printf("\n");
    }

    return 0;
}