#include <math.h>

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
	
  int i = (int) d;
  return ((double) i);
}

 /// <summary>		
	///This Function Is Return MaximumRound Number.
	/// </summary>
double ceill(double d)
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
