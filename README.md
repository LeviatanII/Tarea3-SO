# Simulador de asignación de archivos

Tarea 3 de Sistemas Operativos.

## Descripción

El objetivo del programa es simular en C tres mecanismos de asignación de archivos en disco: contigua, enlazada e indexada.

El programa lee y valida la configuración ingresada por el usuario. Luego calcula la cantidad de bloques completos que caben en el disco usando la equivalencia de 1 MB a 1024 KB.

Los bloques se numeran desde cero. El disco se representa mediante un arreglo que registra cuáles bloques están libres y cuáles están ocupados.

En la asignación contigua, el programa busca el primer tramo de bloques libres consecutivos donde quepa el archivo. En la asignación enlazada, ocupa los primeros bloques libres que encuentra y registra el enlace al siguiente bloque de la cadena.

En el modo enlazado, cada bloque reserva 8 bytes para una referencia de tipo `uint64_t`. El último bloque usa ese espacio para guardar `UINT64_MAX` como marca de fin. La cantidad de bloques que necesita un archivo se calcula con el espacio útil de cada bloque.

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

Las opciones `1` y `2` prueban la asignación con un archivo del tamaño máximo ingresado. La opción `3` muestra el resumen de la configuración.

## Organización del código

- `main.c` contiene el punto de entrada del programa. Solicita los parámetros, valida la configuración, reserva la memoria del disco y ejecuta la prueba correspondiente.
- `leer_entero_positivo()` lee y valida cada valor ingresado.
- `Disco` guarda el tamaño de cada bloque, la cantidad de bloques libres, el estado de ocupación y, en el modo enlazado, la referencia al siguiente bloque.
- `Archivo` registra el tamaño del archivo, su bloque inicial y la cantidad de bloques que ocupa.
- `calcular_bloques()` convierte el tamaño del archivo a bytes y redondea hacia arriba la cantidad de bloques necesarios.
- `asignar_contiguo()` busca y ocupa el primer tramo libre donde cabe el archivo.
- `asignar_enlazado()` comprueba que haya suficientes bloques libres y los conecta en una cadena.
- `mostrar_cadena()` parte del bloque inicial y sigue las referencias hasta llegar al fin de la cadena.

## Pruebas de ejecución

Configura un disco de 1 MB, con bloques de 16 KB y archivos de 8 a 64 KB. Con la opción `1`, el archivo de 64 KB ocupa los bloques del 0 al 3 y quedan 60 bloques libres.

Con la opción `2`, la asignación muestra el siguiente resultado.

```text
archivo de prueba: 64 kb
bloques asignados: 5
cadena de bloques: 0 -> 1 -> 2 -> 3 -> 4 -> fin
bloques libres: 59
```

Para probar el límite de capacidad en modo enlazado, configura un disco de 1 MB con bloques de 8 KB. Un archivo de 1023 KB ocupa los 128 bloques disponibles. Si el tamaño máximo se cambia a 1024 KB, el programa informa que el archivo no cabe.

El programa devuelve `1` si la configuración es inválida o si el disco no se puede representar en memoria. En los demás casos devuelve `0`.

## Avances

- **Configuración inicial del simulador.** Se incorporó la lectura y validación de parámetros, el cálculo de bloques completos y la documentación de compilación y ejecución.
- **Representación del disco y asignación contigua.** Se incorporó el registro de bloques libres y ocupados y la búsqueda del primer tramo libre donde cabe un archivo.
- **Asignación enlazada.** Se incorporó la cadena de bloques, el descuento de 8 bytes por enlace y la comprobación de espacio libre antes de asignar el archivo.
