#include <string.h>

char __STRING_H_UPPER_CASE ='F';

inline long atol(const char *nptr)
{
  int c;
  long total=0;
  int sign;

  while (isspace((int)(unsigned char) *nptr)) ++nptr;

  c = (int)(unsigned char) *nptr++;
  sign = c;
  if (c == '-' || c == '+') c = (int)(unsigned char) *nptr++;

  total = 0;
  while ((char)c<='9' && (char)c>='0')
  {
    total = 10 * total + (c - '0');
    c = (int)(unsigned char) *nptr++;
  }

  if (sign == '-')
    return -total;
  else
    return total;
}
inline int atoi(const char *nptr)
{
  return (int) atol(nptr);
}
///Private Function
static inline  void xtoa(unsigned long val, char *buf, unsigned radix, int negative)
{
  char *p;
  char *firstdig;
  char temp;
  unsigned digval;

  p = buf;

  if (negative)
  {
    // Negative, so output '-' and negate
    *p++ = '-';
    val = (unsigned long)(-(long) val);
  }

  // Save pointer to first digit
  firstdig = p;

  do
  {
    digval = (unsigned) (val % radix);
    val /= radix;

    // Convert to ascii and store
    if (digval > 9)
		*p++ = (char) (digval - 10 + (__STRING_H_UPPER_CASE=='F' ? 'a' : 'A'));
    else
      *p++ = (char) (digval + '0');
  } while (val > 0);

  // We now have the digit of the number in the buffer, but in reverse
  // order.  Thus we reverse them now.

  *p-- = '\0';
  do
  {
    temp = *p;
    *p = *firstdig;
    *firstdig = temp;
    p--;
    firstdig++;
  } while (firstdig < p);
}

inline char *itoa(int val, char *buf, int radix)
{
  if (radix == 10 && val < 0)
    xtoa((unsigned long) val, buf, radix, 1);
  else
    xtoa((unsigned long)(unsigned int) val, buf, radix, 0);

  return buf;
}

inline char *ltoa(long val, char *buf, int radix)
{
  xtoa((unsigned long) val, buf, radix, (radix == 10 && val < 0));
  return buf;
}

inline char *ultoa(unsigned long val, char *buf, int radix)
{
  xtoa(val, buf, radix, 0);
  return buf;
}

inline char *Strncpy(char *dest, const char *source, int count)//Note If source[i]=0 not Copy
{
  char *start = dest;

  while (count && (*dest++ = *source++)) count--;
  if (count) while (--count) *dest++ = '\0';
  return start;
}
inline char *Bytencpy(char *dest, const char *source, int count)//Copy All Byte Event 0
{
  char *start = dest;
  while (count){ count--;*dest++ = *source++;}
  return start;
}

int strlen (const char *s) {
  int len=0;
//  while (*s++) len++;
  __asm__ __volatile__ ("movl    %%edi, %%edx	\n"
						"xorb    %%al, %%al		\n"
						"xorl    %%ecx, %%ecx	\n"
						"decl    %%ecx			\n"
						"cld					\n"
						"repne					\n"
						"scasb					\n"
						"stc					\n"
						"sbbl    %%edx, %%edi	\n"
						"movl    %%edi, %0		  "
						: "=g" (len)
						: "D" (s)
						: "eax", "ecx", "edx"//, "edi"
						);
  return len;
}

void *memcpy (void *dest, const void *src, unsigned long len) {
//  while (len--) *dest++=*src++;
  __asm__ __volatile__ ("cld							\n"
						"rep							\n"
						"movsb							  "
						:
						: "S" (src), "D" (dest), "c" (len)
				  //    : "ecx", "esi", "edi"
						);
  return dest;
}

char *Strcpy (char *dest, const char *src) {
//  while (*dest++=*src++) {};
  memcpy (dest, src, strlen(src)+1);
  return dest;
}

char *strcat (char *dest, const char *src) {
  memcpy (dest+strlen(dest), src, strlen(src)+1);
  return dest;
}

char *Strncat (char *dest, const char *src,size_t MaxLen) {	
	int DestLen=strlen(dest);
  memcpy (dest+DestLen ,src , MinNumber(strlen(src),MaxLen)+1);
  dest[DestLen+MinNumber(strlen(src),MaxLen)+1]=0;
  return dest;
}

int memcmp (const void *s_1, const void *s_2, unsigned long len) {
  int result;
  __asm__ __volatile__ ("jecxz   0f					\n"
						"cld						\n"
						"repe						\n"
						"cmpsb						\n"
						"jne     1f					\n"
						"0:							\n"
						"xorl    %%eax, %%eax		\n"
						"jmp     4f					\n"
						"1:							\n"
						"decl    %%esi				\n"
						"decl    %%edi				\n"
						"movb    (%%esi), %%al		\n"
						"subb    (%%edi), %%al		\n"
						"jc      3f					\n"
						"movl    $1, %%eax			\n"
						"jmp     4f					\n"
						"3:							\n"
						"movl    $-1, %%eax			\n"
						"4:							\n"
						"movl    %%eax, %0			  "			
						: "=g" (result)
						: "S" (s_1), "D" (s_2), "c" (len)
						: "eax"//, "ecx", "esi", "edi"
						);
  return result;
}

/// <summary>		
	///This Function Is Compare Two String .		
	/// MaxLen:	 Max Len To Compare.
	///if Is Equal Return 0 Else Return s2-s1.
