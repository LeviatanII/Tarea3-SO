# Simulador de asignación de archivos

Tarea 3 de Sistemas Operativos.

## Descripción

El objetivo del programa es simular en C tres mecanismos de asignación de archivos en disco: contigua, enlazada e indexada.

Actualmente, el programa lee y valida la configuración ingresada por el usuario. Luego calcula la cantidad de bloques completos que caben en el disco usando la equivalencia de 1 MB a 1024 KB.

Cuando se elige la asignación contigua, el programa representa el disco mediante un arreglo que registra cuáles bloques están libres y cuáles están ocupados. Para la prueba de esta etapa, se usa un archivo del tamaño máximo ingresado. El programa busca el primer tramo de bloques libres consecutivos donde pueda almacenarlo. Los bloques se numeran desde cero.

## Compilación

Desde la raíz del repositorio, compila con GCC.

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic main.c -o simulador
```

## Ejecución

```bash
./simulador
```

El programa solicita los siguientes datos. Ingresa un valor por línea, en el orden indicado.

1. Tamaño del disco en MB.
2. Tamaño de cada bloque en KB.
3. Mecanismo de asignación. Ingresa `1` para contigua, `2` para enlazada o `3` para indexada.
4. Tamaño mínimo de los archivos en KB.
5. Tamaño máximo de los archivos en KB.

Los tamaños deben ser enteros positivos. El disco debe tener capacidad para al menos un bloque completo y el tamaño máximo de los archivos debe ser mayor o igual que el mínimo.

La opción `1` ejecuta la prueba de asignación contigua. Las opciones `2` y `3` muestran el resumen de la configuración.

## Organización del código

- `main.c` contiene el punto de entrada del programa. Solicita los parámetros, valida la configuración y ejecuta la prueba de asignación contigua cuando corresponde.
- `leer_entero_positivo()` lee una línea y comprueba que contenga un entero positivo. Permite espacios alrededor del número y rechaza caracteres adicionales.
- `Disco` guarda el número total de bloques, el tamaño de cada bloque y el estado de ocupación del disco.
- `ArchivoContiguo` registra el tamaño del archivo, su bloque inicial y la cantidad de bloques que ocupa.
- `asignar_contiguo()` calcula cuántos bloques necesita el archivo, busca el primer tramo libre de ese tamaño y lo marca como ocupado.

## Prueba de ejecución

Configura un disco de 1 MB, con bloques de 16 KB, asignación contigua y archivos de 8 a 64 KB.

El resumen esperado es el siguiente.

```text
bloques disponibles: 64
mecanismo elegido: contigua
rango de archivos: 8 a 64 kb
archivo de prueba: 64 kb
bloques asignados: 4 (del 0 al 3)
bloques libres: 60
```

Si el tamaño máximo se cambia a 50 KB, el archivo también ocupa cuatro bloques, ya que tres bloques solo permiten almacenar 48 KB. Si se cambia a 1025 KB, el programa informa que el archivo no cabe.

Si se invierten los tamaños mínimo y máximo del ejemplo, el programa rechaza la configuración. El código de retorno es `1` si la configuración es inválida o si el disco no se puede representar en memoria. En los demás casos, el programa termina con código `0`.

## Avances

- **Configuración inicial del simulador.** Se incorporó la lectura y validación de parámetros, el cálculo de bloques completos y la documentación de compilación y ejecución.
- **Representación del disco y asignación contigua.** Se incorporó el registro de bloques libres y ocupados, la búsqueda del primer tramo libre y una prueba que muestra la ubicación del archivo y los bloques disponibles.
