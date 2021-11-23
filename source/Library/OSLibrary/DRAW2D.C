#include <draw2d.h>
#include <color.h>	
#include <mmu.h>
#include <font.h>
#include <system.h>
#include <string.h>
#include <math.h>


#define kernelProcessorWriteDwords(value, dest, count) \
  __asm__ __volatile__ ("pushal \n\t"                  \
			"pushfl \n\t"                  \
			"cld \n\t"                     \
			"rep stosl \n\t"               \
			"popfl \n\t"                   \
			"popal"                        \
			: : "a" (value), "D" (dest), "c" (count))

#define kernelProcessorCopyBytes(src, dest, count) \
  __asm__ __volatile__ ("pushal \n\t"              \
			"pushfl \n\t"              \
			"cld \n\t"                 \
			"rep movsb \n\t"           \
			"popfl \n\t"               \
			"popal"                    \
			: : "S" (src), "D" (dest), "c" (count))
#define SWAP(a, b) do { int tmp = a; a = b; b = tmp; } while (0)
int DrawLine(GraphicArea G_PTR ,ColorRGB *foreground,DrawMode mode, int startX, int startY,int endX, int endY)
{
  // Draws a line on the screen using the preset foreground color

  int status = 0;
  //int lineLength = 0;
  //int lineBytes = 0;
  //unsigned char *framebufferPointer = NULL;
  //short pix = 0;
  int count;
 
  // to make the line
  // Is it a horizontal line?
  if (startY == endY)
	{
		for(count=startX; count<=endX ;count++)
			putpixel(G_PTR,count,startY ,*foreground,mode );  
	}
  // Is it a vertical line?
  else if (startX == endX)
    {
		for(count=startY; count<=endY ;count++)
			putpixel(G_PTR,startX ,count ,*foreground,mode ); 
	}
  else// It's not horizontal or vertical.  We will use a Bresenham algorithm
    {
      // Since I didn't feel like dragging out my old computer science
      // textbooks and re-implementing the Bresenham algorithm, this is a
      // heavily customized version of the code published here:
      // http://www.funducode.com/freec/graphics/graphics2.htm
      // The original author of which seems to be Abhijit Roychoudhuri

      int dx, dy, e = 0, e_inc = 0, e_noinc = 0, incdec = 0, start = 0, end = 0, var = 0;

      if (startX > endX)
	  {
		SWAP(startX, endX);
		SWAP(startY, endY);
	  }

      dx = (endX - startX);
      dy = (endY - startY);
      
      /* 0 < m <= 1 */
      if ((dy <= dx) && (dy > 0))
	  {
		e_noinc = (dy << 1);
		e = ((dy << 1) - dx);
		e_inc = ((dy - dx) << 1);
		start = startX;
		end = endX;
		var = startY;
		incdec = 1;
	  }

      /* 1 < m < infinity */
      if ((dy > dx) && (dy > 0))
	  {
		e_noinc = (dx << 1);
		e = ((dx << 1) - dy);
		e_inc = ((dx - dy) << 1);
		start = startY;
		end = endY;
		var = startX;
		incdec = 1;
	  }

      /* 0 > m > -1, or m = -1 */
      if (((-dy < dx) && (dy < 0)) || ((dy == -dx) && (dy < 0)))
	  {
		dy = -dy;
		e_noinc = (dy << 1);
		if ((-dy < dx) && (dy < 0))
			e = ((dy - dx) << 1);
		else
		    e = ((dy << 1) - dx);
		e_inc = ((dy - dx) << 1);
		start = startX;
		end = endX;
		var = startY;
		incdec = -1;
	  }

      /* -1 > m > 0 */
      if ((-dy > dx) && (dy < 0))
	  {
		dx = -dx;
		e_noinc = -(dx << 1);
		e = ((dx - dy) << 1);
		e_inc = -((dx - dy) << 1);
		SWAP(startX, endX);
		SWAP(startY, endY);
		start = startY;
		end = endY;
		var = startX;
		incdec = -1;
	  }
  
	  for (count = start; count <= end; count++)
	  {
		if (start == startY)
			putpixel(G_PTR,var,count,*foreground ,mode); // driverDrawPixel(buffer, foreground, mode,  var, count);
		else
			putpixel(G_PTR,count,var,*foreground ,mode);//driverDrawPixel(buffer, foreground, mode, count, var);

		if (e < 0)
			e += e_noinc;
		else
		{
			var += incdec;
			e += e_inc;
		}
	  }
	}
  
  return (status = 0);
}


