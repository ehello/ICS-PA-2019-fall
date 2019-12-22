#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
PCB *fg_pcb = NULL;

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

void run_proc(PCB *pcb) {
  extern void proc_mm(_AddressSpace *as, void (*entry)());
  current = pcb;
  proc_mm(&pcb->as, (void (*)())pcb->cp->eip);
}

extern void naive_uload(PCB *, const char *);
extern void context_kload(PCB *, void *);
extern void context_uload(PCB *, const char *);
extern void proc_mm(_AddressSpace *, void (*)());

void init_proc() {
  Log("Initializing processes...");
  // load program here

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
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[3], "/bin/pal");
  
  //fg_pcb = &pcb[1];
  //run_proc(&pcb[1]); 
  //switch_boot_pcb();
}

static uint32_t cnt = 0;
_Context* schedule(_Context *prev) {
  // save the context pointer
  current->cp = prev;

  // PA4.1 always select pcb[0] as the new process
  //current = &pcb[0];

  //PA 4.1-4.2
  //current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  
  int fg = 50;
  current = ((cnt++ % fg) ? fg_pcb : &pcb[0]);
  
  // then return the new context
  return current->cp;
}

void set_fg_pcb(int idx){
  fg_pcb = &pcb[idx]; 
}
