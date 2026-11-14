#ifndef BANKER_H
#define BANKER_H
#define BANKER_MESSAGE_SIZE 256

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
int banker_request(BankerState *state, int customer_id, const int *request, char *out_message);
void banker_release(BankerState *state, int customer_id, const int *release, char *out_message);

#endif