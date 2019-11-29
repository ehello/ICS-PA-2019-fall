#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
  rtl_pop(&s0);
  operand_write(id_dest,&s0);
  
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();
  s0 = cpu.esp;
  //rtl_li(&s0,cpu.esp);
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&s1);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);


  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&s0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);//之前多打了一个edx，要细心呐
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);


  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    //TODO();
    int ax = (int32_t)(int16_t)reg_w(R_AX);//check later
    if (ax < 0)
      reg_w(R_DX) = 0xffff;
    else 
      reg_w(R_DX) = 0;
  }
  else {
    //TODO();
    int ax = (int32_t)cpu.eax;
    if (ax < 0)
      cpu.edx = 0xffffffff;
    else 
      cpu.edx = 0;
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    //TODO();
    s0 = reg_b(R_AL);
    rtl_sext(&s0,&s0,1);
    reg_w(R_AX) = s0;
  }
  else {
    //TODO();
    s0 = reg_w(R_AX);
    rtl_sext(&s0,&s0,2);
    reg_l(R_EAX) = s0;
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(movsb){
  //TODO();
  int incdec = 1;
  rtl_lr(&s0,R_ESI,4);
  rtl_lm(&s1,&s0,1);
  s0+=incdec;
  rtl_sr(R_ESI,&s0,4);
  rtl_lr(&s0,R_EDI,4);
  rtl_sm(&s0,&s1,1);
  s0+=incdec;
  rtl_sr(R_EDI,&s0,4);

  print_asm_template2(movsb);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
