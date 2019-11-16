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

  union {
		GateDesc gd;
		struct { uint32_t lo, hi; };
  } item;

  vaddr_t addr;
	addr = 8 * NO + cpu.idtr.base;
	item.lo = vaddr_read(addr, 4);
  item.hi = vaddr_read(addr + 4, 4);
  //GateDesc gatedesc;
  //gatedesc.offset_15_0 = vaddr_read(cpu.idtr.base+NO*8,4)& 0x0000ffff;
  //gatedesc.offset_31_16 = (vaddr_read(cpu.idtr.base+NO*8+4,4) & 0x0000ffff)<<16;
  
  cpu.eflags.IF = 0;
  //gatedesc.val = (gatedesc.offset_15_0 & 0x0000ffff)+(gatedesc.offset_31_16 & 0xffff0000);
  //vaddr_t goalAddr = gatedesc.offset_15_0|gatedesc.offset_31_16;
  vaddr_t goalAddr = (item.gd.offset_15_0 & 0xFFFF) | ((item.gd.offset_31_16 & 0xFFFF) << 16);
  rtl_j(goalAddr);
}

bool isa_query_intr(void) {
  return false;
}
