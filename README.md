# Ejercicios 1, 2, 3, 4, 5 y 6 — Flex & Bison, Capítulo 1

## Ejercicio 1

¿La calculadora acepta una línea que contiene solamente un comentario? ¿Por qué? ¿Es más fácil corregirlo en el scanner o en el parser?

La calculadora original no acepta correctamente una línea que contiene solamente un comentario.

En el scanner de flex, los comentarios que comienzan con `//` son ignorados mediante una regla como:

```c
"//".*    { }
```

Sin embargo, esta regla no consume el salto de línea. Cuando el usuario presiona Enter, el scanner todavía reconoce `\n` y devuelve el token `EOL` al parser.

El problema es que la gramática original de Bison espera normalmente una expresión `exp` seguida de `EOL`:

```c
calclist exp EOL
```

Cuando la línea contiene solamente un comentario, flex elimina el comentario y Bison recibe únicamente:

```text
EOL
```

Como no existe una expresión antes del `EOL`, el parser produce un error de sintaxis.

La solución se realizó en el parser agregando la siguiente regla:

```c
| calclist EOL
```

De esta manera, una línea vacía o una línea que contenga únicamente un comentario también es aceptada por la calculadora.

### Prueba del Ejercicio 1

Se ejecutó la calculadora ingresando una línea que contiene solamente un comentario:

```text
// comentario
```

El programa aceptó la línea sin producir un error de sintaxis.

### Captura de la prueba

<img width="337" height="85" alt="image" src="https://github.com/user-attachments/assets/6f678d6f-4144-4274-b66f-62ead619ac65" />

---

## Ejercicio 2

Modificar la calculadora para que acepte números decimales y hexadecimales y mostrar el resultado en ambas representaciones.

La calculadora original reconoce números decimales mediante el patrón:

```c
[0-9]+
```

Para aceptar también números hexadecimales se agregó al scanner de flex el siguiente patrón:

```c
0x[a-f0-9]+
```

Cuando flex reconoce un número hexadecimal, se utiliza la función `strtol` con base 16 para convertir el texto reconocido en un número entero:

```c
yylval = strtol(yytext, NULL, 16);
return NUMBER;
```

El valor convertido se guarda en `yylval` y se retorna al parser como un token `NUMBER`, igual que ocurre con los números decimales.

También se modificó el `printf` del parser para mostrar el resultado tanto en decimal como en hexadecimal:

```c
printf("= %d (0x%x)\n", $2, $2);
```

### Pruebas del Ejercicio 2

Primero se comprobó que los números decimales siguieran funcionando:

```text
2+3
```

Salida:

```text
= 5 (0x5)
```

Después se realizó una operación combinando un número hexadecimal y uno decimal:

```text
0x10 + 5
```

Salida:

```text
= 21 (0x15)
```

Esto demuestra que la calculadora acepta números en ambas representaciones y muestra el resultado en decimal y hexadecimal.

### Captura de la prueba

<img width="425" height="64" alt="image" src="https://github.com/user-attachments/assets/c28b9160-1bb9-441b-b736-85829047eb38" />


---
## Ejercicio 3 — Operador OR en la calculadora

### Objetivo

Modificar la calculadora del Ejemplo 1-5 para agregar el operador OR a nivel de bits utilizando el símbolo `|`, que originalmente era utilizado como operador unario de valor absoluto.

### Modificación realizada

Se agregó la siguiente regla a la gramática de la calculadora:

```c
| exp ABS factor { $$ = $1 | $3; }
```

El scanner continúa reconociendo el símbolo `|` mediante:

```c
"|" { return ABS; }
```

Por lo tanto, el mismo token `ABS` puede utilizarse por el parser en dos contextos diferentes:

- `ABS term` para calcular el valor absoluto.
- `exp ABS factor` para realizar la operación OR bit a bit.

### Prueba 1 — Valor absoluto

Se ejecutó la calculadora mediante:

```bash
./calculadora.exe
```

y se ingresó:

```text
|5
```

El resultado obtenido fue:

```text
= 5
```

Esta prueba permite comprobar que el operador `|` continúa funcionando como operador unario de valor absoluto.

**Captura:**

<img width="1545" height="174" alt="image" src="https://github.com/user-attachments/assets/029e06cc-63d8-4f06-ada4-c13f14dec012" />


### Prueba 2 — Operador OR

Se ingresó:

```text
5 | 3
```

El resultado obtenido fue:

```text
= 7
```

