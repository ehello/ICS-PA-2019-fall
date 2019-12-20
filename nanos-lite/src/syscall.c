#include "common.h"
#include "syscall.h"
#include "proc.h"
//#include "/home/james/ics2019/nexus-am/am/am.h"

extern int fs_open(const char *, int, int);
extern __ssize_t fs_read(int, void *, size_t);
extern __ssize_t fs_write(int, const void *, size_t);
extern __off_t fs_lseek(int, __off_t, int);
extern int fs_close(int);
extern void naive_uload(PCB *, const char *);
extern int mm_brk(uintptr_t, intptr_t );

_Context* do_syscall(_Context* c) {
  uintptr_t a[4];
  //printf("GPR1 in do_syscall is %d\n",c->GPR1);//已经成功实现了，只不过当为0时，不会被打印出来；所以错误只是在context结构体上
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: c->GPRx = 0;_yield();  break;
    case SYS_exit: _halt(a[1]);/*naive_uload(NULL,"/bin/init"); */break;//PA4.2暂时改回_halt()来退出
    case SYS_write: /*Log("check the sys_write");c->GPRx = sys_write(a[1],(void*)a[2],a[3]);*/c->GPRx=fs_write(a[1],(void*)a[2],a[3]); break; 
    case SYS_brk: {
      //c->GPRx = 0;
      c->GPRx = mm_brk(a[1], a[2]);
      }break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1],a[2],a[3]);break;
    case SYS_open: c->GPRx = fs_open((const char*)a[1],a[2],a[3]);break;
    case SYS_close: c->GPRx = fs_close(a[1]);break;
    case SYS_read: c->GPRx = fs_read((int)a[1],(void*)a[2],a[3]);break;
    case SYS_execve: naive_uload(NULL,(char*)a[1]); c->GPRx = 0; break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}


