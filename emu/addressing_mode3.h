#ifndef __ADDRESSING_MODE3_H__
#define __ADDRESSING_MODE3_H__

#include <stdint.h>

#include "cpu.h"
#include "addressing_mode3_handlers.h"

addressing_mode3_result addressing_mode3_resolve(CPU_STATE* cpu, uint32_t i);

#endif
