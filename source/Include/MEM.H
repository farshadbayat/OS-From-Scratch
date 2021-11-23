#ifndef __MEM_H
#define	__MEM_H



#ifdef __cplusplus
extern "C"
{
#endif


	#include <ctype.h>
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////
	// FUNCTIONS ACCESSING MEMORY //
	////////////////////////////////
	
	 void farmemset (dword sel, dword buf, int ch, dword len);

	// movedata copies data stored in one segment to another one
	 void movedata (dword src_sel, dword src_offs, dword dest_sel, dword dest_offs, dword len);
	
	// Writes a byte to a far data segment
	 void farpokeb (dword dest_sel, dword dest_offs, dword value);
	
	// Writes a word to a far data segment
	 void farpokew (dword dest_sel, dword dest_offs, dword value);

	// Writes a dword to a far data segment
	 void farpokel (dword dest_sel, dword dest_offs, dword value);
	
	// Reads a byte from a far data segment
	 dword farpeekb (dword src_sel, dword src_offs);
	
	// Reads a word from a far data segment
	 dword farpeekw (dword src_sel, dword src_offs);

	// Reads a dword from a far data segment
	 dword farpeekl (dword src_sel, dword src_offs);
	

#ifdef __cplusplus
}
#endif

#endif
