#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum {
    CMD_REQUEST,
    CMD_RELEASE,
    CMD_PRINT
} CommandType;

typedef struct {
    CommandType type;
    int customer_id;
    int *values;
} Command;

typedef struct {
    Command *items;
    int count;
} CommandList;

int commands_load(CommandList *list, const char *path, int num_resources);

#endif