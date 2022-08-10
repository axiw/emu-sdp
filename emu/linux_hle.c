#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "cpu.h"
#include "linux_hle.h"
#include "log.h"
#include "machine.h"
#include "mmu.h"

extern int invalid_opcode_done;

HEAP_DESCRIPTOR debug_hd;
static void linux_cleanup_heap(HEAP_DESCRIPTOR* hd);

// Don't pass me giant paths please
static int open_relative(const char* base, const char* relative, int mode) {
	// After a lot of googling I've come to the conclusion that there is no right answer here. Fuck it.
#define MAX_PATH_LEN (4096+1)
	char full_path[MAX_PATH_LEN];
	snprintf(full_path, sizeof(full_path), "%s/%s", base, relative);
	return open(full_path, mode);
}

LINUX_STATE* linux_init(const char* fs_root, uint32_t n_qd, uint32_t n_fd, uint32_t n_hd, uint32_t n_td) {
	LINUX_STATE* linux_ctx = malloc(sizeof(LINUX_STATE));
	
	linux_ctx->fs_root = strdup(fs_root);
	linux_ctx->q_arr = calloc(sizeof(QUEUE_DESCRIPTOR*), n_qd);
	linux_ctx->q_sz = n_qd;
	linux_ctx->fd_arr = calloc(sizeof(FILE_DESCRIPTOR*), n_fd);
	linux_ctx->fd_sz = n_qd;
	linux_ctx->hd_arr = calloc(sizeof(HEAP_DESCRIPTOR*), n_hd);
	linux_ctx->hd_sz = n_qd;
	linux_ctx->td_arr = calloc(sizeof(THREAD_DESCRIPTOR*), n_td);
	linux_ctx->td_sz = n_td;
	
	linux_ctx->td_arr[0] = malloc(sizeof(THREAD_DESCRIPTOR));
	linux_ctx->td_arr[0]->cpu = cpu_init();
	linux_ctx->td_arr[0]->priority = 0x40; // Randomly choosen
	linux_ctx->td_arr[0]->thread_state = STATE_RUNNING;
	linux_ctx->current_thread = 0;
	
	return linux_ctx;
}

CPU_STATE* linux_get_thread_cpu(MACHINE_STATE* ms) {
	return ms->linux_ctx->td_arr[ms->linux_ctx->current_thread]->cpu;
}

void linux_free(LINUX_STATE* linux_ctx) {
	if (linux_ctx) {
		free(linux_ctx->fs_root);
		for (uint32_t i = 0; i != linux_ctx->q_sz; i++)
			free(linux_ctx->q_arr[i]);
		free(linux_ctx->q_arr);
		for (uint32_t i = 0; i != linux_ctx->fd_sz; i++)
			free(linux_ctx->fd_arr[i]);
		free(linux_ctx->fd_arr);
		for (uint32_t i = 0; i != linux_ctx->hd_sz; i++)
			free(linux_ctx->hd_arr[i]);
		free(linux_ctx->hd_arr);
		for (uint32_t i = 0; i != linux_ctx->td_sz; i++) {
			if (linux_ctx->td_arr[i])
				cpu_free(linux_ctx->td_arr[i]->cpu);
			free(linux_ctx->td_arr[i]);
		}
		free(linux_ctx->td_arr);
		free(linux_ctx);
	}
	linux_cleanup_heap(&debug_hd);
}

void linux_syscall(MACHINE_STATE* ms, int syscall) {
	uint32_t ret = 0;
        
        switch (syscall) {
                case 1:
                        ret = LINUX_exit(ms);
                        break;
                default:
                        ponii_log(LOG_LEVEL_ERROR, "UNKNOWN (%d)\n", syscall);
                        invalid_opcode_done = 1;
                        break;
        }
        linux_get_thread_cpu(ms)->reg[0] = ret;
}