int DrawRect(GraphicArea G_PTR ,ColorRGB *foreground ,DrawMode mode ,int xCoord ,int yCoord ,int width ,int height ,int thickness )
{
  // Draws a rectangle on the screen using the preset foreground color

  int status = 0;
  int endX = (xCoord + (width - 1));
  int endY = (yCoord + (height - 1));
  //int lineBytes = 0;
  //unsigned char *lineBuffer = NULL;
  //void *framebufferPointer = NULL;
  //short pix = 0;
  int count;

  // Out of the buffer entirely?
  if ((xCoord >= (signed int)MainGraphicArea.width) || (yCoord >= (signed int)MainGraphicArea.hight))
    return (status = -1/*ERR_BOUNDS*/);

      // Draw the top line 'thickness' times
      for (count = (yCoord + thickness - 1); count >= yCoord; count --)
		DrawLine(G_PTR,foreground, mode, xCoord, count, endX, count);	  

      // Draw the left line 'thickness' times
      for (count = (xCoord + thickness - 1); count >= xCoord; count --)
		DrawLine(G_PTR,foreground, mode, count, (yCoord + thickness),  count, (endY - thickness));	  

      // Draw the bottom line 'thickness' times
      for (count = (endY - thickness + 1); count <= endY; count ++)
		DrawLine(G_PTR,foreground, mode, xCoord, count, endX, count);

      // Draw the right line 'thickness' times
      for (count = (endX - thickness + 1); count <= endX; count ++)
		DrawLine(G_PTR,foreground, mode, count, (yCoord + thickness), count, (endY - thickness));
   

  return (status = 0);
}


