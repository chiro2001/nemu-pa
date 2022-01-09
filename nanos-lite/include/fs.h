#ifndef __FS_H__
#define __FS_H__

#include <common.h>

#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif

size_t fread_ramdisk(void *buf, size_t size, size_t n, FILE *f);
size_t fseek_ramdisk(FILE *f, size_t offset, size_t direction);
FILE *fopen_ramdisk(const char *filename, const char *method);
int fclose_ramdisk(FILE *f);

#endif
