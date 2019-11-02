#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <x86.h>

#define KEYDOWN_MASK 0x8000

size_t __am_input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _DEV_INPUT_KBD_t *kbd = (_DEV_INPUT_KBD_t *)buf;
      //kbd->keydown = 0;
      //kbd->keycode = _KEY_NONE;
      int keydata = inl(KBD_ADDR);
      if(keydata != _KEY_NONE){
        kbd->keycode = keydata;
        //kbd->keydown = keydata>>8;//瞎jb试出来的
        kbd->keydown = keydata>>8;
        //kbd->keydown = 1;
      }
      else kbd->keydown = 0;
      
      //if(_KEY_NONE)
      return sizeof(_DEV_INPUT_KBD_t);
    }
  }
  return 0;
}
