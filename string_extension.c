#define _GNU_SOURCE

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "string_extension.h"

void* string_extension(void *input_data, int input_size, int *output_size, struct fuzz_stats* fs) {
    fs->fuzz_type = "String Extender";
    fs->modifications_made = 1;

    char* buff = NULL;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    if (input_size < 4)
        return NULL;

    int length = asprintf(&buff, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1,
                          tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    input_data = (char*)input_data;
    input_data += length;

    free(buff);

    return NULL;
}