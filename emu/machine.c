#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "linux_hle.h"
#include "machine.h"
#include "mmu.h"

MACHINE_STATE* machine_init(const char* fs_root, uint32_t n_qd, uint32_t n_fd, uint32_t n_hd, uint32_t n_td) {
	MACHINE_STATE* ms = calloc(sizeof(MACHINE_STATE), 1);
	ms->mmu = mmu_init();
	ms->linux_ctx = linux_init(fs_root, n_qd, n_fd, n_hd, n_td);
	
	return ms;
}

void machine_free(MACHINE_STATE* ms) {
	linux_free(ms->linux_ctx);
	mmu_free(ms->mmu);
	free(ms);
}

void machine_dump(MACHINE_STATE *ms) {
	mmu_dump_mappings(ms->mmu);
	
	for (int i = 0; i < 14; i++)
		printf("r%02d: 0x%08X\n", i, linux_get_thread_cpu(ms)->reg[i]);
	printf("LR: 0x%08X\n", linux_get_thread_cpu(ms)->reg[REG_LR]);
	printf("PC: 0x%08X\n", linux_get_thread_cpu(ms)->reg[REG_PC]);
	printf("N: %d\n", cpu_get_cpsr_field(linux_get_thread_cpu(ms), CPSR_N));
	printf("Z: %d\n", cpu_get_cpsr_field(linux_get_thread_cpu(ms), CPSR_Z));
	printf("C: %d\n", cpu_get_cpsr_field(linux_get_thread_cpu(ms), CPSR_C));
	printf("V: %d\n", cpu_get_cpsr_field(linux_get_thread_cpu(ms), CPSR_V));
}
