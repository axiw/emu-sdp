#ifndef __ALU_H__
#define __ALU_H__

#include <stdint.h>

uint32_t alu_add_carry(uint32_t a, uint32_t b, int c, int* N_out, int* Z_out, int* C_out, int* V_out);
uint32_t alu_add(uint32_t a, uint32_t b, int* N_out, int* Z_out, int* C_out, int* V_out);
uint32_t alu_sub_carry(uint32_t a, uint32_t b, int c, int* N_out, int* Z_out, int* C_out, int* V_out);
uint32_t alu_sub(uint32_t a, uint32_t b, int* N_out, int* Z_out, int* C_out, int* V_out);
uint32_t alu_and(uint32_t a, uint32_t b, int* N_out, int* Z_out);
uint32_t alu_xor(uint32_t a, uint32_t b, int* N_out, int* Z_out);

#endif
