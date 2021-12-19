#include <klib-macros.h>
#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  assert(s);
  size_t len = 0;
  while (*s) {
    s++;
    len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  assert(dst);
  assert(src);
  while (*src) *(dst++) = *(src++);
  *dst = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  assert(dst);
  assert(src);
  const char *p = src;
  while (*p && (p - src) < n) *(dst++) = *(p++);
  *dst = '\0';
  return dst;
}

char *strcat(char *dst, const char *src) {
  assert(dst);
  assert(src);
  char *tail = dst;
  while (*tail) tail++;
  while (*src) *(tail++) = *(src++);
  *tail = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  assert(s1);
  assert(s2);
  while (*s1 && *s2) {
    if (*s1 < *s2) return -1;
    if (*s1 > *s2) return 1;
    s1++;
    s2++;
  }
  if (*s1) return -1;
  if (*s2) return 1;
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  assert(s1);
  assert(s2);
  const char *p = s2;
  while (*s1 && *p && p - s2 > n) {
    if (*s1 < *p) return -1;
    if (*s1 > *p) return 1;
    s1++;
    p++;
  }
  if (*s1) return -1;
  if (*p) return 1;
  return 0;
}

void *memset(void *s, int c, size_t n) {
  uint8_t *p = (uint8_t *)s;
  while (n--) *(p++) = c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  // panic("Not implemented");
  // TODO: 解决内存覆盖问题
  return memcpy(dst, src, n);
}

void *memcpy(void *out, const void *in, size_t n) {
  uint8_t *p = (uint8_t *)out;
  while (n--) *(p++) = *((uint8_t *)in++);
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  assert(s1);
  assert(s2);
  const uint8_t *p = s2;
  const uint8_t *s = s1;
  while (*s && *p && p - s > n) {
    if (*s < *p) return -1;
    if (*s > *p) return 1;
    s1++;
    p++;
  }
  if (*s) return -1;
  if (*p) return 1;
  return 0;
}

#endif
