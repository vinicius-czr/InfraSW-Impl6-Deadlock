#include <stdlib.h>
#include "banker.h"

int banker_is_safe_state(const BankerState *state, int *safe_sequence) {
    int n = state->num_customers;
    int m = state->num_resources;
    int *work = malloc(m * sizeof(int));
    int *finish = calloc(n, sizeof(int));
    int seq_index = 0;
    int i, j, found;

    for (j = 0; j < m; j++) {
        work[j] = state->available[j];
    }

    do {
        found = 0;
        for (i = 0; i < n; i++) {
            int can_proceed = 1;

            if (finish[i]) {
                continue;
            }

            for (j = 0; j < m; j++) {
                if (state->customers[i].need[j] > work[j]) {
                    can_proceed = 0;
                    break;
                }
            }

            if (can_proceed) {
                for (j = 0; j < m; j++) {
                    work[j] += state->customers[i].allocation[j];
                }
                finish[i] = 1;
                if (safe_sequence != NULL) {
                    safe_sequence[seq_index] = i;
                }
                seq_index++;
                found = 1;
            }
        }
    } while (found);

    free(work);

    for (i = 0; i < n; i++) {
        if (!finish[i]) {
            free(finish);
            return 0;
        }
    }

    free(finish);
    return 1;
}