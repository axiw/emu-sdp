#ifndef __ADDRESSING_MODE3_HANDLERS_H__
#define __ADDRESSING_MODE3_HANDLERS_H__

#include <stdint.h>
#include "cpu.h"

struct addressing_mode3_result {
	uint32_t current_addr;
	uint32_t writeback_addr;
	int do_writeback;
};
typedef struct addressing_mode3_result addressing_mode3_result;

addressing_mode3_result addressing_mode3_immediate(CPU_STATE* cpu, uint32_t i);
addressing_mode3_result addressing_mode3_register(CPU_STATE* cpu, uint32_t i);
addressing_mode3_result addressing_mode3_immediate_preindex(CPU_STATE* cpu, uint32_t i);
addressing_mode3_result addressing_mode3_register_preindex(CPU_STATE* cpu, uint32_t i);
addressing_mode3_result addressing_mode3_immediate_postindex(CPU_STATE* cpu, uint32_t i);
addressing_mode3_result addressing_mode3_register_postindex(CPU_STATE* cpu, uint32_t i);

#endif
