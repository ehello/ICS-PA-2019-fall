#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_and(&id_dest->val, &id_dest->val, &id_src->val);

  rtl_li(&s0,0);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);

  rtl_update_ZFSF(&id_dest->val, id_dest->width);  

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&id_dest->val, &id_dest->val, &id_src->val);
  operand_write(id_dest, &id_dest->val);

  rtl_li(&s0,0);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);

  rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&id_dest->val, &id_dest->val, &id_src->val);
  operand_write(id_dest,&id_dest->val);

  rtl_li(&s0,0);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);

  rtl_update_ZFSF(&id_dest->val, id_dest->width);  

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&id_dest->val, &id_dest->val, &id_src->val);
  operand_write(id_dest,&id_dest->val);
  
  rtl_li(&s0,0);
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);
   
  rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  // unnecessary to update CF and OF in NEMU
  rtl_sar(&id_dest->val,&id_dest->val,&id_src->val);
  operand_write(id_dest,&id_dest->val);

  rtl_update_ZFSF(&id_dest->val,id_dest->width);  

  print_asm_template2(sar);
}

make_EHelper(shl) {
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&id_dest->val,&id_dest->val,&id_src->val);
  operand_write(id_dest,&id_dest->val);
 
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // unnecessary to update CF and OF in NEMU
   rtl_shr(&id_dest->val,&id_dest->val,&id_src->val);
   operand_write(id_dest,&id_dest->val);
 
   rtl_update_ZFSF(&id_dest->val,id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_not(&id_dest->val,&id_dest->val);
  operand_write(id_dest,&id_dest->val);
  
  print_asm_template1(not);
}

make_EHelper(rol) {
  rtl_li(&s0,id_dest->val);
  for (int i=0; i<id_src->val; i++) {
    rtl_msb(&s1,&s0,id_dest->width);
    rtl_shli(&s0,&s0,1);
    rtl_add(&s0,&s0,&s1);
  }
  operand_write(id_dest,&s0);

  print_asm_template1(rol);
}
