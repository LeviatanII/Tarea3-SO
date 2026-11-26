#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int leer_entero_positivo(const char *mensaje, int *valor) {
    char linea[128];
    char *fin;

    fputs(mensaje, stdout);
    fflush(stdout);

    if (fgets(linea, sizeof linea, stdin) == NULL) {
        return 0;
    }

    // rechaza una linea que supera el espacio disponible
    if (strchr(linea, '\n') == NULL && !feof(stdin)) {
        return 0;
    }

    errno = 0;
    long numero = strtol(linea, &fin, 10);
    if (fin == linea || errno == ERANGE || numero < 1 || numero > INT_MAX) {
        return 0;
    }

    // permite espacios al final, pero rechaza otros caracteres
    while (isspace((unsigned char)*fin)) {
        fin++;
    }
    if (*fin != '\0') {
        return 0;
    }

    *valor = (int)numero;
    return 1;
}

int main(void) {
    int disco_mb;
    int bloque_kb;
    int mecanismo;
    int minimo_kb;
    int maximo_kb;

    if (!leer_entero_positivo("tamaño del disco (mb): ", &disco_mb) ||
        !leer_entero_positivo("tamaño de cada bloque (kb): ", &bloque_kb)) {
        fputs("ingrese tamaños enteros positivos\n", stderr);
        return 1;
    }

    // convierte a kb y cuenta solo los bloques completos
    long long total_bloques = disco_mb * 1024LL / bloque_kb;
    if (total_bloques == 0) {
        fputs("el disco no alcanza para un bloque completo\n", stderr);
        return 1;
    }

    puts("1. contigua\n2. enlazada\n3. indexada");
    if (!leer_entero_positivo("mecanismo: ", &mecanismo) || mecanismo > 3) {
        fputs("elige 1, 2 o 3\n", stderr);
        return 1;
    }

    if (!leer_entero_positivo("tamaño minimo del archivo (kb): ", &minimo_kb) ||
        !leer_entero_positivo("tamaño maximo del archivo (kb): ", &maximo_kb) ||
        minimo_kb > maximo_kb) {
        fputs("ingresa un rango positivo con minimo menor o igual al maximo\n", stderr);
        return 1;
    }

    const char *nombres[] = {"contigua", "enlazada", "indexada"};
    printf("bloques disponibles: %lld\n", total_bloques);
    printf("mecanismo elegido: %s\n", nombres[mecanismo - 1]);
    printf("rango de archivos: %d a %d kb\n", minimo_kb, maximo_kb);

    return 0;
}
