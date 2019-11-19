#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context { 
  //根据trap.S,首先是存储通用寄存器，然后是地址空间，然后是事件号，然后是eflags、cs和eip
  //所以要把通用寄存器和eflags等三个寄存器分开放置
  uintptr_t  edi,esi,ebp,esp,ebx,edx,ecx,eax;
  struct _AddressSpace *as;
  int irq;
  uintptr_t eip,cs,eflags;
  //uintptr_t eflags,cs,eip;
  //uintptr_t GPR1,GPR2,GPR3,GPR4,GPRx;
};

#define GPR1 eax 
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax

#endif
