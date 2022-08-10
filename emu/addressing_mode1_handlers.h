#ifndef __ADDRESSING_MODE1_HANDLERS_H__
#define __ADDRESSING_MODE1_HANDLERS_H__

#include <stdint.h>
#include "cpu.h"

struct addressing_mode1_result {
	uint32_t shifter_operand;
	uint8_t shifter_carry_out;
};
typedef struct addressing_mode1_result addressing_mode1_result;

addressing_mode1_result addressing_mode1_immediate(CPU_STATE* cpu, uint32_t i);
addressing_mode1_result addressing_mode1_lsl_immediate(CPU_STATE* cpu, uint32_t i);
addressing_mode1_result addressing_mode1_lsl_register(CPU_STATE* cpu, uint32_t i);
addressing_mode1_result addressing_mode1_lsr_immediate(CPU_STATE* cpu, uint32_t i);
addressing_mode1_result addressing_mode1_lsr_register(CPU_STATE* cpu, uint32_t i);
addressing_mode1_result addressing_mode1_asr_immediate(CPU_STATE* cpu, uint32_t i);
addressing_mode1_result addressing_mode1_asr_register(CPU_STATE* cpu, uint32_t i);
addressing_mode1_result addressing_mode1_rr_immediate(CPU_STATE* cpu, uint32_t i);
addressing_mode1_result addressing_mode1_rr_register(CPU_STATE* cpu, uint32_t i);

#endif