/// </summary>
char Strncmp (char *str1 ,char *str2,unsigned short MaxLen)
{
	char result=0;
	char str1_ch=0,str2_ch=0;
	if(MaxLen<strlen(str1)){str1_ch=str1[MaxLen]; str1[MaxLen]=0;}
	if(MaxLen<strlen(str2)){str2_ch=str2[MaxLen]; str2[MaxLen]=0;}
	result=Strcmp(str1 ,str2 );
	if(str1_ch!=0)str1[MaxLen]=str1_ch;
	if(str2_ch!=0)str2[MaxLen]=str2_ch;
	return result;
}

/// <summary>		
	///This Function Is Compare Two String.		
	///if Is Equal Return 0 Else Return s2-s1.
/// </summary>
char Strcmp(char *str1 ,char *str2) 
{  
  return Strcmpi(str1 ,str2 );
}

/// <summary>		
	///This Function Is Compare Two String With Case Sensitivity.			
	///if Is Equal Return 0 Else Return s2-s1.
/// </summary>
char Strcmpi(char *str1 ,char *str2)
  {    
    dword index = 0;
    byte ch1 = *(str1+index), ch2 = *(str2+index);

    while(ch1 && ch2)
    {
      if(ch1 >=65 && ch1 <= 90)
       ch1 = ch1 +32;
      if(ch2 >=65 && ch2 <= 90)
       ch2 = ch2 +32;
      if(ch1 != ch2)
      {
        if(ch1 > ch2)
	        return 1;
        else
	        return -1;
      }
      index++;
      ch1 = *(str1+index), ch2 = *(str2+index);
    }

    if( ch1 == '\0' && ch2 == '\0' )
    {
      return 0;
    }
    else if(ch1 == '\0')
    {
        return -1;
    }
    else
        return 1;
  }

void *memset(void *buf, int ch, unsigned long len) 
{
  __asm__ __volatile__ ("cld						\n"
						"rep						\n"
						"stosb						  "
						:
						: "D" (buf), "a" (ch), "c" (len)
				  //    : "eax", "ecx", "edi"
						);
 return buf;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val ,size_t count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

/// <summary>		
	///This Function Is Search String From Start Parameter.		
	/// Return First Location From Start. 	
/// </summary>
 short StrFind(const char *String ,const char* StrToFind )
 {
	 unsigned short StrToFindLen=strlen(StrToFind);
     short j=0,i;
     for(i=0 ;i<strlen(String);i++)
         if(StrToFind[j]==String[i]){
			 j++;
			 if(StrToFindLen==j) return i-StrToFindLen+1;
         }
		 else j=0;
        return -1;
}

/// <summary>		
	///This Function Is Search String From Start Parameter.		
	/// Return First Location From Start. 	
/// </summary>
 short StrnFind(const char *String ,unsigned short Start ,const char* StrToFind )
 {
	unsigned short StrToFindLen=strlen(StrToFind);
    short j=0,i;
    for(i=Start ;i<strlen(String)/*+Start-1*/;i++)
		if(StrToFind[j]==String[i]){
			j++;
			if(StrToFindLen==j)return i-StrToFindLen+1;
        }else j=0;
    return -1;
  }

void *strchr(const char *s, int ch)
{
  while (*s && *s != (char) ch) s++;
  if (*s == (char) ch) return (char *) s;
  return NULL;
}

void *strrchr(const char *s, int ch)
{
  char *start = (char *) s;

  while (*s++);
  while (--s != start && *s != (char) ch);
  if (*s == (char) ch) return (char *) s;

  return NULL;
}

void *strstr(const char *str1, const char *str2)
{
  char *cp = (char *) str1;
  char *sstr1, *sstr2;

  if (!*str2) return (char *) str1;

  while (*cp)
  {
    sstr1 = cp;
    sstr2 = (char *) str2;

    while (*sstr1 && *sstr2 && !(*sstr1 - *sstr2)) sstr1++, sstr2++;
    if (!*sstr2) return cp;
    cp++;
  }

  return NULL;
}

size_t strspn(const unsigned char *string, const unsigned char *control)
{
  const unsigned char *str = string;
  const unsigned char *ctrl = control;

  unsigned char map[32];
  int count;

  // Clear out bit map
  for (count = 0; count < 32; count++) map[count] = 0;

  // Set bits in control map
  while (*ctrl)
  {
    map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    ctrl++;
  }

  // 1st char NOT in control map stops search
  if (*str)
  {
    count = 0;
    while (map[*str >> 3] & (1 << (*str & 7)))
    {
      count++;
      str++;
    }
    
    return count;
  }

  return 0;
}

size_t strcspn(const unsigned char *string, const unsigned char *control)
{
  const unsigned char *str = string;
  const unsigned char *ctrl = control;

  unsigned char map[32];
  int count;

  // Clear out bit map
  for (count = 0; count < 32; count++) map[count] = 0;

  // Set bits in control map
  while (*ctrl)
  {
    map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    ctrl++;
  }

  // 1st char in control map stops search
  count = 0;
  map[0] |= 1;
  while (!(map[*str >> 3] & (1 << (*str & 7))))
  {
    count++;
    str++;
  }
  return count;
}

void *strpbrk(const unsigned char *string, const unsigned char *control)
{
  const unsigned char *str = string;
  const unsigned char *ctrl = control;

  unsigned char map[32];
  int count;

  // Clear out bit map
  for (count = 0; count < 32; count++) map[count] = 0;

  // Set bits in control map
  while (*ctrl)
  {
    map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    ctrl++;
  }

  // 1st char in control map stops search
  while (*str)
  {
    if (map[*str >> 3] & (1 << (*str & 7))) return (char *) str;
    str++;
  }

  return NULL;
}

