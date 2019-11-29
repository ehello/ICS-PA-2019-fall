#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *s = (char*)buf;
    size_t i = 0;
    while(i<len){
      _putc(s[i]);
      i++;
    }
    return i;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

extern int read_key();
extern uint32_t uptime();

size_t events_read(void *buf, size_t offset, size_t len) {
  int key = read_key();
  int down = 0;
  if (key & 0x8000) {
    key ^= 0x8000;
    down = 1;
  }
  if (key != _KEY_NONE){
    sprintf(buf,"%s %s\n", down?"kd":"ku", keyname[key]);
  }
  else{
    uint32_t t = uptime();
    sprintf(buf,"t %d\n",t);
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};
extern char* strncpy(char* dst, const char* src, size_t n);

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  strncpy(buf,dispinfo+offset,len);
  return len;
}

extern int screen_width();
extern int screen_height();
extern void draw_rect(uint32_t *pixels, int x, int y, int w, int h);

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int w = screen_width();
  int col = (offset/4) % w;
  int row = (offset/4) / w;
  draw_rect((uint32_t*)buf,col,row,len/4,1);
  return len;
}

extern void draw_sync();

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  int width = screen_width();
	int height = screen_height();
	int x = sprintf(dispinfo, "WIDTH:%d\n", width);
  sprintf(dispinfo + x, "HEIGHT:%d\n", height);
}
