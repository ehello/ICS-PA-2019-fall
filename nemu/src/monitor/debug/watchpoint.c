#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* return_head(){
  return head;
}
WP* new_wp(){// 从free_链表中返回一个空闲的监视点结构，没有空闲时assert退出
  if (free_ == NULL)  assert(0);
  if (head == NULL){
    head = &free_[0];
    head->next = NULL;
    free_ = free_[0].next;
    return head;
  }
  else{
    WP* p = head;
    while (p->next != NULL) p = p->next;
    p->next = free_;
    free_ = free_->next;
    p = p->next;
    p->next = NULL;
    return p;
  }
}

bool free_wp(int n){// 将wp归还到free_链表中
  if (head == NULL) assert(0);
  WP* wp = head;
  int flag = 0;
  while (wp->next != NULL){
    if (wp->NO == n){
      flag = 1;
      break;
    }
    wp = wp->next;
  }
  if (flag == 0){
    printf("wrong number");
    return false;
  }
  memset(wp->expr,'\0',sizeof(wp->expr)-1);
  if (wp == head) head = wp->next;// 从head链表中取出
  else{
    WP* p = head;
    while (p->next != wp) p = p->next;
    p->next = wp->next; 
  }
  if (free_ == NULL){//加入free_链表
    free_ = wp;
    wp->next = NULL;
    return true;
  }
  else if (free_->next == NULL){
    if (n > free_->NO){
      wp->next = free_;
      free_ = wp;
      return true;
    }
    else{
      free_->next = wp;
      wp->next = NULL;
      return true;
    }
  }
  else{
    WP* p = free_;
    while (p->next != NULL){
      if (p->NO < n && p->next->NO > n){
        wp->next = p->next;
        p->next = wp;
        return true;
      }
      p = p->next;
    }
    p->next = wp;
    wp->next = NULL;
    return true;
  }
  return false;
}

void print_all_wp(){
  if(head != NULL){
      printf("No.   expr\n ");
      WP* p = head;
      while(p != NULL){
        printf("%d  %s\n",p->NO,p->expr);
        p = p->next;
      }
    }
    else printf("No watchpoint\n");
}

