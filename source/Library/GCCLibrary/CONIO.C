#include <conio.h>
#include <string.h>
#include <draw2d.h>
#include <keyboard.h>
#include <system.h>
#include <io.h>
#include <font.h>
#include <graphics.h>

/* These define our textpointer, our background and foreground
*  colors (_textattrutes), and x and y cursor coordinates */

/* INNER DATA */
unsigned int X_Resolution,Y_Resolution;
ColorRGB GBackColor; 

short _cursor_x, _cursor_y;
int _scrolling;
unsigned short *textmemptr;
char _textattr;
char _keybuf[_KEYBUF_SIZE];
int _kbhead, _kbtail, _kbcnt;
short _StartLine;
char _symmap[256] =
  {"\0\0331234567890-=\b\tQWERTYUIOP[]\r\0ASDFGHJKL;\'`\0\\ZXCVBNM,./\0*\0 "};
  // \0 - zero
  // \033 - esc (octal 33 = decimal 27 )
  // \b - backspace
  // \t - tab
  // \r - return - enter
  // \\ - "\"
ColorRGB CTColorToGColor(char TColor)//convert Text Color To Graphic Color
{
	switch(TColor)
		{
		case BLACK:	
			return COLOR_BLACK;
		case BLUE:	
			return COLOR_BLUE;
		case GREEN:	
			return COLOR_GREEN;
		case CYAN:	
			return COLOR_CYAN;
		case RED:	
			return COLOR_RED;
		case MAGENTA:	
			return COLOR_MAGENTA;
		case BROWN:	
			return COLOR_BROWN;
		case LIGHTGRAY:	
			return COLOR_LIGHTGRAY;
		case DARKGRAY:	
			return COLOR_DARKGRAY;
		case LIGHTBLUE:	
			return COLOR_LIGHTBLUE;
		case LIGHTGREEN:	
			return COLOR_LIGHTGREEN;
		case LIGHTCYAN:	
			return COLOR_LIGHTCYAN;
		case LIGHTRED:	
			return COLOR_LIGHTRED;
		case LIGHTMAGENTA:	
			return COLOR_LIGHTMAGENTA;
		case YELLOW:	
			return COLOR_YELLOW;
		case WHITE:	
			return COLOR_WHITE;
		default:
			return COLOR_WHITE;
		}
}

/* Sets our text-mode VGA pointer, then clears the screen for us */
void TextVideo_Install(void)
{	

	X_Resolution=80;Y_Resolution=25;
	_cursor_x=0; _cursor_y=0;
	_scrolling=1;
	_textattr=7;
	_kbhead=0;_kbtail=0; _kbcnt=0;
	_StartLine=0;
	GBackColor=COLOR_BLACK; 

	union REGS  inregs,outregs;
	inregs.h.ah=0x03;
	inregs.h.bh=0x00;
	int86(0x10,&inregs,&outregs);// get current cursor
	_cursor_x =outregs.h.dl;
	_cursor_y = outregs.h.dh;
    textmemptr = (unsigned short *)0xB8000;
	_textattr = 7;
	_update_cursor(/*_cursor_x,_cursor_y*/);
//    clrscr();
	kprintf("Text Video Driver Is Install\t\t\t\t[ OK ]");	   
}


/* Scrolls the screen */
void scroll(void)
{
    unsigned blank, temp;

    /* A blank is defined as a space... we need to give it
    *  backcolor too */
    blank = 0x20 | (_textattr << 8);

    /* Row 25 is the end, this means we need to scroll up */
    if((unsigned int)_cursor_y >= Y_Resolution && IsGraphic==false)
    {
        /* Move the current text chunk that makes up the screen
        *  back in the buffer by a line */
        temp = _cursor_y - Y_Resolution + 1;
        memcpy (textmemptr, textmemptr + temp * X_Resolution, (Y_Resolution - temp) * X_Resolution * 2);

        /* Finally, we set the chunk of memory that occupies
        *  the last line of text to our 'blank' character */
        memsetw (textmemptr + (Y_Resolution - temp) * X_Resolution, blank, X_Resolution);
        _cursor_y = Y_Resolution - 1;		
    }
	else if((unsigned int)((CHARROWS+SPACE_BETWEN_LINE)*_cursor_y) >= Y_Resolution && IsGraphic==true)
    {		
        temp = (_cursor_y*(CHARROWS+SPACE_BETWEN_LINE)) - Y_Resolution + (CHARROWS+SPACE_BETWEN_LINE);		        
		GraphicMoveClip(MainGraphicArea ,0 ,0 ,0 ,(CHARROWS+SPACE_BETWEN_LINE) ,MainGraphicArea.width ,MainGraphicArea.hight-(CHARROWS+SPACE_BETWEN_LINE) );
		//getch();
		GraphicClearClip(MainGraphicArea ,0 ,MainGraphicArea.hight-(CHARROWS+SPACE_BETWEN_LINE) ,MainGraphicArea.width ,(CHARROWS+2),CTColorToGColor(GetBackColor()));		
		_cursor_y =(Y_Resolution - temp)/(CHARROWS+SPACE_BETWEN_LINE);
    }
}
/// <summary>		
	/// Updates the hardware cursor: the little blinking line
	/// on the screen under the last character pressed! 
