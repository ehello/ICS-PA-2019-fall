#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char *, bool *);

int main(int argc, char *argv[]) {
 // FILE *fp = fopen("home/james/ics2019/nemu/tools/gen-expr/input","r");
  FILE *fp = fopen("input","r");
  assert(fp != NULL);
  while(fp != NULL){
   char e[65536];
   char n[10];
   fscanf(fp,"%s %s",n,e);
   bool t = true;
   bool *b = &t;
   uint32_t result = expr(e,b);
   printf("%s %d %s\n", n, result, e);
  }

  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
   /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  
  return 0;
}

