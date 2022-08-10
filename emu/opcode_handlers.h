#ifndef __OPCODE_HANDLERS_H__
#define __OPCODE_HANDLERS_H__

#include <stdint.h>

#include "cpu.h"
#include "mmu.h"

int opcode_invalid_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_adc_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_add_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_and_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_b_bl_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_bic_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_bx_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_clz_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_cmn_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_cmp_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_eor_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_ldm_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_ldr_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_ldrb_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_mov_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_mvn_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_nop_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_orr_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_rsb_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_stm_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_str_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_strb_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_sub_handler(MACHINE_STATE* ms, uint32_t i);
int opcode_tst_handler(MACHINE_STATE* ms, uint32_t i);

#endif
