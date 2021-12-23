#include <am.h>
#include <nemu.h>
#include <string.h>
#include <klib.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T){.present = true,
                           .has_accel = false,
                           .width = 400,
                           .height = 300,
                           .vmemsz = 400 * 300 * sizeof(uint32_t)};
}

void *memcpy32(uint32_t *out, const uint32_t *in, size_t n) {
  uint32_t *p = (uint32_t *)out;
  while (n--) *(p++) = *((uint32_t *)in++);
  return out;
}


void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // printf("__am_gpu_fbdraw: Pos(%d, %d), size=(%d, %d)\n", ctl->x, ctl->y, ctl->w, ctl->h);
  // for (int i = 0; i < ctl->w; i++) {
  //   for (int j = 0; j <= ctl->h; j++) {
  //     outl(FB_ADDR + ((i + ctl->x) * ctl->h + (j + ctl->y)) *
  //     sizeof(uint32_t),
  //          ((uint32_t *)ctl->pixels)[i * ctl->h + j]);
  //   }
  // }
  for (int i = 0; i < ctl->h; i++) {
#ifdef CONFIG_EXT_VGA_COPY_BY_BYTE
    memcpy  ((uint32_t *)FB_ADDR + (i + ctl->y) * 400 + ctl->x, (uint32_t *)ctl->pixels + i * ctl->w, ctl->w * sizeof(uint32_t));
#else
    memcpy32((uint32_t *)FB_ADDR + (i + ctl->y) * 400 + ctl->x, (uint32_t *)ctl->pixels + i * ctl->w, ctl->w);
#endif
    // memcpy  ((uint32_t *)FB_ADDR + (i + ctl->y) * 400 + ctl->x, (uint32_t *)ctl->pixels + i * ctl->w, ctl->w * sizeof(uint32_t));
    // memcpy32((uint32_t *)FB_ADDR + (i + ctl->y) * ctl->w + ctl->x + ctl->w / 2, (uint32_t *)ctl->pixels + i * ctl->w + ctl->w / 2, ctl->w / 2);
    // memcpy32((uint32_t *)FB_ADDR + (i + ctl->y) * 400 + ctl->x, (uint32_t *)ctl->pixels + i * ctl->w, ctl->w);
  }
  // for (int i = 0; i < ctl->h; i++) {
  //   memcpy((uint32_t *)FB_ADDR + (i + ctl->x) * ctl->w + ctl->y, (uint32_t *)ctl->pixels + i * ctl->w, ctl->w * sizeof(uint32_t));
  // }
  // memcpy((void *)FB_ADDR, ctl->pixels, 400 * 200 * sizeof(uint32_t));
  // memcpy((uint32_t *)FB_ADDR + 400 * 50, ctl->pixels, 400 * 200 * sizeof(uint32_t));
  // for (int i = 0; i < ctl->h; i++)
  //   memcpy((uint32_t *)FB_ADDR + 400 * (50 + i), (uint32_t *)ctl->pixels + i * ctl->w, ctl->w * 1 * sizeof(uint32_t));
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) { status->ready = true; }
