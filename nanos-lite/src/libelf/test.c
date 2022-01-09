#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"

#define PINT(x) printf(#x "\t= %6d (0x%08lx)\n", (int)(x), (size_t)(x))

int main() {
  Fhdr fhdr;
  FILE *f;
  uint8_t *buf;
  uint64_t len;

  f = fopen(
      "/home/chiro/ics2021/nemu/../navy-apps/tests/dummy/build/dummy-riscv32",
      "rb");
  if (f == NULL) return -1;

  readelf(f, &fhdr);
  // printelfhdr(&fhdr);

  printf("Finding sections...\n");

  Fhdr *fp = &fhdr;
  // char strs[32][32];

  // int count = 0;
  // for (size_t i = 0; i < fp->strndxsize;) {
  //   char *n = getstr(fp, i);
  //   if (n == NULL) continue;
  //   if (*n == '\0') {
  //     i++;
  //     continue;
  //   }
  //   printf("getstr[%lu] = %s\t(%d)\n", i, n, count);
  //   i += strlen(n) + 1;
  //   strcpy(strs[count], n);
  //   count++;
  // }

  // puts("reading data...");

  // for (int i = 0; i < count; i++) {
  //   uint64_t len = 0;
  //   uint8_t *buf = readelfsection(f, strs[i], &len, fp);
  //   printf("section %s done, read %ld bytes; offset = 0x%08lx\n", strs[i],
  //   len, fp->offset); PINT(fp->size);
  // }

  // readelf(f, &fhdr);

  // PINT(fp->name);

  puts("=========================================");
  memset(fp, 0, sizeof(*fp));
  readident(f, fp);

  fp->readelfehdr(f, fp);
  for (int i = 0; i < fp->strndxsize; i++) fp->readelfstrndx(f, fp);
  for (int i = 0; i < fp->shnum; i++) fp->readelfshdr(f, fp);

  for (int i = 0; i < fp->phnum; i++) {
    PINT(i);
    Elf32_Phdr *ph = getelf32phdr(f, fp);
		PINT(ph->offset);
    PINT(ph->vaddr);
    PINT(ph->filesz);
    PINT(ph->memsz);
    if (ph) free(ph);
  }

  puts("exit normally...");

  freeelf(&fhdr);
  return 0;
}
