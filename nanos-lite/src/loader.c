// #include <elf.h>
#include <proc.h>

#include "common.h"
#include "fs.h"
#include "libelf/elf.h"

#ifdef __ISA_NATIVE__
#include <unistd.h>
// #include "../../navy-apps/libs/libos/src/native.cpp"
FILE *(*glibc_fopen)(const char *path, const char *mode) = NULL;
int (*glibc_open)(const char *path, int flags, ...) = NULL;
ssize_t (*glibc_read)(int fd, void *buf, size_t count) = NULL;
ssize_t (*glibc_write)(int fd, const void *buf, size_t count) = NULL;
int (*glibc_execve)(const char *filename, char *const argv[],
                    char *const envp[]) = NULL;

char fsimg_path[512] = "";

static inline void get_fsimg_path(char *newpath, const char *path) {
  if (*path != '/')
    sprintf(newpath, "%s/%s", fsimg_path, path);
  else
    sprintf(newpath, "%s%s", fsimg_path, path);
  // fprintf(stderr, "get_fsimg_path(%s, %s)\n", newpath, path);
}

static const char *redirect_path(char *newpath, const char *path) {
  get_fsimg_path(newpath, path);
  if (0 == access(newpath, 0)) {
    // fprintf(stderr, "Redirecting file open: %s -> %s\n", path, newpath);
    return newpath;
  }
  return path;
}

FILE *fopen(const char *path, const char *mode);
int open(const char *path, int flags, ...);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int execve(const char *filename, char *const argv[], char *const envp[]);

FILE *fopen(const char *path, const char *mode) {
  char newpath[512];
  // printf("fopen(%s, %s)\n", path, mode);
  FILE *f = glibc_fopen(redirect_path(newpath, path), mode);
  // printf("fopen f=%p\n", f);
  return f;
}

int open(const char *path, int flags, ...) {
  char newpath[512];
  return glibc_open(redirect_path(newpath, path), flags);
}

ssize_t read(int fd, void *buf, size_t count) {
  return glibc_read(fd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count) {
  return glibc_write(fd, buf, count);
}
#endif

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
// #define check(x) assert((x) >= 0)
#define check(x)                 \
  do {                           \
    int xx = (int)(x);           \
    if (xx < 0) {                \
      printf(#x " == %d\n", xx); \
      return (uintptr_t)(-1);    \
    }                            \
  } while (0)

#if defined(__ISA_NATIVE__)
#include <dlfcn.h>
#include <unistd.h>

static char execute_file_path[FS_PATH_MAX];

void execute_file() {
  // char *argv[32];
  if (!*execute_file_path) return;
  // memset(argv, 0, sizeof(char *) * 32);
  // argv[0] = execute_file_path;
  // char *envp[] = {0, NULL};get_fsimg_path
  // printf("exec pre ok.\n");
  // execve(execute_file_path, argv, envp);
  system(execute_file_path);
  // printf("exec done.\n");
  execute_file_path[0] = '\0';
}

#endif

uintptr_t loader(PCB *pcb, const char *filename) {
  Fhdr fhdr;
  FILE *f;
  uintptr_t entry = 0;

  check((f = fopen(filename, "rb")) ? 0 : -1);
  // printf("filename=%s, file=%p\n", filename, f);
  Fhdr *fp = &fhdr;

  memset(fp, 0, sizeof(*fp));
  check(readident(f, fp));

  check(fp->readelfehdr(f, fp));
  check(readelfstrndx(f, fp));
  check(readelfshdrs(f, fp));
  check(fseek(f, fp->phoff, SEEK_SET));

  entry = fp->entry;
  // Log("Found entry at: 0x%08x", entry);

#ifndef __ISA_NATIVE__

  for (int i = 0; i < fp->phnum; i++) {
    Elf_Phdr *ph = getelf32phdr(f, fp);
    assert(ph);
    if (ph->filesz == 0) continue;
    // segment使用的内存就是[VirtAddr, VirtAddr + MemSiz)这一连续区间,
    // 然后将segment的内容从ELF文件中读入到这一内存区间, 并将[VirtAddr +
    // FileSiz, VirtAddr + MemSiz)对应的物理区间清零.
    uint8_t *vaddr = (uint8_t *)ph->vaddr;
    if (!vaddr)
      vaddr += 0x83000000;
    else
      vaddr += 0x03000000;
    Log("section: [0x%08x, 0x%08x)", (unsigned int)(vaddr),
        (unsigned int)(vaddr + ph->memsz));
    memset(vaddr, 0, ph->memsz);
    fseek(f, ph->offset, SEEK_SET);
    fread(vaddr, 1, ph->filesz, f);
    if (ph) free(ph);
  }
#endif
  freeelf(&fhdr);
  fclose(f);
#ifndef __ISA_NATIVE__
  return entry;
#endif

#ifdef __ISA_NATIVE__
  char newpath[FS_PATH_MAX + 64];
  redirect_path(newpath, filename);
  // uintptr_t entry = 0;
  printf("filename=%s\n", newpath);
  // void *dlHandler = dlopen(newpath, RTLD_NOW);
  // printf("dlerror: %s\n", dlerror());
  // check(dlHandler == NULL ? -1 : 0);
  // entry = (uintptr_t)dlsym(dlHandler, "main");
  // return entry;
  strcpy(execute_file_path, newpath);
  entry = (uintptr_t)execute_file;
  return entry;
#endif
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  if (entry == (uint32_t)(-1)) {
    Log("Failed to load entry at 0x%08x", entry);
    return;
  }
  // Log("Jump to entry = %p", (void *)entry);
  ((void (*)())entry)();
}
