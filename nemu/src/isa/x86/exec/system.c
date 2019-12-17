#include "cpu/exec.h"

make_EHelper(lidt) {
  //TODO();
  rtl_li(&s0,id_dest->addr);
  //rtl_li(&cpu.idtr.limit,vaddr_read(s0,2));
  if (decinfo.isa.is_operand_size_16){
    //rtl_li(&s1,vaddr_read(s0+2,4));
    //rtl_li(&cpu.idtr.base,s1&0x00ffffff);
    panic("lidt is not implemented when oprand is 16 bit ");
    assert(0);
  }
  else {
    rtl_li(&cpu.idtr.limit,vaddr_read(s0,2));
    rtl_li(&cpu.idtr.base,vaddr_read(s0+2,4));
  }
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  //TODO();
  if(id_dest->reg == 0)
    cpu.cr0.val = id_src->val;
  else if (id_dest->reg == 3)
    cpu.cr3.val = id_src->val;
  else
    assert("No such control register!\n");
  
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  //TODO();
  if(id_src->reg == 0)
    rtl_li(&s0,cpu.cr0.val);
  else if(id_src->reg == 3)
    rtl_li(&s0,cpu.cr3.val);
  else
    assert("No such control register!\n");
  operand_write(id_dest,&s0);
  
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  //TODO();
  switch (decinfo.opcode){
    case 0xcc: raise_intr((uint32_t)0x3,decinfo.seq_pc);break;
    case 0xcd: raise_intr(id_dest->val,decinfo.seq_pc);break;
    case 0xce: raise_intr((uint32_t)0x4,decinfo.seq_pc);break;
    default: raise_intr(id_dest->val,decinfo.seq_pc);break;
  }
  //raise_intr(id_dest->val, decinfo.seq_pc);
  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&decinfo.jmp_pc);
  rtl_j(decinfo.jmp_pc);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags.val);
  

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  //TODO();
  switch (id_dest->width){
    case 1: s0 = pio_read_b(id_src->val); break;
    case 2: s0 = pio_read_w(id_src->val); break;
    case 4: s0 = pio_read_l(id_src->val); break;
    default: break;
  }
  operand_write(id_dest,&s0);
  print_asm_template2(in);
}

make_EHelper(out) {
  //TODO();
  //printf("%x %x\n",id_dest->val,id_src->val);
  switch(id_dest->width){
    case 1: pio_write_b(id_dest->val, id_src->val); break;
    case 2: pio_write_w(id_dest->val, id_src->val); break;
    case 4: pio_write_l(id_dest->val, id_src->val); break;
  }
  print_asm_template2(out);
}
