#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

extern void naive_uload(PCB *, const char *);
extern void context_kload(PCB *, void *);
extern void context_uload(PCB *, const char *);
void init_proc() {
  //PA 3
  //naive_uload(NULL, "/bin/init");

  //PA 4.1
  //context_kload(&pcb[0], (void *)hello_fun);
  //context_uload(&pcb[1], "/bin/init");
  
  //PA 4.2 TASK 2
  //context_uload(&pcb[0], "/bin/dummy");

  //PA4.2 TASK 4
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
 
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  
  
}

_Context* schedule(_Context *prev) {
  
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  //current = &pcb[0];

  //PA 4.1-4.2
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  //PA4.3 
  if (current ==  &pcb[0]){
    for (int i =  0; i < 10; i++)
      current = &pcb[1];
  }
  else if (current == &pcb[1])
    current = &pcb[0];
  
  
  // then return the new context
  return current->cp;
}
