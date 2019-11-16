#include "rtl/rtl.h"
#include "./include/isa/mmu.h"
#include "cpu/exec.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags.val);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);

  GateDesc gatedesc;
  gatedesc.offset_15_0 = vaddr_read(cpu.idtr.base+NO*8,4);
  gatedesc.offset_31_16 = vaddr_read(cpu.idtr.base+NO*8+4,4);
  gatedesc.present =  (uint16_t)vaddr_read(cpu.idtr.base+NO*8,4) & 0b1000000000000000;
  if (gatedesc.present == 0) panic("invalid");
  
  gatedesc.val = (gatedesc.offset_15_0 & 0x0000ffff)+(gatedesc.offset_31_16 & 0xffff0000);
  rtl_j(gatedesc.val);
}

bool isa_query_intr(void) {
  return false;
}