La operación corresponde al OR bit a bit:

```text
  0101
| 0011
------
  0111
```

Por lo tanto, el resultado decimal es `7`.

**Captura:**

<img width="1531" height="202" alt="image" src="https://github.com/user-attachments/assets/b7a717f9-b635-44e9-98a0-77e85e3bd4c6" />


### Prueba 3 — Combinación de operadores

Se realizó una prueba combinando el operador OR con la suma:

```text
|5 | 3 
```

**Resultado obtenido:**

7

**Captura:**

<img width="1521" height="163" alt="image" src="https://github.com/user-attachments/assets/d997abc1-a2cf-4979-ac2c-6b722e1fc770" />


### Análisis del comportamiento

El símbolo `|` puede utilizarse tanto como operador unario de valor absoluto como operador binario OR.

El scanner no necesita distinguir entre ambos casos, ya que en ambos devuelve el token `ABS`. Es el parser, mediante las reglas de la gramática, el encargado de determinar cómo se utiliza el token dependiendo del contexto.

La utilización del mismo token para ambos usos permite que el parser interprete `|` como valor absoluto cuando aparece delante de un término y como OR cuando aparece entre dos expresiones.

---

## Ejercicio 4 — Comparación del scanner manual y Flex

### Objetivo

Comparar el scanner escrito manualmente del Ejemplo 1-4 con la versión del scanner implementada mediante Flex.

Se utilizaron diferentes entradas para comprobar qué tokens reconoce cada implementación y determinar si ambos scanners reconocen exactamente los mismos tokens.

Para la comparación se utilizaron los siguientes tokens:

| Símbolo / entrada | Token | Valor |
|---|---|---:|
| `+` | ADD | 259 |
| `-` | SUB | 260 |
| `*` | MUL | 261 |
| `/` | DIV | 262 |
| `\|` | ABS | 263 |
| `(` | OP | 265 |
| `)` | CP | 266 |
| números | NUMBER | 258 |
| salto de línea | EOL | 264 |

### Prueba 1 — Números

Se utilizó la entrada:

```text
123
```

El scanner manual reconoció el número como `NUMBER` y el salto de línea como `EOL`.

Resultado observado:

```text
258 = 123
264
```

**Captura del scanner manual:**

<img width="1505" height="204" alt="image" src="https://github.com/user-attachments/assets/199adf7f-e38f-408d-bed1-f9cfdf6503b1" />

La versión Flex utiliza las mismas reglas principales para reconocer números:

```text
[0-9]+
```

por lo que la entrada corresponde igualmente al token `NUMBER`.

### Prueba 2 — Operadores

Se utilizó la entrada:

```text
+ - * / |
```

El scanner manual reconoció los operadores de la siguiente manera:

```text
+  → ADD
-  → SUB
*  → MUL
/  → DIV
|  → ABS
```

**Captura del scanner manual:**

<img width="1558" height="385" alt="image" src="https://github.com/user-attachments/assets/8f79e9ea-1b14-47b7-af98-e4913e2de186" />

La versión Flex utiliza reglas equivalentes para estos operadores, por lo que ambos scanners reconocen estos símbolos como los mismos tokens.

### Prueba 3 — Paréntesis

Se utilizó la entrada:

```text
( )
```

El scanner manual contiene las reglas:

```c
case '(': return OP;
case ')': return CP;
```

por lo que reconoce `(` como `OP` y `)` como `CP`.

**Captura del scanner manual:**

<img width="1582" height="251" alt="image" src="https://github.com/user-attachments/assets/53b6f43a-dc5a-4812-9ae1-d8acb8d1d947" />

La versión Flex también fue preparada para reconocer estos símbolos mediante reglas equivalentes.

### Prueba 4 — Comentarios

Se utilizó la entrada:

```text
5 // comentario
```

Esta prueba permite observar una diferencia importante entre ambas implementaciones.

El scanner manual contiene código específico para reconocer `//` como comentario:

```c
if(c == '/') {
    /* it's a comment */
    while((c = getc(yyin)) != '\n')
        if(c == EOF) return 0;
}
```

Por esta razón, cuando encuentra `//`, el scanner manual descarta el contenido del comentario hasta llegar al final de la línea.

**Captura del scanner manual:**

<img width="1539" height="178" alt="image" src="https://github.com/user-attachments/assets/841efc01-af9b-4fb1-9d40-d93853c5d739" />

En cambio, el scanner Flex utilizado para esta comparación solamente contiene una regla para `/`:

