#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "fuzzer.h"

void* get_file(char *name, int *out_len) {
        FILE *f = fopen(name, "rb");
        if (f == NULL) {
                if (out_len)
                        *out_len = 0;
                return NULL;
        }
        
        struct stat st;
        stat(name, &st);
        
        void *data = malloc(st.st_size);
        if (data == NULL) {
                fclose(f);
                if (out_len)
                        *out_len = 0;
                return NULL;
        }
        
        fread(data, 1, st.st_size, f);
        fclose(f);
        
        if (out_len)
                *out_len = st.st_size;
        return data;
}

int main(int argc, char *argv[]) {

        if (argc != 3) {
                printf("Usage:\n");
                printf("\t%s <input_file> <app>\n", argv[0]);
                return 0;
        }

        int input_size = 0;
        void *input_data = get_file(argv[1], &input_size);
        if (input_data == NULL) {
                printf("Failed to read input file %s\n", argv[1]);
                return 1;
        }

        while (1) {
                struct fuzz_stats fs = {NULL, 0};
                int output_size = 0;
                void *modified_data = block_truncator_extender(input_data, input_size, &output_size, &fs);
                if (modified_data == NULL) {
                        modified_data = input_data;
                        output_size = input_size;
                } else {
                        free(input_data);
                }

                /*printf("Stats:\n");
                printf("\tFuzz type: %s\n", fs.fuzz_type);
                printf("\tModification made: %d\n", fs.modifications_made);*/

                FILE *output_f = fopen("fuzzer_output/data.bin", "wb");
                if (output_f == NULL) {
                        printf("Couldn't open output file %s\n", "fuzzer_output/data.bin");
                        free(modified_data);
                        return 1;
                }

                fwrite(modified_data, 1, output_size, output_f);
                fclose(output_f);

                //free(modified_data);

                char cmd[1024];
                snprintf(cmd, 1023, "gdb -q --return-child-result --batch -x dumper.py %s", argv[2]);
                int ret = system(cmd);
                if (ret != 0) {
                        printf("Crash detected\n");
                        break;
                }
        }
        printf("Done!\n");

        return 0;
}
