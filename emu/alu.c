#include <stdint.h>

#include "alu.h"
#include "common.h"

#define ADD_OVERFLOW(a, b, x) (BIT((a), 31) == BIT((b), 31) && BIT((x), 31) ^ BIT((a), 31))
#define SUB_OVERFLOW(a, b, x) ADD_OVERFLOW(a, (~(((uint64_t)(b)) - 1)), x)

uint32_t alu_add_carry(uint32_t a, uint32_t b, int c, int* N_out, int* Z_out, int* C_out, int* V_out) {
	uint64_t x = (uint64_t)a + (uint64_t)b;
	int V = ADD_OVERFLOW(a, b, x);
	uint64_t y = (uint64_t)x + (uint64_t)c;
	if (!V)
		V = ADD_OVERFLOW(x, c, y);
	
	int N = BIT(y, 31);
	int Z = ((uint32_t)y) ? 0 : 1;
	int C = BIT(y, 32);
	
	if (N_out)
		*N_out = N;
	if (Z_out)
		*Z_out = Z;
	if (C_out)
		*C_out = C;
	if (V_out)
		*V_out = V;
	
	return (uint32_t)y;
}

uint32_t alu_add(uint32_t a, uint32_t b, int* N_out, int* Z_out, int* C_out, int* V_out) {
	return alu_add_carry(a, b, 0, N_out, Z_out, C_out, V_out);
}

uint32_t alu_sub_carry(uint32_t a, uint32_t b, int c, int* N_out, int* Z_out, int* C_out, int* V_out) {
	uint64_t x = (uint64_t)a - (uint64_t)b;
	int V = SUB_OVERFLOW(a, b, x);
	uint64_t y = (uint64_t)x - (uint64_t)(!c);
	if (!V)
		V = SUB_OVERFLOW(x, c, y);
	
	int N = BIT(y, 31);
	int Z = ((uint32_t)y) ? 0 : 1;
	int C = !BIT(y, 32);
	
	if (N_out)
		*N_out = N;
	if (Z_out)
		*Z_out = Z;
	if (C_out)
		*C_out = C;
	if (V_out)
		*V_out = V;
	
	return (uint32_t)y;
}

uint32_t alu_sub(uint32_t a, uint32_t b, int* N_out, int* Z_out, int* C_out, int* V_out) {
	return alu_sub_carry(a, b, 1, N_out, Z_out, C_out, V_out);
}

uint32_t alu_and(uint32_t a, uint32_t b, int* N_out, int* Z_out) {
	uint32_t x = a & b;
	
	int N = BIT(x, 31);
	int Z = x ? 0 : 1;
	
	if (N_out)
		*N_out = N;
	if (Z_out)
		*Z_out = Z;
	
	return x;
}

uint32_t alu_xor(uint32_t a, uint32_t b, int* N_out, int* Z_out) {
	uint32_t x = a ^ b;
	
	int N = BIT(x, 31);
	int Z = x ? 0 : 1;
	
	if (N_out)
		*N_out = N;
	if (Z_out)
		*Z_out = Z;
	
	return x;
}
