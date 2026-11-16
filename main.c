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

static void print_state_block(FILE *out, const BankerState *state) {
    int i, j;
    int m = state->num_resources;

    fprintf(out, "MAXIMUM | ALLOCATION | NEED\n");
    for (i = 0; i < state->num_customers; i++) {
        Customer *c = &state->customers[i];

        for (j = 0; j < m; j++) {
            fprintf(out, j == 0 ? "%d" : " %d", c->maximum[j]);
        }
        fprintf(out, " | ");
        for (j = 0; j < m; j++) {
            fprintf(out, j == 0 ? "%d" : " %d", c->allocation[j]);
        }
        fprintf(out, " | ");
        for (j = 0; j < m; j++) {
            fprintf(out, j == 0 ? "%d" : " %d", c->need[j]);
        }
        fprintf(out, "\n");
    }

    fprintf(out, "AVAILABLE");
    for (j = 0; j < m; j++) {
        fprintf(out, " %d", state->available[j]);
    }
    fprintf(out, "\n");
}

int main(int argc, char **argv) {
    BankerState state;
    CommandList commands;
    FILE *out;
    int i;

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

    out = fopen("result.txt", "w");
    if (out == NULL) {
        fprintf(stderr, "Erro: nao foi possivel criar result.txt\n");
        return 1;
    }

    for (i = 0; i < commands.count; i++) {
        Command *c = &commands.items[i];
        char msg[BANKER_MESSAGE_SIZE];

        if (c->type == CMD_REQUEST) {
            banker_request(&state, c->customer_id, c->values, msg);
            fprintf(out, "%s\n", msg);
        } else if (c->type == CMD_RELEASE) {
            banker_release(&state, c->customer_id, c->values, msg);
            fprintf(out, "%s\n", msg);
        } else {
            print_state_block(out, &state);
        }
    }

    fclose(out);
    return 0;
}