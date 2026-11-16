#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int commands_load(CommandList *list, const char *path, int num_resources) {
    FILE *f = fopen(path, "r");
    char line[256];
    int capacity = 8;

    if (f == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir %s\n", path);
        return -1;
    }

    list->items = malloc(capacity * sizeof(Command));
    list->count = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        char *op = strtok(line, " \t\n");
        Command cmd;

        if (op == NULL) {
            continue;
        }

        if (list->count >= capacity) {
            capacity *= 2;
            list->items = realloc(list->items, capacity * sizeof(Command));
        }

        if (strcmp(op, "*") == 0) {
            cmd.type = CMD_PRINT;
            cmd.customer_id = -1;
            cmd.values = NULL;
        } else if (strcmp(op, "RQ") == 0 || strcmp(op, "RL") == 0) {
            int i;
            char *token = strtok(NULL, " \t\n");

            if (token == NULL || parse_int_strict(token, &cmd.customer_id) != 0) {
                fprintf(stderr, "Erro: %s esta em formato invalido\n", path);
                fclose(f);
                return -1;
            }

            cmd.type = (strcmp(op, "RQ") == 0) ? CMD_REQUEST : CMD_RELEASE;
            cmd.values = malloc(num_resources * sizeof(int));

            for (i = 0; i < num_resources; i++) {
                token = strtok(NULL, " \t\n");
                if (token == NULL || parse_int_strict(token, &cmd.values[i]) != 0) {
                    fprintf(stderr, "Erro: %s esta em formato invalido\n", path);
                    fclose(f);
                    return -1;
                }
            }
        } else {
            fprintf(stderr, "Erro: comando desconhecido '%s' em %s\n", op, path);
            fclose(f);
            return -1;
        }

        list->items[list->count] = cmd;
        list->count++;
    }

    fclose(f);
    return 0;
}