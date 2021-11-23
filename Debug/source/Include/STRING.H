#ifndef __STRING_H
#define	__STRING_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ___SIZE_T
typedef int size_t;
#define	___SIZE_T
#endif


extern char __STRING_H_UPPER_CASE;


#include<system.h>
#include <ctype.h>
#include <math.h>

 long atol(const char *str);
 int  atoi(const char *str);  
 char *itoa(int val, char *buf, int radix);
 char *ltoa(long val, char *buf, int radix);
 char *ultoa(unsigned long val, char *buf, int radix);
//extern char *Strncpy(char *dest, const char *source, size_t count);
//extern size_t strlen(const char *str);

 #define MemClear(ptr, num) memset(ptr, 0, num)
 char *Strncpy(char *dest, const char *source, int count);
 char *Bytencpy(char *dest, const char *source, int count);
 int strlen(const char *s);
 void *memcpy(void *dest, const void *src, unsigned long len);
 char *Strcpy(char *dest, const char *src);
 char *strcat(char *dest, const char *src);
 char *Strncat (char *dest, const char *src,size_t MaxLen);
 int memcmp(const void *s_1, const void *s_2, unsigned long len);
 char Strncmp(char *str1,char *str2,unsigned short MaxLen);
 char Strcmp(char *str1,char *str2);
 char Strcmpi(char *str1,char *str2);
 void *memset(void *buf, int ch, unsigned long len);
 unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
 short StrFind(const char *String ,const char* StrToFind );
 short StrnFind(const char *String ,unsigned short Start ,const char* StrToFind );
 void *strchr(const char *s, int ch);
 void *strrchr(const char *s, int ch);
 void *strstr(const char *str1, const char *str2);
 size_t strspn(const unsigned char *string, const unsigned char *control);
 size_t strcspn(const unsigned char *string, const unsigned char *control);
 void *strpbrk(const unsigned char *string, const unsigned char *control);

#ifdef __cplusplus
}
#endif

#endif





