#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "block_truncator_extender.h"

#define MUTATOR_CONST "fuzzzzzz"

void* block_truncator_extender(void *input_data, int input_size, int *output_size, struct fuzz_stats* fs) {
    fs->fuzz_type = "Block Trunc/Extendor";

    srand((unsigned int)time(NULL));
    int i = rand() % 256; // Don't go overboard.
    for(; i > 0; i--) {
        fs->modifications_made += 1;
        // Align ourselves on a random 64 byte block that's in the range of the input data.
        unsigned char b = (unsigned char)rand();
        char* h = input_data + (b * 64);
        if (h > (input_data + input_size + strlen(MUTATOR_CONST) + 1))
                h = input_data; // If we go over set ourselves at the head.
        memcpy(h, MUTATOR_CONST, strlen(MUTATOR_CONST));
    }

    return NULL;
}
