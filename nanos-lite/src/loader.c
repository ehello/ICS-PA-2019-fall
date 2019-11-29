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
extern int fs_open(const char *pathname, int flags, int mode);
extern size_t fs_offset(int fd);
extern __ssize_t fs_read(int fd, void *buf, size_t len);
extern int fs_close(int fd);


static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  //return 0;
  int fd = fs_open(filename,0,0);
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
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
