#include "klib.h"
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)



int printf(const char *fmt, ...) {
  return 0;
}

static char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
char *number(char *str,long num,int base){
  char tmp[66];
  char* dig = digits;
  int i = 0;
  if (num < 0) {
    num = -num;
    tmp[0]='-';
    i++;
  }
  while(num != 0){
    tmp[i++] = dig[(unsigned long)num % (unsigned)base];
    num /= base;
  }
  while(i-- >= 0) *str++ = tmp[i];
  return str;
}
int vsprintf(char *out, const char *fmt, va_list ap) {
  int base;
  char* tmp = out;
  for(;*fmt;++fmt){
    if(*fmt != '%'){
      *out = *fmt;
      out++;
      continue;
    }

    fmt++;
    if('\0' == *fmt){
      assert(0);
      break;
    }

    base = 10;
    switch(*fmt){
      case '%':{
        *out = *fmt;
        //out++;
        break;
      }
      case 'd':{
        long num = va_arg(ap,int);
        out = number(out,num,base);
        //out++;
        break;
      }
      case 's':{
        char *s = va_arg(ap, char*);
        while(*s) *out++=*s++;
        //out++;
        continue;
      }
      default: break;
    }
    
  }
  *out = '\0';
  return out-tmp;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  int n;

  va_start(args,fmt);
  n = vsprintf(out,fmt,args);
  va_end(args);

  return n;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
