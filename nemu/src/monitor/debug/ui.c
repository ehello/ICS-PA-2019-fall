#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "reg.h"

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int step_exec(char *args){
  char *arg = strtok(NULL," ");
  if(arg != NULL){
    int n;
    sscanf(arg,"%d",&n);
    n = (uint64_t) n;
    cpu_exec(n);
    return 0;
  }
  else
    return -1;
};

static int print_reg(char *args){
  char *arg = strtok(NULL," ");//参数设成NULL后，直接从第二个子串开始
  if (strcmp(arg, "r") == 0){
    isa_reg_display();
    return 0;
  }
  else
     return -1;
  /*else if (strcmp(arg,"w") == 0){
  }*/
}


static int compute_EXPR(char *args){  
  bool p = true;
  bool *success = &p;
  printf("%d\n",expr(args, success));
  return 0;
}

static int scan_memory(char *args){
  char *num = strtok(args," ");
  char *add = strtok(NULL," ");
  int n;
  if ((num != NULL)&&(add != NULL)){
    sscanf(num,"%d",&n);
    vaddr_t addr = strtol(add,NULL,16);
    for(int i=0;i<n;i++){
       vaddr_t data = vaddr_read(addr+i*4,4);
       printf("%d\n",data);
    }
    return 0;
  }
  else
    return -1;	  
}
/*
static void watchpoint_list(WP* wp){
   
}
static int set_watchpoint(char *args){
  WP* wp = new_wp();
  strcpy(wp->expr,args);
  bool b = true;
  bool* success = &b;
  wp->value = expr(wp->expr, success);
  

}*/
static int delete_watchpoint(char *args){
  int n;
  sscanf(args,"%d",&n);
  if(free_wp(n)){
    printf("Successfully delete the watchpoint");
    return 1;
  }
  else return false;


}
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si","Step one instruction exactly",step_exec},
  {"info","Print the state of registers",print_reg},
  {"x","Memory address",scan_memory},
  {"p", "Compute the value of EXPR", compute_EXPR},
  //{"w", "Set a watchpoint",set_watchpoint},
  {"d", "Delete the watchpoint", delete_watchpoint}

  /* TODO: Add more commands */

};


#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