int DrawOval(GraphicArea G_PTR,ColorRGB  *foreground, DrawMode mode, int centerX, int centerY, int width,int height, int thickness, int fill)
{
  // Draws an oval on the screen using the preset foreground color.  We use
  // a version of the Bresenham circle algorithm, but in the case of an
  // (unfilled) oval with (thickness > 1) we calculate inner and outer ovals,
  // and draw lines between the inner and outer X coordinates of both, for
  // any given Y coordinate.

  int status = 0;
  unsigned int outerRadius = (width >> 1);
  unsigned int outerD = (3 - (outerRadius << 1));
  unsigned int outerX = 0;
  unsigned int outerY =(unsigned )outerRadius;
  unsigned int innerRadius = 0, innerD = 0, innerX = 0, innerY = 0;
  unsigned int *outerBitmap = NULL;
  unsigned int *innerBitmap = NULL;

  // For now, we only support circles
  if (width != height)
    {
      kprintf("The framebuffer driver only supports circular ovals");
      return (status = -1/*ERR_NOTIMPLEMENTED*/);
    }

  outerBitmap = (unsigned int *)kmalloc((outerRadius + 1) * sizeof(int),"DrawOval: OuterBitmap");
  if (outerBitmap == NULL)
    return (status = -1/*ERR_MEMORY*/);

  if ((thickness > 1) && !fill)
    {
      innerRadius = (outerRadius - thickness + 1);
      innerD = (3 - (innerRadius << 1));
      innerY = innerRadius;

      innerBitmap =(unsigned int *) kmalloc((innerRadius + 1) * sizeof(int),"DrawOval: Inner Bitmap");
      if (innerBitmap == NULL)
	return (status = -1/*ERR_MEMORY*/);
    }

  while (outerX <= outerY)
    {
      if (!fill && (thickness == 1))
	{
		putpixel(G_PTR,(centerX + outerX), (centerY + outerY), *foreground, mode);	  
		putpixel(G_PTR,(centerX + outerX), (centerY - outerY), *foreground, mode);
	    putpixel(G_PTR,(centerX - outerX), (centerY + outerY), *foreground, mode);
	    putpixel(G_PTR,(centerX - outerX), (centerY - outerY), *foreground, mode);
	    putpixel(G_PTR,(centerX + outerY), (centerY + outerX), *foreground, mode);
	    putpixel(G_PTR,(centerX + outerY), (centerY - outerX), *foreground, mode);
	    putpixel(G_PTR,(centerX - outerY), (centerY + outerX), *foreground, mode);
	    putpixel(G_PTR,(centerX - outerY), (centerY - outerX), *foreground, mode);
	}
      
      if (outerY > outerBitmap[outerX])
		outerBitmap[outerX] = outerY;
      if (outerX > outerBitmap[outerY])
		outerBitmap[outerY] = outerX;
      
      if (outerD < 0)
		outerD += ((outerX << 2) + 6);
      else
	  {
		outerD += (((outerX - outerY) << 2) + 10);
		outerY -= 1;
	  }
      outerX += 1;

      if ((thickness > 1) && !fill)
	  {
	  if (!innerBitmap[innerX] || (innerY < innerBitmap[innerX]))
	    innerBitmap[innerX] = innerY;
	  if (!innerBitmap[innerY] || (innerX < innerBitmap[innerY]))
	    innerBitmap[innerY] = innerX;

	  if (innerD < 0)
	    innerD += ((innerX << 2) + 6);
	  else
	    {
	      innerD += (((innerX - innerY) << 2) + 10);
	      innerY -= 1;
	    }
	  innerX += 1;
	}
    }

  if ((thickness > 1) || fill)
    for (outerY = 0; outerY <= outerRadius; outerY++)
      {
	if ((outerY > innerRadius) || fill)
	  {
	    DrawLine(G_PTR,foreground ,mode ,(centerX - outerBitmap[outerY]) ,(centerY - outerY) ,(centerX + outerBitmap[outerY]) ,(centerY - outerY));
	    DrawLine(G_PTR,foreground ,mode ,(centerX - outerBitmap[outerY]) ,(centerY + outerY) ,(centerX + outerBitmap[outerY]) ,(centerY + outerY));
	  }
	else
	  {
	    DrawLine(G_PTR,foreground ,mode ,(centerX - outerBitmap[outerY]) ,(centerY - outerY) ,(centerX - innerBitmap[outerY]) ,(centerY - outerY));
	    DrawLine(G_PTR,foreground ,mode ,(centerX + innerBitmap[outerY]) ,(centerY - outerY) ,(centerX + outerBitmap[outerY]) ,(centerY - outerY));
	    DrawLine(G_PTR,foreground ,mode ,(centerX - outerBitmap[outerY]) ,(centerY + outerY) ,(centerX - innerBitmap[outerY]) ,(centerY + outerY));
	    DrawLine(G_PTR,foreground ,mode ,(centerX + innerBitmap[outerY]) ,(centerY + outerY) ,(centerX + outerBitmap[outerY]) ,(centerY + outerY));
	  }
      }

  free(outerBitmap);
  if ((thickness > 1) && !fill)
    free(innerBitmap);

  return (status = 0);
}


