#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include "monitor/expr.h"
#include <stdlib.h>
enum {
  TK_NOTYPE = 256, TK_EQ, TK_FIG

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\(", '('},         // left parenthesis
  {"\\)", ')'},         // right parenthesis
  {"\\*", '*'},         // multiply
  {"/", '/'},           // divide
  {"\\+", '+'},         // plus
  {"-", '-'},           // minus
  {"==", TK_EQ},        // equal
  {"[0-9]+", TK_FIG}     // figures
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

#define tokens_len  65536
static Token tokens[tokens_len] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  for (int i = 0; i<tokens_len; i++){
   memset(tokens[i].str,'\0',sizeof(tokens[i].str)-1);	   
  }
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE : break;
          case (int)'(' : case (int)')' : case (int)'*': case (int)'/' : case (int)'+' : case (int)'-' :
              {tokens[nr_token].type = rules[i].token_type;
               tokens[nr_token].str[0] = e[position - substr_len];
               nr_token += 1;
              } break;
          case TK_EQ : 
              {tokens[nr_token].type = rules[i].token_type;
               tokens[nr_token].str[0] = e[position - substr_len];
               tokens[nr_token].str[1] = e[position - substr_len + 1];
              } break;
          case TK_FIG : 
              {tokens[nr_token].type = rules[i].token_type;
               int t = position - substr_len;
               if (substr_len> 31 )
                assert(0);
               for (int j = 0; j< substr_len; j++){
                tokens[nr_token].str[j] = e[t];
                t+=1;
               }
               nr_token += 1;
              } break;
          default: TODO();
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


bool check_parentheses(int p,int q){// examing parentheses
  if(tokens[p].type !=(int)'(' || tokens[q].type !=(int)')') //整个表达式左右两头是否都有括号   
    return false;
  /*else{
    int count = 0;//count用来计数左右匹配情况，低于-1说明右括号多了
    int flag = 0; //flag用来判断整个表达式两边的括号是否匹配，如果count在某一位置时值为-1，则说明括号内部配对但两头不配对
    for (int j = p+1; j<q; j++){
      if (count >=-1){
        if (count == -1)  flag = 1;
        if (tokens[j].type == (int)'(')
          count += 1;
        else if (tokens[j].type == (int)')')
          count -= 1;
      }
      else
        return false;//右括号多了
    }
    if (count == 0){
      switch (flag){
       case 0 : return true;//两头配对
       case 1 : return false;//两头不配对，中间配对
      }
    }   
  }
  return false;// 左括号多了
  */
  return true;
}

int find_main_op(int p, int q){
  int prio[32];
  for (int j=0; j<32; j++){
    prio[j] = -1;
  }
  int i = 0;
  for (int t = q; t>p; ){
   if (tokens[t].type == (int)')'){
     int flag = 1;
     while (flag != 0){
        t-=1;
	if (tokens[t].type == (int)')')
	  flag +=1;
	else if (tokens[t].type == (int)'(')
          flag -=1;
	else 
	  continue;     
     }
   }
   else if(tokens[t].type == (int)'+'||tokens[t].type == (int)'-')
     return t;
   else if(tokens[t].type == (int)'*'||tokens[t].type == (int)'/'){
     prio[i] = t;
     i+=1;
     t-=1;
   }
   else
     t-=1;
 }
 return prio[0];
}


uint32_t eval(int head, int tail){
  if (head > tail){
    return false;
  }
  else if (head == tail){
    uint32_t  number; 
    sscanf(tokens[head].str, "%d", &number);
    return number;
  }
  else if (check_parentheses(head, tail) == true)
    return eval(head+1, tail-1);  
  else {
    int op = find_main_op(head, tail);
    uint32_t val1 = eval(head, op-1);
    uint32_t val2 = eval(op+1,tail);
    switch (tokens[op].type){
      case (int)'+' : return val1 + val2;break;
      case (int)'-' : return val1 - val2;break;
      case (int)'*' : return val1 * val2;break;
      case (int)'/' : return val1 / val2; break;
      default : return false;
    }
  }
  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  else{
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
    int p = 0, q = nr_token-1;

  /*check validity of expr*/
    int v = 1;
    if(!(tokens[p].type == (int)'('||tokens[p].type == TK_FIG))  v = 0;
    if (!(tokens[q].type == (int)')'||tokens[q].type == TK_FIG)) v = 0;
    for (int i=0; i<q; i++){
      switch(tokens[i].type){
        case (int)'+' : case (int)'-': case (int)'*': case (int)'/': case (int)'(':
            { if (!(tokens[i+1].type == TK_FIG || tokens[i+1].type == (int)'(' ))
                v = 0;
            }break;
        case (int)')' : 
            { if (tokens[i+1].type == TK_FIG || tokens[i+1].type == (int)'(' )
                v = 0;
            }break; 
        case TK_FIG: 
            { if (tokens[i+1].type == (int)'(' )
                v = 0;
            }break;
        default : break;
      }
      if ( v == 0)  break;
    }
    int par = 0;//括号匹配情况
    for (int i=0; i<=q; i++){
      if (tokens[i].type == (int)'(') par+=1;
      else if (tokens[i].type == (int)')') par-=1;
      if (par < 0)  v = 0;
    }
    if ( par > 0) v = 0;

    if (v == 1)  return eval(p, q);
    else{
      printf("invalid expr. return:");
      return false;
    }

 }
  return 0;
}
