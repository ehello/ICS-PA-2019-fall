#include "common.h"
#include "syscall.h"
#include "/home/james/ics2019/nexus-am/am/am.h"

extern void _yield();
extern void _exit(int status);

_Context* do_syscall(_Context* c) {
  uintptr_t a[4];
  printf("GPR1 is %d\n",c->GPR1);
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  a[4] = c->GPRx;

  switch (a[0]) {
    case SYS_yield:_yield(); break;
    //case SYS_exit:_exit(a[2]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
