#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  //assert(s != NULL);
  while(*s != '\0'){
    len+=1;
    s+=1;
  }
  return len;
}

char *strcpy(char* dst,const char* src) {
  char *ret = dst;
  //assert((dst != NULL)&&(src != NULL));
  while((*dst++ = *src++) !='\0') ;
  return ret;
}

char* strncpy(char* dst, const char* src, size_t n) {
 
  return NULL;
}

char* strcat(char* dst, const char* src) {
  char *ret = dst;
  //assert((dst != NULL)&&(src != NULL));
  while(*dst) dst+=1;
  while ((*dst++ = *src++)) ;
  return ret;
  
}

int strcmp(const char* s1, const char* s2) {
  //assert((s1 != NULL)&&(s2 != NULL));
  while (*s1 && *s2 && (*s1 == *s2)){
    s1+=1;
    s2+=1;
  }
  if (*s1 == *s2) return 0;
  else if (*s1>*s2) return 1;
  return -1;
  //return *s1-*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  //TODO();
  return 0;
}

void* memset(void* v,int c,size_t n) {
  char *ret = (char*)v;
  //assert(v != NULL || n>0);
  while (n--){
    *ret++ = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char *dst = (char*)out;
  char *src = (char*)in;
  while(n--) *dst++ = *src++;
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  //assert(s1 != NULL || s2 != NULL || n>0);
  const char *str1 = (char*)s1;
  const char *str2 = (char*)s2;
  while(--n && (*str1 == *str2)){
    str1+=1;
    str2+=1;
  }
  if (*str1>*str2) return 1;
  else if(*str1<*str2) return -1;
  return 0;
}

#endif
