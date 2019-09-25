#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#define expr_len 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[expr_len];
  int value;
  /* TODO: Add more members if necessary */


} WP;
WP* return_head();
WP* new_wp();
bool free_wp(int n);
#endif
