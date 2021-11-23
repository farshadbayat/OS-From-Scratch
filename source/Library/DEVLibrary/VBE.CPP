/*#include <dos.h>
#include <conio.h>
#include <stdio.h>*/
#include <vbe.h>
#include <system.h>

#define FORCE_CTYPE
#include "ctype.h"
#undef FORCE_CTYPE

//---------------------------------------------------
//
// Color palette addresses
//

#define PAL_WRITE_ADDR (0x3c8)      // palette write address
#define PAL_READ_ADDR  (0x3c7)      // palette write address
#define PAL_DATA       (0x3c9)      // palette data register

//---------------------------------------------------
//
// Set one entry in the color palatte
//

void vgaSetColor(int16 index, uint16 r, uint16 g, uint16 b)
{
    if (index < 0 || index > 255)
    {
        return;
    }

    while(!(inp16(0x3da) & 0x01));    // wait for blanking

    outp16(PAL_WRITE_ADDR, index);
    outp16(PAL_DATA, r);
    outp16(PAL_DATA, g);
    outp16(PAL_DATA, b);
}

//---------------------------------------------------
//
// Set a range of entries in the color table
//

void vgaSetPalette(int16 start, int16 count, vgaColor *p)
{
    int i;

    if (start < 0 || (start + count - 1) > 255)
    {
        return;
    }

    while(!(inp16(0x3da) & 0x08));    // wait vertical retrace

    outp16(PAL_WRITE_ADDR, start);
    for (i = 0; i < count; i++)
    {
        outp16(PAL_DATA, p->red);
        outp16(PAL_DATA, p->green);
        outp16(PAL_DATA, p->blue);
        p++;
    }
}

