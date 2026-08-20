# Ejercicios 1, 2, 5 y 6 — Flex & Bison, Capítulo 1

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

### Capturas de las pruebas

<!-- Insertar aquí las capturas correspondientes al Ejercicio 6 -->

![Prueba Ejercicio 6](imagenes/ejercicio6.png)
