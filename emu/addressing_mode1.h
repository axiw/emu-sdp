#ifndef __ADDRESSING_MODE1_H__
#define __ADDRESSING_MODE1_H__

#include <stdint.h>

#include "cpu.h"
#include "addressing_mode1_handlers.h"

addressing_mode1_result addressing_mode1_resolve(CPU_STATE* cpu, uint32_t i);

#endif
