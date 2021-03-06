#include <system.h>
#include <stdio.h>


#define KDEBUG

	#ifndef _XTONX_DEFINED
	#define _XTONX_DEFINED	

	unsigned short htons(unsigned short n)
	{
	  return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
	}

	unsigned short ntohs(unsigned short n)
	{
		return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
	}

	unsigned long htonl(unsigned long n)
	{
		return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
	}

	unsigned long ntohl(unsigned long n)
	{
		return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
	}

	#endif

void reboot(void)
{
	unsigned temp;
	disable();
/* flush the keyboard controller */
	do
	{
		temp = inportb(0x64);
		if((temp & 0x01) != 0)
		{
			(void)inportb(0x60);
			continue;
		}
	} while((temp & 0x02) != 0);
/* pulse the CPU reset line */
	outportb(0x64, 0xFE);
/* ...and if that didn't work, just halt */
	while(1)
		/* nothing */;
}



/// <summary>
	///This Function To Set Or Clear[Set=1,Clear=0] The Variable.
	///BitNumber Is Which Bit Is Set Or Clear.
	///Value Is [True To Set] Or [False To Clear].
/// </summary>
void SetBit(char *Variable,int BitNumber,bool Value)
{
	int ByteN=ceill( BitNumber/8);
	char MaskByte=~(1<<(BitNumber%8));

	Variable[ByteN]=(Variable[ByteN] & MaskByte) | (~MaskByte);
}

/// <summary>
	///This Function To Check That Variable Bit[BitNumber] Is Value[1,0].
	///BitNumber Is That Which Bit Is Value.
	///Value Is [True = 1] Or [False = 0].
/// </summary>
bool IsSetBit(long Variable,int BitNumber,bool Value)
{
	if(((Variable&(1<<BitNumber))>>BitNumber)==Value)
		return true;
	else
		return false;
}

/// <summary>
	///This Function Is Set Variable To Null.
	///Variable:	Pointr To The Variable;
	///Len:			Length To Be Null;
	///Return Pointer To The Variable;
