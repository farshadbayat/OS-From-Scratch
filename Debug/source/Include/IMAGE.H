#ifndef __IMAGE_H
#define	__IMAGE_H

#ifdef __cplusplus
extern "C"
{
#endif
	#include <graphics.h>
	#include <color.h>

	// Definitions
	#define IMAGETYPE_MONO   1
	#define IMAGETYPE_COLOR  2
	
	#define IMAGEFORMAT_BMP  1
	#define IMAGEFORMAT_JPG  2

	#define MAXVIDEOMODES    20	
	
	// Structures for manipulating generic images.
	
	typedef ColorRGB pixel;
	
	typedef struct imageTag{
	  int type;
	  ColorRGB translucentColor;
	  unsigned pixels;
	  unsigned width;
	  unsigned height;
	  unsigned dataLength;
	  void *data;

	} image;
	
	// A data structure to describe a graphics mode
	typedef struct {
	  int mode;
	  int xRes;
	  int yRes;
	  int bitsPerPixel;
	
	} videoMode;
	
#ifdef __cplusplus
}
#endif

#endif

