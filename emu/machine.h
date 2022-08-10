#ifndef __MACHINE_H__
#define __MACHINE_H__

#include <stdint.h>
#include <stddef.h>
#include "mmu.h"
#include "linux_hle.h"

struct MACHINE_STATE {
	LINUX_STATE* linux_ctx;
	MMU_STATE* mmu;
};
typedef struct MACHINE_STATE MACHINE_STATE;

MACHINE_STATE* machine_init(const char* fs_root, uint32_t n_qd, uint32_t n_fd, uint32_t n_hd, uint32_t n_td);
void machine_free(MACHINE_STATE* ms);
void machine_dump(MACHINE_STATE *ms);

#else

struct MACHINE_STATE;
typedef struct MACHINE_STATE MACHINE_STATE;

#endif
