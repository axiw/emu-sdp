#ifndef __ADDRESSING_MODE4_HANDLERS_H__
#define __ADDRESSING_MODE4_HANDLERS_H__

#include <stdint.h>
#include "cpu.h"

struct addressing_mode4_result {
	uint32_t start_addr;
	uint32_t end_addr;
	uint32_t writeback_addr;
	uint32_t register_list;
};
typedef struct addressing_mode4_result addressing_mode4_result;

addressing_mode4_result addressing_mode4_inc_after(CPU_STATE* cpu, uint32_t i);
addressing_mode4_result addressing_mode4_inc_before(CPU_STATE* cpu, uint32_t i);
addressing_mode4_result addressing_mode4_dec_after(CPU_STATE* cpu, uint32_t i);
addressing_mode4_result addressing_mode4_dec_before(CPU_STATE* cpu, uint32_t i);

#endif
