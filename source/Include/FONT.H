/***************************************************************************
                          font.h  -  description
                             -------------------
    begin                : Mon Feb 16 2004
    copyright            : (C) 2004 by Dynacube Team
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __FONT_H
#define __FONT_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <mmu.h>

  #define CHARSET_SIZE 256  
  #define CHARROWS 8
  #define CHARCOLS 8
  
	extern unsigned char fonts8x8[225][8];
  //Now the only type font supported in Dynacube
  //extern byte charset[128][CHARROWS][CHARCOLS];//extern byte *charset;//  
  extern void FontInit();
  //extern char MyFile[304062];

#ifdef __cplusplus
}
#endif

#endif

