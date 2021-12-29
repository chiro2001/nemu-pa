#include <klib.h>
#include <stdio.h>

int fputc(int ch, FILE *f) {
  putch(ch);
  return 0;
}
