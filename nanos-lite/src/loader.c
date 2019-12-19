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

  int fd = fs_open(filename, 0, 0);
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
  return ehdr.e_entry;

  
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
