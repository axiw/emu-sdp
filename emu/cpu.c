#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "cpu.h"
#include "linux_hle.h"
#include "log.h"
#include "machine.h"
// LUT generated from bits 27-20,7-4 inclusive, total of 12
#include "opcode_table.h"
// LUT generated from bits 15-6,0 inclusive, total of 11
#include "thumb_opcode_table.h"

#define STOP_CONDITION 0x504F4E59

CPU_STATE* cpu_init() {
	CPU_STATE* cpu = calloc(sizeof(CPU_STATE), 1);
	cpu->reg[REG_PC] = 0;
	cpu->reg[REG_SP] = 0;
	cpu->reg[REG_LR] = STOP_CONDITION;
	cpu->CPSR = 0;
	
	return cpu;
}

void cpu_free(CPU_STATE* cpu) {
	free(cpu);
}

void cpu_set_PC(CPU_STATE* cpu, uint32_t addr) {
	cpu->reg[REG_PC] = addr;
}

uint32_t cpu_get_PC(CPU_STATE* cpu) {
	return cpu->reg[REG_PC];
}

extern int invalid_opcode_done;

void cpu_advance(MACHINE_STATE* ms) {
	CPU_STATE *cpu = linux_get_thread_cpu(ms);
	ponii_log(LOG_LEVEL_EXTRA_INFO, "0x%08X ", cpu->reg[REG_PC]);
	
	if (cpu_get_cpsr_field(cpu, CPSR_T) == 0) { // Arm
		// Fetch instruction from MMU
		uint32_t i = mmu_read_dword(ms, cpu->reg[REG_PC]);
		
		if (i == 0xEF000000) {
                        ponii_log(LOG_LEVEL_EXTRA_INFO, "L: ");
                        
                        int syscall = cpu->reg[7];
                        
                        linux_syscall(ms, syscall);
                        
                        cpu->reg[REG_PC] += 4;
			return;
                }
		
		// Decode instruction
		uint16_t lut_code = (BITS(i, 20, 27) << 4) | BITS(i, 4, 7);
		int (*handler)(MACHINE_STATE*, uint32_t) = opcode_func_table[opcode_lookup_table[lut_code]];
		
		// Pretend we have a pipeline
		cpu->reg[REG_PC] += 8;
		
		// Execute instruction
		ponii_log(LOG_LEVEL_EXTRA_INFO, "A: ");
		int branched = handler(ms, i);
		
		// If we branched then we can't rely on our pipeline trick
		if (!branched) {
			// Stop pretending again
			cpu->reg[REG_PC] -= 8;
		}
	} else { // Thumb
		uint16_t i = mmu_read_word(ms, cpu->reg[REG_PC]);
		
		if ((i & 0xFF00) == 0xDF00) {
			uint32_t interrupt_num = BITS(i, 0, 7);
			if (interrupt_num == 0xAB) {
				if (cpu->reg[0] == 4) {
					uint32_t addr = cpu->reg[1];
					char c = mmu_read_byte(ms, addr);
					while (c != 0x00) {
						ponii_log(LOG_LEVEL_ERROR, "%c", c);
						addr += 1;
						c = mmu_read_byte(ms, addr);
					}
				}
			}
			cpu->reg[REG_PC] += 2;
			return;
		}
		
		uint16_t lut_code = (BITS(i, 6, 15) << 1) | BIT(i, 0);
		int (*handler)(MACHINE_STATE*, uint16_t) = thumb_opcode_func_table[thumb_opcode_lookup_table[lut_code]];
		
		cpu->reg[REG_PC] += 4;
		
		ponii_log(LOG_LEVEL_EXTRA_INFO, "T: ");
		int branched = handler(ms, i);
		
		if (!branched) {
			cpu->reg[REG_PC] -= 4;
		}
	}
	
	if (cpu->reg[REG_PC] == (STOP_CONDITION | 1) || cpu->reg[REG_PC] == (STOP_CONDITION & 0xFFFFFFFE)) {
		ponii_log(LOG_LEVEL_INFO, "Detected stop condition\n");
		invalid_opcode_done = 1;
	}
}

uint32_t cpu_get_cpsr_field(CPU_STATE* cpu, uint32_t field) {
	switch (field) {
		case CPSR_N:
			return BIT(cpu->CPSR, 31);
		case CPSR_Z:
			return BIT(cpu->CPSR, 30);
		case CPSR_C:
			return BIT(cpu->CPSR, 29);
		case CPSR_V:
			return BIT(cpu->CPSR, 28);
		case CPSR_Q:
			return BIT(cpu->CPSR, 27);
		case CPSR_RES:
			return BITS(cpu->CPSR, 25, 26);
		case CPSR_J:
			return BIT(cpu->CPSR, 24);
		case CPSR_GE:
			return BITS(cpu->CPSR, 16, 19);
		case CPSR_E:
			return BIT(cpu->CPSR, 9);
		case CPSR_A:
			return BIT(cpu->CPSR, 8);
		case CPSR_I:
			return BIT(cpu->CPSR, 7);
		case CPSR_F:
			return BIT(cpu->CPSR, 6);
		case CPSR_T:
			return BIT(cpu->CPSR, 5);
		case CPSR_M:
			return BITS(cpu->CPSR, 0, 4);
		default:
			ponii_log(LOG_LEVEL_ERROR, "Invalid cpsr field!\n");
			return 0;
	}
}

void cpu_set_cpsr_field(CPU_STATE* cpu, uint32_t field, uint32_t value) {
	switch (field) {
		case CPSR_N:
			cpu->CPSR = (cpu->CPSR & (~(1 << 31))) | (value << 31);
			break;
		case CPSR_Z:
			cpu->CPSR = (cpu->CPSR & (~(1 << 30))) | (value << 30);
			break;
		case CPSR_C:
			cpu->CPSR = (cpu->CPSR & (~(1 << 29))) | (value << 29);
			break;
		case CPSR_V:
			cpu->CPSR = (cpu->CPSR & (~(1 << 28))) | (value << 28);
			break;
		case CPSR_Q:
			cpu->CPSR = (cpu->CPSR & (~(1 << 27))) | (value << 27);
			break;
		case CPSR_RES:
			cpu->CPSR = (cpu->CPSR & (~(0b11 << 25))) | (value << 25);
			break;
		case CPSR_J:
			cpu->CPSR = (cpu->CPSR & (~(1 << 24))) | (value << 24);
			break;
		case CPSR_GE:
			cpu->CPSR = (cpu->CPSR & (~(0b1111 << 16))) | (value << 16);
			break;
		case CPSR_E:
			cpu->CPSR = (cpu->CPSR & (~(1 << 9))) | (value << 9);
			break;
		case CPSR_A:
			cpu->CPSR = (cpu->CPSR & (~(1 << 8))) | (value << 8);
			break;
		case CPSR_I:
			cpu->CPSR = (cpu->CPSR & (~(1 << 7))) | (value << 7);
			break;
		case CPSR_F:
			cpu->CPSR = (cpu->CPSR & (~(1 << 6))) | (value << 6);
			break;
		case CPSR_T:
			cpu->CPSR = (cpu->CPSR & (~(1 << 5))) | (value << 5);
			break;
		case CPSR_M:
			cpu->CPSR = (cpu->CPSR & (~(0b11111 << 0))) | (value << 0);
			break;
		default:
			ponii_log(LOG_LEVEL_ERROR, "Invalid cpsr field!\n");
			break;
	}
}
