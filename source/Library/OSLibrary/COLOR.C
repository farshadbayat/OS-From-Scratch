#include <color.h>
#include <graphics.h>

#define COLORRGB_TO_COLORNO(color, BlueMaskSize, GreenMaskSize, RedMaskSize) (unsigned int) ((color.red >> g_RedMaskShift)<<(BlueMaskSize+GreenMaskSize))+((color.green >> g_GreenMaskShift)<<BlueMaskSize) + (color.blue >> g_BlueMaskShift)


static unsigned short MaskColor(short MaskSize)
{ 
	 
	switch(MaskSize)
	{
	 case 1:
	 	 return 0x0001;
	 case 2:
	 	 return 0x0003;
	 case 3:
	 	 return 0x0007;
	 case 4:
	 	 return 0x000F;
	 case 5:
	 	 return 0x001F;
	 case 6:
	 	 return 0x003F;
	 case 7:
	 	 return 0x007F;
	 case 8:
	 	 return 0x00FF;
	 case 9:
	 	 return 0x01FF;
	 case 10:
	 	 return 0x03FF;
	 case 11:
	 	 return 0x07FF;
	 case 12:
	 	 return 0x0FFF;
	}
	return 0;
}


ColorRGB ColorRGBNOToColorRGB(unsigned char Blue ,unsigned char Green ,unsigned char Red)
{
	/*ColorRGB RGB;
	RGB.blue =(Blue & MaskColor(SVGA1->bluemasksize))<< g_BlueMaskShift; 	
	RGB.green =(Green & MaskColor(SVGA1->greenmasksize))<< g_GreenMaskShift;	
	RGB.red =(Red & MaskColor(SVGA1->redmasksize))<< g_RedMaskShift;
	return RGB;*/
	return ((ColorRGB) { Blue, Green,  Red} );
}

ColorRGB ColorNOToColorRGB(unsigned int ColorNO)
{
	ColorRGB RGB;
	RGB.blue =(ColorNO & MaskColor(SVGA1->bluemasksize))<< g_BlueMaskShift; 
	ColorNO =ColorNO >> SVGA1->bluemasksize;
	RGB.green =(ColorNO & MaskColor(SVGA1->greenmasksize))<< g_GreenMaskShift;
	ColorNO =ColorNO >> SVGA1->greenmasksize;
	RGB.red =(ColorNO & MaskColor(SVGA1->redmasksize))<< g_RedMaskShift;
	return RGB;
}
unsigned int ColorRGBToColorNO(ColorRGB Color_RGB)
{
	return COLORRGB_TO_COLORNO(Color_RGB ,SVGA1->bluemasksize ,SVGA1->greenmasksize ,SVGA1->redmasksize );
}






