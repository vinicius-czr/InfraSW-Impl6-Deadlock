#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "banker.h"

static int load_customers(BankerState *state, const char *path) {
    FILE *f = fopen(path, "r");
    char line[256];
    int capacity = 8;
    int n = 0;

    if (f == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir %s\n", path);
        return -1;
    }

    state->customers = malloc(capacity * sizeof(Customer));

    while (fgets(line, sizeof(line), f) != NULL) {
        int values[32];
        int count = 0;
        char *token = strtok(line, ",\n");

        while (token != NULL) {
            values[count++] = atoi(token);
            token = strtok(NULL, ",\n");
        }

        if (count == 0) {
            continue;
        }

        if (n >= capacity) {
            capacity *= 2;
            state->customers = realloc(state->customers, capacity * sizeof(Customer));
        }

        state->customers[n].maximum = malloc(count * sizeof(int));
        state->customers[n].allocation = calloc(count, sizeof(int));
        state->customers[n].need = malloc(count * sizeof(int));
        memcpy(state->customers[n].maximum, values, count * sizeof(int));
        memcpy(state->customers[n].need, values, count * sizeof(int));

        state->num_resources = count;
        n++;
    }

    state->num_customers = n;
    fclose(f);
    return 0;
}

int main(int argc, char **argv) {
    BankerState state;
    int i, j;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <recurso_1> ... <recurso_N>\n", argv[0]);
        return 1;
    }

    if (load_customers(&state, "customer.txt") != 0) {
        return 1;
    }

    state.available = malloc(state.num_resources * sizeof(int));
    for (i = 0; i < state.num_resources; i++) {
        state.available[i] = atoi(argv[i + 1]);
    }

    printf("num_customers=%d num_resources=%d\n", state.num_customers, state.num_resources);
    printf("AVAILABLE:");
    for (j = 0; j < state.num_resources; j++) {
        printf(" %d", state.available[j]);
    }
    printf("\n");

    for (i = 0; i < state.num_customers; i++) {
        printf("customer %d MAXIMUM:", i);
        for (j = 0; j < state.num_resources; j++) {
            printf(" %d", state.customers[i].maximum[j]);
        }
        printf(" NEED:");
        for (j = 0; j < state.num_resources; j++) {
            printf(" %d", state.customers[i].need[j]);
        }
        printf("\n");
    }

    return 0;
}