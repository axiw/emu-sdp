#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "log.h"
#include "machine.h"
#include "mmu.h"

//TODO: The MMU currently doesn't support writing across allocated regions. Even if they are virtualy contiguous.
// Ex.
// Alloc at 0x100 of 0x100
// Alloc at 0x200 of 0x100
// A 4 byte write at 0x1FE will fail.

extern int invalid_opcode_done;

MMU_STATE* mmu_init() {
	MMU_STATE* mmu = calloc(sizeof(MMU_STATE), 1);
	mmu->num_mappings = 0;
	mmu->mem_mappings = NULL;
	
	return mmu;
}

void mmu_free(MMU_STATE* mmu) {
	if (!mmu)
		return;
	
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		free(mmu->mem_mappings[i].host_addr);
	}
	free(mmu->mem_mappings);
	free(mmu);
}

void mmu_dump_mappings(MMU_STATE* mmu) {
	printf("Memory Mappings:\n");
        FILE* f = fopen("core.dmp", "wb");
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		printf("\t0x%08lX - 0x%08lX (0x%lX)\n", (size_t)mmu->mem_mappings[i].guest_addr, mmu->mem_mappings[i].guest_addr + mmu->mem_mappings[i].size, mmu->mem_mappings[i].size);
                fwrite(mmu->mem_mappings[i].host_addr, mmu->mem_mappings[i].size, 1, f);
	}
	fclose(f);
}

void mmu_add_mapping(MMU_STATE* mmu, uint32_t guest_addr, size_t size, uint32_t (*read_cb)(MACHINE_STATE* ms, void* data, uint32_t relative_addr, uint32_t read_size), void (*write_cb)(MACHINE_STATE* ms, void* data,  uint32_t relative_addr, uint32_t value, uint32_t write_size), void* data) {
	if (mmu->num_mappings == 0) {
		mmu->mem_mappings = malloc(sizeof(MMU_MAPPING));
		mmu->mem_mappings[0].size = size;
		mmu->mem_mappings[0].guest_addr = guest_addr;
		if (read_cb == NULL) {
			mmu->mem_mappings[0].host_addr = calloc(size, 1);
			mmu->mem_mappings[0].read_cb = NULL;
			mmu->mem_mappings[0].write_cb = NULL;
			mmu->mem_mappings[0].data = NULL;
		} else {
			mmu->mem_mappings[0].host_addr = NULL;
			mmu->mem_mappings[0].read_cb = read_cb;
			mmu->mem_mappings[0].write_cb = write_cb;
			mmu->mem_mappings[0].data = data;
		}
		mmu->num_mappings++;
		return;
	}
	
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		// Determine if we should go before or after this mapping.
		if (guest_addr < mmu->mem_mappings[i].guest_addr) {
			// We go before
			mmu->mem_mappings = realloc(mmu->mem_mappings, sizeof(MMU_MAPPING) * (mmu->num_mappings + 1));
			memmove(&mmu->mem_mappings[i + 1], &mmu->mem_mappings[i], (mmu->num_mappings - i) * sizeof(MMU_MAPPING));
			mmu->mem_mappings[i].size = size;
			mmu->mem_mappings[i].guest_addr = guest_addr;
			if (read_cb == NULL) {
				mmu->mem_mappings[i].host_addr = calloc(size, 1);
				mmu->mem_mappings[i].read_cb = NULL;
				mmu->mem_mappings[i].write_cb = NULL;
				mmu->mem_mappings[i].data = NULL;
			} else {
				mmu->mem_mappings[i].host_addr = NULL;
				mmu->mem_mappings[i].read_cb = read_cb;
				mmu->mem_mappings[i].write_cb = write_cb;
				mmu->mem_mappings[i].data = data;
			}
			mmu->num_mappings++;
			return;
		} else {
			// We go after
			if (i == (mmu->num_mappings - 1)) {
				// We are at the end so add here.
				mmu->mem_mappings = realloc(mmu->mem_mappings, sizeof(MMU_MAPPING) * (mmu->num_mappings + 1));
				mmu->mem_mappings[i + 1].size = size;
				mmu->mem_mappings[i + 1].guest_addr = guest_addr;
				if (read_cb == NULL) {
					mmu->mem_mappings[i + 1].host_addr = calloc(size, 1);
					mmu->mem_mappings[i + 1].read_cb = NULL;
					mmu->mem_mappings[i + 1].write_cb = NULL;
					mmu->mem_mappings[i + 1].data = NULL;
				} else {
					mmu->mem_mappings[i + 1].host_addr = NULL;
					mmu->mem_mappings[i + 1].read_cb = read_cb;
					mmu->mem_mappings[i + 1].write_cb = write_cb;
					mmu->mem_mappings[i + 1].data = data;
				}
				mmu->num_mappings++;
				return;
			} else {
				continue;
			}
		}
	}
	
	ponii_log(LOG_LEVEL_ERROR, "Failed to add memory mapping!\n");
}

