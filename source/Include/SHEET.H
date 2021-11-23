#ifndef __SHEET_H
#define	__SHEET_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <MMU.h>

#define MAX_SHEETS    256

	typedef struct _sheet {
		unsigned char *buf;
		int bxsize, bysize, vx0, vy0, col_inv, height, flags;
		struct _shtctl *ctl;
	} SHEET, *PSHEET;

	typedef struct _shtctl {
		unsigned char *vram, *map;   // VRAM
		int xsize, ysize, top; 
		PSHEET sheets[MAX_SHEETS];
		SHEET sheets0[MAX_SHEETS];
	} SHTCTL, *PSHTCTL;

	extern PSHTCTL shtctl_init(PMEMMAN memman, unsigned char *vram, int xsize, int ysize);
	extern PSHEET sheet_alloc(PSHTCTL ctl);

	extern void sheet_setbuf(PSHEET sht, unsigned char *buf, int xsize, int ysize, int col_inv);
	extern void sheet_updown(PSHEET sht, int height);
	extern void sheet_refresh(PSHEET sht, int bx0, int by0, int bx1, int by1);
	extern void sheet_slide(PSHEET sht, int vx0, int vy0);
	extern void sheet_free(PSHEET sht);


#ifdef __cplusplus
}
#endif

#endif


