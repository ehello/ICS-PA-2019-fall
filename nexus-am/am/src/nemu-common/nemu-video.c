#include <am.h>
#include <amdev.h>
#include "nemu.h"
#include "x86.h"
#include "klib.h"

//#define SCREEN_H 300
//#define SCREEN_W 400

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      uint32_t vga_data = inl(SCREEN_ADDR);
      info->width = vga_data >> 16;//todo?
      info->height = vga_data &0xffff;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
    
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;

      if (ctl->sync) {
        //outl(SYNC_ADDR,ctl->y);//todo
      }
      return size;
    }
    
  }
  return 0;
}

void __am_vga_init() {
  /*int i;
  int size = screen_width() * screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();*/
}
