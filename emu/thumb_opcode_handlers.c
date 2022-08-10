#include <stdint.h>
#include <stdio.h>

#include "alu.h"
#include "common.h"
#include "cpu.h"
#include "log.h"
#include "mmu.h"
#include "thumb_opcode_handlers.h"

static void print_register_list(uint32_t register_list, uint8_t LR, uint8_t PC) {
	int i = 0;
	for (; i <= 7; i++) {
		if (BIT(register_list, i) == 1) {
			ponii_log(LOG_LEVEL_EXTRA_INFO, "r%d", i);
			i++;
			break;
		}
	}
	
	for (; i <= 7; i++) {
		if (BIT(register_list, i) == 1)
			ponii_log(LOG_LEVEL_EXTRA_INFO, ", r%d", i);
	}
	
	if (LR) {
		if (register_list != 0)
			ponii_log(LOG_LEVEL_EXTRA_INFO, ", ");
		ponii_log(LOG_LEVEL_EXTRA_INFO, "r14");
	}
	
	if (PC) {
		if (register_list != 0 || LR != 0)
			ponii_log(LOG_LEVEL_EXTRA_INFO, ", ");
		ponii_log(LOG_LEVEL_EXTRA_INFO, "r15");
	}
}

extern int invalid_opcode_done;

int thumb_opcode_invalid_handler(MACHINE_STATE* ms, uint16_t i) {
	UNUSED(ms);
	UNUSED(i);
	
	ponii_log(LOG_LEVEL_ERROR, "Invalid thumb opcode\n");
	ponii_log(LOG_LEVEL_ERROR, "Opcode: 0x%04X\n", i);
	invalid_opcode_done = 1;
	return 0;
}

