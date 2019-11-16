#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context { 
  //uintptr_t  eip,cs,eflags,edi,esi,ebp,esp,ebx,edx,ecx,eax;
  uintptr_t  edi,esi,ebp,esp,ebx,edx,ecx,eax,eip,cs,eflags;
  struct _AddressSpace *as;
  int irq;
};

#define GPR1 eax
#define GPR2 eip
#define GPR3 eip
#define GPR4 eip
#define GPRx eip

#endif
