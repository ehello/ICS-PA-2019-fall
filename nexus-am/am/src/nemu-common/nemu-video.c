#include <am.h>
#include <amdev.h>
#include "nemu.h"
#include "x86.h"
#include "klib.h"

#define SCREEN_H 300
#define SCREEN_W 400

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      info->width = SCREEN_W;
      info->height = SCREEN_H;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
    
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;

      uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
      uint32_t *base = fb + ctl->y *SCREEN_H+ ctl->x;
      for(int dy = 0; dy < ctl->h; dy++) {
        for(int dx = 0; dx < ctl->w; dx++) {
          base[dy * SCREEN_W+ dx] = ctl->pixels[dy * ctl->w + dx];
        }
      }

      if (ctl->sync) {
        outl(SYNC_ADDR,0);
      }
      return size;
    }
    
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width() * screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}
