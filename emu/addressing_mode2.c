#include <stdint.h>
#include <stdio.h>

// LUT generated from bits 25-24,21,4 inclusive, total of 4
#include "addressing_mode2_table.h"
#include "addressing_mode2_handlers.h"

#include "common.h"
#include "cpu.h"
#include "log.h"

addressing_mode2_result addressing_mode2_resolve(CPU_STATE* cpu, uint32_t i) {
	uint32_t lut_code = (BITS(i, 24, 25) << 2) | (BIT(i, 21) << 1) | BIT(i, 4);
	
	addressing_mode2_result (*handler)(CPU_STATE*, uint32_t) = addressing_mode2_func_table[addressing_mode2_lookup_table[lut_code]];
	
	if (handler == NULL) {
		ponii_log(LOG_LEVEL_ERROR, "Invalid addressing mode 2 encoding!\n");
		return (addressing_mode2_result){0,0,0};
	}
	
	return handler(cpu, i);
}

addressing_mode2_result addressing_mode2_immediate(CPU_STATE* cpu, uint32_t i) {
	addressing_mode2_result r;
	uint8_t Rn = BITS(i, 16, 19);
	uint32_t offset = BITS(i, 0, 11);
	uint8_t U = BIT(i, 23);
	
	if (U) {
		r.current_addr = cpu->reg[Rn] + offset;
	} else {
		r.current_addr = cpu->reg[Rn] - offset;
	}
	
	if (offset) {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d, #0x%08X]", Rn, offset);
	} else {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d]", Rn);
	}
	r.writeback_addr = 0;
	r.do_writeback = 0;
	return r;
}

addressing_mode2_result addressing_mode2_scaled_register(CPU_STATE* cpu, uint32_t i) {
	addressing_mode2_result r;
	uint8_t Rn = BITS(i, 16, 19);
	uint8_t Rm = BITS(i, 0, 3);
	uint32_t shift_imm = BITS(i, 7, 11);
	uint32_t shift = BITS(i, 5, 6);
	uint8_t U = BIT(i, 23);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d, %sr%d", Rn, U ? "" : "-", Rm);
	
	uint32_t index;
	switch (shift) {
		case 0b00:
			index = cpu->reg[Rm] << shift_imm;
			if (shift_imm != 0) {
				ponii_log(LOG_LEVEL_EXTRA_INFO, ", LSL #0x%08X", shift_imm);
			}
			break;
		case 0b01:
			if (shift_imm == 0) {
				index = 0;
			} else {
				index = cpu->reg[Rm] >> shift_imm;
			}
			ponii_log(LOG_LEVEL_EXTRA_INFO, ", LSR #0x%08X", shift_imm);
			break;
		case 0b10:
			if (shift_imm == 0) {
				if (BIT(cpu->reg[Rm], 31) == 1) {
					index = 0xFFFFFFFF;
				} else {
					index = 0;
				}
			} else {
				uint32_t sign = BIT(cpu->reg[Rm], 31);
				index = (cpu->reg[Rm] >> shift_imm) | ((sign ? 0xFFFFFFFF : 0x00000000) & MASK(32 - shift_imm, 32));
			}
			ponii_log(LOG_LEVEL_EXTRA_INFO, ", ASR #0x%08X", shift_imm);
			break;
		case 0b11:
			if (shift_imm == 0) {
				index = (cpu_get_cpsr_field(cpu, CPSR_C) << 31) | (cpu->reg[Rm] >> 1);
				ponii_log(LOG_LEVEL_EXTRA_INFO, ", RRX");
			} else {
				index = (cpu->reg[Rm] << (32 - shift_imm)) | (cpu->reg[Rm] >> shift_imm);
				ponii_log(LOG_LEVEL_EXTRA_INFO, ", ROR #0x%08X", shift_imm);
			}
			break;
		default:
			ponii_log(LOG_LEVEL_ERROR, "Impossible shift case!\n");
			return (addressing_mode2_result){0,0,0};
	}
	if (U) {
		r.current_addr = cpu->reg[Rn] + index;
	} else {
		r.current_addr = cpu->reg[Rn] - index;
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "]");
	r.writeback_addr = 0;
	r.do_writeback = 0;
	return r;
}

addressing_mode2_result addressing_mode2_immediate_preindexed(CPU_STATE* cpu, uint32_t i) {
	addressing_mode2_result r;
	uint8_t Rn = BITS(i, 16, 19);
	uint32_t offset = BITS(i, 0, 11);
	uint8_t U = BIT(i, 23);
	
	if (U) {
		r.current_addr = cpu->reg[Rn] + offset;
	} else {
		r.current_addr = cpu->reg[Rn] - offset;
	}
	
	if (offset) {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d, #0x%08X]!", Rn, offset);
	} else {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d]!", Rn);
	}
	r.writeback_addr = r.current_addr;
	r.do_writeback = 1;
	return r;
}

