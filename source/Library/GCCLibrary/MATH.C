#include <math.h>
#include <ctype.h>

	#ifndef _STRING_H_UPPER_CASE
	char STRING_H_UPPER_CASE ='F';
	#define	_STRING_H_UPPER_CASE
	#endif

	#ifndef _XTONX_DEFINED
	#define _XTONX_DEFINED	

	unsigned short htons(unsigned short n)
	{
	  return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
	}

	unsigned short ntohs(unsigned short n)
	{
		return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
	}

	unsigned long htonl(unsigned long n)
	{
		return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
	}

	unsigned long ntohl(unsigned long n)
	{
		return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
	}

	#endif


 long atol(const char *nptr)
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
 int atoi(const char *nptr)
{
  return (int) atol(nptr);
}
///Private Function
static   void xtoa(unsigned long val, char *buf, unsigned radix, int negative)
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
		*p++ = (char) (digval - 10 + (STRING_H_UPPER_CASE=='F' ? 'a' : 'A'));
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

 char *itoa(int val, char *buf, int radix)
{
  if (radix == 10 && val < 0)
    xtoa((unsigned long) val, buf, radix, 1);
  else
    xtoa((unsigned long)(unsigned int) val, buf, radix, 0);

  return buf;
}

 char *ltoa(long val, char *buf, int radix)
{
  xtoa((unsigned long) val, buf, radix, (radix == 10 && val < 0));
  return buf;
}

 char *ultoa(unsigned long val, char *buf, int radix)
{
  xtoa(val, buf, radix, 0);
  return buf;
}

double pow(double b,double e)
{
 b=(e==0?1:b);
 int i;
 for(i=1;i<e;i++)	
	 b*=b;
 return b;
}

long double powl(long double b,long double e)
{
 b=(e==0?1:b);
 int i;
 for(i=1;i<e;i++)	
	 b*=b;
 return b;
}

int pow10w(short e)
{
 int b=(e==0?1:10); 
 int i;
 for(i=1;i<e;i++)	
	b*=10;
 return b;
}

double pow10(int e)
{
 double b=(e==0?1:10); 
 int i;
 for(i=1;i<e;i++)	
	b*=10;
 return b;
}

long double pow10l(int e)
{
 long double b=(e==0?1:10);
 int i;
 for(i=1;i<e;i++)	
	 b*=10;
 return b;
}

float absf(float num)
 {
   if(num >= 0) return num;
   else return -num;
 }

int abs(int num)
 {
   if(num >= 0) return num;
   else return -num;
 }

 /// <summary>		
	///This Function Is Return Minimum Round Number.
	/// </summary>
double floorl(double d)
{
	int c = (int) d;
    
  if (d > 0)
    d = (double) (c + 1);

  else if (d < 0)
    // ??? what to do ???  Is ceil(-5.5) == -6.0, or is
    // ceil(-5.5) == -5.0?  -5.0 according to the description.
    d = (double) c;

  return (d);
  
}

 /// <summary>		
	///This Function Is Return MaximumRound Number.
	/// </summary>
double ceill(double d)
{
int i = (int) d;
  return ((double) i);
  
}

int MaxNumber(int Num1 ,int Num2)
{
	if(Num1>Num2)
		return Num1;
	else
		return Num2;
}

int MinNumber(int Num1 ,int Num2)
{
	if(Num1>Num2)
		return Num2;
	else
		return Num1;
}
