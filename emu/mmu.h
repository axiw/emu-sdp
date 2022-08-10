#ifndef __MMU_H__
#define __MMU_H__

#include <stdint.h>
#include <stddef.h>
#include "machine.h"

struct MMU_MAPPING {
	size_t size;
	uint32_t guest_addr;
	void* host_addr;
	uint32_t (*read_cb)(MACHINE_STATE* ms, void* data, uint32_t relative_addr, uint32_t read_size);
	void (*write_cb)(MACHINE_STATE* ms, void* data, uint32_t relative_addr, uint32_t value, uint32_t write_size);
	void* data;
};
typedef struct MMU_MAPPING MMU_MAPPING;

struct MMU_STATE {
	uint64_t num_mappings;
	MMU_MAPPING *mem_mappings;
};
typedef struct MMU_STATE MMU_STATE;

MMU_STATE* mmu_init();
void mmu_free();
void mmu_dump_mappings(MMU_STATE* mmu);
void mmu_add_mapping(MMU_STATE* mmu, uint32_t guest_addr, size_t size, uint32_t (*read_cb)(MACHINE_STATE* ms, void* data, uint32_t relative_addr, uint32_t read_size), void (*write_cb)(MACHINE_STATE* ms, void* data, uint32_t relative_addr, uint32_t value, uint32_t write_size), void* data);
void mmu_delete_mapping(MMU_STATE* mmu, uint32_t guest_addr);
void mmu_write_byte(MACHINE_STATE* ms, uint8_t b, uint32_t addr);
void mmu_write_word(MACHINE_STATE* ms, uint16_t w, uint32_t addr);
void mmu_write_dword(MACHINE_STATE* ms, uint32_t dw, uint32_t addr);
uint8_t mmu_read_byte(MACHINE_STATE* ms, uint32_t addr);
uint16_t mmu_read_word(MACHINE_STATE* ms, uint32_t addr);
uint32_t mmu_read_dword(MACHINE_STATE* ms, uint32_t addr);
void mmu_copy_in(MMU_STATE* mmu, void* source, uint32_t addr, uint32_t size);
void mmu_copy_out(MMU_STATE* mmu, void* dest, uint32_t addr, uint32_t size);

#else

struct MMU_MAPPING;
typedef struct MMU_STATE MMU_STATE;

struct MMU_STATE;
typedef struct MMU_STATE MMU_STATE;

#endif
