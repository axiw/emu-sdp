#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "breakpoint.h"
#include "log.h"
#include "machine.h"

struct BREAKPOINT_ELEMENT {
	uint32_t addr;
	int (*cb)(MACHINE_STATE* ms, uint32_t addr);
};
typedef struct BREAKPOINT_ELEMENT BREAKPOINT_ELEMENT;

struct BREAKPOINT_TABLE {
	int num_bps;
	BREAKPOINT_ELEMENT* addr_list;
} bp_t;

void add_breakpoint(uint32_t addr, int (*cb)(MACHINE_STATE* ms, uint32_t addr)) {
	bp_t.addr_list = realloc(bp_t.addr_list, ++bp_t.num_bps * sizeof(BREAKPOINT_ELEMENT));
	bp_t.addr_list[bp_t.num_bps - 1].addr = addr;
	bp_t.addr_list[bp_t.num_bps - 1].cb = cb;
}

void remove_breakpoint(uint32_t addr) {
	for (int i = 0; i < bp_t.num_bps; i++) {
		if (bp_t.addr_list[i].addr == addr) {
			memmove(bp_t.addr_list + i, bp_t.addr_list + i + 1, bp_t.num_bps - i - 1);
			bp_t.addr_list = realloc(bp_t.addr_list, --bp_t.num_bps * sizeof(BREAKPOINT_ELEMENT));
			return;
		}
	}
	ponii_log(LOG_LEVEL_ERROR, "Couldn't find address 0x%08X in breakpoint list\n", addr);
}

int is_breakpoint(MACHINE_STATE* ms, uint32_t addr) {
	for (int i = 0; i < bp_t.num_bps; i++) {
		if (bp_t.addr_list[i].addr == addr) {
			if (bp_t.addr_list[i].cb != NULL) {
				return bp_t.addr_list[i].cb(ms, addr);
			} else {
				return 1;
			}
		}
	}
	return 0;
}
