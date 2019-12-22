#include "common.h"
//#include "/home/james/ics2019/nexus-am/am/am.h"

extern _Context* do_syscall(_Context*);
extern _Context* schedule(_Context *);
extern void _yield();

static _Context* do_event(_Event e, _Context* c) {
  //printf("c->irq in do_event is %d\n",c->irq);
  //printf("e.event in do_event is %d\n",e.event);
  switch (e.event) {
    //case _EVENT_YIELD: printf("This is a yield event.\n"); break;
    case _EVENT_YIELD: return schedule(c); break;
    case _EVENT_SYSCALL: do_syscall(c); break;
    //case _EVENT_IRQ_TIMER: Log("This is irq_timer working !\n"); _yield(); break;
    default: panic("Unhandled event ID = %d", e.event);
  } 

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