void mmu_delete_mapping(MMU_STATE* mmu, uint32_t guest_addr) {
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		if (mmu->mem_mappings[i].guest_addr == guest_addr) {
			free(mmu->mem_mappings[i].host_addr);
			mmu->num_mappings--;
			memmove(&mmu->mem_mappings[i], &mmu->mem_mappings[i + 1], sizeof(MMU_MAPPING) * (mmu->num_mappings - i));
			return;
		}
	}
}

void mmu_write_byte(MACHINE_STATE* ms, uint8_t b, uint32_t addr) {
	MMU_STATE* mmu = ms->mmu;
	
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		MMU_MAPPING *cur = &mmu->mem_mappings[i];
		uint64_t base = cur->guest_addr;
		uint64_t top = cur->guest_addr + cur->size;
		if (addr >= base &&
			addr < top
		) {
			uint32_t write_addr = addr - cur->guest_addr;
			if (cur->write_cb == NULL) {
				*(uint8_t*)(cur->host_addr + write_addr) = b;
			} else {
				cur->write_cb(ms, cur->data, write_addr, (uint32_t)b, 1);
			}
			return;
		}
	}
	
	ponii_log(LOG_LEVEL_ERROR, "Didn't write to memory: 0x%08X\n", addr);
	invalid_opcode_done = 1;
}

void mmu_write_word(MACHINE_STATE* ms, uint16_t w, uint32_t addr) {
	MMU_STATE* mmu = ms->mmu;
	
	//mmu_write_byte(ms, (w & 0x00FF) >> 0, addr + 0);
	//mmu_write_byte(ms, (w & 0xFF00) >> 8, addr + 1);
	
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		MMU_MAPPING *cur = &mmu->mem_mappings[i];
		uint64_t base = cur->guest_addr;
		uint64_t top = cur->guest_addr + cur->size;
		if (addr >= base &&
			addr < top
		) {
			if (addr + 2 > top) {
				ponii_log(LOG_LEVEL_ERROR, "Can't write past mapping\n");
				invalid_opcode_done = 1;
				return;
			}
			uint32_t write_addr = addr - cur->guest_addr;
			if (cur->write_cb == NULL) {
				*(uint16_t*)(cur->host_addr + write_addr) = HOST_TO_TARGET_WORD(w);
			} else {
				cur->write_cb(ms, cur->data, write_addr, (uint32_t)HOST_TO_TARGET_WORD(w), 2);
			}
			return;
		}
	}
	
	ponii_log(LOG_LEVEL_ERROR, "Didn't write to memory: 0x%08X\n", addr);
	invalid_opcode_done = 1;
}

void mmu_write_dword(MACHINE_STATE* ms, uint32_t dw, uint32_t addr) {
	MMU_STATE* mmu = ms->mmu;
	
	//mmu_write_byte(ms, (dw & 0x000000FF) >>  0, addr + 0);
	//mmu_write_byte(ms, (dw & 0x0000FF00) >>  8, addr + 1);
	//mmu_write_byte(ms, (dw & 0x00FF0000) >> 16, addr + 2);
	//mmu_write_byte(ms, (dw & 0xFF000000) >> 24, addr + 3);
	
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		MMU_MAPPING *cur = &mmu->mem_mappings[i];
		uint64_t base = cur->guest_addr;
		uint64_t top = cur->guest_addr + cur->size;
		if (addr >= base &&
			addr < top
		) {
			if (addr + 4 > top) {
				ponii_log(LOG_LEVEL_ERROR, "Can't write past mapping\n");
				invalid_opcode_done = 1;
				return;
			}
			uint32_t write_addr = addr - cur->guest_addr;
			if (cur->write_cb == NULL) {
				*(uint32_t*)(cur->host_addr + write_addr) = HOST_TO_TARGET_DWORD(dw);
			} else {
				cur->write_cb(ms, cur->data, write_addr, (uint32_t)HOST_TO_TARGET_DWORD(dw), 4);
			}
			return;
		}
	}
	
	ponii_log(LOG_LEVEL_ERROR, "Didn't write to memory: 0x%08X\n", addr);
	invalid_opcode_done = 1;
}