int thumb_opcode_adc_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ADC r%d, r%d\n", Rd, Rm);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	cpu->reg[Rd] = alu_add_carry(cpu->reg[Rd], cpu->reg[Rm], cpu_get_cpsr_field(cpu, CPSR_C), &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_add1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t immed_3 = BITS(i, 6, 8);
	
	if (immed_3 != 0) {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "ADD r%d, r%d, #0x%02X\n", Rd, Rn, immed_3);
	} else {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "MOV r%d, r%d\n", Rd, Rn);
	}
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	cpu->reg[Rd] = alu_add(cpu->reg[Rn], immed_3, &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_add2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t immed_8 = BITS(i, 0, 7);
	uint32_t Rd = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ADD r%d, #0x%02X\n", Rd, immed_8);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	cpu->reg[Rd] = alu_add(cpu->reg[Rd], immed_8, &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_add3_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t Rm = BITS(i, 6, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ADD r%d, r%d, r%d\n", Rd, Rn, Rm);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	cpu->reg[Rd] = alu_add(cpu->reg[Rn], cpu->reg[Rm], &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_add4_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t Rd = (BIT(i, 7) << 3) | BITS(i, 0, 2);
	uint32_t Rm = (BIT(i, 6) << 3) | BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ADD r%d, r%d\n", Rd, Rm);
	
	cpu->reg[Rd] = alu_add(cpu->reg[Rd], cpu->reg[Rm], NULL, NULL, NULL, NULL);
	
	if (Rd == 15)
		return 1;
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_add5_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t immed_8 = BITS(i, 0, 7);
	uint32_t Rd = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ADD r%d, r15, #0x%04X\n", Rd, immed_8 << 2);
	
	cpu->reg[Rd] = alu_add(cpu->reg[REG_PC] & 0xFFFFFFFC, immed_8 << 2, NULL, NULL, NULL, NULL);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_add6_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t immed_8 = BITS(i, 0, 7);
	uint32_t Rd = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ADD r%d, r13, #0x%04X\n", Rd, immed_8 << 2);
	
	cpu->reg[Rd] = alu_add(cpu->reg[REG_SP], immed_8 << 2, NULL, NULL, NULL, NULL);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_add7_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t immed_7 = BITS(i, 0, 6);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ADD r13, #0x%04X\n", immed_7 << 2);
	
	cpu->reg[REG_SP] = alu_add(cpu->reg[REG_SP], immed_7 << 2, NULL, NULL, NULL, NULL);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_and_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "AND r%d, r%d\n", Rd, Rm);
	
	int N = 0;
	int Z = 0;
	cpu->reg[Rd] = alu_and(cpu->reg[Rd], cpu->reg[Rm], &N, &Z);
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_asr1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	uint32_t immed_5 = BITS(i, 6, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ASR r%d, r%d, #0x%02X\n", Rd, Rm, immed_5);
	
	if (immed_5 == 0) {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rm], 31));
		if (BIT(cpu->reg[Rm], 31) == 0) {
			cpu->reg[Rd] = 0;
		} else {
			cpu->reg[Rd] = 0xFFFFFFFF;
		}
	} else {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rm], immed_5 - 1));
		cpu->reg[Rd] = cpu->reg[Rm] >> immed_5;
	}
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_asr2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rs = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ASR r%d, r%d\n", Rd, Rs);
	
	if (BITS(cpu->reg[Rs], 0, 7) == 0) {
		// Nothing
	} else if (BITS(cpu->reg[Rs], 0, 7) < 32) {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rd], BITS(cpu->reg[Rs], 0, 7) - 1));
		cpu->reg[Rd] = cpu->reg[Rd] >> BITS(cpu->reg[Rs], 0, 7);
	} else {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rd], 31));
		if (BIT(cpu->reg[Rd], 31) == 0) {
			cpu->reg[Rd] = 0;
		} else {
			cpu->reg[Rd] = 0xFFFFFFFF;
		}
	}
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_b1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t signed_immed_8 = BITS(i, 0, 7);
	uint32_t cond = BITS(i, 8, 11);
	
	if (BIT(signed_immed_8, 7) == 1) {
		signed_immed_8 |= 0xFFFFFF00;
	}
	
	uint32_t addr = cpu->reg[REG_PC] + (signed_immed_8 << 1);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "B%s 0x%08X\n", get_ascii_conditional(cond), addr);
	
	if (condition_passed(cpu, cond)) {
		cpu->reg[REG_PC] = addr;
		return 1;
	}
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_bic_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "BIC r%d, r%d\n", Rd, Rm);
	
	cpu->reg[Rd] = cpu->reg[Rd] & (~cpu->reg[Rm]);
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_bl_blx1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint8_t H = BITS(i, 11, 12);
	
	// B (2)
	if (H == 0) {
		uint32_t signed_immed_11 = BITS(i, 0, 10);
		if (BIT(signed_immed_11, 10) == 1) {
			signed_immed_11 |= 0xFFFFF800;
		}
		
		uint32_t addr = cpu->reg[REG_PC] + (signed_immed_11 << 1);
		
		ponii_log(LOG_LEVEL_EXTRA_INFO, "B 0x%08X\n", addr);
		
		cpu->reg[REG_PC] = addr;
		return 1;
	}
	
	uint16_t i2 = mmu_read_word(ms, cpu->reg[REG_PC] - 4 + 2);
	uint8_t H2 = BITS(i2, 11, 12);
	
	if (H != 2) {
		ponii_log(LOG_LEVEL_ERROR, "Paired instruction error!\n");
		ponii_log(LOG_LEVEL_ERROR, "H != 2\n");
		invalid_opcode_done = 1;
		return 0;
	}
	
	if (H2 != 1 && H2 != 3) {
		ponii_log(LOG_LEVEL_ERROR, "Paired instruction error!\n");
		ponii_log(LOG_LEVEL_ERROR, "H2 != 1 && H2 != 3\n");
		invalid_opcode_done = 1;
		return 0;
	}
	
	uint32_t offset_11_1 = BITS(i, 0, 10);
	uint32_t offset_11_2 = BITS(i2, 0, 10);
	
	if (BIT(offset_11_1, 10) == 1) {
		offset_11_1 |= 0xFFFFF800;
	}
	cpu->reg[REG_LR] = cpu->reg[REG_PC] + (offset_11_1 << 12);
	
	if (H2 == 3) {
		uint32_t addr = cpu->reg[REG_LR] + (offset_11_2 << 1);
		cpu->reg[REG_LR] = (cpu->reg[REG_PC] - 4 + 4) | 1;
		cpu->reg[REG_PC] = addr;
		
		ponii_log(LOG_LEVEL_EXTRA_INFO, "BL 0x%08X\n", addr);
	} else if (H2 == 1) {
		uint32_t addr = (cpu->reg[REG_LR] + (offset_11_2 << 1)) & 0xFFFFFFFC;
		cpu->reg[REG_LR] = (cpu->reg[REG_PC] - 4 + 4) | 1;
		cpu->reg[REG_PC] = addr;
		cpu_set_cpsr_field(cpu, CPSR_T, 0);
		
		ponii_log(LOG_LEVEL_EXTRA_INFO, "BLX 0x%08X\n", addr);
	}
	
	return 1;
}

