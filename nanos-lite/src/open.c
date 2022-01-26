#include <unistd.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include "syscall.h"
#include "amdev.h"
#include "arch.h"
#include "myFs.h"
#include "fs.h"
#include "syscall.h"


int _open(const char *path, int flags, mode_t mode) {
  // _exit(SYS_open);
  Log("_open(path=%s, flags=%d, mode)", path, flags);
  // extern Finfo file_table[];
  // if (!path) return _open("stdout", flags, mode);
  // for (size_t i = 0; i < FD_UNUSED; i++) {
  //   if (strcmp(path, file_table[i].name) == 0) {
  //     return i;
  //   }
  // }
  FILE *f = fopen_myfs(path, "r");
  if (!f) return -1;
  int file = get_file_no(f);
  fclose_myfs(f);
  Log("got %s file=%d", path, file);
  return file;
  // return 0;
}