```c
"/" { return DIV; }
```

y no contiene una regla específica para `//`.

Por lo tanto, el tratamiento de esta entrada no es idéntico entre las dos implementaciones.

### Prueba 5 — Carácter desconocido

Se utilizó la entrada:

```text
5 @ 3
```

El carácter `@` no corresponde a ninguno de los tokens definidos para la calculadora.

El scanner manual utiliza una regla `default` para informar la presencia de caracteres desconocidos.

**Captura del scanner manual:**

<img width="1541" height="280" alt="image" src="https://github.com/user-attachments/assets/5e1bcc9f-c29b-4d07-95a0-2bd5e0accedc" />

La versión Flex también posee una regla general para caracteres que no coinciden con los patrones definidos, por lo que puede informar este tipo de entrada como un carácter desconocido.

### Análisis del comportamiento

A partir de las pruebas realizadas y de la comparación de las reglas de ambos scanners, se observa que las dos implementaciones reconocen de manera equivalente los principales tokens de la calculadora, incluyendo números, operadores y saltos de línea.

Sin embargo, **no reconocen exactamente las mismas entradas en todos los casos**.

La diferencia principal encontrada corresponde al tratamiento de los comentarios `//`.

El scanner manual contiene una lógica específica para detectar `//` y tratarlo como comentario, mientras que el scanner Flex utilizado en esta comparación reconoce `/` como el operador `DIV` y no posee una regla específica para `//`.

Por lo tanto, la respuesta a la pregunta del ejercicio es:

> **No. El scanner manual y el scanner de Flex no reconocen exactamente los mismos tokens para todas las entradas posibles.**

Ambos reconocen los tokens principales de la calculadora de manera equivalente, pero existen diferencias en el comportamiento frente a determinadas entradas.

### Conclusión

El ejercicio permitió comparar dos formas diferentes de construir un scanner.

El scanner manual realiza el análisis carácter por carácter mediante código C, utilizando estructuras como `switch`, `getc()` y `ungetc()`.

Por otro lado, Flex permite definir los patrones de reconocimiento mediante expresiones regulares y asociar acciones a cada patrón.

La comparación demuestra que Flex facilita la implementación de un scanner, pero que las reglas especificadas determinan exactamente qué entradas reconoce. Por esta razón, dos scanners que tienen como objetivo reconocer el mismo lenguaje pueden presentar comportamientos diferentes si sus reglas no son completamente equivalentes.

## Evidencias

### Ejercicio 3

- Prueba de valor absoluto: `|5`
- Prueba de OR: `5 | 3`
- Prueba combinada: `|5 | 3 `

### Ejercicio 4

- Prueba de números: `123`
- Prueba de operadores: `+ - * / |`
- Prueba de paréntesis: `( )`
- Prueba de comentarios: `5 // comentario`
- Prueba de carácter desconocido: `5 @ 3`

Las capturas correspondientes se encuentran en las rutas indicadas anteriormente.
## Ejercicio 5

¿Para qué lenguajes flex NO sería una buena herramienta?

Flex genera analizadores basados en expresiones regulares, y las expresiones regulares solo pueden reconocer lenguajes regulares (los que se describen con un autómata finito, sin memoria de lo que vino antes más allá de un número fijo de estados). Cualquier lenguaje que necesite "recordar" una cantidad no acotada de información no puede reconocerse solo con flex.

Ejemplos concretos donde flex no alcanza:

Lenguajes con anidamiento o balanceo, por ejemplo verificar que los paréntesis de una expresión matemática están bien balanceados como en `((3+4)*(5-2))`. Un autómata finito no puede contar cuántos paréntesis lleva abiertos si ese número puede crecer indefinidamente; se necesita una pila, es decir, un autómata de pila. Eso es justo lo que hace Bison (un lenguaje libre de contexto), no flex.

XML/HTML, donde hay que validar que cada etiqueta de apertura como `<div>` tenga su cierre correspondiente `</div>` con el mismo nombre y en el orden correcto. Es otra vez un problema de anidamiento no regular.

Lenguajes sensibles a indentación, como Python, que usa la sangría para delimitar bloques de código. Flex no tiene forma natural de recordar el nivel de indentación anterior y compararlo con el actual; normalmente se resuelve con lógica extra fuera del propio flex, como contadores de espacios llevados a mano.

