#ifndef __ADDRESSING_MODE2_HANDLERS_H__
#define __ADDRESSING_MODE2_HANDLERS_H__

#include <stdint.h>
#include "cpu.h"

struct addressing_mode2_result {
	uint32_t current_addr;
	uint32_t writeback_addr;
	int do_writeback;
};
typedef struct addressing_mode2_result addressing_mode2_result;

addressing_mode2_result addressing_mode2_immediate(CPU_STATE* cpu, uint32_t i);
addressing_mode2_result addressing_mode2_scaled_register(CPU_STATE* cpu, uint32_t i);
addressing_mode2_result addressing_mode2_immediate_preindexed(CPU_STATE* cpu, uint32_t i);
addressing_mode2_result addressing_mode2_scaled_register_preindexed(CPU_STATE* cpu, uint32_t i);
addressing_mode2_result addressing_mode2_immediate_postindexed(CPU_STATE* cpu, uint32_t i);
addressing_mode2_result addressing_mode2_scaled_register_postindexed(CPU_STATE* cpu, uint32_t i);

#endif
