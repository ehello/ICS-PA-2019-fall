#include "common.h"
#include "syscall.h"

extern void _yield();

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  printf("GPR1 is %d\n",c->GPR1);
  a[0] = c->GPR1;
  //a[1] = c->GPR2;
  //a[3] = c->GPR3;
  //a[4] = c->GPR4;

  switch (a[0]) {
    case SYS_yield:_yield();c->GPRx=0;break;
    
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
