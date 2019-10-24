#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  //TODO();
  return 0;
}

char *strcpy(char* dst,const char* src) {
  if (dst == NULL || src ==NULL) return NULL;
  char* ret = dst;
  while(*src != '\0'){
    *dst++ = *src++;
  }
  *dst = '\0';
  return ret;
}

char* strncpy(char* dst, const char* src, size_t n) {
  //TODO();
  return NULL;
}

char* strcat(char* dst, const char* src) {
  if((dst == NULL)||(src == NULL)) return NULL;
  char *ret = dst;
  while(*dst != '\0') 
    dst++;
  while(*src != '\0')
  {
    *dst = *src;
    dst++;
    src++;
  }
  return ret;
  
}

int strcmp(const char* s1, const char* s2) {
  assert((s1 != NULL)&&(s2 != NULL));
  while ((*s1 != '\0')&&(*s2 !='\0')&&(*s1 == *s2)){
    s1+=1;
    s2+=1;
  }
  return *s1-*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  //TODO();
  return 0;
}

void* memset(void* v,int c,size_t n) {
  assert(v != NULL);
  char* ret = (char*) v;
  while (n !=0){
    *ret++ = c;
    n-=1;
  }
  return ret;
}

void* memcpy(void* out, const void* in, size_t n) {
  //TODO();
  return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n){
  const unsigned char* su1=s1;
  const unsigned char* su2=s2;
  int ret = 0;
  while(n>0){
    ret = *su1-*su2;
    if(ret != 0)
      break;
    su1++;
    su2++;
  }
  return ret;
}

#endif
