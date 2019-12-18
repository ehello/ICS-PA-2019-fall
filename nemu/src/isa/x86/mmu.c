#include "nemu.h"

paddr_t page_translate(vaddr_t addr){
  //见手册图5.8
  paddr_t dir = (addr >> 22) & 0x3ff;//vaddr的22-31位,共10位
  paddr_t page = (addr >> 12) & 0x3ff;//vaddr的12-21位，共10位
  paddr_t offset = addr & 0xfff;//vaddr的0-11位，共12位

  if(cpu.cr0.paging){//CR0的PG位为1时，开启分页模式
    uint32_t pg_dir_base = cpu.cr3.page_directory_base;
    uint32_t pg_tab = paddr_read((pg_dir_base << 12)+(dir << 2), 4);
    //assert(pg_tab & 0x1);//检查present位
    uint32_t pg_frame = paddr_read((pg_tab & 0xfffff000)+(page << 2),4);
    //assert(pg_frame & 0x1);

    return (pg_frame & 0xfffff000) + offset;

  }
  return addr;//未开启分页模式，直接返回虚拟地址
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  //return paddr_read(addr, len);
  if((addr & 0xfff)+len > 0x1000 ){
    //assert(0);
    uint8_t temp[8];
    uint32_t temp_offset = addr & 3;

    paddr_t paddr = page_translate(addr);
    *(uint32_t*)(temp + temp_offset) = paddr_read(paddr, 4 - temp_offset);

    paddr = page_translate((addr & ~0xfff) + 0x1000);
    //paddr = page_translate((addr & 0xfffff000) + 0x1000);
    *(uint32_t*)(temp + 4) = paddr_read(paddr, len + temp_offset -4);

    return (*(uint32_t*)(temp + temp_offset)) & (~0u >> ((4 - len) << 3));

  }
  else{
    paddr_t paddr = page_translate(addr);
    return paddr_read(paddr,len);
  }
  //paddr_t paddr = page_translate(addr);
  //return paddr_read(paddr,len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  //paddr_write(addr, data, len);
  if((addr & 0xfff)+len > 0x1000 ){
    assert(0);
  }
  else
   paddr_write(page_translate(addr), data, len);
}
