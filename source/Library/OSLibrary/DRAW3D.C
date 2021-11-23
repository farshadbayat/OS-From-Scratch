#include <draw3d.h>
#include <math.h>

void blend(GraphicArea G_PTR ,ColorRGB FirstColor,ColorRGB SecondColor ,int startX ,int startY ,int width ,int hight,bool IsVertical)
{
	ColorRGB Color=FirstColor,ColorTmp;
	double r,g,b;
	double r1=0,g1=0,b1=0;
	//r=.1;g=.2;b=.7;
	unsigned short count;
	int EndX,EndY;
	EndX=startX+width;
	EndY=startY+hight;	
	
	if(IsVertical==false)//Horizon
	{
		r=absf((float)(SecondColor.red-FirstColor.red)/(float)hight);	
		g=absf((float)(SecondColor.green-FirstColor.green)/(float)hight);	
		b=absf((float)(SecondColor.blue-FirstColor.blue)/(float)hight);
		for(count=startY;count<EndY;count++)
		{
			r1+=r;
			g1+=g;
			b1+=b;
			ColorTmp.blue= (ColorType)ceill(Color.blue+b1);
			ColorTmp.green=(ColorType)ceill(Color.green+g1);
			ColorTmp.red=(ColorType)ceill(Color.red+r1);			
			DrawLine(G_PTR,&ColorTmp ,Draw_Normal ,startX ,count ,EndX ,count );			
		}
	}
	else//Vertical
	{
		r=absf((float)(SecondColor.red-FirstColor.red)/(float)width);	
		g=absf((float)(SecondColor.green-FirstColor.green)/(float)width);	
		b=absf((float)(SecondColor.blue-FirstColor.blue)/(float)width);
		for(count=startX;count<EndX;count++)
		{
			r1+=r;
			g1+=g;
			b1+=b;
			ColorTmp.blue=(ColorType)ceill(Color.blue+b1);
			ColorTmp.green=(ColorType)ceill(Color.green+g1);
			ColorTmp.red=(ColorType)ceill(Color.red+r1);			
			DrawLine(G_PTR ,&ColorTmp ,Draw_Normal ,count ,startY ,count ,EndY );
			//printf("r=%d ,g=%d ,b=%d",r1,g1,b1);getch();
		}
	}

}




