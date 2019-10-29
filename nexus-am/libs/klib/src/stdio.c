#include "klib.h"
#include <stdarg.h>
#include "stdbool.h"

#define ZEROPAD 1               // Pad with zero
#define SIGN    2               // Unsigned/signed long
#define PLUS    4               // Show plus
#define SPACE   8               // Space if plus
#define LEFT    16              // Left justified
#define SPECIAL 32              // 0x
#define LARGE   64              // Use 'ABCDEF' instead of 'abcdef'

#define is_digit(c) ((c) >= '0' && (c) <= '9')
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static char *upper_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int printf(const char *fmt, ...) {
  return 0;
}

static unsigned long strnlen(const char *s, int count){
  const char *sc;
  for (sc = s; *sc != '\0' && count--; ++sc);
  return sc - s;
}
static int skip_atoi(const char **s){
  int i = 0;
  while (is_digit(**s)){
    i = i*10 + *((*s)++) - '0';
  }
  return i;
}
static char * number(char *str, long num, int base, int size, int precision, int type)
{
    char c, sign, tmp[66];
    char *dig = digits;
    int i;
 
    if (type & LARGE)  dig = upper_digits;
    if (type & LEFT) type &= ~ZEROPAD;
    if (base < 2 || base > 36) return 0;
 
    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN){
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
      else if (8 == base)
        size--;
    }
 
    i = 0;
 
    if (0 == num){
      tmp[i++] = '0';
    }
    else{
      while (num != 0){
        tmp[i++] = dig[((unsigned long) num) % (unsigned) base];
        num = ((unsigned long) num) / (unsigned) base;
      }
    }
 
    if (i > precision) precision = i;
    size -= precision;
    if (!(type & (ZEROPAD | LEFT))){
      while(size-- > 0) *str++ = ' ';
    }
    if (sign) *str++ = sign;
 
    if (type & SPECIAL){
      if (8 == base){
        *str++ = '0';
      }
      else if (16 == base){
        *str++ = '0';
        *str++ = digits[33];
      }
    }
 
    if(!(type & LEFT)){
      while(size-- > 0) *str++ = c;
    }
    while(i < precision--) *str++ = '0';
    while(i-- > 0) *str++ = tmp[i];
    while(size-- > 0) *str++ = ' ';
 
    return str;
}
int vsprintf(char *out, const char *fmt, va_list ap) {
  char *str;
  int field_width;      /* Width of output field */
 
  for (str = out; *fmt; fmt++){
    unsigned long num;
    int base = 10;
    int flags = 0; /* Flags to number()    Process flags */
    int qualifier = -1;        /* 'h', 'l', or 'L' for integer fields */
    int precision = -1;     /* Min. # of digits for integers; max number of chars for from string */
    bool bFmt = true;
    if (*fmt != '%'){
      *str++ = *fmt;
      continue;
    }
    
    bFmt = true;
    while(bFmt){
      fmt++; /* This also skips first '%' */
      switch (*fmt){
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

    /* Get the conversion qualifier */
    qualifier = -1;
    if ('h' == *fmt || 'l' == *fmt || 'L' == *fmt){
      qualifier = *fmt;
      fmt++;
    }

    /* Default base */
    base = 10;
    switch (*fmt){
      case 's':{
        int len;
        char * s = va_arg(ap, char *);
        if (!s) s = "<NULL>";
        len = strnlen(s, precision);
        if (!(flags & LEFT)) while (len < field_width--) *str++ = ' ';
        for (int i = 0; i < len; ++i) *str++ = *s++;
        while (len < field_width--) *str++ = ' ';
        continue;
      }
      case 'd': case 'i':{
        flags |= SIGN; /* no break */
      }
      case 'u':{
        break;
      }
      default:{
        if (*fmt != '%') *str++ = '%';
        if (*fmt){
          *str++ = *fmt;
        }
        else{
          --fmt;
        }
        continue;
      }
    }
    if (qualifier == 'l'){
        num = va_arg(ap, unsigned long);
    }
    else if (qualifier == 'h'){
      if (flags & SIGN)
        num = va_arg(ap, int);
      else
        num = va_arg(ap, int);
    }
    else if (flags & SIGN){
      num = va_arg(ap, int);
    }
    else{
      num = va_arg(ap, unsigned long);
    }
    
    str = number(str, num, base, field_width, precision, flags);
  } /* end of for (str = buf; *fmt; fmt++) */
 
    *str = '\0';
    return str - out;
  return 0;
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
