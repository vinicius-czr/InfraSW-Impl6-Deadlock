#include <stdio.h>
#include <string.h>
#include "banker.h"

static void format_vector(const int *v, int n, char *out) {
    int i;
    char buf[16];

    out[0] = '\0';
    for (i = 0; i < n; i++) {
        sprintf(buf, i == 0 ? "%d" : " %d", v[i]);
        strcat(out, buf);
    }
}

int banker_request(BankerState *state, int customer_id, const int *request, char *out_message) {
    int m = state->num_resources;
    Customer *c = &state->customers[customer_id];
    char req_str[128];
    int j;

    format_vector(request, m, req_str);

    for (j = 0; j < m; j++) {
        if (request[j] > c->need[j]) {
            sprintf(out_message, "The customer %d request %s was denied because exceed its maximum allocation", customer_id, req_str);
            return 0;
        }
    }

    for (j = 0; j < m; j++) {
        if (request[j] > state->available[j]) {
            char avail_str[128];
            format_vector(state->available, m, avail_str);
            sprintf(out_message, "The resources %s are not enough to customer %d request %s", avail_str, customer_id, req_str);
            return 0;
        }
    }

    for (j = 0; j < m; j++) {
        state->available[j] -= request[j];
        c->allocation[j] += request[j];
        c->need[j] -= request[j];
    }

    if (!banker_is_safe_state(state, NULL)) {
        for (j = 0; j < m; j++) {
            state->available[j] += request[j];
            c->allocation[j] -= request[j];
            c->need[j] += request[j];
        }
        sprintf(out_message, "The customer %d request %s was denied because result in an unsafe state", customer_id, req_str);
        return 0;
    }

    sprintf(out_message, "Allocate to customer %d the resources %s", customer_id, req_str);
    return 1;
}

void banker_release(BankerState *state, int customer_id, const int *release, char *out_message) {
    int m = state->num_resources;
    Customer *c = &state->customers[customer_id];
    char rel_str[128];
    int j;

    format_vector(release, m, rel_str);

    for (j = 0; j < m; j++) {
        state->available[j] += release[j];
        c->allocation[j] -= release[j];
        c->need[j] += release[j];
    }

    sprintf(out_message, "Release from customer %d the resources %s", customer_id, rel_str);
}