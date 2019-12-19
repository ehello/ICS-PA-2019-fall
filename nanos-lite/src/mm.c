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
  if (current->max_brk == 0){
    current->max_brk = ((brk + increment) & 0xfff) ? (((brk + increment) & 0xfffff000) + PGSIZE) : (brk + increment);
    return 0;
  }

  while (current->max_brk < (brk + increment)){
    _map(&current->as, (void*)current->max_brk, new_page(1), 0);
    current->max_brk += PGSIZE;
  }
  
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