Comentarios anidados: si un lenguaje permite comentarios dentro de comentarios, como `/* uno /* dos */ */`, una expresión regular simple no puede llevar la cuenta de cuántos niveles de anidamiento lleva abiertos.

Conclusión: flex es excelente para la parte léxica, es decir, identificar palabras, números y operadores, pero para cualquier estructura recursiva o anidada se necesita algo con más poder computacional, como Bison (gramáticas libres de contexto) o lógica adicional escrita a mano.

---

## Ejercicio 6

Reescribir el contador de palabras (wc) en C puro, sin flex, y comparar velocidad y dificultad de depuración con la versión original.

La versión original del libro es el Ejemplo 1-1 (`fb1-1.l`), que usa el patrón `[a-zA-Z]+` para contar palabras. La versión en C puro que se hizo manualmente está en el archivo `wc_c.c` adjunto.

La idea del algoritmo en C puro es usar una bandera llamada `in_word` que indica si el caracter anterior era parte de una palabra. Cada vez que se pasa de "no letra" a "letra" se cuenta una palabra nueva, así se evita contar la misma palabra letra por letra.

Se probó la equivalencia de ambas versiones corriéndolas sobre el mismo texto, y dieron resultados idénticos en líneas, palabras y caracteres.

Para la prueba de rendimiento se generó un archivo de 100 MB, con aproximadamente 2 millones de líneas y 16 millones de palabras, y se midió el tiempo de ejecución de cada versión, con un promedio de 3 corridas. La versión flex tardó en promedio 1.73 segundos, mientras que la versión en C puro tardó en promedio 0.81 segundos. La versión en C puro fue aproximadamente 2.1 veces más rápida.

La razón de esta diferencia es que el escáner que genera flex construye internamente una tabla de estados (DFA) para reconocer patrones, y en cada carácter tiene que consultar esa tabla y a veces hacer backtracking para decidir dónde termina un match. La versión en C es más directa: un solo ciclo `while` con una comparación `isalpha` y una bandera, sin ninguna maquinaria de reconocimiento de patrones general.

En cuanto a la dificultad de depuración, la versión flex fue más fácil de escribir, ya que la lógica de contar letras seguidas la resuelve el patrón `[a-zA-Z]+` por sí solo. La versión en C puro fue más fácil de depurar paso a paso con un debugger normal, aunque hubo que pensar con más cuidado en la lógica de la bandera `in_word`.

En conclusión, flex gana en velocidad de desarrollo y legibilidad cuando los patrones son complejos, mientras que C puro gana en rendimiento crudo cuando el patrón es simple y se necesita procesar archivos grandes rápidamente.

### Evidencia de ejecución
 
Compilación de ambas versiones:
 
```
$ flex fb1-1.l
$ cc -o wc_flex lex.yy.c -lfl
 
$ cc -O2 -o wc_c wc_c.c
```
 
Prueba de equivalencia con un archivo de texto pequeño:
 
```
$ echo "Hola mundo, esto es una prueba.
Segunda linea con mas palabras 123 y simbolos!!" > prueba_chica.txt
 
--- flex ---
$ ./wc_flex < prueba_chica.txt
       2      13      80
 
--- C puro ---
$ ./wc_c < prueba_chica.txt
       2      13      80
```
 
Ambas versiones dieron el mismo resultado: 2 líneas, 13 palabras, 80 caracteres.
 
Archivo grande generado para la prueba de rendimiento (100 MB, aproximadamente 2 millones de líneas y 16 millones de palabras):
 
```
$ ls -lh archivo_grande.txt
-rw-r--r-- 1 root root 100M archivo_grande.txt
 
$ wc archivo_grande.txt
  1999999  15994556 104758553 archivo_grande.txt
```
 
Verificación de que ambas versiones dan el mismo resultado en el archivo grande:
 
```
$ ./wc_flex < archivo_grande.txt
 1999999 15994556 104758553
 
$ ./wc_c < archivo_grande.txt
 1999999 15994556 104758553
```
 
Tiempos de ejecución, tres corridas por cada versión:
 
```
=== Tiempo version FLEX ===
corrida 1: 1.724175049 s
corrida 2: 1.749044464 s
corrida 3: 1.729085080 s
 
=== Tiempo version C PURO ===
corrida 1: 0.811875433 s
corrida 2: 0.813062914 s
corrida 3: 0.816572690 s
```
 
Promedios: flex 1.734 s, C puro 0.814 s, lo que da un factor de mejora de aproximadamente 2.13 veces más rápido en la versión de C puro.
