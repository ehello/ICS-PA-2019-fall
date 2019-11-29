#include "klib.h"
#include <stdarg.h>
#include <stdbool.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
int printf(const char *fmt, ...) {
  char buff[512];//原来设置为100，太小了，所以在PA3一开始报错了
  //_putc('p');_putc('r');
  va_list args;
  int i=0,n;
  va_start(args,fmt);
  n = vsprintf(buff,fmt,args);
  while(buff[i] != '\0'){
    _putc(buff[i]);
    i++;
  }
  va_end(args);
  return n;
  
}

#define ZEROPAD 1               // Pad with zero
#define SPACE   8               // Space if plus
#define is_digit(c) ((c) >= '0' && (c) <= '9')

int vsprintf(char *out, const char *fmt, va_list ap) {
  //_putc('v');_putc('s');
  char *str = out;
  for(;*fmt; fmt++){
    if(*fmt != '%'){
    *str = *fmt;
    str++;
    continue;
    }

    int flags = 0;
    fmt++;
    
    while(*fmt == ' ' || *fmt == '0'){
      if(*fmt == ' ')  flags |= SPACE;
      else if(*fmt == '0') flags |= ZEROPAD;
      fmt++;
    }

    int field_width = 0;
    if(is_digit(*fmt)){
      while(is_digit(*fmt)){
        field_width = field_width*10 + (*fmt -'0');
        fmt++;
      }
    }
    else if(*fmt == '*'){
      field_width = va_arg(ap,int);
      fmt++;
    }
    switch(*fmt){
      case 's':{
        const char *tmp = va_arg(ap,char *);
        size_t len = strlen(tmp);
        for(int i = 0;i < len;i ++){
          *str = *tmp;
          str++;
          tmp++;
        }
        continue;
      }
      case 'd': {
        int num = va_arg(ap,int);
        char num_s[512];
        int j = 0;
        if(num == 0) {
          num_s[j] = '0';
          j++;
        }
        else{
          if(num < 0){
            *str = '-';
            str++; 
            num = -num;
          }
          while(num !=0 ){
            num_s[j] = num % 10 + '0';
            num /= 10;
            j++;
          }
        }
        
        if(j < field_width){
          int d = field_width - j;
          char c = flags & ZEROPAD ? '0' : ' ';
          while(d != 0) {
            *str = c;
            str++;
            d--;
          }
        }
        for (int i = j-1;i>=0;i--){
          *str = num_s[i];
          str++;
        }
      }break;
      case '%': {
        *str = '%'; 
        str++;
        *str = '%';
        str++;
      }break;
    }
  }
  *str = '\0';
  return str-out;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  int  val;
  va_start(args,fmt);
  val = vsprintf(out,fmt,args);
  va_end(args);
  return val;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}



#endif
