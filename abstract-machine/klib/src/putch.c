#include <klib.h>
#include <stdio.h>
#include "amdev.h"

int fputc(int ch, FILE *f) {
  putch(ch);
  return 0;
}

// const char input_key_data[128] =
//     "\0\0\0\0\0\0\0\0\0\0\0\0\0"
//     "`1234567890-=\b"
//     "\tqwertyuiop[]\\"
//     "\0asdfghjkl;'\n"
//     "\0zxcvbnm,./";
// static AM_INPUT_KEYBRD_T g_ev;

// int fgetc(FILE *stream) {
//   // putch('.');
//   while (1) {
//     // __am_input_keybrd(&g_ev);
//     void nanos_input_keybrd(AM_INPUT_KEYBRD_T *kbd);
//     nanos_input_keybrd(&g_ev);
//     if (g_ev.keycode == AM_KEY_NONE) break;
//     if (g_ev.keydown) {
//       return input_key_data[g_ev.keycode];
//     }
//   }
//   return EOF;
// }

// void nanos_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
//   kbd->keydown = ((*((uint32_t *)KBD_ADDR)) & KEYDOWN_MASK) != 0;
//   kbd->keycode = ((*((uint32_t *)KBD_ADDR)) & KEYDOWN_MASK)
//                      ? (*((uint32_t *)KBD_ADDR)) - KEYDOWN_MASK
//                      : (*((uint32_t *)KBD_ADDR));
// }

// int readch() {
//   while (1) {
//     void nanos_input_keybrd(AM_INPUT_KEYBRD_T *kbd);
//     nanos_input_keybrd(&g_ev);
//     if (g_ev.keycode == AM_KEY_NONE) break;
//     if (g_ev.keydown) {
//       return input_key_data[g_ev.keycode];
//     }
//   }
//   return EOF;
// }
