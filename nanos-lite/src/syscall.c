#include "common.h"
#include "syscall.h"
//#include "/home/james/ics2019/nexus-am/am/am.h"


extern void _exit(int status);

_Context* do_syscall(_Context* c) {
  uintptr_t a[4];
  //printf("GPR1 in do_syscall is %d\n",c->GPR1);已经成功实现了，只不过当为0时，不会被打印出来；所以错误只是在context结构体上
  a[0] = c->GPR1;
  //a[1] = c->GPR2;
  //a[2] = c->GPR3;
  //a[3] = c->GPR4;
  //a[4] = c->GPRx;

  switch (a[0]) {
    case SYS_yield:_yield(); break;
    case SYS_exit:_halt(0); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
