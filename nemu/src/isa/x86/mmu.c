#include "nemu.h"

paddr_t page_translate(vaddr_t addr){
  // +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |      Index     |                     |
// +----------------+----------------+---------------------+
  //见手册图5.8
  paddr_t pg_dir_idx = (addr >> 22) & 0x3ff;//vaddr的22-31位,共10位
  paddr_t pg_tab_idx = (addr >> 12) & 0x3ff;//vaddr的12-21位，共10位
  paddr_t off_in_pg = addr & 0xfff;//vaddr的0-11位，共12位

  uint32_t pg_dir_base = cpu.cr3.page_directory_base;
  uint32_t pg_tab_base = paddr_read(pg_dir_base + pg_dir_idx * sizeof(PDE), sizeof(PDE));
  //检查page table第一个entry的present位
  assert(pg_tab_base & 0x1);

  uint32_t pg_frame = paddr_read(pg_tab_base + pg_tab_idx * sizeof(PTE), sizeof(PTE));
  //检查page_table第pg_tab_idx个entry的present位
  assert(pg_frame & 0x1);

  //pg_frame的低三位为空，加（或）上页内offset即得到物理地址
  return (pg_frame & 0xfffff000) | off_in_pg;
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  //return paddr_read(addr, len);
  //if((addr & 0xfff)+len > 0x1000 ){
  //  assert(0);
  //}
  if(cpu.cr0.paging){
    if((addr & 0xfffff000) != ((addr + len - 1) & 0xfffff000)){
      uint8_t temp[4];
      for (int i = 0; i < len; i++)
        temp[i] = isa_vaddr_read(addr + i, 1);
      if (len == 2)
        return *(uint16_t *)temp;
      else
        return *(uint32_t *)temp;
    }
    else
      return paddr_read(page_translate(addr), len);
  }
  else
    return paddr_read(addr, len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  //paddr_write(addr, data, len);
  //if((addr & 0xfff)+len > 0x1000 ){
  //  assert(0);
  //}
  if(cpu.cr0.paging){
    if ((addr & 0xfffff000) != ((addr + len - 1) & 0xfffff000)){
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
  else
    paddr_write(addr, data, len);
}
