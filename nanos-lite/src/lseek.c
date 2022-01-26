#include <klib.h>
#include <stdio.h>

#include "fs.h"

int _lseek(int file, size_t offset, int whence) {
  // printf("_lseek(fildes=%d, offset=%d, whence=%d)\n", file, (int)offset,
  //        whence);
  FIL *fil = FsFilFindByFile(file);
  if (!fil) return -1;
  switch (whence) {
    case SEEK_CUR:
      fil->offset += offset;
      break;
    case SEEK_END:
      fil->offset = fil->size_file - offset;
      break;
    case SEEK_SET:
    default:
      fil->offset = offset;
      break;
  }
  return 0;
}