uint8_t mmu_read_byte(MACHINE_STATE* ms, uint32_t addr) {
	MMU_STATE* mmu = ms->mmu;
	
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		MMU_MAPPING *cur = &mmu->mem_mappings[i];
		uint64_t base = cur->guest_addr;
		uint64_t top = cur->guest_addr + cur->size;
		if (addr >= base &&
			addr < top
		) {
			uint32_t read_addr = addr - cur->guest_addr;
			if (cur->read_cb == NULL) {
				return *(uint8_t*)(cur->host_addr + read_addr);
			} else {
				return cur->read_cb(ms, cur->data, read_addr, 1);
			}
		}
	}
	
	ponii_log(LOG_LEVEL_ERROR, "Didn't read from memory: 0x%08X\n", addr);
	invalid_opcode_done = 1;
	return 0;
}

uint16_t mmu_read_word(MACHINE_STATE* ms, uint32_t addr) {
	MMU_STATE* mmu = ms->mmu;
	//uint16_t res = 0;
	
	//res |= mmu_read_byte(ms, addr + 0) << 0;
	//res |= mmu_read_byte(ms, addr + 1) << 8;
	//return HOST_TO_TARGET_WORD(res);
	
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		MMU_MAPPING *cur = &mmu->mem_mappings[i];
		uint64_t base = cur->guest_addr;
		uint64_t top = cur->guest_addr + cur->size;
		if (addr >= base &&
			addr < top
		) {
			if (addr + 2 > top) {
				ponii_log(LOG_LEVEL_ERROR, "Can't read past mapping\n");
				invalid_opcode_done = 1;
				return 0;
			}
			uint32_t read_addr = addr - cur->guest_addr;
			if (cur->read_cb == NULL) {
				return HOST_TO_TARGET_WORD(*(uint16_t*)(cur->host_addr + read_addr));
			} else {
				return HOST_TO_TARGET_WORD(cur->read_cb(ms, cur->data, read_addr, 2));
			}
		}
	}
	
	ponii_log(LOG_LEVEL_ERROR, "Didn't read from memory: 0x%08X\n", addr);
	invalid_opcode_done = 1;
	return 0;
}

uint32_t mmu_read_dword(MACHINE_STATE* ms, uint32_t addr) {
	MMU_STATE* mmu = ms->mmu;
	//uint32_t res = 0;
	
	//res |= mmu_read_byte(ms, addr + 0) <<  0;
	//res |= mmu_read_byte(ms, addr + 1) <<  8;
	//res |= mmu_read_byte(ms, addr + 2) << 16;
	//res |= mmu_read_byte(ms, addr + 3) << 24;
	//return HOST_TO_TARGET_DWORD(res);
	
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		MMU_MAPPING *cur = &mmu->mem_mappings[i];
		uint64_t base = cur->guest_addr;
		uint64_t top = cur->guest_addr + cur->size;
		if (addr >= base &&
			addr < top
		) {
			if (addr + 4 > top) {
				ponii_log(LOG_LEVEL_ERROR, "Can't read past mapping\n");
				invalid_opcode_done = 1;
				return 0;
			}
			uint32_t read_addr = addr - cur->guest_addr;
			if (cur->read_cb == NULL) {
				return HOST_TO_TARGET_DWORD(*(uint32_t*)(cur->host_addr + read_addr));
			} else {
				return HOST_TO_TARGET_DWORD(cur->read_cb(ms, cur->data, read_addr, 4));
			}
		}
	}
	
	ponii_log(LOG_LEVEL_ERROR, "Didn't read from memory: 0x%08X\n", addr);
	invalid_opcode_done = 1;
	return 0;
}

void mmu_copy_in(MMU_STATE* mmu, void* source, uint32_t addr, uint32_t size) {
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		MMU_MAPPING *cur = &mmu->mem_mappings[i];
		uint64_t base = cur->guest_addr;
		uint64_t top = cur->guest_addr + cur->size;
		if (addr >= base &&
			addr < top
		) {
			if (addr + size > top) {
				ponii_log(LOG_LEVEL_ERROR, "Can't copy past mapping\n");
				return;
			}
			uint32_t write_addr = addr - cur->guest_addr;
			memcpy(cur->host_addr + write_addr, source, size);
			return;
		}
	}
}

void mmu_copy_out(MMU_STATE* mmu, void* dest, uint32_t addr, uint32_t size) {
	for (uint64_t i = 0; i < mmu->num_mappings; i++) {
		MMU_MAPPING *cur = &mmu->mem_mappings[i];
		uint64_t base = cur->guest_addr;
		uint64_t top = cur->guest_addr + cur->size;
		if (addr >= base &&
			addr < top
		) {
			if (addr + size > top) {
				ponii_log(LOG_LEVEL_ERROR, "Can't copy past mapping\n");
				return;
			}
			uint32_t read_addr = addr - cur->guest_addr;
			memcpy(dest, cur->host_addr + read_addr, size);
			return;
		}
	}
}
