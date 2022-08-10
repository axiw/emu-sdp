#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "bit_flipper.h"

void* bit_flipper(void *input_data, int input_size, int *output_size, struct fuzz_stats* fs) {
    fs->fuzz_type = "Bit Flipper";

    if (input_size < 4)
        return NULL;

    srand(time(NULL));
    fs->modifications_made = rand() % (input_size - 2) + 1;

    for(int i = 0; i <= fs->modifications_made; i++) {
        char* mask_buf = NULL;
        int random_bit_to_flip = rand() % (input_size - 3) + 2;
        unsigned int length = asprintf(&mask_buf, "%x", random_bit_to_flip);

        int mask = (int)strtol(mask_buf, NULL, 0);
        *(int*)input_data ^= mask;

        free(mask_buf);
    }

    return NULL;
}
