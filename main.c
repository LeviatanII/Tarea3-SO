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
    size_t bloques_libres;
    uint64_t bytes_bloque;
    unsigned char *ocupados;
    uint64_t *siguientes;
} Disco;

typedef struct {
    int tamaño_kb;
    size_t bloque_inicial;
    size_t cantidad_bloques;
} Archivo;

static uint64_t calcular_bloques(int tamaño_kb, uint64_t bytes_utiles) {
    uint64_t bytes_archivo = (uint64_t)tamaño_kb * 1024;
    uint64_t necesarios = bytes_archivo / bytes_utiles;

    // si el tamaño deja un resto, se necesita un bloque mas
    if (bytes_archivo % bytes_utiles != 0) {
        necesarios++;
    }
    return necesarios;
}

static int asignar_contiguo(Disco *disco, Archivo *archivo) {
    uint64_t necesarios = calcular_bloques(archivo->tamaño_kb, disco->bytes_bloque);
    if (necesarios > disco->bloques_libres) {
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
            size_t inicio = i - (consecutivos - 1);
            memset(disco->ocupados + inicio, 1, consecutivos);

            archivo->bloque_inicial = inicio;
            archivo->cantidad_bloques = consecutivos;
            disco->bloques_libres -= consecutivos;
            return 1;
        }
    }

    return 0;
}

static int asignar_enlazado(Disco *disco, Archivo *archivo) {
    // cada bloque reserva 8 bytes para guardar el indice del siguiente
    uint64_t bytes_utiles = disco->bytes_bloque - sizeof *disco->siguientes;
    uint64_t necesarios = calcular_bloques(archivo->tamaño_kb, bytes_utiles);
    if (necesarios > disco->bloques_libres) {
        return 0;
    }

    size_t anterior = 0;
    size_t asignados = 0;
    for (size_t i = 0; i < disco->total_bloques && asignados < necesarios; i++) {
        if (disco->ocupados[i]) {
            continue;
        }

        disco->ocupados[i] = 1;
        disco->siguientes[i] = UINT64_MAX;

        if (asignados == 0) {
            archivo->bloque_inicial = i;
        } else {
            // conecta el bloque anterior con el bloque libre que encontramos
            disco->siguientes[anterior] = i;
        }

        anterior = i;
        asignados++;
    }

    archivo->cantidad_bloques = asignados;
    disco->bloques_libres -= asignados;
    return 1;
}

static void mostrar_cadena(const Disco *disco, const Archivo *archivo) {
    uint64_t actual = archivo->bloque_inicial;

    printf("cadena de bloques: ");
    while (actual != UINT64_MAX) {
        printf("%zu -> ", (size_t)actual);
        actual = disco->siguientes[actual];
    }
    puts("fin");
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

    if (mecanismo == 3) {
        return 0;
    }

    if ((uintmax_t)total_bloques > SIZE_MAX) {
        fputs("el disco tiene demasiados bloques para representarlo en memoria\n", stderr);
        return 1;
    }

    Disco disco = {
        .total_bloques = (size_t)total_bloques,
        .bloques_libres = (size_t)total_bloques,
        .bytes_bloque = (uint64_t)bloque_kb * 1024
    };

    // calloc inicializa todos los bloques como libres
    disco.ocupados = calloc(disco.total_bloques, sizeof *disco.ocupados);
    if (mecanismo == 2) {
        disco.siguientes = calloc(disco.total_bloques, sizeof *disco.siguientes);
    }
    if (disco.ocupados == NULL || (mecanismo == 2 && disco.siguientes == NULL)) {
        fputs("no se pudo reservar memoria para el disco\n", stderr);
        free(disco.siguientes);
        free(disco.ocupados);
        return 1;
    }

    // usa el tamaño maximo para repetir la prueba con los mismos datos
    Archivo archivo = {.tamaño_kb = maximo_kb};
    int asignado = mecanismo == 1 ? asignar_contiguo(&disco, &archivo)
                                  : asignar_enlazado(&disco, &archivo);

    if (asignado) {
        printf("archivo de prueba: %d kb\n", archivo.tamaño_kb);
        if (mecanismo == 1) {
            size_t ultimo = archivo.bloque_inicial + (archivo.cantidad_bloques - 1);
            printf("bloques asignados: %zu (del %zu al %zu)\n",
                   archivo.cantidad_bloques, archivo.bloque_inicial, ultimo);
        } else {
            printf("bloques asignados: %zu\n", archivo.cantidad_bloques);
            mostrar_cadena(&disco, &archivo);
        }
        printf("bloques libres: %zu\n", disco.bloques_libres);
    } else if (mecanismo == 1) {
        puts("no hay suficientes bloques contiguos para el archivo de prueba");
    } else {
        puts("no hay suficientes bloques libres para el archivo de prueba");
    }

    free(disco.siguientes);
    free(disco.ocupados);
    return 0;
}
