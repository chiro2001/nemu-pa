#include <stdio.h>

int _write(int file, char *ptr, int len) {
  int l = len;
  while (l--) {
    putch(*(ptr++));
  }
  return len;
}

#include <klib.h>
