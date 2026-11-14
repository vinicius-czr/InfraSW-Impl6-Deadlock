#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "banker.h"
#include "commands.h"

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

static void print_commands(const CommandList *list, int num_resources) {
    int i, j;

    printf("commands.txt: %d comando(s)\n", list->count);
    for (i = 0; i < list->count; i++) {
        const Command *c = &list->items[i];

        if (c->type == CMD_PRINT) {
            printf("  [%d] *\n", i);
            continue;
        }

        printf("  [%d] %s customer=%d values=", i, c->type == CMD_REQUEST ? "RQ" : "RL", c->customer_id);
        for (j = 0; j < num_resources; j++) {
            printf("%d ", c->values[j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    BankerState state;
    CommandList commands;
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

    if (commands_load(&commands, "commands.txt", state.num_resources) != 0) {
        return 1;
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

        {
        int *sequence = malloc(state.num_customers * sizeof(int));
        int safe = banker_is_safe_state(&state, sequence);

        printf("is_safe_state: %s\n", safe ? "seguro" : "inseguro");
        if (safe) {
            printf("sequencia segura:");
            for (i = 0; i < state.num_customers; i++) {
                printf(" %d", sequence[i]);
            }
            printf("\n");
        }
        free(sequence);
    }
    
    print_commands(&commands, state.num_resources);

    return 0;
}