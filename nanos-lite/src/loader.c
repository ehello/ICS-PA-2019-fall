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
  Elf_Ehdr ehdr;
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));

  for (int i = 0; i < ehdr.e_phnum; i++) {
    Elf_Phdr phdr;
    fs_lseek(fd, ehdr.e_phoff + ehdr.e_phentsize * i, SEEK_SET);
    fs_read(fd, &phdr, ehdr.e_phentsize);

    if (phdr.p_type == PT_LOAD) {
      fs_lseek(fd, phdr.p_offset, SEEK_SET);

      // 为用户进程申请虚存空间
      void *va = (void *)phdr.p_vaddr;
      void *pa;
      int32_t filesz_left = phdr.p_filesz;

      // 这里注意在映射后由于没有修改CR3寄存器，所以现在的映射还没有启用，在
      // fs_read和memset时要对物理地址进行修改而不是虚拟地址

      // 处理第一页 (第一页可能不是页对齐)
      pa = new_page(1);
      _map(&pcb->as, va, pa, 0);
      uint32_t pg_write_size;
      if (filesz_left < PTE_ADDR((uint32_t)va + PGSIZE) - (uint32_t)va)
        pg_write_size = filesz_left;
      else
        pg_write_size = PTE_ADDR((uint32_t)va + PGSIZE) - (uint32_t)va;
       
      fs_read(fd, (void *)(PTE_ADDR(pa) | OFF(va)), pg_write_size);
      filesz_left -= pg_write_size;
      va += pg_write_size;

      //处理其他页吧？
      while(filesz_left > 0){
        assert(!OFF(va));//判断是否对齐
        pa = new_page(1);
        _map(&pcb->as, va, pa, 0);

        if (filesz_left < PGSIZE)
          pg_write_size = filesz_left;
        else
          pg_write_size = PGSIZE;

        fs_read(fd, pa, pg_write_size);
        
        filesz_left -= pg_write_size;
        va += pg_write_size;
      }

      // 将进程剩下的地址空间赋值为0
      int32_t delt_size = phdr.p_memsz - phdr.p_filesz;
      uint32_t pg_reset_size;
      if (OFF(va)) {
        if (delt_size < PTE_ADDR((uint32_t)va + PGSIZE) - (uint32_t)va)
          pg_reset_size = delt_size;
        else 
          pg_reset_size = PTE_ADDR((uint32_t)va + PGSIZE) - (uint32_t)va;
        memset((void *)(PTE_ADDR(pa) | OFF(va)), 0, pg_reset_size);
        delt_size -= pg_reset_size;
        va += pg_reset_size;
      }
      while(delt_size > 0){
        assert(!OFF(va));
        pa = new_page(1);
        _map(&pcb->as, va, pa, 0);
        memset(pa, 0, PGSIZE);
        delt_size -= PGSIZE;
        va += PGSIZE;
      }

      //fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
      //memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
    }
  }

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
