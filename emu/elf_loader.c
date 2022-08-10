#include <elf.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "common.h"
#include "linux_hle.h"
#include "log.h"
#include "machine.h"
#include "mmu.h"

int elf_file_load(MACHINE_STATE* ms, void* elf_file) {
	Elf32_Ehdr* ehdr = (Elf32_Ehdr*)elf_file;
	
	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1 || ehdr->e_ident[EI_MAG2] != ELFMAG2 || ehdr->e_ident[EI_MAG3] != ELFMAG3) {
		ponii_log(LOG_LEVEL_ERROR, "ELF LOADER: Bad magic\n");
		return 1;
	}
	
	if (ehdr->e_ident[EI_CLASS] != ELFCLASS32) {
		ponii_log(LOG_LEVEL_ERROR, "ELF LOADER: Not a 32 bit binary\n");
		return 1;
	}
	
	/*if (ehdr->e_ident[EI_DATA] != ELFDATA2MSB) {
		ponii_log(LOG_LEVEL_ERROR, "ELF LOADER: Not a big endian binary\n");
		return 1;
	}*/
	
	/*if (ehdr->e_ident[EI_OSABI] != ELFOSABI_ARM) {
		ponii_log(LOG_LEVEL_ERROR, "ELF LOADER: Not a ARM binary\n");
		return NULL;
	}*/
	
	Elf32_Phdr* phdr = elf_file + HOST_TO_TARGET_DWORD(ehdr->e_phoff);
	
	for (uint16_t i = 0; i < HOST_TO_TARGET_WORD(ehdr->e_phnum); i++) {
		if (HOST_TO_TARGET_DWORD(phdr[i].p_type) == PT_LOAD) {
			ponii_log(LOG_LEVEL_EXTRA_INFO, "Loading mapping 0x%08X\n", HOST_TO_TARGET_DWORD(phdr[i].p_vaddr));
			if (HOST_TO_TARGET_DWORD(phdr[i].p_vaddr) == 0x00000000)
				continue; // Don't load the PHDR since it isn't mapped by Linux and I don't want the page at 0 mapped to anything so I can spot things better.
			mmu_add_mapping(ms->mmu, HOST_TO_TARGET_DWORD(phdr[i].p_vaddr), HOST_TO_TARGET_DWORD(phdr[i].p_memsz), NULL, NULL, NULL);
			mmu_copy_in(ms->mmu, elf_file + HOST_TO_TARGET_DWORD(phdr[i].p_offset), HOST_TO_TARGET_DWORD(phdr[i].p_vaddr), HOST_TO_TARGET_DWORD(phdr[i].p_filesz));
		}
	}
	
	linux_get_thread_cpu(ms)->reg[REG_PC] = HOST_TO_TARGET_DWORD(ehdr->e_entry);
	uint32_t stack_addr = 0xA0000000;
	uint32_t stack_size = 0x10000; // If this isn't enough then something is wrong with the module (64Kb)
	mmu_add_mapping(ms->mmu, stack_addr, stack_size * 2, NULL, NULL, NULL);
	linux_get_thread_cpu(ms)->reg[REG_SP] = stack_addr + stack_size;
	
	return 0;
}
