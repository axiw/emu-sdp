#include <stdint.h>
#include <stdio.h>

#include "alu.h"
#include "common.h"
#include "log.h"
#include "opcode_handlers.h"
#include "cpu.h"
#include "mmu.h"

#include "addressing_mode1.h"
#include "addressing_mode2.h"
#include "addressing_mode3.h"
#include "addressing_mode4.h"

extern int invalid_opcode_done;

int opcode_invalid_handler(MACHINE_STATE *ms, uint32_t i) {
	UNUSED(ms);
	UNUSED(i);
	
	ponii_log(LOG_LEVEL_ERROR, "Invalid opcode\n");
	ponii_log(LOG_LEVEL_ERROR, "Opcode: 0x%08X\n", i);
	invalid_opcode_done = 1;
	return 0;
}

static void SBZ(uint32_t i, uint64_t a, uint64_t b) {
	if (BITS(i, a, b) != 0) {
		ponii_log(LOG_LEVEL_ERROR, "SBZ FAILURE: bits %ld-%ld\n", a, b);
		invalid_opcode_done = 1;
	}
}

static void SBO(uint32_t i, uint64_t a, uint64_t b) {
	if (BITS(i, a, b) != MASK(0, b - a)) {
		ponii_log(LOG_LEVEL_ERROR, "SBO FAILURE: bits %ld-%ld\n", a, b);
		invalid_opcode_done = 1;
	}
}

static void register_list_printer(uint32_t register_list) {
	int i = 0;
	for (; i <= 15; i++) {
		if (BIT(register_list, i) == 1) {
			ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d", i);
			i++;
			break;
		}
	}
	
	for (; i <= 15; i++) {
		if (BIT(register_list, i) == 1) {
			ponii_log(LOG_LEVEL_EXTRA_INFO, ", r%d", i);
		}
	}
}

