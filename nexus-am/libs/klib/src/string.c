#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  assert(s != NULL);
  while(*s != '\0'){
    len+=1;
    s+=1;
  }
  return len;
}

char *strcpy(char* dst,const char* src) {
  char* ret = dst;
  assert((dst != NULL)&&(src != NULL));
  while(*src != '\0'){
    *dst++ = *src++;
  }
  *dst = '\0';
  return ret;
}

char* strncpy(char* dst, const char* src, size_t n) {
 
  return NULL;
}

char* strcat(char* dst, const char* src) {
  char* ret = dst;
  assert((dst != NULL)&&(src != NULL));
  while(*dst != '\0') 
    dst++;
  while (*src)
  {
    *dst = *src;
    dst+=1;
    src+=1;
  }
  return ret;
  
}

int strcmp(const char* s1, const char* s2) {
  assert((s1 != NULL)&&(s2 != NULL));
  while (*s1 && *s2 && (*s1 == *s2)){
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
  while (n-- > 0){
    *ret++ = (char)c;
  }
  return ret;
}

void* memcpy(void* out, const void* in, size_t n) {
  //TODO();
  return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n){
  if (n<0) return 0;
  while(n>0 && (*(char*)s1 == *(char*)s2)){
    s1 = (char*)s1+1;
    s2 = (char*)s2+1;
  }
  return (*((unsigned char *)s1) - *((unsigned char *)s2));
}

#endif
