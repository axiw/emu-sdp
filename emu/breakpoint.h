#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__

#include <stdint.h>
#include "machine.h"

void add_breakpoint(uint32_t addr, int (*cb)(MACHINE_STATE* ms, uint32_t addr));
void remove_breakpoint(uint32_t addr);
int is_breakpoint(MACHINE_STATE* ms, uint32_t addr);

#endif
