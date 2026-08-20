# Ejercicios 5 y 6 — Flex & Bison, Capítulo 1

## Ejercicio 5

¿Para qué lenguajes flex NO sería una buena herramienta?

Flex genera analizadores basados en expresiones regulares, y las expresiones regulares solo pueden reconocer lenguajes regulares (los que se describen con un autómata finito, sin memoria de lo que vino antes más allá de un número fijo de estados). Cualquier lenguaje que necesite "recordar" una cantidad no acotada de información no puede reconocerse solo con flex.

Ejemplos concretos donde flex no alcanza:

Lenguajes con anidamiento o balanceo, por ejemplo verificar que los paréntesis de una expresión matemática están bien balanceados como en `((3+4)*(5-2))`. Un autómata finito no puede contar cuántos paréntesis lleva abiertos si ese número puede crecer indefinidamente; se necesita una pila, es decir, un autómata de pila. Eso es justo lo que hace Bison (un lenguaje libre de contexto), no flex.

XML/HTML, donde hay que validar que cada etiqueta de apertura como `<div>` tenga su cierre correspondiente `</div>` con el mismo nombre y en el orden correcto. Es otra vez un problema de anidamiento no regular.

Lenguajes sensibles a indentación, como Python, que usa la sangría para delimitar bloques de código. Flex no tiene forma natural de recordar el nivel de indentación anterior y compararlo con el actual; normalmente se resuelve con lógica extra fuera del propio flex, como contadores de espacios llevados a mano.

Comentarios anidados: si un lenguaje permite comentarios dentro de comentarios, como `/* uno /* dos */ */`, una expresión regular simple no puede llevar la cuenta de cuántos niveles de anidamiento lleva abiertos.

Conclusión: flex es excelente para la parte léxica, es decir, identificar palabras, números y operadores, pero para cualquier estructura recursiva o anidada se necesita algo con más poder computacional, como Bison (gramáticas libres de contexto) o lógica adicional escrita a mano.

## Ejercicio 6

Reescribir el contador de palabras (wc) en C puro, sin flex, y comparar velocidad y dificultad de depuración con la versión original.

La versión original del libro es el Ejemplo 1-1 (`fb1-1.l`), que usa el patrón `[a-zA-Z]+` para contar palabras. La versión en C puro que se hizo manualmente está en el archivo `wc_c.c` adjunto.

La idea del algoritmo en C puro es usar una bandera llamada `in_word` que indica si el caracter anterior era parte de una palabra. Cada vez que se pasa de "no letra" a "letra" se cuenta una palabra nueva, así se evita contar la misma palabra letra por letra.

Se probó la equivalencia de ambas versiones corriéndolas sobre el mismo texto, y dieron resultados idénticos en líneas, palabras y caracteres.

Para la prueba de rendimiento se generó un archivo de 100 MB, con aproximadamente 2 millones de líneas y 16 millones de palabras, y se midió el tiempo de ejecución de cada versión, con un promedio de 3 corridas. La versión flex tardó en promedio 1.73 segundos, mientras que la versión en C puro tardó en promedio 0.81 segundos. La versión en C puro fue aproximadamente 2.1 veces más rápida.

La razón de esta diferencia es que el escáner que genera flex construye internamente una tabla de estados (DFA) para reconocer patrones, y en cada carácter tiene que consultar esa tabla y a veces hacer backtracking para decidir dónde termina un match. La versión en C es más directa: un solo ciclo `while` con una comparación `isalpha` y una bandera, sin ninguna maquinaria de reconocimiento de patrones general. Cuando el patrón es tan simple como "una letra sí o no", ese motor genérico de flex resulta un costo innecesario.

En cuanto a la dificultad de depuración, la versión flex fue más fácil de escribir, ya que la lógica de contar letras seguidas la resuelve el patrón `[a-zA-Z]+` por sí solo, pero es más difícil de entender a fondo qué pasa internamente si algo falla, porque el código real que se ejecuta (`lex.yy.c`) es generado automáticamente y mucho más largo. La versión en C puro fue más fácil de depurar paso a paso con un debugger normal, ya que es código explícito donde uno mismo controla cada condición, aunque hubo que pensar con más cuidado en la lógica de la bandera `in_word` para no contar palabras de más o de menos.

En conclusión, flex gana en velocidad de desarrollo y legibilidad cuando los patrones son complejos, mientras que C puro gana en rendimiento crudo cuando el patrón es simple y se necesita procesar archivos grandes rápidamente.
