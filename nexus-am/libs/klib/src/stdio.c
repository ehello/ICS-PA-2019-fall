#include "klib.h"
#include <stdarg.h>
#include <stdbool.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
//自己修复版
int printf(const char *fmt, ...) {
  char buff[65536];//原来设置为100，太小了，所以在PA3一开始报错了
  va_list args;
  int n;

  va_start(args,fmt);
  n = vsprintf(buff,fmt,args);
  int i = 0;
  while(buff[i] !='\0') {
    _putc(buff[i]);
    i++;
  }
  //_putc('p');
  va_end(args);
  return n;
  
}

#define ZEROPAD 1               // Pad with zero
#define SIGN    2               // Unsigned/signed long
#define PLUS    4               // Show plus
#define SPACE   8               // Space if plus
#define LEFT    16              // Left justified
#define SPECIAL 32              // 0x
#define LARGE   64              // Use 'ABCDEF' instead of 'abcdef'
static char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
char *number(char *str,long num,int base, int size, int precision, int type){
  char c, sign, tmp[65536];
  char* dig = digits;
  int i = 0;

  if (type & LEFT) type &= ~ZEROPAD;
  c = (type & ZEROPAD) ? '0' : ' ';
  sign = 0;
  if (type & SIGN){//check type
    if (num < 0){
      sign = '-';
      num = -num;
      size--;
    }
    else if (type & PLUS){
      sign = '+';
      size--;
    }
    else if (type & SPACE){
      sign = ' ';
      size--;
    }
  }

  if (type & SPECIAL){
    if (16 == base)
      size -= 2;
  }

  /*if (num < 0) {
    num = -num;
    *str = '-';
    str++;
  }*/
  if(num == 0){
    tmp[i++]='0';
  }
  else{
    while(num != 0){
      tmp[i++] = dig[(unsigned long)num % (unsigned)base];
      num = ((unsigned long) num) / (unsigned) base;
    }
  }

  if (i > precision) precision = i;
  size -= precision;
  if (!(type & (ZEROPAD | LEFT))){
    while(size-- > 0) *str++ = ' ';
  }
  if(sign) *str++ = sign;

  if(type & SPECIAL){
    if (base == 16){
      *str++ = '0';
      *str++ = digits[33];
    }
  }

  if(!(type & LEFT)){
    while(size-- > 0) *str++ = c;
  }
  while(i < precision--) *str++ = '0';
  while(i-- > 0) *str ++= tmp[i];
  while(size-- > 0) *str++ = ' ';

  return str;
}

#define is_digit(c) ((c) >= '0' && (c) <= '9')
static int skip_atoi(const char **s){
  int i = 0;
  while (is_digit(**s)){
    i = i*10 + *((*s)++) - '0';
  }
  return i;
}
static unsigned long strnlen(const char *s, int count){
  const char *sc;
  for (sc = s; *sc != '\0' && count--; ++sc);
  return sc - s;
}



int vsprintf(char *out, const char *fmt, va_list ap) {
  char* tmp = out;
  int field_width;

  for(;*fmt;++fmt){
    long num = 250;
    int base = 10;
    int flags = 0; /* Flags to number()    Process flags */
    int precision = -1;
    bool bFmt = true;
    if(*fmt != '%'){//正常的字符，直接输出即可
      *out = *fmt;
      out++;
      continue;
    }
    while(bFmt){//判断填充类型
      fmt++;
      switch(*fmt){
        case '-': flags |= LEFT; break;
        case '+': flags |= PLUS; break;
        case ' ': flags |= SPACE; break;
        case '#': flags |= SPECIAL; break;
        case '0': flags |= ZEROPAD; break;
        default:  bFmt = false;
      }
    }

    /* Get field width */
    field_width = -1;
    if (is_digit(*fmt)){
      field_width = skip_atoi(&fmt);
    }
    else if ('*' == *fmt){
      fmt++;
      field_width = va_arg(ap, int);
      if (field_width < 0){
        field_width = -field_width;
        flags |= LEFT;
      }
    }

    /* Get the precision */
    precision = -1;
    if ('.' == *fmt){
        ++fmt;
        if (is_digit(*fmt)){
          precision = skip_atoi(&fmt);
        }
        else if ('*' == *fmt){
          ++fmt;
          precision = va_arg(ap, int);
        }
        if (precision < 0) precision = 0;
    }

    //fmt++;
    if('\0' == *fmt){
      assert(0);
      break;
    }
    base = 10;
    switch(*fmt){
      case '%':{
        *out = *fmt;
        break;
      }
      case 'd':{
        //long num = va_arg(ap,int);
        //out = number(out,num,base);
        num = va_arg(ap,int);
        flags |=SIGN;
        break;
      }
      case 'x':{
        num = va_arg(ap,long);
        base = 16;
        break;
      }
      case 's':{
        char *s = va_arg(ap, char*);
        //while(*s) *out++=*s++;
        if (!s) s = "<NULL>";
        int len = strnlen(s, precision);
        if (!(flags & LEFT)) while (len < field_width--) *out++ = ' ';
        for (int i = 0; i < len; ++i) *out++ = *s++;
        while (len < field_width--) *out++ = ' ';
        continue;
      }
      default: break;
    }
    out=number(out,num,base,field_width,precision,flags);
    
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
