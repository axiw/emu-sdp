#include <stdint.h>
#include <stdio.h>

// LUT generated from bits 25,7-4 inclusive, total of 5
#include "addressing_mode1_table.h"
#include "addressing_mode1_handlers.h"

#include "common.h"
#include "cpu.h"
#include "log.h"

addressing_mode1_result addressing_mode1_resolve(CPU_STATE* cpu, uint32_t i) {
	uint32_t lut_code = (BIT(i, 25) << 4) | BITS(i, 4, 7);
	
	addressing_mode1_result (*handler)(CPU_STATE*, uint32_t) = addressing_mode1_func_table[addressing_mode1_lookup_table[lut_code]];
	
	if (handler == NULL) {
		ponii_log(LOG_LEVEL_ERROR, "Invalid addressing mode 1 encoding!\n");
		return (addressing_mode1_result){0, 0};
	}
	
	return handler(cpu, i);
}

addressing_mode1_result addressing_mode1_immediate(CPU_STATE* cpu, uint32_t i) {
	addressing_mode1_result r;
	uint32_t x = BITS(i, 0, 7);
	uint8_t rr = BITS(i, 8, 11) * 2;
	
	r.shifter_operand = (x << (32 - rr)) | (x >> rr);
	if (rr == 0) {
		r.shifter_carry_out = cpu_get_cpsr_field(cpu, CPSR_N);
	} else {
		r.shifter_carry_out = BIT(r.shifter_operand, 31);
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "#0x%08X", r.shifter_operand);
	return r;
}

addressing_mode1_result addressing_mode1_lsl_immediate(CPU_STATE* cpu, uint32_t i) {
	addressing_mode1_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t lsl = BITS(i, 7, 11);
	
	if (lsl == 0) {
		r.shifter_operand = cpu->reg[Rm];
		r.shifter_carry_out = cpu_get_cpsr_field(cpu, CPSR_N);
	} else {
		uint64_t x = ((uint64_t)cpu->reg[Rm]) << lsl;
		r.shifter_operand = (uint32_t)x;
		r.shifter_carry_out = BIT(x, 32 - lsl);
	}
	
	if (lsl != 0) {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d, LSL #0x%08X", Rm, lsl);
	} else {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d", Rm);
	}
	return r;
}