/// </summary>  
void move_csr(void)
{
    unsigned temp;
	if(IsGraphic==false){
    /* The equation for finding the index in a linear
    *  chunk of memory can be represented by:
    *  Index = [(y * width) + x] */
    temp = _cursor_y * X_Resolution + _cursor_x;

    /* This sends a command to indicies 14 and 15 in the
    *  CRT Control Register of the VGA controller. These
    *  are the high and low bytes of the index that show
    *  where the hardware cursor is to be 'blinking'. To
    *  learn more, you should look up some VGA specific
    *  programming documents. A great start to graphics:
    *  http://www.brackeen.com/home/vga */
    outportb(0x3D4, 14);
    outportb(0x3D5, temp >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, temp);
	} 
}


// Updates location of a hardware text cursor

void _update_cursor() {
  unsigned short offs;	
  if(IsGraphic==false)
  {
	offs = _cursor_y*X_Resolution+_cursor_x;
	outportb (0x3D4, 0x0F); outportb (0x3D5, offs & 0xFF);
	offs>>=8;
	outportb (0x3D4, 0x0E); outportb (0x3D5, offs & 0xFF);
  }
}

// Uses the above routine to output a string...  
void puts(char *text)
{
    int i;

    for (i = 0; i < strlen((const char *)text); i++)
    {
        putch(text[i]);
    }
}

// Sets the forecolor and backcolor that we will use 
void settextcolor(unsigned char forecolor, unsigned char backcolor)
{
    // Top 4 bytes are the background, bottom 4 bytes      are the foreground color 
    _textattr = (backcolor << 4) | (forecolor & 0x0F);
}

// Locates the cursor
void gotoxy (short x, short y) {
  _cursor_x = x;
  _cursor_y = y;
  _update_cursor();
}

// Reads cursor X
short wherex() {
		return _cursor_x;
}

//short wherex_Grphic() {
//	if(IsGraphic==true)
//		return _cursor_x*(CHARCOLS+SPACE_BETWEN_CHAR);
//	else
//		return _cursor_x;
//}

// Reads cursor Y
short wherey() {
		return _cursor_y;
}
//short wherey_Grphic() {
//	if(IsGraphic==true)
//		return _cursor_y*(CHARROWS+SPACE_BETWEN_LINE);
//	else
//		return _cursor_y;
//}

// Sets back color
void textbackground (char color) {
  _textattr = (_textattr & 0x0F) | (color << 4);
}

/// <summary>		
	/// Set foreground color
	/// Color Is 4bit
/// </summary>
void textcolor(char Color) {
  _textattr = (_textattr & 0xF0) | (Color & 0x0F);
}


/// <summary>		
	/// Get foreground color
	/// Return Is 4bit
/// </summary>
char GetTextColor (void) {
  return (_textattr & 0x0F);
}

/// <summary>		
	/// Get GetBackColor color
	/// Return Is 4bit
/// </summary>
char GetBackColor (void) {
  return (_textattr & 0xF0)>>4;
}

// Clears the screen with current color (_textattr)
void clrscr()
{
    unsigned int blank;
    unsigned int i;
    /* Again, we need the 'short' that will be used to  represent a space with color */
    blank = 0x20 | (_textattr << 8);
    /* Sets the entire screen to spaces in our current  color */
    for(i = 0; i < Y_Resolution; i++)
		if(IsGraphic==false)
			memsetw (textmemptr + i * X_Resolution, blank, X_Resolution);
		else
			memsetw ((unsigned short *)(MainGraphicArea.Buffer + i * X_Resolution), 0, X_Resolution);


    /* Update out virtual cursor, and then move the  hardware cursor */
    _cursor_x = 0;
    _cursor_y = 0;
    move_csr();
}



