#ifndef __THUMB_OPCODE_HANDLERS_H__
#define __THUMB_OPCODE_HANDLERS_H__

#include <stdint.h>

#include "cpu.h"
#include "mmu.h"

int thumb_opcode_invalid_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_adc_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_add1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_add2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_add3_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_add4_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_add5_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_add6_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_add7_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_and_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_asr1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_asr2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_b1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_bic_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_bl_blx1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_bl_blx2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_bx_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_cmn_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_cmp1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_cmp2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_cmp3_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_eor_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_ldmia_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_ldr1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_ldr2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_ldr3_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_ldr4_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_ldrb1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_ldrb2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_ldrh1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_ldrh2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_lsl1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_lsl2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_lsr1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_lsr2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_mov1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_mov3_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_mul_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_mvn_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_orr_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_neg_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_pop_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_push_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_ror_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_sbc_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_stmia_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_str1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_str2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_str3_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_strb1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_strb2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_strh1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_strh2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_sub1_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_sub2_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_sub3_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_sub4_handler(MACHINE_STATE* ms, uint16_t i);
int thumb_opcode_tst_handler(MACHINE_STATE* ms, uint16_t i);

#endif
