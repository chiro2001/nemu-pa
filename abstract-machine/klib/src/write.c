#include <klib.h>
#include <stdio.h>

#ifndef __ARCH_NATIVE

int _write(int file, char *ptr, int len) {
  int l = len;
  while (l--) {
    putch(*(ptr++));
  }
  return len;
}

#endif