// Puts a single character on the screen 
void putch(unsigned char c)
{
    unsigned short *where;
    unsigned att = _textattr << 8;	
    /* Handle a backspace, by moving the cursor back one space */
    if(c == 0x08)//\b
    {
        if((_cursor_x-_StartLine) != 0) 
		{
			_cursor_x--;
			
			if(IsGraphic==true){						
				drawChar(MainGraphicArea ,(CHARROWS+SPACE_BETWEN_LINE)*_cursor_x,(dword)((CHARCOLS+SPACE_BETWEN_CHAR)*_cursor_y),225/* Index Character BS*/,DESKTOP_BACKGROUND ,false);
			}
			else
			{
				where = textmemptr + (_cursor_y * X_Resolution + _cursor_x);
				*where = ' ' | att;	/* Character AND _textattrutes: color */
			}
		}
    }
    /* Handles a tab by incrementing the cursor's x, but only
    *  to a point that will make it divisible by 8 */
    else if(c == 0x09)// \t
    {
        _cursor_x = (_cursor_x + 8) & ~(8 - 1);
    }
    /* Handles a 'Carriage Return', which simply brings the
    *  cursor back to the margin */
    else if(c == '\r')
    {	
        _cursor_x = _StartLine;//_cursor_x = 0;//Cursor Return To Start Of Line
    }
	/* We handle our newlines the way DOS and the BIOS do: we
    *  treat it as if a 'CR' was also there, so we bring the
    *  cursor to the margin and we increment the 'y' value */
    else if(c == '\n')
    {		
        _StartLine=_cursor_x = 0;
        _cursor_y++;
    }
    /* Any character greater than and including a space, is a
    *  printable character. The equation for finding the index
    *  in a linear chunk of memory can be represented by:
    *  Index = [(y * width) + x] */
    else if(c >= ' ')
    {
        where = textmemptr + (_cursor_y * X_Resolution + _cursor_x);		
		if(IsGraphic==true)
		{
			ColorRGB Color=CTColorToGColor(GetTextColor())/*{0xff,0xff,0xff}*/;			
			drawChar(MainGraphicArea ,(CHARROWS+SPACE_BETWEN_LINE)*_cursor_x ,(dword)((CHARCOLS+SPACE_BETWEN_CHAR)*_cursor_y) ,c-32,Color ,false);
		}
		else
			*where = c | att;	/* Character AND _textattrutes: color */
        _cursor_x++;		
    }

    /* If the cursor has reached the edge of the screen's width, we
    *  insert a new line in there */
    if((unsigned int)_cursor_x >= X_Resolution && IsGraphic==false)
    {
        _StartLine=_cursor_x = 0;
        _cursor_y++;
    }
	else if((CHARROWS+SPACE_BETWEN_LINE)*_cursor_x >= X_Resolution && IsGraphic==true)
	{
        _StartLine=_cursor_x = 0;
        _cursor_y++;
    }
    /* Scroll the screen if needed, and finally move the cursor */
    scroll();
    move_csr();
}


// Returns non-zero result in case of pressed key
int kbhit() {
  return _kbcnt;
}

// Returns scan code of the pressed key
char getscan() {
  char r;

  while (!_kbcnt) {};
  r = _keybuf[_kbhead++];
  if (_kbhead == _KEYBUF_SIZE) _kbhead = 0;
  _kbcnt--;
  return r;
}

// Returns ASCII code (in upper case) of the pressed key
char getch() 
{
	int scancode;
	while(1)
	{
	  scancode=(int)getscan();
	  if(scancode==sLShift || scancode==sRShift || scancode==sCtrl || scancode==sAlt);
	  else break;
	}
	//printf("l:%d\n",scancode);
  return s_scanTable[scancode];
}

// Cleans the keyboard buffer
void clear_keybuf() 
{
  bool reEnable = false;
  if (IsInterruptsEnabled()) 
	{		
		disable();
		reEnable = true;
    }

  _kbhead = _kbtail = _kbcnt=0;
  if (reEnable==true) enable();
}

char CharToUpper(char ch)
{
	if(ch>='a' && ch<='z')
		return ch+('A'-'a');
	return ch;
}

char CharToLower(char ch)
{
	if(ch>='A' && ch<='Z')
		return ch-('A'-'a');
	return ch;
}

void StrToLower(char *Str,char *StrToLower)
{
	int i;
	for(i=0;i<strlen(Str);i++)
		StrToLower[i]= CharToLower(Str[i]);
}

void StrToUpper(char *Str,char *StrToUpper)
{
	int i;
	for(i=0;i<strlen(Str);i++)
		StrToUpper[i]= CharToUpper(Str[i]);
}



