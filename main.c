#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
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

    // comprueba que despues del numero solo queden espacios
    while (isspace((unsigned char)*fin)) {
        fin++;
    }
    if (*fin != '\0') {
        return 0;
    }

    *valor = (int)numero;
    return 1;
}

typedef struct {
    size_t total_bloques;
    int bloque_kb;
    unsigned char *ocupados;
} Disco;

typedef struct {
    int tamaño_kb;
    size_t bloque_inicial;
    size_t cantidad_bloques;
} ArchivoContiguo;

static int asignar_contiguo(Disco *disco, int tamaño_kb, ArchivoContiguo *archivo) {
    // si el tamaño deja un resto, se necesita un bloque mas
    size_t necesarios = (size_t)(tamaño_kb / disco->bloque_kb);
    if (tamaño_kb % disco->bloque_kb != 0) {
        necesarios++;
    }
    if (necesarios == 0 || necesarios > disco->total_bloques) {
        return 0;
    }

    size_t consecutivos = 0;
    for (size_t i = 0; i < disco->total_bloques; i++) {
        if (disco->ocupados[i]) {
            consecutivos = 0;
            continue;
        }

        consecutivos++;
        if (consecutivos == necesarios) {
            // marca el primer tramo libre que tiene el tamaño requerido
            size_t inicio = i - (necesarios - 1);
            memset(disco->ocupados + inicio, 1, necesarios);

            archivo->tamaño_kb = tamaño_kb;
            archivo->bloque_inicial = inicio;
            archivo->cantidad_bloques = necesarios;
            return 1;
        }
    }

    return 0;
}

int main(void) {
    int disco_mb;
    int bloque_kb;
    int mecanismo;
    int minimo_kb;
    int maximo_kb;

    if (!leer_entero_positivo("tamaño del disco (mb): ", &disco_mb) ||
        !leer_entero_positivo("tamaño de cada bloque (kb): ", &bloque_kb)) {
        fputs("ingresa tamaños enteros positivos\n", stderr);
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

    if (mecanismo != 1) {
        return 0;
    }

    if ((uintmax_t)total_bloques > SIZE_MAX) {
        fputs("el disco tiene demasiados bloques para representarlo en memoria\n", stderr);
        return 1;
    }

    Disco disco = {
        .total_bloques = (size_t)total_bloques,
        .bloque_kb = bloque_kb
    };

    // calloc inicializa todos los bloques como libres
    disco.ocupados = calloc(disco.total_bloques, sizeof *disco.ocupados);
    if (disco.ocupados == NULL) {
        fputs("no se pudo reservar memoria para el disco\n", stderr);
        return 1;
    }

    // usa el tamaño maximo para repetir la misma prueba con los mismos datos
    ArchivoContiguo archivo;
    if (asignar_contiguo(&disco, maximo_kb, &archivo)) {
        size_t ultimo = archivo.bloque_inicial + (archivo.cantidad_bloques - 1);
        printf("archivo de prueba: %d kb\n", archivo.tamaño_kb);
        printf("bloques asignados: %zu (del %zu al %zu)\n",
               archivo.cantidad_bloques, archivo.bloque_inicial, ultimo);
        printf("bloques libres: %zu\n", disco.total_bloques - archivo.cantidad_bloques);
    } else {
        puts("no hay suficientes bloques contiguos para el archivo de prueba");
    }

    free(disco.ocupados);
    return 0;
}
