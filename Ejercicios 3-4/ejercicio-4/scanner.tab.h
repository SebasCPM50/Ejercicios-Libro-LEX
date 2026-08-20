#ifndef SCANNER_TAB_H
#define SCANNER_TAB_H

enum yytokentype {
    NUMBER = 258,
    ADD = 259,
    SUB = 260,
    MUL = 261,
    DIV = 262,
    ABS = 263,
    EOL = 264,
    OP = 265,
    CP = 266
};

extern int yylval;

#endif