//
// Written by: Robert C. Pendleton
// 
// Copyright 1993 by Robert C. Pendleton
//
// Non-commercial use by individuals
// is permitted.
//

#ifndef __VBE_H
#define __VBE_H

#include <ctype.h>

//
// VBE interface routines. It supports version 1.2 of 
// the VESA BIOS Extensions.
// 

//---------------------------------------------------
//
//
//

typedef struct
{
    uint16 red;
    uint16 green;
    uint16 blue;
} vgaColor;


//---------------------------------------------------
//
// VBE modes.
//
// Note that ALL standard VGA video modes can be used
// with the vbe functions.
//

typedef enum
{
    g320x200c256    = 0x013,
    g640x400c256    = 0x100,
    g640x480c256    = 0x101,
    g800x600c16     = 0x102,
    g800x600c256    = 0x103,
    g1024x768c16    = 0x104,
    g1024x768c256   = 0x105,
    g1280x1024c16   = 0x106,
    g1280x1024c256  = 0x107,
    t80x60          = 0x108,
    t132x25         = 0x109,
    t132x43         = 0x10a,
    t132x50         = 0x10b,
    t132x60         = 0x10c,
    g320x200c32k    = 0x10d,
    g320x200c64k    = 0x10e,
    g320x200c16m    = 0x10f,
    g640x480c32k    = 0x110,
    g640x480c64k    = 0x111,
    g640x480c16m    = 0x112,
    g800x600c32k    = 0x113,
    g800x600c64k    = 0x114,
    g800x600c16m    = 0x115,
    g1024x768c32k   = 0x116,
    g1024x768c64k   = 0x117,
    g1024x768c16m   = 0x118,
    g1280x1024c32k  = 0x119,
    g1280x1024c64k  = 0x11a,
    g1280x1024c16m  = 0x11b,
} vbeMode;

//---------------------------------------------------
//
// vbe SVGA information record
//

typedef struct
{
    char vesa[4];
    uint8 minorMode;
    uint8 majorMode;
    char *vendorName;
    uint32 capabilities;
    uint16 *modes;
    uint16 memory;
    char reserved_236[236];
} vbeInfo;

//---------------------------------------------------
//
// vbe SVGA mode information record
//

typedef struct
{
    uint16 modeAttr;
    uint8 bankAAttr;
    uint8 bankBAttr;
    uint16 bankGranularity;
    uint16 bankSize;
    uint16 bankASegment;
    uint16 bankBSegment;
    uint32 posFuncPtr;
    uint16 bytesPerScanLine;
    uint16 width;
    uint16 height;
    uint8 charWidth;
    uint8 charHeight;
    uint8 numberOfPlanes;
    uint8 bitsPerPixel;
    uint8 numberOfBanks;
    uint8 memoryModel;
    uint8 videoBankSize;
    uint8 imagePages;

    uint8 reserved_1;

    uint8 redMaskSize;
    uint8 redFieldPos;
    uint8 greenMaskSize;
    uint8 greenFieldPos;
    uint8 blueMaskSize;
    uint8 blueFieldPos;
    uint8 rsvdMaskSize;
    uint8 rsvdFieldPos;
    uint8 DirectColorInfo;

    uint8 reserved_216[216];

} vbeModeInfo;

//---------------------------------------------------
//
// function prototypes
//

extern void vgaSetColor(int16 index, uint16 r, uint16 g, uint16 b);

extern void vgaSetPalette(int16 start, int16 count, vgaColor *p);

extern int vbeGetInfo(vbeInfo *infoPtr);

extern void vbePrintInfo(char *buffer, vbeInfo *info);

extern int vbeGetModeInfo(int16 mode, vbeModeInfo *infoPtr);

extern void vbePrintModeInfo(char *buffer, vbeModeInfo *info);

extern int vbeSetMode(int16 mode);

extern int vbeGetMode(int16 *mode);

extern int vbeGetStateSize(uint8 flags, int32 *size);

extern int vbeGetState(uint8 flags, uint8 *buffer);

extern int vbeSetState(uint8 flags, uint8 *buffer);

extern int vbeSetBankAAddr(uint16 addr);

extern int vbeSetBankBAddr(uint16 addr);

extern int vbeGetBankAAddr(uint16 *addr);

extern int vbeGetBankBAddr(uint16 *addr);

extern int vbeSetScanLineLength(uint16 length);

extern int vbeGetScanLineLength(uint16 *bytesPerScanLine,
                                uint16 *pixelsPerScanLine,
                                uint16 *maxScanLines);

extern int vbeSetDisplayStart(uint16 pixel, uint16 line);

extern int vbeGetDisplayStart(uint16 *pixel, uint16 *line);

extern int vbeSetDACPaletteControl(uint16 paletteWidth);

extern int vbeGetDACPaletteControl(uint16 *paletteWidth);

#endif
