#include "common.h"
#include "syscall.h"
//#include "/home/james/ics2019/nexus-am/am/am.h"

extern void _putc(char ch);
int write(int fd, void *buf, size_t count){
  char *s = (char*)buf;
  if (fd == 1 || fd == 2){
    size_t i = 0;
    while(i<count){
      _putc(s[i]);
      i++;
    }
    return (int)i;
  }
  else return -1;
}

_Context* do_syscall(_Context* c) {
  uintptr_t a[4];
  //printf("GPR1 in do_syscall is %d\n",c->GPR1);//已经成功实现了，只不过当为0时，不会被打印出来；所以错误只是在context结构体上
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield:_yield(); c->GPRx = 0; break;
    case SYS_exit:_halt(a[1]); break;//may not correct
    case SYS_write: Log("check the sys_write");c->GPRx = write(a[1],(void*)a[2],a[3]);break; 
    //case SYS_brk: c->GPRx = 0;break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}