uint32_t LINUX_CreateThread(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t proc = cpu->reg[0];
	uint32_t arg = cpu->reg[1];
	uint32_t stack_top = cpu->reg[2];
	uint32_t stacksize = cpu->reg[3];
	uint32_t priority = cpu->reg[4];
	uint32_t detached = cpu->reg[5];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_CreateThread(0x%08X, 0x%08X, 0x%08X, 0x%X, %d, %d)\n", proc, arg, stack_top, stacksize, priority, detached);
	
	uint32_t thread_index = 0;
	for (; thread_index != ms->linux_ctx->td_sz && ms->linux_ctx->td_arr[thread_index]; thread_index++)
		;
	
	if (thread_index == ms->linux_ctx->td_sz) {
		ponii_log(LOG_LEVEL_ERROR, "Out of thread descriptors\n");
		return -1;
	}
	
	THREAD_DESCRIPTOR* new_td = malloc(sizeof(THREAD_DESCRIPTOR));
	new_td->cpu = cpu_init();
	new_td->priority = priority;
	//TODO: Does detached mean start immediatly vs start paused?
	// For now I'm just going to start them all "paused"
	new_td->thread_state = STATE_PAUSED;
	
	mmu_add_mapping(ms->mmu, stack_top - stacksize, stacksize, NULL, NULL, NULL);
	
	cpu_set_PC(new_td->cpu, proc);
	new_td->cpu->reg[REG_SP] = stack_top;
	
	ms->linux_ctx->td_arr[thread_index] = new_td;
	
	return thread_index;
}

uint32_t LINUX_CancelThread(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t threadid = cpu->reg[0];
	uint32_t return_value = cpu->reg[1];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_CancelThread(%d, 0x%08X)\n", threadid, return_value);
	
	if (threadid >= ms->linux_ctx->td_sz) {
		ponii_log(LOG_LEVEL_ERROR, "Attempt to cancel thread outside of the thread descriptor range\n");
		return -1;
	}
	
	if (ms->linux_ctx->td_arr[threadid] == NULL) {
		ponii_log(LOG_LEVEL_ERROR, "Attempt to cancel invalid thread\n");
		return -1;
	}
	
	
	
	return -1;
}

uint32_t LINUX_GetThreadId(MACHINE_STATE *ms) {
	UNUSED(ms);
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_GetThreadId()\n");
	
	return -1;
}

uint32_t LINUX_StartThread(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t threadid = cpu->reg[0];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_StartThread(%d)\n", threadid);
	
	return -1;
}

uint32_t LINUX_GetThreadPriority(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t threadid = cpu->reg[0];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_GetThreadPriority(%d)\n", threadid);
	
	return 40; // Random number *shrug*
}

uint32_t LINUX_SetThreadPriority(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t threadid = cpu->reg[0];
	uint32_t priority = cpu->reg[1];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_SetThreadPriority(%d, %d)\n", threadid, priority);
	
	return 0;
}

uint32_t LINUX_CreateMessageQueue(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	LINUX_STATE* linux_ctx = ms->linux_ctx;
	uint32_t ptr = cpu->reg[0];
	uint32_t n_msgs = cpu->reg[1];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_CreateMessageQueue(0x%08X, %d)\n", ptr, n_msgs);
	
	uint32_t i = 0;
	while (i != linux_ctx->q_sz && linux_ctx->q_arr[i] != NULL)
		i++;
	
	if (i == linux_ctx->q_sz){
		ponii_log(LOG_LEVEL_ERROR, "Out of queue descriptors\n");
		return -1;
	}
	
	linux_ctx->q_arr[i] = malloc(sizeof(QUEUE_DESCRIPTOR));
	
	linux_ctx->q_arr[i]->ptr = ptr;
	linux_ctx->q_arr[i]->n_msgs = n_msgs;
	linux_ctx->q_arr[i]->n_filled = 0;
	
	return i;
}

uint32_t LINUX_DestroyMessageQueue(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	LINUX_STATE* linux_ctx = ms->linux_ctx;
	uint32_t queueid = cpu->reg[0];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_DestroyMessageQueue(%d)\n", queueid);
	
	if (queueid >= linux_ctx->q_sz) {
		ponii_log(LOG_LEVEL_ERROR, "Invalid queue descriptor\n");
		return -1;
	}
	
	free(linux_ctx->q_arr[queueid]);
	linux_ctx->q_arr[queueid] = NULL;
	
	return 0;
}

