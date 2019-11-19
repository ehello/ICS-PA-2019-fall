#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();

#define DEFAULT_ENTRY 0x300000
static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  //return 0
  
  //printf("size is %d\n",get_ramdisk_size());
  Elf_Ehdr temp;
  Elf_Ehdr *ehdr = &temp;
  ramdisk_read(ehdr,0,sizeof(Elf_Ehdr));

  //printf("type is %d\n",temp.e_type);
  //printf("entry is %d\n",temp.e_entry);
  //printf("pht num is %d\n",temp.e_phnum);
  printf("e_entry is %d\n",temp.e_entry);

  Elf_Phdr pht[temp.e_phnum];
  ramdisk_read(pht,temp.e_ehsize,sizeof(Elf_Phdr)*temp.e_phnum);
  for(int i = 0;i<temp.e_phnum;i++){
    printf("p_type is %d\n",pht[i].p_type);
    if(pht[i].p_type == PT_LOAD){
      printf("vaddr is %d,paddr is %d\n ",pht[i].p_vaddr,pht[i].p_paddr);
      ramdisk_read((void*)pht[i].p_vaddr, pht[i].p_offset, pht[i].p_memsz);
      memset((void*)(pht[i].p_vaddr+pht[i].p_filesz),'\0',pht[i].p_memsz-pht[i].p_filesz);
    }
  }
  //ramdisk_write((void*)temp.e_entry,temp1.p_vaddr,ge);
  return temp.e_entry;
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
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
