#ifndef __ImageBmp_H
#define	__ImageBmp_H

#ifdef __cplusplus
extern "C"
{
#endif

	#include <system.h>
	#include <color.h>
	#include <image.h>
	#include <graphics.h>
	// Constants

	#define BMP_BPP_MONO  1
	#define BMP_BPP_16    4
	#define BMP_BPP_256   8
	#define BMP_BPP_16BIT 16
	#define BMP_BPP_24BIT 24
	#define BMP_BPP_32BIT 32
	
	#define BMP_COMP_NONE 0
	#define BMP_COMP_RLE8 1
	#define BMP_COMP_RLE4 2
	#define BMP_COMP_BITF 3
	
	// The .bmp header
	
	typedef struct bmpHeaderTag{
	  unsigned size;
	  unsigned reserved;
	  unsigned dataStart;
 	  unsigned headerSize;
  	  unsigned width;
  	  unsigned height;
  	  short planes;
  	  short bitsPerPixel;
  	  unsigned compression;
  	  unsigned dataSize;
  	  unsigned hResolution;
  	  unsigned vResolution;
  	  unsigned colors;
  	  unsigned importantColors;	  
	} bmpHeader;

	//////////////////////////////////////////File Type Pointer//////////////////////
	#define FILECLASS_NAME_EMPTY    "empty"
	#define FILECLASS_NAME_TEXT     "text"
	#define FILECLASS_NAME_BIN      "binary"
	#define FILECLASS_NAME_STATIC   "static"
	#define FILECLASS_NAME_DYNAMIC  "dynamic"
	#define FILECLASS_NAME_EXEC     "executable"
	#define FILECLASS_NAME_OBJ      "object"
	#define FILECLASS_NAME_LIB      "library"
	#define FILECLASS_NAME_CORE     "core"
	#define FILECLASS_NAME_IMAGE    "image"
	#define FILECLASS_NAME_DATA     "data"
	
	#define FILECLASS_NAME_ELF      "ELF"
	#define FILECLASS_NAME_BMP      "bitmap"
	#define FILECLASS_NAME_ICO      "icon"
	#define FILECLASS_NAME_JPG      "JPEG"
	#define FILECLASS_NAME_CONFIG   "configuration"
	#define FILECLASS_NAME_BOOT     "boot"
	#define LOADER_NUM_FILECLASSES  8
	// This is a structure for a file class.  It contains a standard name for
	// the file class and function pointers for managing that class of file.
	//typedef struct kernelFileClassTag{
	//  char *className;
	//  int (*detect)(const char *, void *, int/*, loaderFileClass **/);
	//  /*union {
	//    struct {
	//      loaderSymbolTable * (*getSymbols)(void *, int, int);
	//      int (*layoutLibrary)(void *, kernelDynamicLibrary *);
	//      int (*layoutExecutable)(void *, processImage *);
	//      int (*link)(int, void *, processImage *);
	//    } executable;*/
	//    struct {
	//      int (*load)(unsigned char *, int, int, int, image *);
	//      int (*save)(const char *, image *);
	//    } image;
	//  /*};*/
 // 
	//} kernelFileClass;
 int BMPLoad(unsigned char *imageFileData ,int dataLength ,image *loadImage) ;
//int driverDrawImage(image *drawImage ,DrawMode mode ,int xCoord ,int yCoord,int xOffset ,int yOffset ,int width ,int height);	

#ifdef __cplusplus
}
#endif

#endif