int thumb_opcode_bl_blx2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rm = (BIT(i, 6) << 3) | BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "BLX r%d\n", Rm);
	
	uint32_t target = cpu->reg[Rm];
	cpu->reg[REG_LR] = (cpu->reg[REG_PC] - 4 + 2) | 1;
	cpu_set_cpsr_field(cpu, CPSR_T, BIT(target, 0));
	cpu->reg[REG_PC] = target & 0xFFFFFFFE;
	
	return 1;
}

int thumb_opcode_bx_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rm = (BIT(i, 6) << 3) | BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "BX r%d\n", Rm);
	
	cpu_set_cpsr_field(cpu, CPSR_T, BIT(cpu->reg[Rm], 0));
	cpu->reg[REG_PC] = BITS(cpu->reg[Rm], 1, 31) << 1;
	
	return 1;
}

int thumb_opcode_cmn_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "CMN r%d, r%d\n", Rn, Rm);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	alu_add(cpu->reg[Rn], cpu->reg[Rm], &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_cmp1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t immed_8 = BITS(i, 0, 7);
	uint32_t Rn = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "CMP r%d, #0x%02X\n", Rn, immed_8);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	alu_sub(cpu->reg[Rn], immed_8, &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_cmp2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "CMP r%d, r%d\n", Rn, Rm);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	alu_sub(cpu->reg[Rn], cpu->reg[Rm], &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_cmp3_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = (BIT(i, 7) << 3) | BITS(i, 0, 2);
	uint32_t Rm = (BIT(i, 6) << 3) | BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "CMP r%d, r%d\n", Rn, Rm);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	alu_sub(cpu->reg[Rn], cpu->reg[Rm], &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_eor_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "EOR r%d, r%d\n", Rd, Rm);
	
	int N = 0;
	int Z = 0;
	cpu->reg[Rd] = alu_xor(cpu->reg[Rd], cpu->reg[Rm], &N, &Z);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_ldmia_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t register_list = BITS(i, 0, 7);
	uint32_t Rn = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDMIA r%d!, {", Rn);
	print_register_list(register_list, 0, 0);
	ponii_log(LOG_LEVEL_EXTRA_INFO, "}\n");
	
	int num_bits = 0;
	for (int i = 0; i <= 7; i++) {
		if (BIT(register_list, i) == 1)
			num_bits++;
	}
	
	uint32_t start_addr = cpu->reg[Rn];
	uint32_t end_addr = cpu->reg[Rn] + (num_bits * 4) - 4;
	uint32_t addr = start_addr;
	
	for (int i = 0; i <= 7; i++) {
		if (BIT(register_list, i) == 1) {
			cpu->reg[i] = mmu_read_dword(ms, addr);
			addr += 4;
		}
	}
	
	if (end_addr != (addr - 4)) {
		ponii_log(LOG_LEVEL_ERROR, "LDMIA read the wrong amount of registers\n");
		ponii_log(LOG_LEVEL_ERROR, "0x%08X != 0x%08X\n", end_addr, addr - 4);
		return 0;
	}
	
	cpu->reg[Rn] += num_bits * 4;
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_ldr1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t immed_5 = BITS(i, 6, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDR r%d, [r%d, #0x%02X]\n", Rd, Rn, immed_5 * 4);
	
	uint32_t addr = cpu->reg[Rn] + (immed_5 * 4);
	cpu->reg[Rd] = mmu_read_dword(ms, addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_ldr2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t Rm = BITS(i, 6, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDR r%d, [r%d, r%d]\n", Rd, Rn, Rm);
	
	uint32_t addr = cpu->reg[Rn] + cpu->reg[Rm];
	cpu->reg[Rd] = mmu_read_dword(ms, addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_ldr3_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t immed_8 = BITS(i, 0, 7);
	uint32_t Rd = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDR r%d, [r15, #0x%04X]\n", Rd, immed_8 * 4);
	
	uint32_t addr = (cpu->reg[REG_PC] & 0xFFFFFFFC) + (immed_8 * 4);
	cpu->reg[Rd] = mmu_read_dword(ms, addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_ldr4_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t immed_8 = BITS(i, 0, 7);
	uint32_t Rd = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDR r%d, [r13, #0x%04X]\n", Rd, immed_8 * 4);
	
	uint32_t addr = cpu->reg[REG_SP] + (immed_8 * 4);
	cpu->reg[Rd] = mmu_read_dword(ms, addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_ldrb1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t immed_5 = BITS(i, 6, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDRB r%d, [r%d, #0x%02X]\n", Rd, Rn, immed_5);
	
	uint32_t addr = cpu->reg[Rn] + immed_5;
	cpu->reg[Rd] = mmu_read_byte(ms, addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_ldrb2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t Rm = BITS(i, 6, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDRB r%d, [r%d, r%d]\n", Rd, Rn, Rm);
	
	uint32_t addr = cpu->reg[Rn] + cpu->reg[Rm];
	cpu->reg[Rd] = mmu_read_byte(ms, addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_ldrh1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t immed_5 = BITS(i, 6, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDRH r%d, [r%d, #0x%08X]\n", Rd, Rn, immed_5 * 2);
	
	uint32_t addr = cpu->reg[Rn] + (immed_5 * 2);
	cpu->reg[Rd] = mmu_read_word(ms, addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_ldrh2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t Rm = BITS(i, 6, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LDRH r%d, [r%d, r%d]\n", Rd, Rn, Rm);
	
	uint32_t addr = cpu->reg[Rn] + cpu->reg[Rm];
	cpu->reg[Rd] = mmu_read_word(ms, addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_lsl1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	uint32_t immed_5 = BITS(i, 6, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LSL r%d, r%d, #0x%02X\n", Rd, Rm, immed_5);
	
	if (immed_5 == 0) {
		cpu->reg[Rd] = cpu->reg[Rm];
	} else {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rm], 32 - immed_5));
		cpu->reg[Rd] = cpu->reg[Rm] << immed_5;
	}
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_lsl2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rs = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LSL r%d, r%d\n", Rd, Rs);
	
	uint32_t immed_8 = BITS(cpu->reg[Rs], 0, 7);
	
	if (immed_8 == 0) {
		// Nothing
	} else if (immed_8 < 32) {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rd], 32 - immed_8));
		cpu->reg[Rd] = cpu->reg[Rd] << immed_8;
	} else if (immed_8 == 32) {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rd], 0));
		cpu->reg[Rd] = 0;
	} else {
		cpu_set_cpsr_field(cpu, CPSR_C, 0);
		cpu->reg[Rd] = 0;
	}
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_lsr1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	uint32_t immed_5 = BITS(i, 6, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LSR r%d, r%d, #0x%02X\n", Rd, Rm, immed_5);
	
	if (immed_5 == 0) {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rm], 31));
		cpu->reg[Rd] = 0;
	} else {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rm], immed_5 - 1));
		cpu->reg[Rd] = cpu->reg[Rm] >> immed_5;
	}
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_lsr2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rs = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "LSR r%d, r%d\n", Rd, Rs);
	
	uint32_t immed_8 = BITS(cpu->reg[Rs], 0, 7);
	
	if (immed_8 == 0) {
		// Nothing
	} else if (immed_8 < 32) {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rd], immed_8 - 1));
		cpu->reg[Rd] = cpu->reg[Rd] >> immed_8;
	} else if (immed_8 == 32) {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rd], 31));
		cpu->reg[Rd] = 0;
	} else {
		cpu_set_cpsr_field(cpu, CPSR_C, 0);
		cpu->reg[Rd] = 0;
	}
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_mov1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t immed_8 = BITS(i, 0, 7);
	uint32_t Rd = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "MOV r%d, #0x%02X\n", Rd, immed_8);
	
	cpu->reg[Rd] = immed_8;
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_mov3_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = (BIT(i, 7) << 3) | BITS(i, 0, 2);
	uint32_t Rm = (BIT(i, 6) << 3) | BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "MOV r%d, r%d\n", Rd, Rm);
	
	cpu->reg[Rd] = cpu->reg[Rm];
	
	if (Rd == 15) {
		return 1;
	}
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_mul_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "MUL r%d, r%d\n", Rd, Rm);
	
	cpu->reg[Rd] = BITS(cpu->reg[Rm] * cpu->reg[Rd], 0, 31);
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_mvn_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "MVN r%d, r%d\n", Rd, Rm);
	
	cpu->reg[Rd] = ~cpu->reg[Rm];
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_neg_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "NEG r%d, r%d\n", Rd, Rm);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	cpu->reg[Rd] = alu_sub(0, cpu->reg[Rm], &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_orr_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ORR r%d, r%d\n", Rd, Rm);
	
	cpu->reg[Rd] = cpu->reg[Rm] | cpu->reg[Rd];
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_pop_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t register_list = BITS(i, 0, 7);
	uint8_t R = BIT(i, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "POP {");
	print_register_list(register_list, 0, R);
	ponii_log(LOG_LEVEL_EXTRA_INFO, "}\n");
	
	int num_bits = 0;
	for (int i = 0; i <= 7; i++) {
		if (BIT(register_list, i) == 1)
			num_bits++;
	}
	
	uint32_t start_addr = cpu->reg[REG_SP];
	uint32_t end_addr = cpu->reg[REG_SP] + (4 * (R + num_bits));
	uint32_t addr = start_addr;
	
	for (int i = 0; i <= 7; i++) {
		if (BIT(register_list, i) == 1) {
			cpu->reg[i] = mmu_read_dword(ms, addr);
			addr += 4;
		}
	}
	
	if (R == 1) {
		uint32_t value = mmu_read_dword(ms, addr);
		cpu->reg[REG_PC] = value & 0xFFFFFFFE;
		cpu_set_cpsr_field(cpu, CPSR_T, BIT(value, 0));
		addr += 4;
	}
	
	if (end_addr != addr) {
		ponii_log(LOG_LEVEL_ERROR, "POP read the wrong amount of registers\n");
		ponii_log(LOG_LEVEL_ERROR, "0x%08X != 0x%08X\n", end_addr, addr);
		invalid_opcode_done = 1;
		return 0;
	}
	
	cpu->reg[REG_SP] = end_addr;
	
	if (R) {
		return 1;
	}
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_push_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t register_list = BITS(i, 0, 7);
	uint8_t R = BIT(i, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "PUSH {");
	print_register_list(register_list, R, 0);
	ponii_log(LOG_LEVEL_EXTRA_INFO, "}\n");
	
	int num_bits = 0;
	for (int i = 0; i <= 7; i++) {
		if (BIT(register_list, i) == 1)
			num_bits++;
	}
	
	uint32_t start_addr = cpu->reg[REG_SP] - (4 * (R + num_bits));
	uint32_t end_addr = cpu->reg[REG_SP] - 4;
	uint32_t addr = start_addr;
	
	for (int i = 0; i <= 7; i++) {
		if (BIT(register_list, i) == 1) {
			mmu_write_dword(ms, cpu->reg[i], addr);
			addr += 4;
		}
	}
	
	if (R == 1) {
		mmu_write_dword(ms, cpu->reg[REG_LR], addr);
		addr += 4;
	}
	
	if (end_addr != (addr - 4)) {
		ponii_log(LOG_LEVEL_ERROR, "PUSH wrote the wrong amount of registers\n");
		ponii_log(LOG_LEVEL_ERROR, "0x%08X != 0x%08X\n", end_addr, addr - 4);
		invalid_opcode_done = 1;
	}
	
	cpu->reg[REG_SP] -= (R + num_bits) * 4;
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_ror_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rs = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "ROR r%d, r%d\n", Rd, Rs);
	
	uint32_t immed_5 = BITS(cpu->reg[Rs], 0, 4);
	
	if (BITS(cpu->reg[Rs], 0, 7) == 0) {
		// Nothing
	} else if (immed_5 == 0) {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rd], BIT(cpu->reg[Rd], 31)));
	}else {
		cpu_set_cpsr_field(cpu, CPSR_C, BIT(cpu->reg[Rd], immed_5 - 1));
		cpu->reg[Rd] = (cpu->reg[Rd] << (32 - immed_5)) | (cpu->reg[Rd] >> immed_5);
	}
	
	cpu_set_cpsr_field(cpu, CPSR_N, BIT(cpu->reg[Rd], 31));
	cpu_set_cpsr_field(cpu, CPSR_Z, cpu->reg[Rd] ? 0 : 1);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_sbc_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "SBC r%d, r%d\n", Rd, Rm);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	cpu->reg[Rd] = alu_sub_carry(cpu->reg[Rd], cpu->reg[Rm], !cpu_get_cpsr_field(cpu, CPSR_C), &N, &Z, &C, &V); //TODO: Should be invert C here or not?
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_stmia_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t register_list = BITS(i, 0, 7);
	uint8_t Rn = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STMIA r%d! {", Rn);
	print_register_list(register_list, 0, 0);
	ponii_log(LOG_LEVEL_EXTRA_INFO, "}\n");
	
	int num_bits = 0;
	for (int i = 0; i <= 7; i++) {
		if (BIT(register_list, i) == 1)
			num_bits++;
	}
	
	uint32_t start_addr = cpu->reg[Rn];
	uint32_t end_addr = cpu->reg[Rn] + (num_bits * 4) - 4;
	uint32_t addr = start_addr;
	
	for (int i = 0; i <= 7; i++) {
		if (BIT(register_list, i) == 1) {
			mmu_write_dword(ms, cpu->reg[i], addr);
			addr += 4;
		}
	}
	
	if (end_addr != (addr - 4)) {
		ponii_log(LOG_LEVEL_ERROR, "STMIA wrote the wrong amount of registers\n");
		ponii_log(LOG_LEVEL_ERROR, "0x%08X != 0x%08X\n", end_addr, addr - 4);
		invalid_opcode_done = 1;
	}
	
	cpu->reg[Rn] += num_bits * 4;
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_str1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t immed_5 = BITS(i, 6, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STR r%d, [r%d, #0x%02X]\n", Rd, Rn, immed_5 * 4);
	
	uint32_t addr = cpu->reg[Rn] + (immed_5 * 4);
	
	mmu_write_dword(ms, cpu->reg[Rd], addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_str2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t Rm = BITS(i, 6, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STR r%d, [r%d, r%d]\n", Rd, Rn, Rm);
	
	uint32_t addr = cpu->reg[Rn] + cpu->reg[Rm];
	
	mmu_write_dword(ms, cpu->reg[Rd], addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_str3_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t immed_8 = BITS(i, 0, 7);
	uint32_t Rd = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STR r%d, [r13, #0x%04X]\n", Rd, immed_8 * 4);
	
	uint32_t addr = cpu->reg[REG_SP] + (immed_8 * 4);
	
	mmu_write_dword(ms, cpu->reg[Rd], addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_strb1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t immed_5 = BITS(i, 6, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STRB r%d, [r%d, #0x%02X]\n", Rd, Rn, immed_5);
	
	uint32_t addr = cpu->reg[Rn] + immed_5;
	
	mmu_write_byte(ms, BITS(cpu->reg[Rd], 0, 7), addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_strb2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t Rm = BITS(i, 6, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STRB r%d, [r%d, r%d]\n", Rd, Rn, Rm);
	
	uint32_t addr = cpu->reg[Rn] + cpu->reg[Rm];
	
	mmu_write_byte(ms, BITS(cpu->reg[Rd], 0, 7), addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_strh1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t immed_5 = BITS(i, 6, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STRB r%d, [r%d, #0x%08X]\n", Rd, Rn, immed_5 * 2);
	
	uint32_t addr = cpu->reg[Rn] + (immed_5 * 2);
	
	mmu_write_word(ms, BITS(cpu->reg[Rd], 0, 15), addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_strh2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t Rm = BITS(i, 6, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "STRB r%d, [r%d, r%d]\n", Rd, Rn, Rm);
	
	uint32_t addr = cpu->reg[Rn] + cpu->reg[Rm];
	
	mmu_write_word(ms, BITS(cpu->reg[Rd], 0, 15), addr);
	
	cpu->reg[REG_PC] += 2;
	return 0;
}

int thumb_opcode_sub1_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t immed_3 = BITS(i, 6, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "SUB r%d, r%d, #0x%02X\n", Rd, Rn, immed_3);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	cpu->reg[Rd] = alu_sub(cpu->reg[Rn], immed_3, &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_sub2_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t immed_8 = BITS(i, 0, 7);
	uint32_t Rd = BITS(i, 8, 10);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "SUB r%d, #0x%02X\n", Rd, immed_8);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	cpu->reg[Rd] = alu_sub(cpu->reg[Rd], immed_8, &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_sub3_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rd = BITS(i, 0, 2);
	uint32_t Rn = BITS(i, 3, 5);
	uint32_t Rm = BITS(i, 6, 8);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "SUB r%d, r%d, r%d\n", Rd, Rn, Rm);
	
	int N = 0;
	int Z = 0;
	int C = 0;
	int V = 0;
	cpu->reg[Rd] = alu_sub(cpu->reg[Rn], cpu->reg[Rm], &N, &Z, &C, &V);
	
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	cpu_set_cpsr_field(cpu, CPSR_C, C);
	cpu_set_cpsr_field(cpu, CPSR_V, V);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_sub4_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t immed_7 = BITS(i, 0, 6);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "SUB r13, #0x%04X\n", immed_7 << 2);
	
	cpu->reg[REG_SP] = alu_sub(cpu->reg[REG_SP], immed_7 << 2, NULL, NULL, NULL, NULL);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}

int thumb_opcode_tst_handler(MACHINE_STATE* ms, uint16_t i) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t Rn = BITS(i, 0, 2);
	uint32_t Rm = BITS(i, 3, 5);
	
	ponii_log(LOG_LEVEL_EXTRA_INFO, "TST r%d, r%d\n", Rn, Rm);
	
	int N = 0;
	int Z = 0;
	alu_and(cpu->reg[Rn], cpu->reg[Rm], &N, &Z);
	cpu_set_cpsr_field(cpu, CPSR_N, N);
	cpu_set_cpsr_field(cpu, CPSR_Z, Z);
	
	cpu->reg[REG_PC] += 2;
	
	return 0;
}
