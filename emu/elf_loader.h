#ifndef __ELF_LOADER_H__
#define __ELF_LOADER_H__

#include "machine.h"

int elf_file_load(MACHINE_STATE* ms, void* elf_file);

#endif
