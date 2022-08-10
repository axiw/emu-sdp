#ifndef __FUZZER_H__
#define __FUZZER_H__

struct fuzz_stats {
        char *fuzz_type; // Name of the module you are implementing.
        int modifications_made; // Number of modifications that you made to the input, up to you to decide how to count it.
};

void* block_truncator_extender(void *input_data, int input_size, int *output_size, struct fuzz_stats* fs);

#endif
