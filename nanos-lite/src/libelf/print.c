#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "elf.h"
#include "dat.h"
#include "fns.h"

void
printident(Fhdr *fp)
{
	printf("class %s (0x%.2x)\n", elfclass(fp->class), fp->class);
	printf("data %s (0x%.2x)\n", elfdata(fp->data), fp->data);
	printf("version %s (%lu)\n", elfversion(fp->version), fp->version);
	printf("os abi %s (0x%.2x)\n", elfosabi(fp->osabi), fp->osabi);
	printf("abi version %lu\n", fp->abiversion);
}

void
printelf32ehdr(Elf32_Ehdr *e, Fhdr *fp)
{
	printf("ident %.2x %c %c %c %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
		e->ident[0], e->ident[1], e->ident[2], e->ident[3], e->ident[4], e->ident[5], e->ident[6], e->ident[7],
		e->ident[8], e->ident[9], e->ident[10], e->ident[11], e->ident[12], e->ident[13], e->ident[14], e->ident[15]);
	printident(fp);
	printf("type %s (0x%.4x)\n", elftype(e->type), e->type);
	printf("machine %s (0x%.4x)\n", elfmachine(e->machine), e->machine);
	printf("version %s (%lu)\n", elfversion(e->version), e->version);
	printf("entry 0x%.8lx\n", e->entry);
	printf("phoff %lu\n", e->phoff);
	printf("shoff %lu\n", e->shoff);
	printf("flags 0x%.8lx\n", e->flags);
	printf("ehsize %lu\n", e->ehsize);
	printf("phentsize %lu\n", e->phentsize);
	printf("phnum %lu\n", e->phnum);
	printf("shentsize %lu\n", e->shentsize);
	printf("shnum %lu\n", e->shnum);
	printf("shstrndx %lu\n", e->shstrndx);
	printf("\n");
}

void
printelf64ehdr(Elf64_Ehdr *e, Fhdr *fp)
{
	printf("ident %.2x %c %c %c %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
		e->ident[0], e->ident[1], e->ident[2], e->ident[3], e->ident[4], e->ident[5], e->ident[6], e->ident[7],
		e->ident[8], e->ident[9], e->ident[10], e->ident[11], e->ident[12], e->ident[13], e->ident[14], e->ident[15]);
	printident(fp);
	printf("type %s (0x%.4x)\n", elftype(e->type), e->type);
	printf("machine %s (0x%.4x)\n", elfmachine(e->machine), e->machine);
	printf("version %s (%lu)\n", elfversion(e->version), e->version);
	printf("entry 0x%.16" PRIx64 "\n", e->entry);
	printf("phoff %" PRIu64 "\n", e->phoff);
	printf("shoff %" PRIu64 "\n", e->shoff);
	printf("flags 0x%.8lx\n", e->flags);
	printf("ehsize %lu\n", e->ehsize);
	printf("phentsize %lu\n", e->phentsize);
	printf("phnum %lu\n", e->phnum);
	printf("shentsize %lu\n", e->shentsize);
	printf("shnum %lu\n", e->shnum);
	printf("shstrndx %lu\n", e->shstrndx);
	printf("\n");
}

void
printelf32shdr(Elf32_Shdr *sh, Fhdr *fp)
{
	printf("section header\n");
	printf("name %s (%lu)\n", getstr(fp, sh->name), sh->name);
	printf("type %lu\n", sh->type);
	printf("flags 0x%.8lx\n", sh->flags);
	printf("addr 0x%.8lx\n", sh->addr);
	printf("offset 0x%.8lx\n", sh->offset);
	printf("size %lu\n", sh->size);
	printf("link %lu\n", sh->link);
	printf("info %lu\n", sh->info);
	printf("addralign 0x%.8lx\n", sh->addralign);
	printf("entsize %lu\n", sh->entsize);
	printf("\n");
}

void
printelf32phdr(Elf32_Phdr *ph, Fhdr *fp)
{
	USED(fp);
	printf("program header\n");
	printf("type %lu\n", ph->type);
	printf("offset 0x%.8lx\n", ph->offset);
	printf("vaddr 0x%.8lx\n", ph->vaddr);
	printf("paddr 0x%.8lx\n", ph->paddr);
	printf("filesz %lu\n", ph->filesz);
	printf("memsz %lu\n", ph->memsz);
	printf("flags 0x%.8lx\n", ph->flags);
	printf("align 0x%.8lx\n", ph->align);
	printf("\n");
}

void
printelf64shdr(Elf64_Shdr *sh, Fhdr *fp)
{
	printf("section header\n");
	printf("name %s (%lu)\n", getstr(fp, sh->name), sh->name);
	printf("type %lu\n", sh->type);
	printf("flags 0x%.16" PRIx64 "\n", sh->flags);
	printf("addr 0x%.16" PRIx64 "\n", sh->addr);
	printf("offset 0x%.16" PRIx64 "\n", sh->offset);
	printf("size %" PRIu64 "\n", sh->size);
	printf("link %lu\n", sh->link);
	printf("info %lu\n", sh->info);
	printf("addralign 0x%.16" PRIx64 "\n", sh->addralign);
	printf("entsize %" PRIu64 "\n", sh->entsize);
	printf("\n");
}

void
printelf64phdr(Elf64_Phdr *ph, Fhdr *fp)
{
	USED(fp);
	printf("program header\n");
	printf("type %lu\n", ph->type);
	printf("flags 0x%.8lx\n", ph->flags);
	printf("offset 0x%.16" PRIx64 "\n", ph->offset);
	printf("vaddr 0x%.16" PRIx64 "\n", ph->vaddr);
	printf("paddr 0x%.16" PRIx64 "\n", ph->paddr);
	printf("filesz %" PRIu64 "\n", ph->filesz);
	printf("memsz %" PRIu64 "\n", ph->memsz);
	printf("align 0x%.16" PRIx64 "\n", ph->align);
	printf("\n");
}

void
printelfhdr(Fhdr *fp)
{
	printident(fp);
	printf("type %s (0x%.4x)\n", elftype(fp->type), fp->type);
	printf("machine %s (0x%.4x)\n", elfmachine(fp->machine), fp->machine);
	printf("version %s (%lu)\n", elfversion(fp->version), fp->version);
	if (fp->class == ELFCLASS32)
		printf("entry 0x%.4lux\n", (uint32_t)fp->entry);
	if (fp->class == ELFCLASS64)
		printf("entry 0x%.8" PRIx64 "\n", fp->entry);
}
