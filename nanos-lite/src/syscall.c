#include "common.h"
#include "syscall.h"
#include "proc.h"
//#include "/home/james/ics2019/nexus-am/am/am.h"

extern int fs_open(const char *pathname, int flags, int mode);
extern __ssize_t fs_read(int fd, void *buf, size_t len);
extern __ssize_t fs_write(int fd, const void *buf, size_t len);
extern __off_t fs_lseek(int fd, __off_t offset, int whence);
extern int fs_close(int fd);
extern void naive_uload(PCB *pcb, const char *filename);

_Context* do_syscall(_Context* c) {
  uintptr_t a[4];
  //printf("GPR1 in do_syscall is %d\n",c->GPR1);//已经成功实现了，只不过当为0时，不会被打印出来；所以错误只是在context结构体上
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield:_yield(); c->GPRx = 0; break;
    case SYS_exit:/*_halt(a[1]);*/naive_uload(NULL,"/bin/init"); break;//may not correct
    case SYS_write: /*Log("check the sys_write");c->GPRx = sys_write(a[1],(void*)a[2],a[3]);*/c->GPRx=fs_write(a[1],(void*)a[2],a[3]); break; 
    case SYS_brk: c->GPRx = 0;break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1],a[2],a[3]);break;
    case SYS_open: c->GPRx = fs_open((const char*)a[1],a[2],a[3]);break;
    case SYS_close: c->GPRx = fs_close(a[1]);break;
    case SYS_read: c->GPRx = fs_read((int)a[1],(void*)a[2],a[3]);break;
    case SYS_execve: naive_uload(NULL,(char*)a[1]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}


