#include <am.h>
#include <nemu.h>
#include <string.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T){.present = true,
                           .has_accel = false,
                           .width = 400,
                           .height = 300,
                           .vmemsz = 400 * 300 * sizeof(uint32_t)};
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // for (int i = 0; i < ctl->w; i++) {
  //   for (int j = 0; j <= ctl->h; j++) {
  //     outl(FB_ADDR + ((i + ctl->x) * ctl->h + (j + ctl->y)) *
  //     sizeof(uint32_t),
  //          ((uint32_t *)ctl->pixels)[i * ctl->h + j]);
  //   }
  // }
  for (int i = 0; i < ctl->h; i++) {
    memcpy((uint32_t *)FB_ADDR + (i + ctl->y) * ctl->w + ctl->x, ctl->pixels + i * ctl->w, ctl->w * sizeof(uint32_t));
  }
  // memcpy((void *)FB_ADDR, ctl->pixels, 400 * 300 * sizeof(uint32_t));
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
