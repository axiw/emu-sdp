#include <stdio.h>
#include <stdint.h>

#include "number_mutator.h"

void* number_mutator(void *input_data, int input_size, int *output_size, struct fuzz_stats* fs) {
    fs->fuzz_type = "Number Mutator";
    fs->modifications_made = 1;

    if (input_size < 4)
        return NULL;

    *(uint32_t*)input_data ^= 0xabcdef01;

    fs->modifications_made = 1;
    return NULL;
}