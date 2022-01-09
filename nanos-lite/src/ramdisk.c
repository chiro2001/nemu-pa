#include <common.h>

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
#define RAMDISK_SIZE ((&ramdisk_end) - (&ramdisk_start))

/* The kernel is monolithic, therefore we do not need to
 * translate the address `buf' from the user process to
 * a physical one, which is necessary for a microkernel.
 */

/* read `len' bytes starting from `offset' of ramdisk into `buf' */
size_t ramdisk_read(void *buf, size_t offset, size_t len) {
  assert(offset + len <= RAMDISK_SIZE);
  memcpy(buf, &ramdisk_start + offset, len);
  return len;
}

/* write `len' bytes starting from `buf' into the `offset' of ramdisk */
size_t ramdisk_write(const void *buf, size_t offset, size_t len) {
  assert(offset + len <= RAMDISK_SIZE);
  memcpy(&ramdisk_start + offset, buf, len);
  return len;
}

size_t fread_ramdisk(void *buf, size_t size, size_t n, FILE *f) {
  Log("fread(0x%08x, %d, %d, 0x%08x), offset = 0x%08x", (size_t)buf, (int)size, (int)n, (size_t)f, (size_t)f->_offset);
  size_t res = ramdisk_read(buf, f->_offset, size * n) / size;
  f->_offset += size * n;
  return res;
}

size_t fseek_ramdisk(FILE *f, size_t offset, size_t direction) {
  f->_offset = offset;
  return 0;
}

FILE *fopen_ramdisk(const char *filename, const char *method) {
  FILE *f = (FILE *)malloc(sizeof(FILE));
  memset(f, 0, sizeof(*f));
  return f;
}

int fclose_ramdisk(FILE *f) {
  free(f);
  return 0;
}

void init_ramdisk() {
  Log("ramdisk info: start = %p, end = %p, size = %d bytes",
      &ramdisk_start, &ramdisk_end, RAMDISK_SIZE);
}

size_t get_ramdisk_size() {
  return RAMDISK_SIZE;
}