/// </summary>
char* SetNull(char *Variable,unsigned short Len)
{
	word i;
	for(i=0;i<Len;i++)
		Variable[i]=0;
	return Variable;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////
//		Debugging function	  //
////////////////////////////////

#define BPERL		18	/* byte/line for dump */
#define LPERP		18	/* line/page for dump */

void dump(unsigned char *data ,unsigned count ,bool Paging)
{
	unsigned char byte1, byte2;
	unsigned char LCounter=0;

	while(count != 0)
	{
		for(byte1 = 0;byte1 < BPERL; byte1++)
		{
			if(count == 0){
				unsigned char byte3=byte1;
				for(;byte3 < BPERL;byte3++)
					printf("   ");
				break;
			}
				
			printf("%02X ", data[byte1]);
			count--;
		}
		printf("\t");
		for(byte2 = 0; byte2 < byte1; byte2++)
		{
			if(data[byte2] < ' ')
				printf("%c", '.');
			else
				printf("%c", data[byte2]);
		}
		printf("\n");
		LCounter++;
		if(LCounter==LPERP && page_num==false){						
			cprintf(YELLOW,"Please Enetr To Continue...");			
			getch();
		}
		data += BPERL;
	}
}

void dump_regs(struct regs *regs)
{

	printf("EAX=%08X	EBX=%08X	ECX=%08X	EDX=%08X    ES=%08X\n" ,regs->eax ,regs->ebx, regs->ecx ,regs->edx ,regs->es);
	printf("EDI=%08X    ESI=%08X    EBP=%08X    ESP=%08X    DS=%08X\n" ,regs->edi ,regs->esi, regs->ebp ,regs->esp , regs->ds);
	printf(" FS=%08X     GS=%08X intnum=%08X  error=%08X    EIP=%08X\n",regs->fs  ,regs->gs , regs->int_no, regs->err_code,regs->eip);
	printf(" CS=%08X EFLAGS=%08X", regs->cs, regs->eflags);
//	if((regs->eflags & 0x20000uL) || (regs->cs & 0x03))
//		kprintf("   uESP=%08X    uSS=%08X",	regs->user_esp, regs->user_ss);
//	kprintf("\n");
//	if(regs->eflags & 0x20000uL)
//		kprintf("vES=%04X    vDS=%04X    vFS=%04X    vGS=%04X\n",regs->v_es & 0xFFFF, regs->v_ds & 0xFFFF,regs->v_fs & 0xFFFF, regs->v_gs & 0xFFFF);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////
// Real Mode Function Execute //
////////////////////////////////
void* i386LinearToFp(void *ptr)
{
    dword seg, off;
    off = ((dword) ptr & 0xFFFF);
    seg = ((dword) ptr - off) >> 12;	
return (void *)off+seg;
}
void int86(unsigned short int_no, union REGS *in_regs, union REGS *out_regs)
{		
	disable();
	GDT_REG gdtr_content;
	IDT_REG idtr_content;	
	SGDT(&gdtr_content);
	SIDT(&idtr_content);
	farpokeb(FLAT_SEL,IS_SEG_NOT_LOAD_ADDRESS,0x00);//segment must not loaded into register and set it 0x04;for infromation go to header file
	farpokew(FLAT_SEL,INTERUPT_NO_ADDRESS,int_no);
	movedata (FLAT_SEL, (dword)in_regs, FLAT_SEL, (dword)IN_OUT_REGS_ADDRESS, sizeof(union REGS));
	realmode();
	*out_regs=*((union REGS *)IN_OUT_REGS_ADDRESS);
	LGDT(&gdtr_content);
	LIDT(&idtr_content);
	enable();
}

void int86x(unsigned short int_no ,union REGS *in_regs,union REGS *out_regs ,struct SREGS *segregs,char mask)
{
	/* 0| 0| 0| 0|cs|es|ss|ds*/
	disable();
	GDT_REG gdtr_content;
	IDT_REG idtr_content;	
	SGDT(&gdtr_content);
	SIDT(&idtr_content);	
	farpokeb(FLAT_SEL,IS_SEG_NOT_LOAD_ADDRESS,mask);//segment must loaded into register and set it true
	farpokew(FLAT_SEL,INTERUPT_NO_ADDRESS,int_no);
	movedata (FLAT_SEL, (dword)in_regs, FLAT_SEL, (dword)IN_OUT_REGS_ADDRESS, sizeof(union REGS));
	movedata (FLAT_SEL, (dword)segregs, FLAT_SEL, (dword)SEG_REGS_ADDRESS, sizeof(struct SREGS));
	realmode();
	*out_regs=*((union REGS *)IN_OUT_REGS_ADDRESS);
	LGDT(&gdtr_content);
	LIDT(&idtr_content);
	enable();
}

void PrintREGS(union REGS Regs)
{
	printf("AX[AH,AL]=%2X , %2X \tBX[BH,BL]=%2X , %2X \n",Regs.h.ah ,Regs.h.al ,Regs.h.bh ,Regs.h.bl );
	printf("CX[CH,CL]=%2X , %2X \tDX[DH,DL]=%2X , %2X \n",Regs.h.ch ,Regs.h.cl ,Regs.h.dh ,Regs.h.dl );
	printf("SI=%4X\tDI=%4X\tFlag=%4X\n",Regs.x.si ,Regs.x.di ,Regs.x.flags );
}
////////////////////////////////
// FUNCTIONS ACCESSING MEMORY //
////////////////////////////////

void farmemset (dword sel, dword buf, int ch, dword len) {
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
void movedata (dword src_sel, dword src_offs, dword dest_sel, dword dest_offs, dword len) {
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

///////////////////////////////////////////////
// FUNCTIONS DEALING WITH SELECTORS/SEGMENTS //
///////////////////////////////////////////////

// Loads GDTR
void LGDT (GDT_REG *gdtr) {
  __asm__ __volatile__("lgdt    (%%eax)"
						:
						: "a" ((unsigned long) gdtr)
						);
}

// Saves GDTR
void SGDT (GDT_REG *gdtr) {
  __asm__ __volatile__("sgdt    (%%eax)"
						:
						: "a" ((unsigned long) gdtr)				  
						);
}

// Loads IDTR
void LIDT (IDT_REG *idtr) {
  __asm__ __volatile__("lidt    (%%eax)"
						:
						: "a" ((unsigned long) idtr)
						);
}

// Saves IDTR
void SIDT (IDT_REG *idtr) {
  __asm__ __volatile__("sidt    (%%eax)"
						:
						: "a" ((unsigned long) idtr)				  
						);
}

// Loads TR
void LTR (unsigned long sel) {
  __asm__ __volatile__("ltr     %%ax"
						:
						: "a" (sel)				  
						);
}

// Saves TR
unsigned long STR() {
	dword rtn;
 __asm__ __volatile__("str     %%ax			\n"
					  "movzwl  %%ax, %0		  "
					: "=r" (rtn)
					:
					: "eax"
					);
	return rtn;
}

// Loads CR3
void LCR3 (unsigned long value) {
  __asm__ __volatile__("movl    %%eax, %%cr3	"
						:
						: "a" (value)				  
						);
}

// Saves CR3
unsigned long SCR3() {
	dword rtn;
  __asm__ __volatile__("movl    %%cr3, %0	"
						: "=r" (rtn)
						:
						: "eax"
						);
return rtn;
}

// Far jump
//void JUMPF (FARPTR *ptr) {
//  __asm__ __volatile__ ("ljmp    (%%eax)"
//						:
//						: "a" (ptr)
//						);
//}

int kprintf(const char *frmt,...)
{
	va_list ap;
	unsigned char *buffer=KernelBuffer;
	char OldColor=GetTextColor();
	int len;	
	va_start(ap, frmt);
	len=vsprintf((unsigned char *)buffer,frmt,ap);
	va_end(ap);

	#ifdef KDEBUG
		textcolor(LIGHTRED);
		puts("\nIn Kernel Debuging[");
		textcolor(LIGHTMAGENTA);
		puts(buffer);
		textcolor(LIGHTRED);
		puts("]");
		textcolor(OldColor);
	#endif

	return len;
    
}










/*Help Of Inline assmbeler


asm(assembly language template
: output operands
: input operands
: list of clobbered registers);
If you want to prevent the compiler from trying to optimize your assembly language
code, you can use the volatile keyword, like the following:
asm volatile ( ...
Also, if you need to be POSIX compliant, you can use the keywords __asm__ and
__volatile__ instead of asm and volatile.
The Assembly Language Template
The assembly language template consists of one or more statements of assembly language
and is the actual code to be inserted inline. The opcodes can address immediate (constant)
values, the contents of registers, and memory locations. The following is a summary of
the syntax rules for addressing values:
 Aregister name begins with two percent signs, such as %%eax and %%esi. The
Intel register names normally begin with a percent sign, and the asm template
also requires a percent sign, which is why there must be two.
 A memory location is one of the input or output operands. Each of these is
specified by a number according to the order of its declaration following the
colons. The first output operand is %0. If there is another output operand, it
will be %1, and so on. The numbers continue with the input operands�for
example, if there are two output operands, the first input operand will be %2.
 A memory location can also be addressed by having its address stored in
a register and enclosing the register name in parentheses. For example, the
following will load the byte addressed by the contents of register %%esi
into the %%al register:
movb (%%esi),%al
 An immediate (constant) value is designated by the dollar ($) character
followed by the number itself, as in $86 or $0xF12A.
 All the assembly language is a single-quoted string, and each line of the
assembly code requires a terminator. The terminator can be a semicolon or
a newline (\n) character. Also, tabs can be inserted to improve readability
of assembly language listings.
C h a p t e r 1 5 : T h e G N U A s s e m b l e r 323
PERIPHERALS AND
INTERNALS
324 G C C : T h e C o m p l e t e R e f e r e n c e
Input and Output Operands
The input and output operands consists of a list of variable names that you wish to be
able to reference in the assembly code. You can use any valid C expression to address
memory. For example, the following code is a variation on the preceding example that
uses an array to store the input and output values, and doubles the number by shifting
it to the left:
// double.c 
#include <stdio.h>
int main(int argc,char *argv[])
{
int array[2];
array[0] = 150;
int i = 0;
asm("movl %1,%%eax; \
shl %%eax; \
movl %%eax,%0;"
:"=r"(array[i+1])
:"r"(array[i])
:"%eax");
printf("array[0]=%d array[1]=%d\n",array[0],array[1]);
return(0);
}
The rules for specifying the input and output variables are as follows:
 The C expression, which results in an address in your program, is enclosed
in parentheses.
 If the address is preceded by "r", it applies the constraint that the value must
be stored in a register. Input variables will be loaded before your assembly
language is executed, and output variables will be stored in memory after
your code has executed. The "=r" form should be used for output operands.
 A variable may be constrained to a specific register with one of the following:
"a" %%eax
"b" %%ebx
"c" %%ecx
"d" %%edx
PERIPHERALS AND
INTERNALS
"S" %%esi
"D" %%edi
 A variable can be constrained to be addressed in memory instead of being
loaded into a register by using the "m" constraint.
 In the case of the same variable being used as both an input and output value,
the "=a" constraint is used for its output constraint, and its reference number
is used for its input constraint. The following example uses counter for both
input and output:
asm("incw %0;"
: "=a"(counter)
: "0"(counter));
 You may use any number of input and output operands by separating them
with commas.
 The output and input operands are numbered sequentially beginning with $0
and continuing through $n-1, where n is the total number of both input and
output operands. For example, if there is a total of six operands, the last one
would be named $5.
List of Clobbered Registers
The list of registers that are clobbered by your code is simply a list of the register names
separated by commas, as in the following example:
. . .
"%eax", "%esi");
This information is passed on to the compiler so it will know not to expect any values
to be retained in these registers.*/
