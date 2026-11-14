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

#endif