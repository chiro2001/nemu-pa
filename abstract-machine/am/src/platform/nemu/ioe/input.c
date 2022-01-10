#include <am.h>
#include <nemu.h>
#include <klib.h>

#ifndef KEYDOWN_MASK
#define KEYDOWN_MASK 0x8000
#endif

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keydown = ((*((uint32_t* )KBD_ADDR)) & KEYDOWN_MASK) != 0;
  kbd->keycode = ((*((uint32_t* )KBD_ADDR)) & KEYDOWN_MASK) ? (*((uint32_t* )KBD_ADDR)) - KEYDOWN_MASK : (*((uint32_t* )KBD_ADDR));
  // kbd->keydown = 0;
  // kbd->keycode = AM_KEY_NONE;
  // if (kbd->keydown)
  //   printf("kbd down: %d\n", kbd->keycode);
  
}
