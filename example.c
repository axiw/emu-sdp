#include <stdio.h>
#include <stdint.h>

#include "fuzzer.h"

void* my_fuzz_function(void *input_data, int input_size, int *output_size, struct fuzz_stats* fs) {
        fs->fuzz_type = "Header corruptor";
        fs->modifications_made = 0;
        
        if (input_size < 4)
                return NULL;
        
        *(uint32_t*)input_data = 0xdeadbeef;
        fs->modifications_made = 1;
        return NULL;
}
