#ifndef __ADDRESSING_MODE2_H__
#define __ADDRESSING_MODE2_H__

#include <stdint.h>

#include "cpu.h"
#include "addressing_mode2_handlers.h"

addressing_mode2_result addressing_mode2_resolve(CPU_STATE* cpu, uint32_t i);

#endif
