# Simulador de asignación de archivos

Tarea 3 de Sistemas Operativos.

## Descripción

El objetivo del programa es simular en C tres mecanismos de asignación de archivos en disco: contigua, enlazada e indexada.

Actualmente el programa lee y valida la configuración ingresada por el usuario. Luego calcula la cantidad de bloques completos que caben en el disco usando la equivalencia de 1 MB a 1024 KB.

## Compilación

Desde la raíz del repositorio, compilar con gcc.

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

## Organización del código

- `main.c` contiene el punto de entrada del programa. Solicita los parámetros, valida la configuración y muestra el resumen.
- `leer_entero_positivo()` lee una línea y comprueba que contenga un entero positivo. Permite espacios alrededor del número y rechaza caracteres adicionales.

## Prueba de ejecución

Configurando como ejemplo un disco de 1 MB, con bloques de 16 KB, asignación contigua y archivos de 8 a 64 KB

El resumen esperado es el siguiente.

```text
bloques disponibles: 64
mecanismo elegido: contigua
rango de archivos: 8 a 64 kb
```

Si se invierten los dos últimos valores, el programa muestra un error porque el tamaño mínimo supera al máximo. El código de retorno es `0` cuando la configuración es válida y `1` cuando se rechaza.

## Avances

- **Configuración inicial del simulador.** Se incorporó la lectura y validación de parámetros, el cálculo de bloques completos y la documentación de compilación y ejecución.
