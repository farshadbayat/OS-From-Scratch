#ifndef __COLOR_H
#define	__COLOR_H

#ifdef __cplusplus
extern "C"
{
#endif

	typedef unsigned char ColorType;
	typedef struct colorTag{
	  ColorType blue;
	  ColorType green;
	  ColorType red;
	} ColorRGB;
	
	// Standard PC (text-mode) color values, and our real-color translations
	// of them
	#define COLOR_BLACK         ((ColorRGB) { 0, 0, 0 } )        // 0=Black
	#define COLOR_BLUE          ((ColorRGB) { 170, 0, 0 } )      // 1=Blue
	#define COLOR_GREEN         ((ColorRGB) { 0, 170, 0 } )      // 2=Green
	#define COLOR_CYAN          ((ColorRGB) { 170, 170, 0 } )    // 3=Cyan
	#define COLOR_RED           ((ColorRGB) { 0, 0, 170 } )      // 4=Red
	#define COLOR_MAGENTA       ((ColorRGB) { 170, 0, 170 } )    // 5=Magenta
	#define COLOR_BROWN         ((ColorRGB) { 0, 85, 170 } )     // 6=Brown
	#define COLOR_LIGHTGRAY     ((ColorRGB) { 170, 170, 170 } )  // 7=Light gray
	#define COLOR_DARKGRAY      ((ColorRGB) { 85, 85, 85 } )     // 8=Dark gray
	#define COLOR_LIGHTBLUE     ((ColorRGB) { 255, 85, 85 } )    // 9=Light blue
	#define COLOR_LIGHTGREEN    ((ColorRGB) { 85, 255, 85 } )    // 10=Light green
	#define COLOR_LIGHTCYAN     ((ColorRGB) { 255, 255, 85 } )   // 11=Light cyan
	#define COLOR_LIGHTRED      ((ColorRGB) { 85, 85, 255 } )    // 12=Light red
	#define COLOR_LIGHTMAGENTA  ((ColorRGB) { 255, 85, 255 } )   // 13=Light magenta
	#define COLOR_YELLOW        ((ColorRGB) { 85, 255, 255 } )   // 14=Yellow
	#define COLOR_WHITE         ((ColorRGB) { 255, 255, 255 } )  // 15=White	

	typedef struct SizeTag Size;
	struct SizeTag{
		unsigned int Width;
		unsigned int Hight;
	};
	typedef struct PointTag Point;

	struct PointTag{
		unsigned int X,Y;
	};
	/////////////////////////////////////////////////////////////////////////////	
	ColorRGB ColorRGBNOToColorRGB(unsigned char Blue ,unsigned char Green ,unsigned char Red);
	ColorRGB ColorNOToColorRGB(unsigned int ColorNO);
	unsigned int ColorRGBToColorNO(ColorRGB Color_RGB);

	
#ifdef __cplusplus
}
#endif

#endif