void drawChar(GraphicArea G_PTR ,dword x, dword y, dword index, ColorRGB Color,bool ClearLayout)
 {
   dword i = 0; 
   /*ColorRGB c = {0xFF,0xFF,0xFF};*/
   word a=1,b=1;

   x %= SVGA1->Xresolution;
   y %= SVGA1->Yresolution;
      
   for(i = 0 ; i < CHARROWS ; i++)
   {	  
     //0x01 0x02 0x04 0x04 0x08 0x10 0x20 0x40 0x80	
	if (fonts8x8[index][i] & 0x80)
		putpixel(G_PTR,x+a*0,y+b*i,Color ,Draw_Normal );	
	else if(ClearLayout==false)
		putpixel(G_PTR,x+a*0,y+b*i,DESKTOP_BACKGROUND ,Draw_Normal );	

	if (fonts8x8[index][i] & 0x40)
		putpixel(G_PTR,x+a*1,y+b*i,Color ,Draw_Normal );	
	else if(ClearLayout==false)
		putpixel(G_PTR,x+a*1,y+b*i,DESKTOP_BACKGROUND ,Draw_Normal );	

	if (fonts8x8[index][i] & 0x20)
		putpixel(G_PTR,x+a*2,y+b*i,Color ,Draw_Normal );	
	else if(ClearLayout==false)
		putpixel(G_PTR,x+a*2,y+b*i,DESKTOP_BACKGROUND ,Draw_Normal );	

	if (fonts8x8[index][i] & 0x10)
		putpixel(G_PTR,x+a*3,y+b*i,Color ,Draw_Normal );	
	else if(ClearLayout==false)
		putpixel(G_PTR,x+a*3,y+b*i,DESKTOP_BACKGROUND ,Draw_Normal );	

	if (fonts8x8[index][i] & 0x08)
		putpixel(G_PTR,x+a*4,y+b*i,Color ,Draw_Normal );	
	else if(ClearLayout==false)
		putpixel(G_PTR,x+a*4,y+b*i,DESKTOP_BACKGROUND ,Draw_Normal );	

	if (fonts8x8[index][i] & 0x04)
		putpixel(G_PTR,x+a*5,y+b*i,Color ,Draw_Normal );	
	else if(ClearLayout==false)
		putpixel(G_PTR,x+a*5,y+b*i,DESKTOP_BACKGROUND ,Draw_Normal );	

	if (fonts8x8[index][i] & 0x02)
		putpixel(G_PTR,x+a*6,y+b*i,Color ,Draw_Normal );	
	else if(ClearLayout==false)
		putpixel(G_PTR,x+a*6,y+b*i,DESKTOP_BACKGROUND ,Draw_Normal );

	if (fonts8x8[index][i] & 0x01)
		putpixel(G_PTR,x+a*7,y+b*i,Color ,Draw_Normal );
	else if(ClearLayout==false)
		putpixel(G_PTR,x+a*7,y+b*i,DESKTOP_BACKGROUND ,Draw_Normal );
   }
   
 }

void drawstring(GraphicArea G_PTR ,dword x,dword y,char *title,ColorRGB Color)
 {
   word i;
   for(i = 0 ; i < strlen(title) ; i++)
	   drawChar(G_PTR ,x+i*(CHARCOLS+SPACE_BETWEN_CHAR) ,y ,title[i]-32 ,Color ,true);
 }