////---------------------------------------------------
////
//// Use the VBE to get information about the video card
////
//
//int
//vbeGetInfo(vbeInfo *infoPtr)
//{
//    union REGS reg;
//    struct SREGS sreg;
//
//    reg.x.ax = 0x4f00;
//
//    sreg.es = FP_SEG(infoPtr);
//    reg.x.di = FP_OFF(infoPtr);
//
//    int86x(0x10, &reg, &reg, &sreg);
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Print the info you get using vbeGetInfo
////
//
//void
//vbePrintInfo(FILE *file, vbeInfo *info)
//{
//    uint16 *mode;
//
//    fprintf(file, "name=%c%c%c%c\n", info->vesa[0],
//            info->vesa[1],
//            info->vesa[2],
//            info->vesa[3]);
//
//    fprintf(file, "Version %d.%d\n", info->majorMode, info->minorMode);
//    fprintf(file, "Vendor %s\n", info->vendorName);
//    fprintf(file, "Capabilities %lX\n", info->capabilities);
//    fprintf(file, "Memory %dk\n", info->memory * 64);
//
//    mode = info->modes;
//    while ((*mode) != 0xffff)
//    {
//        fprintf(file, "mode=%x\n", *mode);
//        mode++;
//    }
//    fflush(file);
//}
//
////---------------------------------------------------
////
//// Get details about a specific SVGA video mode
////
//
//int
//vbeGetModeInfo(int16 mode, vbeModeInfo *infoPtr)
//{
//    union REGS reg;
//    struct SREGS sreg;
//
//    reg.x.ax = 0x4f01;
//    reg.x.cx = mode;
//
//    sreg.es = FP_SEG(infoPtr);
//    reg.x.di = FP_OFF(infoPtr);
//
//    int86x(0x10, &reg, &reg, &sreg);
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Print the info we get from vbeGetModeInfo
////
//
//void
//vbePrintModeInfo(FILE *file, vbeModeInfo *info)
//{
//    char *memoryModels[] = 
//    {
//        "Text mode",
//        "CGA graphics",
//        "Hercules graphics",
//        "4-plane planar",
//        "Packed pixel",
//        "Non-chain 4, 256 color",
//        "Direct Color",
//        "YUV",
//    };
//
//    // mode attributes
//
//    if (info->modeAttr & (1 << 0))
//    {
//        fprintf(file, "mode supported in hardware\n");
//    }
//    else
//    {
//        fprintf(file, "mode not supported in hardware\n");
//    }
//
//    if (info->modeAttr & (1 << 1))
//    {
//        fprintf(file, "optional info available\n");
//    }
//    else
//    {
//        fprintf(file, "no optional info\n");
//    }
//
//    if (info->modeAttr & (1 << 2))
//    {
//        fprintf(file, "BIOS output\n");
//    }
//    else
//    {
//        fprintf(file, "no BIOS output\n");
//    }
//
//    if (info->modeAttr & (1 << 3))
//    {
//        fprintf(file, "color mode\n");
//    }
//    else
//    {
//        fprintf(file, "monochrome mode\n");
//    }
//
//    if (info->modeAttr & (1 << 4))
//    {
//        fprintf(file, "graphics mode\n");
//    }
//    else
//    {
//        fprintf(file, "text mode\n");
//    }
//
//    // bank A attributes
//
//    if (info->bankAAttr & (1 << 0))
//    {
//        fprintf(file, "bank A exists\n");
//    }
//    else
//    {
//        fprintf(file, "bank A does not exist\n");
//    }
//
//    if (info->bankAAttr & (1 << 1))
//    {
//        fprintf(file, "bank A readable\n");
//    }
//    else
//    {
//        fprintf(file, "bank A not readable\n");
//    }
//
//    if (info->bankAAttr & (1 << 2))
//    {
//        fprintf(file, "bank A writable\n");
//    }
//    else
//    {
//        fprintf(file, "bank A not writable\n");
//    }
//
//    // bank B attributes
//
//    if (info->bankBAttr & (1 << 0))
//    {
//        fprintf(file, "bank B exists\n");
//    }
//    else
//    {
//        fprintf(file, "bank B does not exist\n");
//    }
//
//    if (info->bankBAttr & (1 << 1))
//    {
//        fprintf(file, "bank B readable\n");
//    }
//    else
//    {
//        fprintf(file, "bank B not readable\n");
//    }
//
//    if (info->bankBAttr & (1 << 2))
//    {
//        fprintf(file, "bank B writable\n");
//    }
//    else
//    {
//        fprintf(file, "bank B not writable\n");
//    }
//
//    // bank attributes
//
//    fprintf(file, "bank granularity=%d\n", info->bankGranularity);
//    fprintf(file, "bank size=%d\n", info->bankSize);
//    fprintf(file, "bank A start seg=%x\n", info->bankASegment);
//    fprintf(file, "bank B start seg=%x\n", info->bankBSegment);
//
//    fprintf(file, "positioning function=%p\n", info->posFuncPtr);
//
//    fprintf(file, "bytes/scanline=%d\n", info->bytesPerScanLine);
//
//
//    fprintf(file, "width =%d\n", info->width);
//    fprintf(file, "height=%d\n", info->height);
//
//    fprintf(file, "char width =%d\n", (int) (info->charWidth));
//    fprintf(file, "char height=%d\n", (int) (info->charHeight));
//
//    fprintf(file, "number of planes=%d\n", (int) (info->numberOfPlanes));
//    fprintf(file, "bits/pixel=%d\n", (int) (info->bitsPerPixel));
//    fprintf(file, "number of banks=%d\n", (int) (info->numberOfBanks));
//
//    if (0x00 <= (int)info->memoryModel && (int)info->memoryModel <= 0x07)
//    {
//        fprintf(file, "memory model=%d, %s\n",
//                (int) (info->memoryModel), memoryModels[info->memoryModel]);
//    }
//    else if (0x08 <= (int)info->memoryModel && (int)info->memoryModel <= 0x0f)
//    {
//        fprintf(file, "memory model=%d, Reserved by VESA\n", 
//                (int) (info->memoryModel));
//    }
//    else if (0x10 <= (int)info->memoryModel && (int)info->memoryModel <= 0xff)
//    {
//        fprintf(file, "memory model=%d, OEM custom mode\n",
//                (int) (info->memoryModel));
//    }
//
//    fprintf(file, "video bank size=%d\n", (int) (info->videoBankSize));
//    fprintf(file, "image pages=%d\n", (int) (info->imagePages));
//
//    fprintf(file, "Red Mask Size=%d\n", (int) (info->redMaskSize));
//    fprintf(file, "Green Mask Size=%d\n", (int) (info->greenMaskSize));
//    fprintf(file, "Blue Mask Size=%d\n", (int) (info->blueMaskSize));
//    fprintf(file, "Rsvd Mask Size=%d\n", (int) (info->rsvdMaskSize));
//
//    fprintf(file, "Red Field Pos=%d\n", (int) (info->redFieldPos));
//    fprintf(file, "Green Field Pos=%d\n", (int) (info->greenFieldPos));
//    fprintf(file, "Blue Field Pos=%d\n", (int) (info->blueFieldPos));
//    fprintf(file, "Rsvd Field Pos=%d\n", (int) (info->rsvdFieldPos));
//
//    if (info->DirectColorInfo & (1 << 0))
//    {
//        fprintf(file, "color ramp is fixed\n");
//    }
//    else
//    {
//        fprintf(file, "color ramp is programmable\n");
//    }
//
//    if (info->DirectColorInfo & (1 << 1))
//    {
//        fprintf(file, "Bits in Rsvd field are reserved\n");
//    }
//    else
//    {
//        fprintf(file, "Bits in Rsvd field are usable\n");
//    }
//
//    fflush(file);
//}
//
////---------------------------------------------------
////
//// Set the video mode. This works with normal vga modes
//// and with vbe modes.
////
//
//int
//vbeSetMode(int16 mode)
//{
//    union REGS reg;
//    struct SREGS sreg;
//
//    reg.x.ax = 0x4f02;
//    reg.x.bx = mode;
//
//    int86x(0x10, &reg, &reg, &sreg);
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Return the current video mode. Works with vga and
//// vbe modes.
////
//
//int
//vbeGetMode(int16 *mode)
//{
//    union REGS reg;
//    struct SREGS sreg;
//
//    reg.x.ax = 0x4f03;
//
//    int86x(0x10, &reg, &reg, &sreg);
//
//    *mode = reg.x.bx;
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Find out how much space we need to store the
//// entire state of the video card and BIOS
////
//
//int
//vbeGetStateSize(uint8 flags, int32 *size)
//{
//    union REGS reg;
//    struct SREGS sreg;
//
//    reg.x.ax = 0x4f04;
//    reg.h.dl = 0x00;
//    reg.x.cx = flags;
//
//    int86x(0x10, &reg, &reg, &sreg);
//
//    *size = ((int32) reg.x.bx) * 64;
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Retrieve the entire state of the video card and
//// BIOS.
////
//
//int
//vbeGetState(uint8 flags, uint8 *buffer)
//{
//    union REGS reg;
//    struct SREGS sreg;
//
//    reg.x.ax = 0x4f04;
//    reg.h.dl = 0x01;
//    reg.x.cx = flags;
//
//    sreg.es = FP_SEG(buffer);
//    reg.x.bx = FP_OFF(buffer);
//
//    int86x(0x10, &reg, &reg, &sreg);
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Restore a saved state
////
//
//int
//vbeSetState(uint8 flags, uint8 *buffer)
//{
//    union REGS reg;
//    struct SREGS sreg;
//
//    reg.x.ax = 0x4f04;
//    reg.h.dl = 0x02;
//    reg.x.cx = flags;
//
//    sreg.es = FP_SEG(buffer);
//    reg.x.bx = FP_OFF(buffer);
//
//    int86x(0x10, &reg, &reg, &sreg);
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Position bank A in video memory
////
//
//int
//vbeSetBankAAddr(uint16 addr)
//{
//    union REGS reg;
//
//    reg.x.ax = 0x4f05;
//    reg.h.bh = 0x00;
//    reg.h.bl = 0x00;
//    reg.x.dx = addr;
//
//    int86(0x10, &reg, &reg);
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Position bank B in video memory
////
//
//int
//vbeSetBankBAddr(uint16 addr)
//{
//    union REGS reg;
//
//    reg.x.ax = 0x4f05;
//    reg.h.bh = 0x00;
//    reg.h.bl = 0x01;
//    reg.x.dx = addr;
//
//    int86(0x10, &reg, &reg);
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Get the current position of bank A in video memory
////
//
//int
//vbeGetBankAAddr(uint16 *addr)
//{
//    union REGS reg;
//
//    reg.x.ax = 0x4f05;
//    reg.h.bh = 0x01;
//    reg.h.bl = 0x00;
//
//    int86(0x10, &reg, &reg);
//
//    *addr = reg.x.dx;
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Get the current position of bank B in video memory
////
//
//int
//vbeGetBankBAddr(uint16 *addr)
//{
//    union REGS reg;
//
//    reg.x.ax = 0x4f05;
//    reg.h.bh = 0x01;
//    reg.h.bl = 0x01;
//
//    int86(0x10, &reg, &reg);
//
//    *addr = reg.x.dx;
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Set the logical scan line length
////
//
//int
//vbeSetScanLineLength(uint16 length)
//{
//    union REGS reg;
//
//    reg.x.ax = 0x4f06;
//    reg.h.bl = 0x00;
//    reg.x.cx = length;
//
//    int86(0x10, &reg, &reg);
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Get the logical scan line length
////
//
//int
//vbeGetScanLineLength(uint16 *bytesPerScanLine,
//                     uint16 *pixelsPerScanLine,
//                     uint16 *maxScanLines)
//{
//    union REGS reg;
//
//    reg.x.ax = 0x4f06;
//    reg.h.bl = 0x01;
//
//    int86(0x10, &reg, &reg);
//
//    *bytesPerScanLine = reg.x.bx;
//    *pixelsPerScanLine = reg.x.cx;
//    *maxScanLines = reg.x.dx;
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Used for panning, scrolling and page flipping
////
//
//int
//vbeSetDisplayStart(uint16 pixel, uint16 line)
//{
//    union REGS reg;
//
//    reg.x.ax = 0x4f07;
//    reg.h.bh = 0x00;
//    reg.h.bl = 0x00;
//
//    reg.x.cx = pixel;
//    reg.x.dx = line;
//
//    int86(0x10, &reg, &reg);
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Find out where the current start of the display is.
////
//
//int
//vbeGetDisplayStart(uint16 *pixel, uint16 *line)
//{
//    union REGS reg;
//
//    reg.x.ax = 0x4f07;
//    reg.h.bh = 0x00;
//    reg.h.bl = 0x01;
//
//    int86(0x10, &reg, &reg);
//
//    *pixel = reg.x.cx;
//    *line = reg.x.dx;
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Set the desired number of bits of color in the
//// palette. Normal VGA is 6. Many SVGAs support 8.
////
//
//int
//vbeSetDACPaletteControl(uint16 paletteWidth)
//{
//    union REGS reg;
//
//    reg.x.ax = 0x4f08;
//    reg.h.bl = 0x00;
//
//    reg.h.bh = paletteWidth;
//
//    int86(0x10, &reg, &reg);
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
////
//// Get the current width of colors
////
//
//int
//vbeGetDACPaletteControl(uint16 *paletteWidth)
//{
//    union REGS reg;
//
//    reg.x.ax = 0x4f08;
//    reg.h.bl = 0x01;
//
//    int86(0x10, &reg, &reg);
//
//    *paletteWidth = reg.h.bh;
//
//    return (reg.x.ax == 0x004f);
//}
//
////---------------------------------------------------
