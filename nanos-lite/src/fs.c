#include <fs.h>

#include "arch.h"
#include "myFs.h"
#include "syscall.h"
#include "amdev.h"

extern uint8_t ramdisk_start;

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

void nanos_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keydown = ((*((uint32_t *)KBD_ADDR)) & KEYDOWN_MASK) != 0;
  kbd->keycode = ((*((uint32_t *)KBD_ADDR)) & KEYDOWN_MASK)
                     ? (*((uint32_t *)KBD_ADDR)) - KEYDOWN_MASK
                     : (*((uint32_t *)KBD_ADDR));
}

enum { FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB };

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t kbd_read(void *buf, size_t offset, size_t len) {
  uint8_t *p = (uint8_t *)buf + offset;
  while (len--) {
    int c = fgetc(stdin);
    if (c == EOF) return EOF;
    *p = (uint8_t) c;
    p++;
  }
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, kbd_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

static size_t file_count = 0;

Fs fs = NULL;

void init_fs() {
  // TODO: initialize the size of /dev/fb
  if (!fs) fs = FsNew();
  file_count = sizeof(file_table) / sizeof(Finfo);
  Log("Loading %d files...", (int)file_count);
  // mkdirs
  char dirs[][32] = {"/share",       "/share/pictures", "/share/fonts",
                     "/share/files", "/share/music",    "/share/music/rhythm",
                     "/share/games", "/share/games/bird", 
                     "/bin"};
  for (size_t i = 0; i < sizeof(dirs) / 32; i++) {
    // Log("MkDir: %s", dirs[i]);
    FsMkdir(fs, dirs[i]);
  }
  // Load file table -> memory
  for (size_t i = 0; i < file_count; i++) {
    Finfo *fd = &file_table[i];
    // 只加载普通文件
    if (fd->read != NULL) continue;
    char *p = fd->name;
    bool ready_to_load = true;
    // while (*p && *p != '.') p++;
    // if (*p) ready_to_load = false;
    if (!ready_to_load) continue;
    // Log("Loading %s...", fd->name);
    FIL *target = FsMkfile(fs, fd->name);
    if (!target) {
      Log("Failed to create file!");
      continue;
    }
    FsWrite(target, fd->size, (&ramdisk_start) + fd->disk_offset, FS_WRITE_NEW);
  }
}
