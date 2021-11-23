#include <mem.h>

////////////////////////////////
// FUNCTIONS ACCESSING MEMORY //
////////////////////////////////

inline void farmemset (dword sel, dword buf, int ch, dword len) {
  __asm__ __volatile__ ("pushw   %%es		\n"
						"movw    %%bx, %%es	\n"
						"cld				\n"
						"rep				\n"
						"stosb				\n"
						"popw    %%es		  "
						:
						: "D" (buf), "a" (ch), "c" (len), "b" (sel)
						);
}

// movedata copies data stored in one segment to another one
void movedata(dword src_sel, dword src_offs, dword dest_sel, dword dest_offs, dword len) {
  __asm__ __volatile__ ("pushw   %%ds		\n"
						"pushw   %%es		\n"
						"movw    %%ax, %%ds	\n"
						"movw    %%bx, %%es	\n"
						"cld				\n"
						"rep				\n"
						"movsb				\n"
						"popw    %%es		\n"
						"popw    %%ds		  "
						:
						: "a" (src_sel), "S" (src_offs), "b" (dest_sel), "D" (dest_offs), "c" (len)
						);
}

// Writes a byte to a far data segment
void farpokeb (dword dest_sel, dword dest_offs, dword value) {
  __asm__ __volatile__ ("pushw   %%es				\n"
						"movw    %%bx, %%es			\n"
						"movb    %%al, %%es:(%%edi)	\n"
						"popw    %%es				  "
						:
						: "b" (dest_sel), "D" (dest_offs), "a" (value)
						);
}

// Writes a word to a far data segment
void farpokew (dword dest_sel, dword dest_offs, dword value) {
  __asm__ __volatile__ ("pushw   %%es				\n"
						"movw    %%bx, %%es			\n"
						"movw    %%ax, %%es:(%%edi)	\n"
						"popw    %%es				  "
						:
						: "b" (dest_sel), "D" (dest_offs), "a" (value)
						);
}

// Writes a dword to a far data segment
void farpokel (dword dest_sel, dword dest_offs, dword value) {
  __asm__ __volatile__ ("pushw   %%es				 \n"
						"movw    %%bx, %%es			 \n"
						"movl    %%eax, %%es:(%%edi) \n"
						"popw    %%es				   "
						:
						: "b" (dest_sel), "D" (dest_offs), "a" (value)
						);
}

// Reads a byte from a far data segment
dword farpeekb (dword src_sel, dword src_offs) {
	dword rtn;  
  __asm__ __volatile__ ("pushw   %%es				\n"
						"movw    %%bx, %%es			\n"
						"movb    %%es:(%%edi), %%al	\n"
						"movzbl  %%al, %0			\n"
						"popw    %%es				  "
						: "=r" (rtn)
						: "b" (src_sel), "D" (src_offs)
						);
 return rtn;
}

// Reads a word from a far data segment
dword farpeekw (dword src_sel, dword src_offs) {
	dword rtn;
  __asm__ __volatile__ ("pushw   %%es				\n"
						"movw    %%bx, %%es			\n"
						"movw    %%es:(%%edi), %%ax	\n"
						"movzwl  %%ax, %0			\n"
						"popw    %%es				  "
						: "=r" (rtn)
						: "b" (src_sel), "D" (src_offs)
						);
  return rtn;
}

// Reads a dword from a far data segment
dword farpeekl (dword src_sel, dword src_offs) {
	dword rtn;
  __asm__ __volatile__ ("pushw   %%es					\n"
						"movw    %%bx, %%es				\n"
						"movl    %%es:(%%edi), %0		\n"
						"popw    %%es					  "
						: "=r" (rtn)
						: "b" (src_sel), "D" (src_offs)
						);
	return rtn;
}


