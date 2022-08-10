#ifndef __LINUX_HLE_H__
#define __LINUX_HLE_H__

#include <stdint.h>
#include <setjmp.h>
#include "cpu.h"
#include "machine.h"

struct QUEUE_DESCRIPTOR {
	uint32_t ptr;
	uint32_t n_msgs;
	uint32_t n_filled;
};
typedef struct QUEUE_DESCRIPTOR QUEUE_DESCRIPTOR;

struct FILE_DESCRIPTOR {
	int host_fd;
};
typedef struct FILE_DESCRIPTOR FILE_DESCRIPTOR;

struct BLOCK_MAP {
	uint32_t block_start;
	uint32_t block_size;
};
typedef struct BLOCK_MAP BLOCK_MAP;

struct HEAP_DESCRIPTOR {
	uint32_t guest_start_addr;
	uint32_t heap_size;
	BLOCK_MAP** block_map;
	uint32_t block_map_sz;
};
typedef struct HEAP_DESCRIPTOR HEAP_DESCRIPTOR;

enum THREAD_STATE {
	STATE_PAUSED,
	STATE_BLOCKED,
	STATE_WAITING,
	STATE_RUNNING
};

struct THREAD_DESCRIPTOR {
	CPU_STATE* cpu;
	uint32_t priority;
	enum THREAD_STATE thread_state;
	uint32_t* joining_threads;
	int num_joining_threads;
};
typedef struct THREAD_DESCRIPTOR THREAD_DESCRIPTOR;

struct LINUX_STATE {
	char* fs_root;
	QUEUE_DESCRIPTOR** q_arr;
	uint32_t q_sz;
	FILE_DESCRIPTOR** fd_arr;
	uint32_t fd_sz;
	HEAP_DESCRIPTOR** hd_arr;
	uint32_t hd_sz;
	THREAD_DESCRIPTOR** td_arr;
	uint32_t td_sz;
	uint32_t current_thread;
};
typedef struct LINUX_STATE LINUX_STATE;

LINUX_STATE* linux_init(const char* fs_root, uint32_t n_qd, uint32_t n_fd, uint32_t n_hd, uint32_t n_td);
void linux_free(LINUX_STATE*);

CPU_STATE* linux_get_thread_cpu(MACHINE_STATE* ms);

void linux_syscall(MACHINE_STATE* ms, int i);

uint32_t LINUX_CreateThread(MACHINE_STATE* ms);
uint32_t LINUX_CancelThread(MACHINE_STATE* ms);
uint32_t LINUX_GetThreadId(MACHINE_STATE* ms);
uint32_t LINUX_StartThread(MACHINE_STATE* ms);
uint32_t LINUX_GetThreadPriority(MACHINE_STATE* ms);
uint32_t LINUX_SetThreadPriority(MACHINE_STATE* ms);
uint32_t LINUX_CreateMessageQueue(MACHINE_STATE* ms);
uint32_t LINUX_DestroyMessageQueue(MACHINE_STATE* ms);
uint32_t LINUX_SendMessage(MACHINE_STATE *ms);
uint32_t LINUX_ReceiveMessage(MACHINE_STATE *ms);
uint32_t LINUX_CreateTimer(MACHINE_STATE* ms);
uint32_t LINUX_CreateHeap(MACHINE_STATE* ms);
uint32_t LINUX_Alloc(MACHINE_STATE* ms);
uint32_t LINUX_AllocAligned(MACHINE_STATE* ms);
uint32_t LINUX_Free(MACHINE_STATE* ms);
uint32_t LINUX_exit(MACHINE_STATE *ms);

#else

struct QUEUE_DESCRIPTOR;
typedef struct QUEUE_DESCRIPTOR QUEUE_DESCRIPTOR;

struct FILE_DESCRIPTOR;
typedef struct FILE_DESCRIPTOR FILE_DESCRIPTOR;

struct BLOCK_MAP;
typedef struct BLOCK_MAP BLOCK_MAP;

struct HEAP_DESCRIPTOR;
typedef struct HEAP_DESCRIPTOR HEAP_DESCRIPTOR;

struct THREAD_DESCRIPTOR;
typedef struct THREAD_DESCRIPTOR THREAD_DESCRIPTOR;

struct LINUX_STATE;
typedef struct LINUX_STATE LINUX_STATE;

#endif