addressing_mode2_result addressing_mode2_scaled_register_preindexed(CPU_STATE* cpu, uint32_t i) {
	addressing_mode2_result r;
	uint8_t Rn = BITS(i, 16, 19);
	uint8_t Rm = BITS(i, 0, 3);
	uint32_t shift_imm = BITS(i, 7, 11);
	uint32_t shift = BITS(i, 5, 6);
	uint8_t U = BIT(i, 23);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d, %sr%d", Rn, U ? "" : "-", Rm);
	
	uint32_t index;
	switch (shift) {
		case 0b00:
			index = cpu->reg[Rm] << shift_imm;
			if (shift_imm != 0) {
				ponii_log(LOG_LEVEL_EXTRA_INFO, ", LSL #0x%08X", shift_imm);
			}
			break;
		case 0b01:
			if (shift_imm == 0) {
				index = 0;
			} else {
				index = cpu->reg[Rm] >> shift_imm;
			}
			ponii_log(LOG_LEVEL_EXTRA_INFO, ", LSR #0x%08X", shift_imm);
			break;
		case 0b10:
			if (shift_imm == 0) {
				if (BIT(cpu->reg[Rm], 31) == 1) {
					index = 0xFFFFFFFF;
				} else {
					index = 0;
				}
			} else {
				uint32_t sign = BIT(cpu->reg[Rm], 31);
				index = (cpu->reg[Rm] >> shift_imm) | ((sign ? 0xFFFFFFFF : 0x00000000) & MASK(32 - shift_imm, 32));
			}
			ponii_log(LOG_LEVEL_EXTRA_INFO, ", ASR #0x%08X", shift_imm);
			break;
		case 0b11:
			if (shift_imm == 0) {
				index = (cpu_get_cpsr_field(cpu, CPSR_C) << 31) | (cpu->reg[Rm] >> 1);
				ponii_log(LOG_LEVEL_EXTRA_INFO, ", RRX");
			} else {
				index = (cpu->reg[Rm] << (32 - shift_imm)) | (cpu->reg[Rm] >> shift_imm);
				ponii_log(LOG_LEVEL_EXTRA_INFO, ", ROR #0x%08X", shift_imm);
			}
			break;
		default:
			ponii_log(LOG_LEVEL_ERROR, "Impossible shift case!\n");
			return (addressing_mode2_result){0,0,0};
	}
	if (U) {
		r.current_addr = cpu->reg[Rn] + index;
	} else {
		r.current_addr = cpu->reg[Rn] - index;
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "]!");
	r.writeback_addr = r.current_addr;
	r.do_writeback = 1;
	return r;
}

addressing_mode2_result addressing_mode2_immediate_postindexed(CPU_STATE* cpu, uint32_t i) {
	addressing_mode2_result r;
	uint8_t Rn = BITS(i, 16, 19);
	uint32_t offset = BITS(i, 0, 11);
	uint8_t U = BIT(i, 23);
	
	if (U) {
		r.writeback_addr = cpu->reg[Rn] + offset;
	} else {
		r.writeback_addr = cpu->reg[Rn] - offset;
	}
	
	if (offset) {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d], #0x%08X", Rn, offset);
	} else {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d]", Rn);
	}
	r.current_addr = cpu->reg[Rn];
	r.do_writeback = 1;
	return r;
}

addressing_mode2_result addressing_mode2_scaled_register_postindexed(CPU_STATE* cpu, uint32_t i) {
	addressing_mode2_result r;
	uint8_t Rn = BITS(i, 16, 19);
	uint8_t Rm = BITS(i, 0, 3);
	uint32_t shift_imm = BITS(i, 7, 11);
	uint32_t shift = BITS(i, 5, 6);
	uint8_t U = BIT(i, 23);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "[r%d], %sr%d", Rn, U ? "" : "-", Rm);
	
	uint32_t index;
	switch (shift) {
		case 0b00:
			index = cpu->reg[Rm] << shift_imm;
			if (shift_imm != 0) {
				ponii_log(LOG_LEVEL_EXTRA_INFO, ", LSL #0x%08X", shift_imm);
			}
			break;
		case 0b01:
			if (shift_imm == 0) {
				index = 0;
			} else {
				index = cpu->reg[Rm] >> shift_imm;
			}
			ponii_log(LOG_LEVEL_EXTRA_INFO, ", LSR #0x%08X", shift_imm);
			break;
		case 0b10:
			if (shift_imm == 0) {
				if (BIT(cpu->reg[Rm], 31) == 1) {
					index = 0xFFFFFFFF;
				} else {
					index = 0;
				}
			} else {
				uint32_t sign = BIT(cpu->reg[Rm], 31);
				index = (cpu->reg[Rm] >> shift_imm) | ((sign ? 0xFFFFFFFF : 0x00000000) & MASK(32 - shift_imm, 32));
			}
			ponii_log(LOG_LEVEL_EXTRA_INFO, ", ASR #0x%08X", shift_imm);
			break;
		case 0b11:
			if (shift_imm == 0) {
				index = (cpu_get_cpsr_field(cpu, CPSR_C) << 31) | (cpu->reg[Rm] >> 1);
				ponii_log(LOG_LEVEL_EXTRA_INFO, ", RRX");
			} else {
				index = (cpu->reg[Rm] << (32 - shift_imm)) | (cpu->reg[Rm] >> shift_imm);
				ponii_log(LOG_LEVEL_EXTRA_INFO, ", ROR #0x%08X", shift_imm);
			}
			break;
		default:
			ponii_log(LOG_LEVEL_ERROR, "Impossible shift case!\n");
			return (addressing_mode2_result){0,0,0};
	}
	if (U) {
		r.writeback_addr = cpu->reg[Rn] + index;
	} else {
		r.writeback_addr = cpu->reg[Rn] - index;
	}
	
	r.current_addr = cpu->reg[Rn];
	r.do_writeback = 1;
	return r;
}
