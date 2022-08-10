#include <stdio.h>
#include <stdint.h>

#include "common.h"
#include "cpu.h"
#include "log.h"

extern int invalid_opcode_done;

const char* get_ascii_conditional(uint8_t c) {
	switch (c) {
		case 0b0000:
			return "EQ";
		case 0b0001:
			return "NE";
		case 0b0010:
			return "CS";
		case 0b0011:
			return "CC";
		case 0b0100:
			return "MI";
		case 0b0101:
			return "PL";
		case 0b0110:
			return "VS";
		case 0b0111:
			return "VC";
		case 0b1000:
			return "HI";
		case 0b1001:
			return "LS";
		case 0b1010:
			return "GE";
		case 0b1011:
			return "LT";
		case 0b1100:
			return "GT";
		case 0b1101:
			return "LE";
		case 0b1110:
			//return "AL";
			return "";
		case 0b1111:
			return "";
		default:
			ponii_log(LOG_LEVEL_ERROR, "Invalid conditional\n");
			invalid_opcode_done = 1;
			return "";
	}
}

int condition_passed(CPU_STATE* cpu, uint8_t cond) {
	switch (cond) {
		case 0b0000:
			return cpu_get_cpsr_field(cpu, CPSR_Z);
		case 0b0001:
			return !cpu_get_cpsr_field(cpu, CPSR_Z);
		case 0b0010:
			return cpu_get_cpsr_field(cpu, CPSR_C);
		case 0b0011:
			return !cpu_get_cpsr_field(cpu, CPSR_C);
		case 0b0100:
			return cpu_get_cpsr_field(cpu, CPSR_N);
		case 0b0101:
			return !cpu_get_cpsr_field(cpu, CPSR_N);
		case 0b0110:
			return cpu_get_cpsr_field(cpu, CPSR_V);
		case 0b0111:
			return !cpu_get_cpsr_field(cpu, CPSR_V);
		case 0b1000:
			return cpu_get_cpsr_field(cpu, CPSR_C) && (!cpu_get_cpsr_field(cpu, CPSR_Z));
		case 0b1001:
			return (!cpu_get_cpsr_field(cpu, CPSR_C)) || cpu_get_cpsr_field(cpu, CPSR_Z);
		case 0b1010:
			return cpu_get_cpsr_field(cpu, CPSR_N) == cpu_get_cpsr_field(cpu, CPSR_V);
		case 0b1011:
			return cpu_get_cpsr_field(cpu, CPSR_N) != cpu_get_cpsr_field(cpu, CPSR_V);
		case 0b1100:
			return (!cpu_get_cpsr_field(cpu, CPSR_Z)) && (cpu_get_cpsr_field(cpu, CPSR_N) == cpu_get_cpsr_field(cpu, CPSR_V));
		case 0b1101:
			return cpu_get_cpsr_field(cpu, CPSR_Z) || (cpu_get_cpsr_field(cpu, CPSR_N) != cpu_get_cpsr_field(cpu, CPSR_V));
		case 0b1110:
			return 1;
		default:
			ponii_log(LOG_LEVEL_ERROR, "Unsupported/invalid conditional!\n");
			invalid_opcode_done = 1;
			return 0;
	}
}

uint16_t swap_endian_word(uint16_t x) {
	return (x << 8) | (x >> 8);
}

uint32_t swap_endian_dword(uint32_t x) {
	return (x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24);
}