uint32_t LINUX_SendMessage(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	LINUX_STATE* linux_ctx = ms->linux_ctx;
	uint32_t queueid = cpu->reg[0];
	uint32_t message = cpu->reg[1];
	uint32_t flags = cpu->reg[2];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_SendMessage(%d, 0x%08X, 0x%08X)\n", queueid, message, flags);
	
	if (queueid >= linux_ctx->q_sz) {
		ponii_log(LOG_LEVEL_ERROR, "Invalid queue descriptor\n");
		return -1;
	}
	
	QUEUE_DESCRIPTOR* q = linux_ctx->q_arr[queueid];
	if (q == NULL) {
		ponii_log(LOG_LEVEL_ERROR, "Queue not initilized yet\n");
		return -1;
	}
	
	if (q->n_msgs == q->n_filled) {
		ponii_log(LOG_LEVEL_ERROR, "Queue is full\n");
		return -1;
	}
	
	mmu_write_dword(ms, message, q->ptr + (4 * q->n_filled++));
	
	return 0;
}

uint32_t LINUX_ReceiveMessage(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	LINUX_STATE* linux_ctx = ms->linux_ctx;
	uint32_t queueid = cpu->reg[0];
	uint32_t message_ptr = cpu->reg[1];
	uint32_t flags = cpu->reg[2];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_ReceiveMessage(%d, 0x%08X, 0x%08X)\n", queueid, message_ptr, flags);
	
	if (queueid >= linux_ctx->q_sz) {
		ponii_log(LOG_LEVEL_ERROR, "Invalid queue descriptor\n");
		return -1;
	}
	
	QUEUE_DESCRIPTOR* q = linux_ctx->q_arr[queueid];
	if (q == NULL) {
		ponii_log(LOG_LEVEL_ERROR, "Queue not initilized yet\n");
		return -1;
	}
	
	if (q->n_filled == 0) {
		ponii_log(LOG_LEVEL_ERROR, "TODO: Queue is empty and should hang\n");
		return 0;
	}
	
	uint32_t message = mmu_read_dword(ms, q->ptr + (4 * --q->n_filled));
	mmu_write_dword(ms, message, message_ptr);
	
	return 0;
}

uint32_t LINUX_CreateTimer(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	uint32_t time_us = cpu->reg[0];
	uint32_t repeat_time_us = cpu->reg[1];
	uint32_t queueid = cpu->reg[2];
	uint32_t message = cpu->reg[3];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_CreateTimer(0x%X, 0x%X, %d, 0x%08X)\n", time_us, repeat_time_us, queueid, message);
	
	return 0;
}

uint32_t LINUX_CreateHeap(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	LINUX_STATE* linux_ctx = ms->linux_ctx;
	uint32_t heap_start = cpu->reg[0];
	uint32_t heap_size = cpu->reg[1];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_CreateHeap(0x%08X, 0x%X)\n", heap_start, heap_size);
	
	uint32_t i = 0;
	while (i != linux_ctx->hd_sz && linux_ctx->hd_arr[i] != NULL)
		i++;
	
	if (i == linux_ctx->hd_sz){
		ponii_log(LOG_LEVEL_ERROR, "Out of heap descriptors\n");
		return -1;
	}
	
	linux_ctx->hd_arr[i] = malloc(sizeof(HEAP_DESCRIPTOR));
	
	linux_ctx->hd_arr[i]->guest_start_addr = heap_start;
	linux_ctx->hd_arr[i]->heap_size = heap_size;
	
	//TODO: Enable when using the real alloc algorithms
	//mmu_write_dword(mmu, 0x00000000, heap_start);
	
	return i;
}

// HEAP_AREA must be a multiple of HEAP_GRANULARITY
#define HEAP_AREA 0x30000000
#define HEAP_SIZE 0x01000000 // 16 MB for the global heap (ignored right now TODO)
#define HEAP_PADDING 0x1000
#define HEAP_GRANULARITY 0x4
#define HEAP_UAF_CHECKER