//void outtextxy(dword x,dword y,char *title,color c)
// {
//   dword i = 0, j = 0, k = 0;
//   x %= SVGA1->Xresolution;
//   y %= SVGA1->Yresolution;
//
//   for(i = 0 ; i < strlen(title) ; i++)
//   {
//     for(j = 0 ; j < CHARROWS ; j++)
//     {
//       for(k = 0 ; k < CHARCOLS ; k++)
//       {
//         if(charset[title[i]-32][j][k]==1)
//          putpixel(G_PTR,x+i*7+k,y+j,c);
//       }
//     }
//   }
// }
//
static int KernelDrawImage(GraphicArea G_PTR ,image *drawImage ,DrawMode mode ,int xCoord ,int yCoord,unsigned int xOffset ,unsigned int yOffset ,int width ,int height)
{
  // Draws the requested width and height of the supplied image on the screen
  // at the requested coordinates, with the requested offset

  int status = 0;
  unsigned lineLength = 0;
  unsigned lineBytes = 0;
  int numberLines = 0;
  unsigned char *framebufferPointer = NULL;
  pixel *imageData = NULL;
  int lineCounter = 0;
  unsigned pixelCounter = 0;
  short pix = 0;
  unsigned count;

  // whole screen
  if (SVGA1 == NULL)
  {
	 // kprintf("VGA Not Init...");
	  return -1;
  }

  // If the image is outside the buffer entirely, skip it
  if ((xCoord >= (signed int)G_PTR.width) || (yCoord >= (signed int)G_PTR.hight))
		return (status = -1/*ERR_BOUNDS*/);

  // Make sure it's a color image
  if (drawImage->type == IMAGETYPE_MONO)
    return (status = -1/*ERR_INVALID*/);

  if (width)
    lineLength = width;
  else
    lineLength = drawImage->width;

  // If the image goes off the sides of the screen, only attempt to display
  // the pixels that will fit
  if (xCoord < 0)
    {
      lineLength += xCoord;
      xOffset += -xCoord;
      xCoord = 0;
    }
  if ((int)(xCoord + lineLength) >= (signed int)G_PTR.width)
     lineLength -= ((xCoord + lineLength) - G_PTR.width);
  if ((unsigned int)(xOffset + lineLength) >= drawImage->width)
     lineLength -= ((xOffset + lineLength) - drawImage->width);

  if (height)
     numberLines = height;
  else
     numberLines = drawImage->height;

  // If the image goes off the top or bottom of the screen, only show the
  // lines that will fit
  if (yCoord < 0)
    {
      numberLines += yCoord;
      yOffset += -yCoord;
      yCoord = 0;
    }
  if ((yCoord + numberLines) >= (signed int)G_PTR.hight)
     numberLines -= ((yCoord + numberLines) - G_PTR.hight);
  if ((unsigned)(yOffset + numberLines) >= drawImage->height)
     numberLines -= ((yOffset + numberLines) - drawImage->height);

  // Images are lovely little data structures that give us image data in the
  // most convenient form we can imagine.

  // How many bytes in a line of data?
  lineBytes = (G_PTR.BytePerPixel * lineLength);

  framebufferPointer = (unsigned char *)G_PTR.Buffer + (((G_PTR.width * yCoord) + xCoord) * G_PTR.BytePerPixel );

  imageData = (pixel *)((unsigned int)drawImage->data + (((yOffset * drawImage->width) + xOffset) * 3));
  
  // Loop for each line

  for (lineCounter = 0; lineCounter < numberLines; lineCounter++)
    {	  
      // Do a loop through the line, copying the color values from the
      // image into the framebuffer      
      if ((G_PTR.BitsPerPixel  == 32) || (G_PTR.BitsPerPixel == 24))
		  for (count = 0; count < lineBytes; )
		  {
			if ((mode == Draw_Translucent) &&(imageData[pixelCounter].red == drawImage->translucentColor.red) && (imageData[pixelCounter].green == drawImage->translucentColor.green) && (imageData[pixelCounter].blue == drawImage->translucentColor.blue))
			  count += G_PTR.BytePerPixel;
		    else
		    {
			  framebufferPointer[count++] = imageData[pixelCounter].blue;
			  framebufferPointer[count++] = imageData[pixelCounter].green;
			  framebufferPointer[count++] = imageData[pixelCounter].red;
			  if (G_PTR.BitsPerPixel == 32)
				count++;
			}
			pixelCounter += 1;
		  }
      else if ((G_PTR.BitsPerPixel == 16) || (G_PTR.BitsPerPixel == 15))
	  {
	   for (count = 0; count < lineLength; count ++)
	   {
	    if (G_PTR.BitsPerPixel == 16)
	 	  pix = (((imageData[pixelCounter].red >> 3) << 11) | ((imageData[pixelCounter].green >> 2) << 5) | (imageData[pixelCounter].blue >> 3));
	    else
		  pix = (((imageData[pixelCounter].red >> 3) << 10) | ((imageData[pixelCounter].green >> 3) << 5) | (imageData[pixelCounter].blue >> 3));

	    if ((mode != Draw_Translucent ) || ((imageData[pixelCounter].red != drawImage->translucentColor.red) || (imageData[pixelCounter].green != drawImage->translucentColor.green) || (imageData[pixelCounter].blue != drawImage->translucentColor.blue)))
		  ((short *) framebufferPointer)[count] = pix;
	      
	    pixelCounter += 1;
	   }
	  }

      // Move to the next line in the framebuffer
      framebufferPointer += (G_PTR.width * G_PTR.BytePerPixel);
      
      // Are we skipping any of this line because it's off the screen?
      if (drawImage->width > lineLength)
		pixelCounter += (drawImage->width - lineLength);
    }

  // Success
  return (status = 0);
}


