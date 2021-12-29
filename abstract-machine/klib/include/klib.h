#ifndef KLIB_H__
#define KLIB_H__

#include <am.h>
#include <stddef.h>
#include <stdarg.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

// #ifndef putch
// #endif

#ifdef __cplusplus
extern "C" {
#endif

// assert.h
#ifdef NDEBUG
  #define assert(ignore) ((void)0)
#else
  #define assert(cond) \
    do { \
      if (!(cond)) { \
        printf("Assertion fail at %s:%d\n", __FILE__, __LINE__); \
        halt(1); \
      } \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif


#endif