addressing_mode1_result addressing_mode1_lsl_register(CPU_STATE* cpu, uint32_t i) {
	addressing_mode1_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t Rs = BITS(i, 8, 11);
	uint8_t lsl = BITS(cpu->reg[Rs], 0, 7);
	
	if (lsl == 0) {
		r.shifter_operand = cpu->reg[Rm];
		r.shifter_carry_out = cpu_get_cpsr_field(cpu, CPSR_N);
	} else if (lsl < 32) {
		uint64_t x = ((uint64_t)cpu->reg[Rm]) << lsl;
		r.shifter_operand = (uint32_t)x;
		r.shifter_carry_out = BIT(x, 32 - lsl);
	} else if (lsl == 32) {
		r.shifter_operand = 0;
		r.shifter_carry_out = BIT(cpu->reg[Rm], 0);
	} else {
		r.shifter_operand = 0;
		r.shifter_carry_out = 0;
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d, LSL r%d", Rm, Rs);
	return r;
}

addressing_mode1_result addressing_mode1_lsr_immediate(CPU_STATE* cpu, uint32_t i) {
	addressing_mode1_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t lsr = BITS(i, 7, 11);
	
	if (lsr == 0) {
		r.shifter_operand = 0;
		r.shifter_carry_out = BIT(cpu->reg[Rm], 31);
	} else {
		uint32_t x = cpu->reg[Rm] >> lsr;
		r.shifter_operand = x;
		r.shifter_carry_out = BIT(x, lsr - 1);
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d, LSR #0x%08X", Rm, lsr);
	return r;
}

addressing_mode1_result addressing_mode1_lsr_register(CPU_STATE* cpu, uint32_t i) {
	addressing_mode1_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t Rs = BITS(i, 8, 11);
	uint8_t lsr = BITS(cpu->reg[Rs], 0, 7);
	
	if (lsr == 0) {
		r.shifter_operand = cpu->reg[Rm];
		r.shifter_carry_out = cpu_get_cpsr_field(cpu, CPSR_N);
	} else if (lsr < 32) {
		uint64_t x = ((uint64_t)cpu->reg[Rm]) >> lsr;
		r.shifter_operand = (uint32_t)x;
		r.shifter_carry_out = BIT(x, lsr - 1);
	} else if (lsr == 32) {
		r.shifter_operand = 0;
		r.shifter_carry_out = BIT(cpu->reg[Rm], 31);
	} else {
		r.shifter_operand = 0;
		r.shifter_carry_out = 0;
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d, LSR r%d", Rm, Rs);
	return r;
}

addressing_mode1_result addressing_mode1_asr_immediate(CPU_STATE* cpu, uint32_t i) {
	addressing_mode1_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t asr = BITS(i, 7, 11);
	uint32_t sign = BIT(cpu->reg[Rm], 31);
	
	if (asr == 0) {
		if (sign == 0) {
			r.shifter_operand = 0;
			r.shifter_carry_out = sign;
		} else  {
			r.shifter_operand = 0xFFFFFFFF;
			r.shifter_carry_out = sign;
		}
	} else {
		r.shifter_operand = (cpu->reg[Rm] >> asr) | ((sign ? 0xFFFFFFFF : 0x00000000) & MASK(32 - asr, 32));
		r.shifter_carry_out = BIT(cpu->reg[Rm], asr - 1);
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d, ASR #0x%08X", Rm, asr);
	return r;
}

addressing_mode1_result addressing_mode1_asr_register(CPU_STATE* cpu, uint32_t i) {
	addressing_mode1_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t Rs = BITS(i, 8, 11);
	uint8_t asr = BITS(cpu->reg[Rs], 0, 7);
	uint32_t sign = BIT(cpu->reg[Rm], 31);
	
	if (asr == 0) {
		r.shifter_operand = cpu->reg[Rm];
		r.shifter_carry_out = cpu_get_cpsr_field(cpu, CPSR_N);
	} else if (asr < 32) {
		r.shifter_operand = (cpu->reg[Rm] >> asr) | ((sign ? 0xFFFFFFFF : 0x00000000) & MASK(32 - asr, 32));
		r.shifter_carry_out = BIT(cpu->reg[Rm], asr - 1);
	} else {
		if (sign == 0) {
			r.shifter_operand = 0;
			r.shifter_carry_out = sign;
		} else  {
			r.shifter_operand = 0xFFFFFFFF;
			r.shifter_carry_out = sign;
		}
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d, ASR r%d", Rm, Rs);
	return r;
}

addressing_mode1_result addressing_mode1_rr_immediate(CPU_STATE* cpu, uint32_t i) {
	addressing_mode1_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t ror = BITS(i, 7, 11);
	
	if (ror == 0) {
		r.shifter_operand = (cpu_get_cpsr_field(cpu, CPSR_N) << 31) | (cpu->reg[Rm] >> 1);
		r.shifter_carry_out = BIT(cpu->reg[Rm], 0);
	} else {
		r.shifter_operand = (cpu->reg[Rm] << (32 - ror)) | (cpu->reg[Rm] >> ror);
		r.shifter_carry_out = BIT(cpu->reg[Rm], ror - 1);
	}
	
	if (ror != 0) {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d, ROR #0x%08X", Rm, ror);
	} else {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d, RRX", Rm);
	}
	
	return r;
}

addressing_mode1_result addressing_mode1_rr_register(CPU_STATE* cpu, uint32_t i) {
	addressing_mode1_result r;
	uint8_t Rm = BITS(i, 0, 3);
	uint8_t Rs = BITS(i, 8, 11);
	uint8_t ror = BITS(cpu->reg[Rs], 0, 4);
	
	if (BITS(cpu->reg[Rs], 0, 7) == 0) {
		r.shifter_operand = cpu->reg[Rm];
		r.shifter_carry_out = cpu_get_cpsr_field(cpu, CPSR_N);
	} else if (ror == 0) {
		r.shifter_operand = cpu->reg[Rm];
		r.shifter_carry_out = BIT(cpu->reg[Rm], 31);
	} else {
		r.shifter_operand = (cpu->reg[Rm] << (32 - ror)) | (cpu->reg[Rm] >> ror);
		r.shifter_carry_out = BIT(cpu->reg[Rm], ror - 1);
	}
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d, ROR r%d", Rm, Rs);
	return r;
}
