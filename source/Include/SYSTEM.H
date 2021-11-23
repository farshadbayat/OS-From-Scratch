//////////////////////////////Start System//////////////////////////////////////////////

#ifndef __SYSTEM_H
#define __SYSTEM_H

#ifdef __cplusplus
extern "C"
{
#endif
	#include <ctype.h>
	

	#define KASSERT_TRUE	1

	#define FIRST_BYTE(Number)	(0xFFFF & Number)
	#define SECOND_BYTE(Number) (0xFFFF & (Number>8))
	#define THIRD_BYTE(Number)  (0xFFFF & (Number>16))
	#define FOURTH_BYTE(Number) (0xFFFF & (Number>24))
	

	/* This defines what the stack looks like after an ISR was running */
	struct regs
	{
	    unsigned int gs, fs, es, ds;						/* pushed the segs last */
	    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;/* pushed by 'pusha' */
	    unsigned int int_no, err_code;						/* our 'push byte #' and ecodes(ERROR CODE IS PUSHED BNY THE PROCESSOR AUTOMATICALLY) do this */
	    unsigned int eip, cs, eflags, useresp, ss;			/* pushed by the processor automatically by INT nn/IRET */ 
	};

	

	#define HALT()          __asm__ __volatile__ ("hlt")
	//other address help
	#define PORT_CMOS       0x70
	#define PORT_KBD_A      0x60
	#define PORT_KBD_B      0x61
	#define PORT_8259_M     0x20
	#define PORT_8259_S     0xA0
	#define PORT_INT_MASK_M 0x21
	#define PORT_INT_MASK_S 0xA1
	#define EOI             0x20	
	
	#define KASSERT()																							\
		do {																										\
			if (KASSERT_TRUE)																							\
			{																										\
				printf("Failed Assertion In Funcation:[%s] At File [%s] Line: [%d].",__func__ ,__FILE__ , __LINE__);\
				/* while(1); */																						\
			}																										\
		}while(0)

	/* SYSTEM.C */
	#define EFLAGS_IF		(1 << 9)

	 extern char KernelBuffer[80*25];
	 void dump(unsigned char *data ,unsigned count ,bool Paging);
	 void dump_regs(struct regs *regs);	 
	 void reboot(void);
	 
	 int pop16(void);
	 int _AX(void),_BX(void),_CX(void),_DX(void);
	 int _EAX(void),_EBX(void),_ECX(void),_EDX(void);
	 int _ESI(void),_EDI(void),_ESP(void);
	 int _ES(void),_DS(void),_CS(void),_SS(void);
	 int kprintf(const char *frmt,...);
	 void SetBit(char *Variable,int BitNumber,bool Value);
	 bool IsSetBit(long Variable,int BitNumber,bool Value);
	 char* SetNull(char *Variable,unsigned short Len);

	/*	My asm Declaration	*/	 
	 int load_eflags(void);
	 void store_eflags(int eflags);
	 int load_cr0(void);
	 void store_cr0(int cr0);
	 unsigned int GetCurrentEFLAGS();
	 unsigned int memtest_sub(unsigned int start, unsigned int end);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////
	//		 GDT.C			 //
	///////////////////////////
	#define GDT_TO_SEL(gdt) ((gdt) << 3)

	//Defines a GDT entry 
	 #define CODE_SEL_FULL	0x08
	 #define DATA_SEL_FULL	0x10
	 
	 #define GDT_NULL		0
	 #define GDT_TEXT		1//TEXT Or Code
	 #define GDT_DATA		2
	 #define GDT_USER_DATA	3
	 #define GDT_USER_CODE	4
	 
	 #define GDT_PNPTEXT	12
	 #define GDT_PNPDATA	13
	 #define GDT_PNPTHUNK	14
	 #define GDT_AUX2		15
	 #define GDT_AUX1		16

	 #define SEL_AUX1     GDT_TO_SEL(GDT_AUX1)	
	 #define SEL_PNPTEXT  GDT_TO_SEL(GDT_PNPTEXT)
	 #define SEL_PNPDATA  GDT_TO_SEL(GDT_PNPDATA)
	 #define SEL_PNPTHUNK GDT_TO_SEL(GDT_PNPTHUNK)
	 #define SEL_AUX2     GDT_TO_SEL(GDT_AUX2)
	 // Descriptors may include the following as appropriate:

	#define D_DPL3         0x60   // DPL3 or mask for DPL
	#define D_DPL2         0x40   // DPL2 or mask for DPL
	#define D_DPL1         0x20   // DPL1 or mask for DPL
	#define D_DPL0         0x00   // DPL0 or mask for DPL


	struct dt_entry
	{
	    unsigned short limit_low;
	    unsigned short base_low;
	    unsigned char base_middle;
	    unsigned char access;
	    unsigned char granularity;
	    unsigned char base_high;
	} __attribute__((packed));
	
	struct gdt_ptr
	{
	    unsigned short limit;
	    unsigned int base;
	} __attribute__((packed));

	#define TOTAL_GDT	20
	extern struct dt_entry gdt[TOTAL_GDT];
	extern struct gdt_ptr gp;
	void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
	void GDT_Install();
	unsigned long long GDT_TO_LONG(struct dt_entry gdt_);
	struct dt_entry LONG_TO_GDT(unsigned long long gdt_);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////
	//		 IDT.C			 //
	///////////////////////////
	 void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
	 void IDT_Install();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////
	//		 ISRS.C			 //
	///////////////////////////
	 void ISRs_Install();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////
	//		 IRQ.C			 //
	///////////////////////////
	#define PIC0_ICW1     0x0020
	#define PIC0_OCW2     0x0020
	#define PIC0_IMR      0x0021
	#define PIC0_ICW2     0x0021
	#define PIC0_ICW3     0x0021
	#define PIC0_ICW4     0x0021
	#define PIC1_ICW1     0x00a0
	#define PIC1_OCW2     0x00a0
	#define PIC1_IMR      0x00a1
	#define PIC1_ICW2     0x00a1
	#define PIC1_ICW3     0x00a1
	#define PIC1_ICW4     0x00a1

	#define IRQBASE       0x20
	#define IRQ2INTR(irq) (IRQBASE + (irq))

	 void irq_install_handler(int irq, void (*handler)(struct regs *r));
	 void irq_uninstall_handler(int irq);
	 void IRQ_Install();
	
		

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////
	// Real Mode Function Execute //
	////////////////////////////////
	#define INTERUPT_NO_ADDRESS		0x1008
	#define IN_OUT_REGS_ADDRESS		0x100A
	#define	SEG_REGS_ADDRESS		0x1018
	#define	IS_SEG_NOT_LOAD_ADDRESS	0x1020 //Format is | 0| 0| 0| 0|cs|es|ss|ds| ;if each segment bit is set (equal 1) this segment is  loaded
	#define GRAPHIC_INFO_ADDRESS	0x1021
	
	#define DS_LOAD	0x01
	#define SS_LOAD	0x02
	#define ES_LOAD	0x04
	#define CS_LOAD	0x08
	
	typedef unsigned long FAR;
	/* Make a FARPTR from a segment and an offset */
	#define MK_FP(seg,off) (void *) (( ((dword)seg)<< 16) | (word)off)
	
	/* Extract the segment part of a FARPTR */
	#define FP_SEG(fp)     (word)((((FAR) fp) >> 16)&0xFFFF)
	
	/* Extract the offset part of a FARPTR */
	#define FP_OFF(fp)     (word)(((FAR) fp)&0xFFFF)

	/* Convert a segment:offset pair to a linear address */
	#define FP_TO_LINEAR(seg, off) (void*) ( (((dword)seg)<< 4) + ((word)off) )
	
	/*convert real address[ that generate in real mode] to the protected mode that each point to same location of ram[tell liner address]*/
	void* i386LinearToFp(void *ptr);
	
	
	
	 struct BYTEREGS  {
	   unsigned char  al, ah, bl, bh;
	   unsigned char  cl, ch, dl, dh;
	 };

	 struct WORDREGS  {
	   unsigned short  ax, bx, cx, dx;
	   unsigned short  si, di, flags;
	 };

  union REGS {
	   struct  WORDREGS  x;
	   struct  BYTEREGS  h;
	 };
	
	 struct SREGS {
		unsigned short  ds;
		unsigned short  ss; 
		unsigned short  es;	  
		unsigned short  cs;
	 };
	void realmode();
	void int86x(unsigned short int_no ,union REGS *in_regs,union REGS *out_regs ,struct SREGS *segregs ,char mask );
	void int86(unsigned short int_no ,union REGS *in_regs ,union REGS *out_regs );
	void PrintREGS(union REGS Regs);
	
	
	///////////////////////////////////////////////
	// FUNCTIONS DEALING WITH SELECTORS/SEGMENTS //
	///////////////////////////////////////////////
	#define flat_sel        0x08		// selector for 4GB segment
	#define data_sel        0x10		// selector for data segment
	
	#define ATTR_GRANUL     0x80
	
	typedef struct {
	  unsigned short limit    __attribute__ ((packed));
	  unsigned short base0_15 __attribute__ ((packed));
	  unsigned char base16_23 __attribute__ ((packed));
	  unsigned char attrs0_7  __attribute__ ((packed));
	  unsigned char attrs8_15 __attribute__ ((packed));
	  unsigned char base24_31 __attribute__ ((packed));
	} SEG_DESCRIPTOR;

	typedef struct {
	  unsigned short limit __attribute__ ((packed));
	  unsigned long	 base  __attribute__ ((packed));
	} GDT_REG,IDT_REG;
	
	typedef struct {
	  unsigned long  offs __attribute__ ((packed));
	  unsigned short sel  __attribute__ ((packed));
	} FARPTR;
	
	
	// Loads GDTR
	 void LGDT (GDT_REG *gdtr);
	
	// Saves GDTR
	 void SGDT (GDT_REG *gdtr);

	// Loads IDTR
	 void LIDT (IDT_REG *idtr);
	
	// Saves IDTR
	 void SIDT (IDT_REG *idtr);
	
	// Loads TR
	 void LTR (unsigned long sel);

	// Saves TR
	 unsigned long STR();
	
	// Loads CR3
	 void LCR3 (unsigned long value);
	
	// Saves CR3
	 unsigned long SCR3();

	// Far jump
	//extern void JUMPF (FARPTR *ptr);
	
	// Converts a selector corresponding to GDT segment to a base 32-bit address
	 unsigned long gdt_sel_to_base (unsigned long _sel);
	
	// Converts a selector corresponding to GDT segment to a 32-bit limit
	 unsigned long gdt_sel_to_limit (unsigned long _sel);

	// Converts a selector corresponding to GDT segment to a 8-bit access
	 unsigned long gdt_sel_to_access (unsigned long _sel);
	
	// Converts a selector corresponding to GDT segment to a 8-bit granularity
	 unsigned long gdt_sel_to_gran (unsigned long _sel);
	
	
	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	#define	RPL_0	 0
	#define	RPL_1	 1
	#define	RPL_2	 2
	#define	RPL_3	 3
	
	#define D_PRESENT      0x80   // Present

	// Segment descriptors (not gates) may include:
	
	#define D_ACCESSED 0x1  // Accessed (data or code)
	
	#define D_WRITE    0x2  // Writable (data segments only)
	#define D_EXDOWN   0x4  // Expand down (data segments only)
	
	#define D_READ     0x2  // Readable (code segments only)
	#define D_CONFORM  0x4  // Conforming (code segments only)
	
	#define D_BUSY     0x2  // Busy (TSS only)
	
	// Granularity flags
	
	#define D_BIG      0x4	  // Default to 32 bit mode
	#define D_BIG_LIM  0x8	  // Limit is in 4K units
          
	#define TS_RUNNING    0
	#define TS_RUNABLE    1
	#define TS_STOPPED    2
	
	// code selector in gdt  
	#define CODE_SEL	0x08
	// data and stack selecter in gdt  
	#define DATA_SEL	0x10
	#define FLAT_SEL	0x50

	#define BIOS_SEL	0x58+RPL_3

	// code selector in ldt */
	#define USER_CODE_SEL	0x08
	
	// data and stack selector in ldt 
	#define USER_DATA_SEL	0x10
	
	   //NULL				0	0
	   //CODE_SEL			8	1
	   //DATA_SEL			10	2
	   //CURR_TASK_TSS	18	3
	   //CURR_TASK_LDT	20	4
	#define CURR_TASK_TSS 	3
	#define	CURR_TASK_LDT	4
	
	#define TSS_SEL		0x18
	#define	LDT_SEL		0x20
	
	#define TSS_SEL_STR	"0x18"
	#define LDT_SEL_STR	"0x20"
	
	#define DEFAULT_LDT_CODE	0x00cffa000000ffffULL
	#define DEFAULT_LDT_DATA	0x00cff2000000ffffULL
	
	#define INITIAL_PRIO		200
	
	struct TSS_STRUCT {
	    int    back_link;
	    int    esp0, ss0;
    	int    esp1, ss1;
    	int    esp2, ss2;
    	int    cr3;
    	int    eip;
    	int    eflags;
    	int    eax,ecx,edx,ebx;
    	int    esp, ebp;
    	int    esi, edi;
	    int    es, cs, ss, ds, fs, gs;
	    int    ldt;
	    int    trace_bitmap;
		}__attribute__ ((packed));
		
	struct TASK_STRUCT {
		struct TSS_STRUCT tss;
		struct dt_entry tss_entry;
		unsigned long long ldt[2];
		struct dt_entry ldt_entry;
		int state;
		int priority;
		struct TASK_STRUCT *next;
		};
	
	extern struct TASK_STRUCT TASK0;
	extern struct dt_entry set_tss(int num, unsigned  long tss) ;
	extern struct dt_entry set_ldt(int num, unsigned  long );
	extern struct dt_entry get_tss(void);
	extern struct dt_entry get_ldt(void);
	extern struct TASK_STRUCT *current;
	extern void scheduler(void);
	
	
	//////////////////////////////END SYSTEM//////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif


