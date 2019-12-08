#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_dest->val);
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  if (decinfo.isa.is_operand_size_16) {
		t0 = reg_w(R_SP);
		rtl_push((rtlreg_t *)&reg_w(R_AX));
		rtl_push((rtlreg_t *)&reg_w(R_CX));
		rtl_push((rtlreg_t *)&reg_w(R_DX));
		rtl_push((rtlreg_t *)&reg_w(R_BX));
		rtl_push((rtlreg_t *)&t0);
		rtl_push((rtlreg_t *)&reg_w(R_BP));
		rtl_push((rtlreg_t *)&reg_w(R_SI));
		rtl_push((rtlreg_t *)&reg_w(R_DI));
	}
  else { 
		t0 = reg_l(R_ESP);
		rtl_push(&reg_l(R_EAX));
		rtl_push(&reg_l(R_ECX));
		rtl_push(&reg_l(R_EDX));
		rtl_push(&reg_l(R_EBX));
		rtl_push(&t0);
		rtl_push(&reg_l(R_EBP));
		rtl_push(&reg_l(R_ESI));
		rtl_push(&reg_l(R_EDI)); 
  }

  print_asm("pusha");
}

make_EHelper(popa) {
  if (decinfo.isa.is_operand_size_16) {
		rtl_pop((rtlreg_t *)&reg_w(R_DI));
		rtl_pop((rtlreg_t *)&reg_w(R_SI));
		rtl_pop((rtlreg_t *)&reg_w(R_BP));
		rtl_pop(&t0);
		rtl_pop((rtlreg_t *)&reg_w(R_BX));
		rtl_pop((rtlreg_t *)&reg_w(R_DX));
		rtl_pop((rtlreg_t *)&reg_w(R_CX));
		rtl_pop((rtlreg_t *)&reg_w(R_AX));
	}
  else { 
		rtl_pop(&reg_l(R_EDI));
		rtl_pop(&reg_l(R_ESI));
		rtl_pop(&reg_l(R_EBP));
		rtl_pop(&t0);
		rtl_pop(&reg_l(R_EBX));
		rtl_pop(&reg_l(R_EDX));
		rtl_pop(&reg_l(R_ECX));
		rtl_pop(&reg_l(R_EAX));
}

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp, &cpu.ebp);
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
  //DX:AX ← sign-extend of AX
    rtl_sext(&s0,&reg_l(R_EAX),2);
    rtl_mv(&reg_l(R_EDX),&s0+2);
  }
  else {
  //EDX:EAX ←sign-extend of EAX
    rtl_sari(&reg_l(R_EDX), &reg_l(R_EAX), 31);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_sext(&s0, &reg_l(R_EAX), 1);
    rtl_mv(&reg_l(R_EAX), &s0);
  }
  else {
    rtl_sext(&s0,&reg_l(R_EAX),2);
    rtl_mv(&reg_l(R_EAX),&s0);
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

make_EHelper(movsb) {
  int incdec = 1;
  rtl_lr(&s0, R_ESI, 4);
  rtl_lm(&s1, &s0, 1);
  s0 += incdec;
  rtl_sr(R_ESI, &s0, 4);
  rtl_lr(&s0, R_EDI, 4);
  rtl_sm(&s0, &s1, 1);
  s0 += incdec;
  rtl_sr(R_EDI, &s0, 4);
  print_asm("movsb")
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
