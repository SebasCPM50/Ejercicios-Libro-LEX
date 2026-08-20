/* wc_c.c - version en C puro del Ejemplo 1-1 (fb1-1.l), sin usar flex */
#include <stdio.h>
#include <ctype.h>

int main(void) {
    int chars = 0;
    int words = 0;
    int lines = 0;
    int in_word = 0; /* bandera: 1 si estamos dentro de una palabra */
    int c;

    while ((c = getchar()) != EOF) {
        chars++;

        if (c == '\n') {
            lines++;
        }

        if (isalpha(c)) {
            /* letra: si no estabamos ya en una palabra, es el inicio de una nueva */
            if (!in_word) {
                words++;
                in_word = 1;
            }
        } else {
            /* cualquier otro caracter termina la palabra actual, si habia una */
            in_word = 0;
        }
    }

    printf("%8d%8d%8d\n", lines, words, chars);
    return 0;
}
