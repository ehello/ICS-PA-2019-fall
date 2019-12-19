#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) { 
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  //return 0;
  uintptr_t new_brk = brk + increment;
  /*if (current->max_brk == 0){
    current->max_brk = (new_brk & 0xfff) ? ((new_brk & 0xfffff000) + PGSIZE) : new_brk;
    return 0;
  }

  while (current->max_brk < new_brk){
    _map(&current->as, (void*)current->max_brk, new_page(1), 0);
    current->max_brk += PGSIZE;
  }

  return 0;*/
  current->max_brk = (brk & 0xfffff000) + PGSIZE;
  if (new_brk < current->max_brk) 
    return 0;
  else{
    while (current->max_brk <= new_brk){
      _map(&current->as, (void*)current->max_brk, new_page(1), 0);
      current->max_brk += PGSIZE;
    }
    return 0;
  }
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