// Finds and allocates a new block of memory for the heap.
// Returns the new address of the block.
static uint32_t add_new_block(MMU_STATE* mmu, HEAP_DESCRIPTOR* hd, uint32_t alloc_size, uint32_t alloc_alignment) {
	uint32_t alignment = HEAP_GRANULARITY;
	if (alloc_alignment != 0)
		alignment = alloc_alignment;
	
	if (hd->block_map_sz == 0) {
		hd->block_map = malloc(sizeof(BLOCK_MAP*));
		hd->block_map[0] = malloc(sizeof(BLOCK_MAP));
		hd->block_map[0]->block_start = HEAP_AREA;
		hd->block_map[0]->block_size = alloc_size;
		hd->block_map_sz++;
		
		mmu_add_mapping(mmu, hd->block_map[0]->block_start, hd->block_map[0]->block_size, NULL, NULL, NULL);
		return hd->block_map[0]->block_start;
	}
	
	for (uint32_t i = 0; i < hd->block_map_sz; i++) {
		if (i == hd->block_map_sz - 1) {
			// We are at the end of the list and none of the blocks had enough space between them, so we add at the end.
			hd->block_map = realloc(hd->block_map, (hd->block_map_sz + 1) * sizeof(BLOCK_MAP*));
			hd->block_map[i + 1] = malloc(sizeof(BLOCK_MAP));
			hd->block_map[i + 1]->block_start = hd->block_map[i]->block_start + hd->block_map[i]->block_size + HEAP_PADDING + alignment;
			hd->block_map[i + 1]->block_start -= hd->block_map[i + 1]->block_start % alignment;
			hd->block_map[i + 1]->block_size = alloc_size;
			hd->block_map_sz++;
			
			mmu_add_mapping(mmu, hd->block_map[i + 1]->block_start, hd->block_map[i + 1]->block_size, NULL, NULL, NULL);
			return hd->block_map[i + 1]->block_start;
		} else {
			BLOCK_MAP* bm_start = hd->block_map[i];
			BLOCK_MAP* bm_end = hd->block_map[i + 1];
			if ((bm_end->block_start - (bm_start->block_start + bm_start->block_size)) >= (alloc_size + (2 * HEAP_PADDING) + alignment)) {
				BLOCK_MAP* new_bm = malloc(sizeof(BLOCK_MAP));
				new_bm->block_start = bm_start->block_start + bm_start->block_size + HEAP_PADDING + alignment;
				new_bm->block_start -= new_bm->block_start % alignment;
				new_bm->block_size = alloc_size;
				
				hd->block_map = realloc(hd->block_map, (hd->block_map_sz + 1) * sizeof(BLOCK_MAP*));
				memmove(&hd->block_map[i + 2], hd->block_map[i + 1], (hd->block_map_sz - i - 1) * sizeof(BLOCK_MAP*));
				hd->block_map[i + 1] = new_bm;
				hd->block_map_sz++;
				
				mmu_add_mapping(mmu, hd->block_map[i + 1]->block_start, hd->block_map[i + 1]->block_size, NULL, NULL, NULL);
				return hd->block_map[i + 1]->block_start;
			}
		}
	}
	
	ponii_log(LOG_LEVEL_ERROR, "Failed to add block mapping!\n");
	return 0;
}

static void remove_block(MMU_STATE* mmu, HEAP_DESCRIPTOR* hd, uint32_t addr) {
	for (uint32_t i = 0; i < hd->block_map_sz; i++) {
		if (hd->block_map[i]->block_start == addr) {
			mmu_delete_mapping(mmu, addr);
#ifndef HEAP_UAF_CHECKER
			memmove(&hd->block_map[i], hd->block_map[i + 1], (hd->block_map_sz - i - 1) * sizeof(BLOCK_MAP*));
			hd->block_map = realloc(hd->block_map, (hd->block_map_sz - 1) * sizeof(BLOCK_MAP*));
			hd->block_map_sz--;
#endif
			return;
		}
	}
	
	ponii_log(LOG_LEVEL_ERROR, "Failed to remove block at address 0x%08X!\n", addr);
}

static void linux_cleanup_heap(HEAP_DESCRIPTOR* hd) {
	if (hd == NULL)
		return;
	
	if (hd->block_map) {
		for (uint32_t i = 0; i < hd->block_map_sz; i++) {
			free(hd->block_map[i]);
		}
		free(hd->block_map);
	}
}

