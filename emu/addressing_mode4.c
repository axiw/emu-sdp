#include <stdint.h>
#include <stdio.h>

// LUT generated from bits 24-23 inclusive, total of 2
#include "addressing_mode4_table.h"
#include "addressing_mode4_handlers.h"

#include "common.h"
#include "cpu.h"
#include "log.h"

addressing_mode4_result addressing_mode4_resolve(CPU_STATE* cpu, uint32_t i) {
	uint32_t lut_code = BITS(i, 23, 24);
	
	addressing_mode4_result (*handler)(CPU_STATE*, uint32_t) = addressing_mode4_func_table[addressing_mode4_lookup_table[lut_code]];
	
	if (handler == NULL) {
		ponii_log(LOG_LEVEL_ERROR, "Invalid addressing mode 4 encoding!\n");
		return (addressing_mode4_result){0,0,0,0};
	}
	
	return handler(cpu, i);
}

addressing_mode4_result addressing_mode4_inc_after(CPU_STATE* cpu, uint32_t i) {
	addressing_mode4_result r;
	uint8_t Rn = BITS(i, 16, 19);
	uint32_t register_list = BITS(i, 0, 15);
	
	int num_bits = 0;
	for (int i = 0; i <= 15; i++) {
		if (BIT(register_list, i) == 1)
			num_bits++;
	}
	
	r.start_addr = cpu->reg[Rn];
	r.end_addr = cpu->reg[Rn] + (num_bits * 4) - 4;
	r.writeback_addr = cpu->reg[Rn] + (num_bits * 4);
	r.register_list = register_list;
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "IA");
	return r;
}

addressing_mode4_result addressing_mode4_inc_before(CPU_STATE* cpu, uint32_t i) {
	addressing_mode4_result r;
	uint8_t Rn = BITS(i, 16, 19);
	uint32_t register_list = BITS(i, 0, 15);
	
	int num_bits = 0;
	for (int i = 0; i <= 15; i++) {
		if (BIT(register_list, i) == 1)
			num_bits++;
	}
	
	r.start_addr = cpu->reg[Rn] + 4;
	r.end_addr = cpu->reg[Rn] + (num_bits * 4);
	r.writeback_addr = cpu->reg[Rn] + (num_bits * 4);
	r.register_list = register_list;
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "IB");
	return r;
}

addressing_mode4_result addressing_mode4_dec_after(CPU_STATE* cpu, uint32_t i) {
	addressing_mode4_result r;
	uint8_t Rn = BITS(i, 16, 19);
	uint32_t register_list = BITS(i, 0, 15);
	
	int num_bits = 0;
	for (int i = 0; i <= 15; i++) {
		if (BIT(register_list, i) == 1)
			num_bits++;
	}
	
	r.start_addr = cpu->reg[Rn] - (num_bits * 4) + 4;
	r.end_addr = cpu->reg[Rn];
	r.writeback_addr = cpu->reg[Rn] - (num_bits * 4);
	r.register_list = register_list;
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "DA");
	return r;
}

addressing_mode4_result addressing_mode4_dec_before(CPU_STATE* cpu, uint32_t i) {
	addressing_mode4_result r;
	uint8_t Rn = BITS(i, 16, 19);
	uint32_t register_list = BITS(i, 0, 15);
	
	int num_bits = 0;
	for (int i = 0; i <= 15; i++) {
		if (BIT(register_list, i) == 1)
			num_bits++;
	}
	
	r.start_addr = cpu->reg[Rn] - (num_bits * 4);
	r.end_addr = cpu->reg[Rn] - 4;
	r.writeback_addr = cpu->reg[Rn] - (num_bits * 4);
	r.register_list = register_list;
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "DB");
	return r;
}