int opcode_adc_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t S = BIT(i, 20);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ADC%s%s r%d, r%d, ", S ? "S" : "", get_ascii_conditional(cond), Rd, Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		int N = 0;
		int Z = 0;
		int C = 0;
		int V = 0;
		cpu->reg[Rd] = alu_add_carry(cpu->reg[Rn], result.shifter_operand, cpu_get_cpsr_field(cpu, CPSR_C), &N, &Z, &C, &V);
		
		if (S && (Rd == 15)) {
			ponii_log(LOG_LEVEL_ERROR, "Invalid destination register\n");
		} else if (S) {
			cpu_set_cpsr_field(cpu, CPSR_N, N);
			cpu_set_cpsr_field(cpu, CPSR_Z, Z);
			cpu_set_cpsr_field(cpu, CPSR_C, C);
			cpu_set_cpsr_field(cpu, CPSR_V, V);
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_add_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t S = BIT(i, 20);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ADD%s%s r%d, r%d, ", S ? "S" : "", get_ascii_conditional(cond), Rd, Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		int N = 0;
		int Z = 0;
		int C = 0;
		int V = 0;
		cpu->reg[Rd] = alu_add(cpu->reg[Rn], result.shifter_operand, &N, &Z, &C, &V);
		
		if (S && (Rd == 15)) {
			ponii_log(LOG_LEVEL_ERROR, "Invalid destination register\n");
		} else if (S) {
			cpu_set_cpsr_field(cpu, CPSR_N, N);
			cpu_set_cpsr_field(cpu, CPSR_Z, Z);
			cpu_set_cpsr_field(cpu, CPSR_C, C);
			cpu_set_cpsr_field(cpu, CPSR_V, V);
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_and_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t S = BIT(i, 20);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "AND%s%s r%d, r%d, ", S ? "S" : "", get_ascii_conditional(cond), Rd, Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		int N = 0;
		int Z = 0;
		cpu->reg[Rd] = alu_and(cpu->reg[Rn], result.shifter_operand, &N, &Z);
		
		if (S && (Rd == 15)) {
			ponii_log(LOG_LEVEL_ERROR, "Invalid destination register\n");
		} else if (S) {
			cpu_set_cpsr_field(cpu, CPSR_N, N);
			cpu_set_cpsr_field(cpu, CPSR_Z, Z);
			cpu_set_cpsr_field(cpu, CPSR_C, result.shifter_carry_out);
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_b_bl_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t offset = BITS(i, 0, 23);
	uint8_t L = BIT(i, 24);
	uint8_t cond = BITS(i, 28, 31);
	
	if (BIT(offset, 23) == 1) {
		offset |= 0x3F000000;
	}
	offset <<= 2;
	offset += cpu->reg[REG_PC];
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "B%s%s 0x%08X\n", L ? "L" : "", get_ascii_conditional(cond), offset);
	
	if (condition_passed(cpu, cond)) {
		if (L)
			cpu->reg[REG_LR] = cpu->reg[REG_PC] + 4 - 8; // Minus 8 because fake pipeline
		cpu->reg[REG_PC] = offset;
		return 1;
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_bic_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t S = BIT(i, 20);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "BIC%s%s r%d, r%d, ", S ? "S" : "", get_ascii_conditional(cond), Rd, Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		cpu->reg[Rd] = cpu->reg[Rn] & (~result.shifter_operand);
		
		if (S && (Rd == 15)) {
			ponii_log(LOG_LEVEL_ERROR, "Invalid destination register\n");
		} else if (S) {
			cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
			cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
			cpu_set_cpsr_field(cpu, CPSR_C, result.shifter_carry_out);
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_bx_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rm = BITS(i, 0, 3);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "BX%s r%d\n", get_ascii_conditional(cond), Rm);
	
	if (condition_passed(cpu, cond)) {
		cpu->reg[REG_PC] = cpu->reg[Rm] & 0xFFFFFFFE;
		cpu_set_cpsr_field(cpu, CPSR_T, BIT(cpu->reg[Rm], 0));
		return 1;
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_clz_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 12, 15);
	uint32_t Rm = BITS(i, 0, 3);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "CLZ%s r%d, r%d\n", get_ascii_conditional(cond), Rd, Rm);
	
	SBO(i, 16, 19);
	SBO(i, 8, 11);
	
	if (condition_passed(cpu, cond)) {
		if (Rm == 0) {
			cpu->reg[Rd] = 32;
		} else {
			int pos = 31;
			while ((pos != 0) && (BIT(cpu->reg[Rd], pos) == 0))
				pos--;
			cpu->reg[Rd] = 31 - pos;
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_cmn_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "CMN%s r%d, ", get_ascii_conditional(cond), Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	SBZ(i, 12, 15);
	
	if (condition_passed(cpu, cond)) {
		int N = 0;
		int Z = 0;
		int C = 0;
		int V = 0;
		alu_add(cpu->reg[Rn], result.shifter_operand, &N, &Z, &C, &V);
		
		cpu_set_cpsr_field(cpu, CPSR_N, N);
		cpu_set_cpsr_field(cpu, CPSR_Z, Z);
		cpu_set_cpsr_field(cpu, CPSR_C, C);
		cpu_set_cpsr_field(cpu, CPSR_V, V);
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_cmp_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "CMP%s r%d, ", get_ascii_conditional(cond), Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	SBZ(i, 12, 15);
	
	if (condition_passed(cpu, cond)) {
		int N = 0;
		int Z = 0;
		int C = 0;
		int V = 0;
		alu_sub(cpu->reg[Rn], result.shifter_operand, &N, &Z, &C, &V);
		
		cpu_set_cpsr_field(cpu, CPSR_N, N);
		cpu_set_cpsr_field(cpu, CPSR_Z, Z);
		cpu_set_cpsr_field(cpu, CPSR_C, C);
		cpu_set_cpsr_field(cpu, CPSR_V, V);
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_eor_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t S = BIT(i, 20);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "EOR%s%s r%d, r%d, ", S ? "S" : "", get_ascii_conditional(cond), Rd, Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		int N = 0;
		int Z = 0;
		cpu->reg[Rd] = alu_xor(cpu->reg[Rn], result.shifter_operand, &N, &Z);
		
		if (S && (Rd == 15)) {
			ponii_log(LOG_LEVEL_ERROR, "Invalid destination register\n");
		} else if (S) {
			cpu_set_cpsr_field(cpu, CPSR_N, N);
			cpu_set_cpsr_field(cpu, CPSR_Z, Z);
			cpu_set_cpsr_field(cpu, CPSR_C, result.shifter_carry_out);
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_ldm_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t W = BIT(i, 21);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDM%s", get_ascii_conditional(cond));
	
	addressing_mode4_result result = addressing_mode4_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, " r%d%s, {", Rn, W ? "!" : "");
	register_list_printer(result.register_list);
	ponii_log(LOG_LEVEL_EXTRA_INFO, "}\n");
	
	if (condition_passed(cpu, cond)) {
		uint32_t addr = result.start_addr;
		
		for (int i = 0; i <= 14; i++) {
			if (BIT(result.register_list, i) == 1) {
				cpu->reg[i] = mmu_read_dword(ms, addr);
				addr += 4;
			}
		}
		
		if (BIT(result.register_list, 15) == 1) {
			uint32_t value = mmu_read_dword(ms, addr);
			cpu->reg[REG_PC] = value & 0xFFFFFFFE;
			cpu_set_cpsr_field(cpu, CPSR_T, BIT(value, 0));
			addr += 4;
		}
		
		if (W) {
			cpu->reg[Rn] = result.writeback_addr;
		}
		
		if (result.end_addr != (addr - 4)) {
			ponii_log(LOG_LEVEL_ERROR, "LDM read the wrong amount of registers\n");
			ponii_log(LOG_LEVEL_ERROR, "0x%08X != 0x%08X\n", result.end_addr, addr - 4);
			invalid_opcode_done = 1;
		}
		
		if (BIT(result.register_list, 15) == 1) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_ldr_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDR%s r%d, ", get_ascii_conditional(cond), Rd);
	
	addressing_mode2_result result = addressing_mode2_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		if (result.do_writeback)
			cpu->reg[Rn] = result.writeback_addr;
		
		//TODO: CP15 ignored since we don't emulate it
		uint32_t data = mmu_read_dword(ms, result.current_addr);
		
		if (Rd == 15) {
			cpu->reg[REG_PC] = data & 0xFFFFFFFE;
			cpu_set_cpsr_field(cpu, CPSR_T, BIT(data, 0));
		} else {
			cpu->reg[Rd] = data;
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_ldrb_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDR%sB r%d, ", get_ascii_conditional(cond), Rd);
	
	addressing_mode2_result result = addressing_mode2_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		if (result.do_writeback)
			cpu->reg[Rn] = result.writeback_addr;
		
		cpu->reg[Rd] = mmu_read_byte(ms, result.current_addr);
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_mov_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t S = BIT(i, 20);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "MOV%s%s r%d, ", S ? "S" : "", get_ascii_conditional(cond), Rd);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		cpu->reg[Rd] = result.shifter_operand;
		if (S && (Rd == 15)) {
			ponii_log(LOG_LEVEL_ERROR, "Invalid destination register\n");
		} else if (S) {
			cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
			cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
			cpu_set_cpsr_field(cpu, CPSR_C, result.shifter_carry_out);
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_mvn_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t S = BIT(i, 20);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "MVN%s%s r%d, ", S ? "S" : "", get_ascii_conditional(cond), Rd);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		cpu->reg[Rd] = ~result.shifter_operand;
		if (S && (Rd == 15)) {
			ponii_log(LOG_LEVEL_ERROR, "Invalid destination register\n");
		} else if (S) {
			cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
			cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
			cpu_set_cpsr_field(cpu, CPSR_C, result.shifter_carry_out);
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_nop_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "NOP%s\n", get_ascii_conditional(cond));
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_orr_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 12, 15);
	uint32_t Rn = BITS(i, 16, 19);
	uint8_t S = BIT(i, 20);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ORR%s%s r%d, r%d, ", S ? "S" : "", get_ascii_conditional(cond), Rd, Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		cpu->reg[Rd] = cpu->reg[Rn] | result.shifter_operand;
		if (S && (Rd == 15)) {
			ponii_log(LOG_LEVEL_ERROR, "Invalid destination register\n");
		} else if (S) {
			cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
			cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
			cpu_set_cpsr_field(cpu, CPSR_C, result.shifter_carry_out);
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_rsb_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 12, 15);
	uint32_t Rn = BITS(i, 16, 19);
	uint8_t S = BIT(i, 20);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "RSB%s%s r%d, r%d, ", S ? "S" : "", get_ascii_conditional(cond), Rd, Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		int N = 0;
		int Z = 0;
		int C = 0;
		int V = 0;
		cpu->reg[Rd] = alu_sub(result.shifter_operand, cpu->reg[Rn], &N, &Z, &C, &V);
		
		if (S && (Rd == 15)) {
			ponii_log(LOG_LEVEL_ERROR, "Invalid destination register\n");
		} else if (S) {
			cpu_set_cpsr_field(cpu, CPSR_N, N);
			cpu_set_cpsr_field(cpu, CPSR_Z, Z);
			cpu_set_cpsr_field(cpu, CPSR_C, C); //TODO: Manual makes weird note about C flag. Make sure it's right. (A4.1.60)
			cpu_set_cpsr_field(cpu, CPSR_V, V);
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_stm_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t W = BIT(i, 21);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STM%s", get_ascii_conditional(cond));
	
	addressing_mode4_result result = addressing_mode4_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, " r%d%s, {", Rn, W ? "!" : "");
	register_list_printer(result.register_list);
	ponii_log(LOG_LEVEL_EXTRA_INFO, "}\n");
	
	if (condition_passed(cpu, cond)) {
		uint32_t addr = result.start_addr;
		
		for (int i = 0; i <= 15; i++) {
			if (BIT(result.register_list, i) == 1) {
				mmu_write_dword(ms, cpu->reg[i], addr);
				addr += 4;
			}
		}
		
		if (W) {
			cpu->reg[Rn] = result.writeback_addr;
		}
		
		if (result.end_addr != (addr - 4)) {
			ponii_log(LOG_LEVEL_ERROR, "STM wrote the wrong amount of registers\n");
			ponii_log(LOG_LEVEL_ERROR, "0x%08X != 0x%08X\n", result.end_addr, addr - 4);
			invalid_opcode_done = 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_str_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STR%s r%d, ", get_ascii_conditional(cond), Rd);
	
	addressing_mode2_result result = addressing_mode2_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		if (result.do_writeback)
			cpu->reg[Rn] = result.writeback_addr;
		
		mmu_write_dword(ms, cpu->reg[Rd], result.current_addr);
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_strb_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STR%sB r%d, ", get_ascii_conditional(cond), Rd);
	
	addressing_mode2_result result = addressing_mode2_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		if (result.do_writeback)
			cpu->reg[Rn] = result.writeback_addr;
		
		mmu_write_byte(ms, BITS(cpu->reg[Rd], 0, 7), result.current_addr);
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_sub_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint32_t Rd = BITS(i, 12, 15);
	uint8_t S = BIT(i, 20);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "SUB%s%s r%d, r%d, ", S ? "S" : "", get_ascii_conditional(cond), Rd, Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		int N = 0;
		int Z = 0;
		int C = 0;
		int V = 0;
		cpu->reg[Rd] = alu_sub(cpu->reg[Rn], result.shifter_operand, &N, &Z, &C, &V);
		
		if (S && (Rd == 15)) {
			ponii_log(LOG_LEVEL_ERROR, "Invalid destination register\n");
		} else if (S) {
			cpu_set_cpsr_field(cpu, CPSR_N, N);
			cpu_set_cpsr_field(cpu, CPSR_Z, Z);
			cpu_set_cpsr_field(cpu, CPSR_C, C);
			cpu_set_cpsr_field(cpu, CPSR_V, V);
		}
		
		if (Rd == 15) {
			return 1;
		}
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}

int opcode_tst_handler(MACHINE_STATE* ms, uint32_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 16, 19);
	uint8_t cond = BITS(i, 28, 31);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "TST%s r%d, ", get_ascii_conditional(cond), Rn);
	
	addressing_mode1_result result = addressing_mode1_resolve(cpu, i);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "\n");
	
	if (condition_passed(cpu, cond)) {
		int N = 0;
		int Z = 0;
		alu_and(cpu->reg[Rn], result.shifter_operand, &N, &Z);
		
		cpu_set_cpsr_field(cpu, CPSR_N, N);
		cpu_set_cpsr_field(cpu, CPSR_Z, Z);
		cpu_set_cpsr_field(cpu, CPSR_C, result.shifter_carry_out);
	}
	
	cpu->reg[REG_PC] += 4;
	return 0;
}
