#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>
#include <stddef.h>

#include "machine.h"

#define REG_SP 13
#define REG_LR 14
#define REG_PC 15

#define CPSR_N    0
#define CPSR_Z    1
#define CPSR_C    2
#define CPSR_V    3
#define CPSR_Q    4
#define CPSR_RES  5
#define CPSR_J    6
#define CPSR_GE   7
#define CPSR_E    8
#define CPSR_A    9
#define CPSR_I   10
#define CPSR_F   11
#define CPSR_T   12
#define CPSR_M   13

struct CPU_STATE {
	uint32_t reg[16];
	uint32_t CPSR;
};
typedef struct CPU_STATE CPU_STATE;

CPU_STATE* cpu_init();
void cpu_free(CPU_STATE* cpu);
void cpu_set_PC(CPU_STATE* cpu, uint32_t addr);
uint32_t cpu_get_PC(CPU_STATE* cpu);
void cpu_advance(MACHINE_STATE* ms);
uint32_t cpu_get_cpsr_field(CPU_STATE* cpu, uint32_t field);
void cpu_set_cpsr_field(CPU_STATE* cpu, uint32_t field, uint32_t value);

#else

struct CPU_STATE;
typedef struct CPU_STATE CPU_STATE;

#endif
