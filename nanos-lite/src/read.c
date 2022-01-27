#include <klib.h>
#include <stdio.h>

#include "fs.h"

#if 1

#ifndef __ISA_NATIVE__

#if defined(__ARCH_X86_NEMU)
#define DEVICE_BASE 0x0
#else
#define DEVICE_BASE 0xa0000000
#endif

#define MMIO_BASE 0xa0000000

#define SERIAL_PORT (DEVICE_BASE + 0x00003f8)
#define KBD_ADDR (DEVICE_BASE + 0x0000060)
#define RTC_ADDR (DEVICE_BASE + 0x0000048)
#define VGACTL_ADDR (DEVICE_BASE + 0x0000100)
#define AUDIO_ADDR (DEVICE_BASE + 0x0000200)
#define DISK_ADDR (DEVICE_BASE + 0x0000300)
#define FB_ADDR (MMIO_BASE + 0x1000000)
#define AUDIO_SBUF_ADDR (MMIO_BASE + 0x1200000)

AM_INPUT_KEYBRD_T g_ev_;
const char input_key_data[128] =
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "`1234567890-=\b"
    "\tqwertyuiop[]\\"
    "\0asdfghjkl;'\n"
    "\0zxcvbnm,./\0"
    "\0\0\0 \0\0"
    "\0\0\0\0\0\0\0\0\0\0";

#ifndef KEYDOWN_MASK
#define KEYDOWN_MASK 0x8000
#endif

void nanos_input_keybrd_(AM_INPUT_KEYBRD_T *kbd)
    __attribute__((optimize("O0")));

void nanos_input_keybrd_(AM_INPUT_KEYBRD_T *kbd) {
  static uint32_t key_data_read;
  key_data_read = *((uint32_t *)KBD_ADDR);
  kbd->keydown = ((key_data_read)&KEYDOWN_MASK) != 0;
  kbd->keycode = ((key_data_read)&KEYDOWN_MASK) ? (key_data_read)-KEYDOWN_MASK
                                                : (key_data_read);
  // if (kbd->keydown)
  //   printf("input: keydown=%d, keycode=%d\n", (int)kbd->keydown,
  //          (int)kbd->keycode);
}

int readch_() {
  while (1) {
    // void nanos_input_keybrd(AM_INPUT_KEYBRD_T *kbd);
    nanos_input_keybrd_(&g_ev_);
    // if (g_ev_.keycode == AM_KEY_NONE) break;
    if (g_ev_.keydown) {
      char c = input_key_data[g_ev_.keycode];
      if (c) return c;
    }
  }
  return EOF;
}

int _read(int file, void *ptr, size_t len) {
  // Log("read file=%d", file);
  if (file < FD_UNUSED) {
    int l = len;
    char *p = (char *)ptr;
    while (l--) {
      int c = readch_();
      *(p++) = (char)c;
      break;
    }
    return len - l;
  } else {
    // Log("fno_read_myfs(ptr, 1, len, file=%d, NULL)", file);
    int l = fno_read_myfs(ptr, 1, len, file, NULL);
    return len - l;
  }
}

#else
// int _read(int file, void *ptr, size_t len) { return len; }
#endif

#endif