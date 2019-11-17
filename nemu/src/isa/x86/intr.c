#include "rtl/rtl.h"
#include "./include/isa/mmu.h"
#include "cpu/exec.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags.val);
  cpu.eflags.IF = 0;
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  union {
    GateDesc gatedesc;
    struct{uint32_t lo,hi;};
  } temp;

  temp.lo = vaddr_read(cpu.idtr.base+NO*8,4);
  temp.hi = vaddr_read(cpu.idtr.base+NO*8+4,4);
  //GateDesc gatedesc;
  //gatedesc.offset_15_0= (vaddr_read(cpu.idtr.base+NO*8,4)& 0x0000ffff);//取低16位赋值给offset15_0
  //gatedesc.offset_31_16 = ((vaddr_read(cpu.idtr.base+NO*8+4,4) & 0xffff0000)>>16);//取高16位赋值给offset31_16
  //以上赋值的原因在于GateDesc联合体的结构

  
  vaddr_t goalAddr = (temp.gatedesc.offset_15_0 & 0xffff)|((temp.gatedesc.offset_31_16 & 0xffff)<<16);
  rtl_j(goalAddr);
}

bool isa_query_intr(void) {
  return false;
}