static int DrawRepeatImage(GraphicArea G_PTR ,image *drawImage ,DrawMode mode ,int xCoord ,int yCoord,int xOffset ,int yOffset ,int width ,int height)
{	
	unsigned short X_Repeat,Y_Repeat,Rep;
	Point SrcPoint=POINT(xCoord,yCoord);	
	Point DestPoint=POINT(xCoord,yCoord);	
	Size imgSize;//=SIZE(drawImage->width,drawImage->height);

	if(drawImage->width==0 || drawImage->height==0)
		return -1;//Zero width or height
	if( (width+xCoord)>(signed int)G_PTR.width )
		width=G_PTR.width-xCoord;//Max availabe width
	if( (height+yCoord)>(signed int)G_PTR.hight )
		height=G_PTR.hight-yCoord;//Max availabe height
	

	X_Repeat=(unsigned short)ceill(width/(drawImage->width));
	Y_Repeat=(unsigned short)ceill(height/(drawImage->height));
	//First Draw From Image And Copy This Image To Other Part	
	KernelDrawImage(G_PTR ,drawImage ,mode ,xCoord ,yCoord ,xOffset ,yOffset ,drawImage->width ,drawImage->height);		
	//set imge size to repeat
	unsigned int WidthAdd=(((signed int)(drawImage->width-xOffset))>0 ? drawImage->width-xOffset : drawImage->width);
	unsigned int HeightAdd=(((signed int)(drawImage->height-yOffset))>0 ? drawImage->height-yOffset : drawImage->height);	
	imgSize.Width=WidthAdd;		
	imgSize.Hight=HeightAdd;	
	DestPoint.X+=imgSize.Width;
	//gotoxy(10,20);
	for(Rep=2;Rep<X_Repeat;Rep+=Rep){
		//printf("DestPoint(%d,%d) imgSize(%d,%d)",DestPoint.X,DestPoint.Y ,imgSize.Width ,imgSize.Hight );getch();
		WindowGraphicMemoryCopy(G_PTR ,DestPoint ,G_PTR ,SrcPoint ,imgSize);
		DestPoint.X+=imgSize.Width;
		imgSize.Width+=WidthAdd;
	}	
	//printf("Finish");getch();
	if((signed int)imgSize.Width<width){		
		imgSize.Width=width-DestPoint.X;
		WindowGraphicMemoryCopy(G_PTR ,DestPoint ,G_PTR ,SrcPoint ,imgSize);
	}
	imgSize.Width=width;
	DestPoint=POINT(xCoord,yCoord+HeightAdd);

	for(Rep=2;Rep<Y_Repeat;Rep+=Rep){						
		//printf("DestPoint(%d,%d) imgSize(%d,%d)",DestPoint.X,DestPoint.Y ,imgSize.Width ,imgSize.Hight );getch();
		WindowGraphicMemoryCopy(G_PTR ,DestPoint ,G_PTR ,SrcPoint ,imgSize);
		DestPoint.Y+=imgSize.Hight;
		imgSize.Hight+=HeightAdd;		
	}
	//printf("Finish");getch();
	if((signed int)imgSize.Hight<height){			
		imgSize.Hight=height-DestPoint.Y;
		WindowGraphicMemoryCopy(G_PTR ,DestPoint ,G_PTR ,SrcPoint ,imgSize);
	}
	return 0;
}

int DrawImage(GraphicArea G_PTR ,image *drawImage ,DrawMode mode ,int xCoord ,int yCoord,int xOffset ,int yOffset ,int width ,int height,ImageMode ImageMode)
{
	switch(ImageMode){
		case ImageMode_Normal :
			return KernelDrawImage(G_PTR ,drawImage ,mode ,xCoord ,yCoord ,xOffset ,yOffset ,width ,height);
			break;
		case ImageMode_RepeatImage :
			return DrawRepeatImage(G_PTR ,drawImage ,mode ,xCoord ,yCoord,xOffset ,yOffset ,width ,height);
			break;
		case ImageMode_AutoSize :
			//return DrawRepeatImage(G_PTR ,drawImage ,mode ,xCoord ,yCoord,xOffset ,yOffset ,width ,height);
			break;
		case ImageMode_CenterImage :
			//return DrawRepeatImage(G_PTR ,drawImage ,mode ,xCoord ,yCoord,xOffset ,yOffset ,width ,height);
			break;
		case ImageMode_Zoom :
			//return DrawRepeatImage(G_PTR ,drawImage ,mode ,xCoord ,yCoord,xOffset ,yOffset ,width ,height);
			break;		
		case ImageMode_StretchImage :
			//return DrawRepeatImage(G_PTR ,drawImage ,mode ,xCoord ,yCoord,xOffset ,yOffset ,width ,height);
			break;			
	}
	return -1;
}

void GetImage(GraphicArea G_PTR ,Point OffsetBuffer,image *drawImage ,DrawMode mode ,Point ImagePoint ,Size ImageSize)
{
	WindowGraphicMemoryCopy(G_PTR ,OffsetBuffer ,MainGraphicArea ,ImagePoint ,ImageSize );
}

