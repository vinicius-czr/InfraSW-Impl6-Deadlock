#ifndef BANKER_H
#define BANKER_H

typedef struct {
    int *maximum;
    int *allocation;
    int *need;
} Customer;

typedef struct {
    int num_customers;
    int num_resources;
    int *available;
    Customer *customers;
} BankerState;

int banker_is_safe_state(const BankerState *state, int *safe_sequence);

#endif