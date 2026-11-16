#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "banker.h"
#include "commands.h"

static int parse_int_strict(const char *s, int *out) {
    char *endptr;
    long v;

    if (s == NULL || *s == '\0') {
        return -1;
    }
    v = strtol(s, &endptr, 10);
    if (*endptr != '\0') {
        return -1;
    }
    *out = (int) v;
    return 0;
}

static int load_customers(BankerState *state, const char *path) {
    FILE *f = fopen(path, "r");
    char line[256];
    int capacity = 8;
    int n = 0;
    int expected_resources = -1;

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
            int v;

            if (count >= 32 || parse_int_strict(token, &v) != 0) {
                fprintf(stderr, "Erro: %s esta em formato invalido\n", path);
                fclose(f);
                return -1;
            }
            values[count++] = v;
            token = strtok(NULL, ",\n");
        }

        if (count == 0) {
            continue;
        }

        if (expected_resources == -1) {
            expected_resources = count;
        } else if (count != expected_resources) {
            fprintf(stderr, "Erro: %s esta em formato invalido\n", path);
            fclose(f);
            return -1;
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

    fclose(f);

    if (n == 0) {
        fprintf(stderr, "Erro: %s nao contem nenhum cliente\n", path);
        return -1;
    }

    state->num_customers = n;
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

    for (i = 1; i < argc; i++) {
        int v;
        if (parse_int_strict(argv[i], &v) != 0 || v < 0) {
            fprintf(stderr, "Erro: argumento '%s' nao e um numero inteiro valido\n", argv[i]);
            return 1;
        }
    }

    if (load_customers(&state, "customer.txt") != 0) {
        return 1;
    }

    if (argc - 1 != state.num_resources) {
        fprintf(stderr, "Erro: numero de argumentos (%d) nao bate com o numero de recursos em customer.txt (%d)\n",
                argc - 1, state.num_resources);
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