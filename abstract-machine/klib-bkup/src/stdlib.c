#include <am.h>
#include <klib-macros.h>
#include <klib.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) { next = seed; }

int abs(int x) { return (x < 0 ? -x : x); }

int atoi(const char *nptr) {
  int x = 0;
  while (*nptr == ' ') {
    nptr++;
  }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr++;
  }
  return x;
}

// #include <generated/autoconf.h>
#include "/home/chiro/ics2021/nemu/include/generated/autoconf.h"

// #ifndef CONFIG_MBASE
// #define CONFIG_MBASE 0x80000000
// #endif
// #ifndef CONFIG_MSIZE
// #define CONFIG_MSIZE 0x8000000
// #endif

void *malloc(size_t size) {
  // TODO: Memory manager
  // static uint8_t *addr = (uint8_t *)(CONFIG_MBASE + (CONFIG_MSIZE / 2));
  // static uint8_t *addr = (uint8_t *)(CONFIG_MBASE + 0x80000);
  static uint8_t *addr = (uint8_t *)(CONFIG_MBASE);
  addr += size;
  return addr;
}

void free(void *ptr) {}

#endif
