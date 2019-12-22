#include "rtl/rtl.h"
#include "./include/isa/mmu.h"
#include "cpu/exec.h"

#define IRQ_TIMER 32

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags.val);
  t0 = cpu.cs;
  rtl_push(&t0);
  rtl_push(&ret_addr);
  cpu.eflags.IF = 0; //关中断
  
  /*union {
		GateDesc gd;
		struct { uint32_t lo, hi; };
	} item;
	vaddr_t addr;

	addr = 8 * NO + cpu.idtr.base;
	item.lo = vaddr_read(addr, 4);
  item.hi = vaddr_read(addr + 4, 4);

	t1 = (item.gd.offset_15_0 & 0xFFFF)
	| ((item.gd.offset_31_16 & 0xFFFF) << 16);
  rtl_j(t1);*/

  GateDesc gatedesc;
  gatedesc.offset_15_0= (vaddr_read(cpu.idtr.base + NO * 8, 4) & 0x0000ffff);//取低16位赋值给offset15_0
  gatedesc.offset_31_16 = ((vaddr_read(cpu.idtr.base + NO * 8 + 4, 4) & 0xffff0000) >> 16);//取高16位赋值给offset31_16
  //以上赋值的原因在于GateDesc联合体的结构

  
  vaddr_t goalAddr = gatedesc.offset_15_0 |(gatedesc.offset_31_16 & 0x0000ffff) << 16;
  rtl_j(goalAddr);
}

bool isa_query_intr(void) {
  if (cpu.INTR & cpu.eflags.IF){
    cpu.INTR = false;
    //raise_intr(IRQ_TIMER, cpu.pc);
    raise_intr(IRQ_TIMER, decinfo.seq_pc);
    return true;
  }
  return false;
}
