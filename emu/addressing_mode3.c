#include <stdint.h>
#include <stdio.h>

// LUT generated from bits 24,22-21 inclusive, total of 3
#include "addressing_mode3_table.h"
#include "addressing_mode3_handlers.h"

#include "common.h"
#include "cpu.h"
#include "log.h"

addressing_mode3_result addressing_mode3_resolve(CPU_STATE* cpu, uint32_t i) {
	uint32_t lut_code = (BIT(i, 24) << 2) | BITS(i, 21, 22);
	
	addressing_mode3_result (*handler)(CPU_STATE*, uint32_t) = addressing_mode3_func_table[addressing_mode3_lookup_table[lut_code]];
	
	if (handler == NULL) {
		ponii_log(LOG_LEVEL_ERROR, "Invalid addressing mode 3 encoding!\n");
		return (addressing_mode3_result){0,0,0};
	}
	
	return handler(cpu, i);
}

addressing_mode3_result addressing_mode3_immediate(CPU_STATE* cpu, uint32_t i) {
	addressing_mode3_result r;
	uint8_t immedL = BITS(i, 0, 3);
	uint8_t immedH = BITS(i, 8, 11);
	uint8_t Rn = BITS(i, 16, 19);
	uint8_t U = BIT(i, 23);
	
	uint8_t offset_8 = (immedH << 4) | immedL;
	if (U == 1) {
		r.current_addr = cpu->reg[Rn] + offset_8;
	} else {
		r.current_addr = cpu->reg[Rn] - offset_8;
	}
	
	if (offset_8 != 0) {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d, %s#0x%08X]", Rn, U ? "" : "-", offset_8);
	} else {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d]", Rn);
	}
	r.writeback_addr = 0;
	r.do_writeback = 0;
	return r;
}

addressing_mode3_result addressing_mode3_register(CPU_STATE* cpu, uint32_t i) {
	addressing_mode3_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t Rn = BITS(i, 16, 19);
	uint8_t U = BIT(i, 23);
	
	//SBZ(i, 8, 11); //TODO
	
	if (U == 1) {
		r.current_addr = cpu->reg[Rn] + cpu->reg[Rm];
	} else {
		r.current_addr = cpu->reg[Rn] - cpu->reg[Rm];
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d, %sr%d]", Rn, U ? "" : "-", Rm);
	r.writeback_addr = 0;
	r.do_writeback = 0;
	return r;
}

addressing_mode3_result addressing_mode3_immediate_preindex(CPU_STATE* cpu, uint32_t i) {
	addressing_mode3_result r;
	uint8_t immedL = BITS(i, 0, 3);
	uint8_t immedH = BITS(i, 8, 11);
	uint8_t Rn = BITS(i, 16, 19);
	uint8_t U = BIT(i, 23);
	
	uint8_t offset_8 = (immedH << 4) | immedL;
	if (U == 1) {
		r.current_addr = cpu->reg[Rn] + offset_8;
	} else {
		r.current_addr = cpu->reg[Rn] - offset_8;
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d, %s#0x%08X]!", Rn, U ? "" : "-", offset_8); // Abbreviations are apparently not allowed according to the spec (A5.3.4)
	r.writeback_addr = r.current_addr;
	r.do_writeback = 1;
	return r;
}

addressing_mode3_result addressing_mode3_register_preindex(CPU_STATE* cpu, uint32_t i) {
	addressing_mode3_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t Rn = BITS(i, 16, 19);
	uint8_t U = BIT(i, 23);
	
	//SBZ(i, 8, 11); //TODO
	
	if (U == 1) {
		r.current_addr = cpu->reg[Rn] + cpu->reg[Rm];
	} else {
		r.current_addr = cpu->reg[Rn] - cpu->reg[Rm];
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d, %sr%d]!", Rn, U ? "" : "-", Rm);
	r.writeback_addr = r.current_addr;
	r.do_writeback = 1;
	return r;
}

addressing_mode3_result addressing_mode3_immediate_postindex(CPU_STATE* cpu, uint32_t i) {
	addressing_mode3_result r;
	uint8_t immedL = BITS(i, 0, 3);
	uint8_t immedH = BITS(i, 8, 11);
	uint8_t Rn = BITS(i, 16, 19);
	uint8_t U = BIT(i, 23);
	
	r.current_addr = cpu->reg[Rn];
	uint8_t offset_8 = (immedH << 4) | immedL;
	if (U == 1) {
		r.writeback_addr = cpu->reg[Rn] + offset_8;
	} else {
		r.writeback_addr = cpu->reg[Rn] - offset_8;
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d], %s#0x%08X", Rn, U ? "" : "-", offset_8); // Abbreviations are apparently not allowed according to the spec (A5.3.6)
	r.do_writeback = 1;
	return r;
}

addressing_mode3_result addressing_mode3_register_postindex(CPU_STATE* cpu, uint32_t i) {
	addressing_mode3_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t Rn = BITS(i, 16, 19);
	uint8_t U = BIT(i, 23);
	
	//SBZ(i, 8, 11); //TODO
	
	r.current_addr = cpu->reg[Rn];
	if (U == 1) {
		r.writeback_addr = cpu->reg[Rn] + cpu->reg[Rm];
	} else {
		r.writeback_addr = cpu->reg[Rn] - cpu->reg[Rm];
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d], %sr%d", Rn, U ? "" : "-", Rm);
	r.do_writeback = 1;
	return r;
}