uint32_t LINUX_Alloc(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	LINUX_STATE* linux_ctx = ms->linux_ctx;
	MMU_STATE* mmu = ms->mmu;
	uint32_t heapid = cpu->reg[0];
	uint32_t alloc_size = cpu->reg[1];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_Alloc(%d, 0x%X)\n", heapid, alloc_size);
	
	if (heapid >= linux_ctx->hd_sz) {
		ponii_log(LOG_LEVEL_ERROR, "Heapid out of range\n");
		return -1;
	}
	
	if (linux_ctx->hd_arr[heapid] == NULL) {
		ponii_log(LOG_LEVEL_ERROR, "Heapid not initilized yet\n");
		return -1;
	}
	
	if (alloc_size == 0)
		return 0x00000000;
	
	/*HEAP_DESCRIPTOR *hd = linux_ctx->hd_arr[heapid];
	
	uint32_t head = hd->guest_start_addr;
	while (mmu_read_dword(mmu, head) != 0x00000000)
		head = mmu_read_dword(mmu, head);
	
	if ((head + alloc_size + 0x4) >= (hd->guest_start_addr + hd->heap_size)) {
		ponii_log(LOG_LEVEL_EXTRA_INFO, "Out of space in heap %d\n", heapid);
		ponii_log(LOG_LEVEL_EXTRA_INFO, "Failed to alloc for size 0x%X\n", alloc_size);
		return -1;
	}
	
	mmu_write_dword(mmu, alloc_size, head);
	mmu_write_dword(mmu, 0x00000000, head + alloc_size + 0x4);
	
	return head + 0x4;*/
	
	//Implemented using isolated memory space for easier UaF and OOB checking.
	return add_new_block(mmu, &debug_hd, alloc_size, 0);
}

uint32_t LINUX_AllocAligned(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	LINUX_STATE* linux_ctx = ms->linux_ctx;
	MMU_STATE* mmu = ms->mmu;
	uint32_t heapid = cpu->reg[0];
	uint32_t alloc_size = cpu->reg[1];
	uint32_t align = cpu->reg[2];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_AllocAligned(%d, 0x%X, 0x%X)\n", heapid, alloc_size, align);
	
	if (heapid >= linux_ctx->hd_sz) {
		ponii_log(LOG_LEVEL_ERROR, "Heapid out of range\n");
		return -1;
	}
	
	if (linux_ctx->hd_arr[heapid] == NULL) {
		ponii_log(LOG_LEVEL_ERROR, "Heapid not initilized yet\n");
		return -1;
	}
	
	if (alloc_size == 0)
		return 0x00000000;
	
	//Implemented using isolated memory space for easier UaF and OOB checking.
	uint32_t addr = add_new_block(mmu, &debug_hd, alloc_size, align);
	ponii_log(LOG_LEVEL_INFO, "\tReturned: 0x%08X\n", addr);
	return addr;
}

uint32_t LINUX_Free(MACHINE_STATE *ms) {
	CPU_STATE* cpu = linux_get_thread_cpu(ms);
	MMU_STATE* mmu = ms->mmu;
	uint32_t heapid = cpu->reg[0];
	uint32_t ptr = cpu->reg[1];
	
	ponii_log(LOG_LEVEL_INFO, "LINUX_Free(%d, 0x%08X)\n", heapid, ptr);
	
	remove_block(mmu, &debug_hd, ptr);
	
	return 0;
}

uint32_t LINUX_exit(MACHINE_STATE *ms) {
        CPU_STATE* cpu = linux_get_thread_cpu(ms);

        ponii_log(LOG_LEVEL_INFO, "exit(%d)\n", cpu->reg[0]);
        invalid_opcode_done = 1;

        return 0;
}

int tmp_data = 0xb0000000;

uint32_t LINUX_uname(MACHINE_STATE *ms) {
        CPU_STATE* cpu = linux_get_thread_cpu(ms);

        ponii_log(LOG_LEVEL_INFO, "uname(0x%08x)\n", cpu->reg[0]);
        uint32_t buf = cpu->reg[0];

        /*struct utsname tmp;
        if (uname(&tmp) != 0)
                return (uint32_t)-1;*/

        //mmu_add_mapping();

        return 0;
}
