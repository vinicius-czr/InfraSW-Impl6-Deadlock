#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

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

            cmd.type = (strcmp(op, "RQ") == 0) ? CMD_REQUEST : CMD_RELEASE;
            cmd.customer_id = atoi(token);
            cmd.values = malloc(num_resources * sizeof(int));

            for (i = 0; i < num_resources; i++) {
                token = strtok(NULL, " \t\n");
                cmd.values[i] = atoi(token);
            }
        } else {
            fprintf(stderr, "Erro: comando desconhecido '%s'\n", op);
            fclose(f);
            return -1;
        }

        list->items[list->count] = cmd;
        list->count++;
    }

    fclose(f);
    return 0;
}