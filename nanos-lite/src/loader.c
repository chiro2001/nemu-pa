// #include <elf.h>
#include <proc.h>

#include "common.h"
#include "fs.h"
#include "libelf/elf.h"

#ifdef __LP64__
#undef __LP64__
#endif

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

#define PINT(x) Log(#x "\t= %6d (0x%08x)", (int)(x), (size_t)(x))
#define check(x) assert((x) >= 0)

uintptr_t loader(PCB *pcb, const char *filename) {
  Fhdr fhdr;
  FILE *f;
  // uint8_t *buf;
  // uint64_t len;
  uintptr_t entry = 0;

  f = fopen_ramdisk(filename, "rb");
  if (f == NULL) return -1;

  check(readelf(f, &fhdr));
  printelfhdr(&fhdr);
  Log("Finding sections...");
  Fhdr *fp = &fhdr;
  FILE *f_elf = fopen_ramdisk(NULL, NULL);

  memset(fp, 0, sizeof(*fp));
  check(readident(f, fp));

  check(fp->readelfehdr(f, fp));
  check(readelfstrndx(f, fp));
  check(readelfshdrs(f, fp));
  // for (int i = 0; i < fp->strndxsize; i++) fp->readelfstrndx(f, fp);
  // for (int i = 0; i < fp->shnum; i++) fp->readelfshdr(f, fp);

  check(fseek_ramdisk(f, fp->phoff, SEEK_SET));

  for (int i = 0; i < fp->phnum; i++) {
    // PINT(i);
    Elf_Phdr *ph = getelf32phdr(f, fp);
    if (ph->filesz == 0) continue;
    // PINT(ph->offset);
    // PINT(ph->vaddr);
    // PINT(ph->filesz);
    // PINT(ph->memsz);
    // segment使用的内存就是[VirtAddr, VirtAddr + MemSiz)这一连续区间,
    // 然后将segment的内容从ELF文件中读入到这一内存区间, 并将[VirtAddr +
    // FileSiz, VirtAddr + MemSiz)对应的物理区间清零.
    uint8_t *vaddr = (uint8_t *)ph->vaddr;
    memset(vaddr, 0, ph->memsz);
    fseek_ramdisk(f_elf, ph->offset, SEEK_SET);
    Log("fread_ramdisk(vaddr, ph->filesz, 1, f_elf) = fread_ramdisk(0x%08x, "
        "%u, 1, f_elf)",
        vaddr, ph->filesz);
    fread_ramdisk(vaddr, ph->filesz, 1, f_elf);
    if (ph) free(ph);
  }

  entry = fp->entry;

  Log("exit normally...");

  freeelf(&fhdr);
  fclose_ramdisk(f);
  fclose_ramdisk(f_elf);
  return entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", (void *)entry);
  ((void (*)())entry)();
}
