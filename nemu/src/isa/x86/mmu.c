#include "nemu.h"

paddr_t page_translate(vaddr_t addr){
  //见手册图5.8
  /*paddr_t dir = (addr >> 22) & 0x3ff;//vaddr的22-31位,共10位
  paddr_t page = (addr >> 12) & 0x3ff;//vaddr的12-21位，共10位
  paddr_t offset = addr & 0xfff;//vaddr的0-11位，共12位

  if(cpu.cr0.paging){//CR0的PG位为1时，开启分页模式
    uint32_t pg_dir_base = cpu.cr3.page_directory_base;
    uint32_t pg_tab = paddr_read((pg_dir_base << 12)+(dir << 2), 4);
    assert(pg_tab & 0x1);//检查present位
    uint32_t pg_frame = paddr_read((pg_tab & 0xfffff000)+(page << 2),4);
    assert(pg_frame & 0x1);

    return (pg_frame & 0xfffff000) + offset;

  }
  return addr;//未开启分页模式，直接返回虚拟地址*/
  paddr_t dir = (addr >> 22) & 0x3ff;//vaddr的22-31位,共10位
  paddr_t page = (addr >> 12) & 0x3ff;//vaddr的12-21位，共10位
  paddr_t offset = addr & 0xfff;//vaddr的0-11位，共12位
  if (cpu.cr0.paging){
    uint32_t pg_dir_base = cpu.cr3.page_directory_base;
    paddr_t ptab = paddr_read(pg_dir_base + sizeof(PDE) * dir, sizeof(PDE));
    assert(ptab & 0x001);

    paddr_t pg = paddr_read((ptab & 0xfffff000) + sizeof(PTE) * page, sizeof(PTE));
    assert(pg & 0x001);

    return ((pg & 0xfffff000) | offset);
  }
  return addr;
  
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  //return paddr_read(addr, len);
  //if((addr & 0xfff)+len > 0x1000 ){
  //  assert(0);
  //}
  if((addr & ~0xfff) != ((addr + len - 1) & ~0xfff)){
    uint8_t temp[4];
    for (int i = 0; i < len; i++)
      temp[i] = isa_vaddr_read(addr + i, 1);
    if (len == 2)
      return *(uint16_t *)temp;
    else
      return *(uint32_t *)temp;
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
  //if((addr & 0xfff)+len > 0x1000 ){
  //  assert(0);
  //}
  if ((addr & ~0xfff) != ((addr + len - 1) & ~0xfff)){
    int8_t temp[4];
    if (len == 2)
      *(uint16_t *)temp = data;
    else
      *(uint32_t *)temp = data;
    for (int i = 0; i < len; i++)
      isa_vaddr_write(addr + i, temp[i], 1);
  }
  else
   paddr_write(page_translate(addr), data, len);
}
