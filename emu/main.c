#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "cpu.h"
#include "log.h"
#include "mmu.h"
#include "elf_loader.h"
#include "linux_hle.h"
#include "breakpoint.h"

int invalid_opcode_done;

int main(int argc, char *argv[]) {
	//log_level = LOG_LEVEL_ERROR;
        log_level = LOG_LEVEL_EXTRA_INFO;
	if (argc != 3) {
		ponii_log(LOG_LEVEL_ERROR, "Usage:\n");
		ponii_log(LOG_LEVEL_ERROR, "\t%s root_fs target.elf\n", argv[0]);
		return 0;
	}
	
	FILE* f_in = fopen(argv[2], "rb");
	fseek(f_in, 0L, SEEK_END);
	size_t target_len = ftell(f_in);
	rewind(f_in);
	void* in = malloc(target_len);
	fread(in, target_len, 1, f_in);
	fclose(f_in);
	
	MACHINE_STATE* ms = machine_init(argv[1], 255, 255, 255, 255);
	int error = elf_file_load(ms, in);
	free(in);
	
	if (error) {
		ponii_log(LOG_LEVEL_ERROR, "Failed to load elf file!\n");
		machine_free(ms);
		return 0;
	}
	
	while (1) {
		if (invalid_opcode_done == 1) {
			break;
		}
		if (is_breakpoint(ms, linux_get_thread_cpu(ms)->reg[REG_PC]) == 1) {
			ponii_log(LOG_LEVEL_INFO, "Hit breakpoint at 0x%08X\n", linux_get_thread_cpu(ms)->reg[REG_PC]);
			machine_dump(ms);
			fgetc(stdin);
		}
		cpu_advance(ms);
	}
	
	
	
	machine_dump(ms);
	machine_free(ms);
	
	return 0;
}
