#ifndef __ADDRESSING_MODE4_H__
#define __ADDRESSING_MODE4_H__

#include <stdint.h>

#include "cpu.h"
#include "addressing_mode4_handlers.h"

addressing_mode4_result addressing_mode4_resolve(CPU_STATE* cpu, uint32_t i);

#endif
