#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

#include "cpu.h"

//#define HOST_LITTLE_ENDIAN
//#define TARGET_LITTLE_ENDIAN

#ifdef HOST_LITTLE_ENDIAN
#define HOST_TO_TARGET_WORD(x) swap_endian_word(x)
#define HOST_TO_TARGET_DWORD(x) swap_endian_dword(x)
#else
#define HOST_TO_TARGET_WORD(x) (x)
#define HOST_TO_TARGET_DWORD(x) (x)
#endif

uint16_t swap_endian_word(uint16_t x);
uint32_t swap_endian_dword(uint32_t x);

// a must be less than or equal to b
#define MASK(a, b) (((uint64_t)1 << (b)) - ((uint64_t)1 << (a)) + ((uint64_t)1 << (b)))
#define BITS(x, a, b) (((x) & MASK((a), (b))) >> (a))
#define BIT(x, a)  BITS((x), (a), (a))

#define UNUSED(x) (void)(x)

const char* get_ascii_conditional(uint8_t c);
int condition_passed(CPU_STATE* cpu, uint8_t cond);

#endif
