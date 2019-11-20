#ifndef __ARCH_H__
#define __ARCH_H__
struct _Context {
  //根据trap.S,首先是存储通用寄存器，然后是地址空间，然后是事件号，然后是eflags、cs和eip
  //所以要把通用寄存器和eflags等三个寄存器分开放置
  struct _AddressSpace *as;//之前放到第二个的位置。而Trap.s中的顺序是反的，所以应该从下往上看
  uintptr_t  edi,esi,ebp,esp,ebx,edx,ecx,eax;
  int irq;
  uintptr_t eip,cs,eflags;
} ;

#define GPR1 eax 
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax

#endif
