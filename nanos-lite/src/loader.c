#include "proc.h"
#include <elf.h>
#include "fs.h"

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

/*extern size_t ramdisk_read(void*, size_t, size_t);
extern size_t ramdisk_write(const void*, size_t, size_t);
extern size_t get_ramdisk_size();*/

extern int fs_open(const char*, int, int);
extern size_t fs_size(int);
extern size_t fs_offset(int);
extern __ssize_t fs_read(int, void*, size_t);
extern __off_t fs_lseek(int , __off_t , int );
extern int fs_close(int);

#define min(x, y)         ((x) < (y) ? (x) : (y))
#define PTE_ADDR(pte)     ((uint32_t)(pte) & ~0xfff)
#define OFF(va) ((uint32_t)(va) & 0xfff)

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  /*int fd = fs_open(filename,0,0);
  Elf_Ehdr ehdr;
  fs_read(fd,&ehdr,sizeof(Elf_Ehdr));

  Elf_Phdr pht[ehdr.e_phnum];
  fs_read(fd,pht,sizeof(Elf_Phdr)*ehdr.e_phnum);
  size_t offset = fs_offset(fd);
  for(int i = 0; i<ehdr.e_phnum; i++){
    if(pht[i].p_type == PT_LOAD){
      ramdisk_read((void*)pht[i].p_vaddr, offset+pht[i].p_offset, pht[i].p_memsz);
      memset((void*)(pht[i].p_vaddr+pht[i].p_filesz), 0, pht[i].p_memsz-pht[i].p_filesz);
    }
  }
  
  fs_close(fd);
  Log("Successfully loaded\n");
  return ehdr.e_entry;*/

  /*int fd = fs_open(filename, 0, 0);
  size_t file_size = fs_size(fd);
  size_t pg_cnt = (file_size + PGSIZE + - 1) / PGSIZE;
  void* pa;
  Elf_Ehdr ehdr;
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
  void* va = (void*)ehdr.e_entry;
  for (int i = 0; i < pg_cnt; i++){
    pa = new_page(1);
    _map(&pcb->as, va, pa, 0);
    if(file_size - PGSIZE * i < PGSIZE) 
      fs_read(fd, pa, file_size - PGSIZE * i);
    else  
      fs_read(fd, pa, PGSIZE);
    va += PGSIZE;
  }
  pcb->max_brk = (uintptr_t)va;
  fs_close(fd);
  return ehdr.e_entry;*/

  int fd = fs_open(filename, 0, 0);
  if (fd == -1) {
    panic("loader: can't open file %s!", filename);
  }

  Elf_Ehdr elf_header;
  fs_read(fd, (void *)&elf_header, sizeof(Elf_Ehdr));
  if (memcmp(elf_header.e_ident, ELFMAG, SELFMAG))
    panic("file %s ELF format error!", filename);

  for (size_t i = 0; i < elf_header.e_phnum; ++i) {
    Elf_Phdr phdr;
    fs_lseek(fd, elf_header.e_phoff + elf_header.e_phentsize * i, SEEK_SET);
    fs_read(fd, (void *)&phdr, elf_header.e_phentsize);
    if (phdr.p_type == PT_LOAD) {
      fs_lseek(fd, phdr.p_offset, SEEK_SET);

#ifdef HAS_VME
      // 为用户进程申请虚存空间
      void *vaddr = (void *)phdr.p_vaddr;
      void *paddr;
      int32_t left_file_size = phdr.p_filesz;

      // 这里注意在映射后由于没有修改CR3寄存器，所以现在的映射还没有启用，在
      // fs_read和memset时要对物理地址进行修改而不是虚拟地址

      // 处理第一页 (第一页可能不是页对齐)
      paddr = new_page(1);
      _map(&pcb->as, vaddr, paddr, 0);
      uint32_t page_write_size = min(left_file_size, PTE_ADDR((uint32_t)vaddr + PGSIZE) - (uint32_t)vaddr);
      fs_read(fd, (void *)(PTE_ADDR(paddr) | OFF(vaddr)), page_write_size);
      left_file_size -= page_write_size;
      vaddr += page_write_size;
      for (; left_file_size > 0; left_file_size -= page_write_size, vaddr += page_write_size) {
        assert(((uint32_t)vaddr & 0xfff) == 0);
        paddr = new_page(1);
        _map(&pcb->as, vaddr, paddr, 0);
        page_write_size = min(left_file_size, PGSIZE);
        fs_read(fd, paddr, page_write_size);
      }

      // 将进程剩下的地址空间赋值为0
      left_file_size = phdr.p_memsz - phdr.p_filesz;
      if (((uint32_t)vaddr & 0xfff) != 0) {
        page_write_size = min(left_file_size, PTE_ADDR((uint32_t)vaddr + PGSIZE) - (uint32_t)vaddr);
        memset((void *)(PTE_ADDR(paddr) | OFF(vaddr)), 0, page_write_size);
        left_file_size -= page_write_size;
        vaddr += page_write_size;
      }
      for (page_write_size = PGSIZE; left_file_size > 0; left_file_size -= page_write_size, vaddr += page_write_size) {
        assert(((uint32_t)vaddr & 0xfff) == 0);
        paddr = new_page(1);
        _map(&pcb->as, vaddr, paddr, 0);
        memset(paddr, 0, page_write_size);
      }
#else
      fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
      memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
#endif
    }
  }

  fs_close(fd);
  printf("%s: %x\n", filename, elf_header.e_entry);
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  _protect(&(pcb->as));
  printf("The address space is %x\n",(uintptr_t)((pcb->as).ptr));
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
