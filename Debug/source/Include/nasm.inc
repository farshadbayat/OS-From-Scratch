%ifndef __TL_NASM_INC
%define	__TL_NASM_INC 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Define Macro And Constant;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%DEFINE	PMODE_16BIT_ADDRESS		0x1000	;Limit is 4 byte(subroutin is at the 0x1000)	IS  FIX
%DEFINE	PMODE_32BIT_ADDRESS		0x1004	;Limit is 4 byte(subroutin is the 0x1004)		IS  FIX
%DEFINE	INTERUPT_NO_ADDRESS		0x1008	;Limit is 2 byte(subroutin is at the 0x1008)	NOT FIX
%DEFINE	IN_OUT_REGS_ADDRESS		0x100A	;Limit is 14 byte(Structure is at the 0x100A)	NOT FIX
%DEFINE	SEG_REGS_ADDRESS		0x1018	;Limit is 8 byte(Structure is at the 0x1018)	NOT FIX
%DEFINE	IS_SEG_ADDRESS			0x1020	;Limit is 1 byte(Structure is at the 0x1020)	NOT FIX
%DEFINE	GRAPHIC_INFO_ADDRESS	0x1021	;Limit is 512 byte(Structure is at the 0x1021)	NOT FIX


%DEFINE	SEL_32CODE		0x08
%DEFINE	SEL_32DATA		0x10
%DEFINE	SEL_USERDATA	0x18
%DEFINE	SEL_USERCODE	0x20
%DEFINE	SEL_LINERDATA	0x28
%DEFINE	SEL_LINERCODE	0x30
%DEFINE	SEL_16DATA		0x38
%DEFINE	SEL_16CODE		0x40
%DEFINE	SEL_TSS			0x48
%DEFINE	SEL_FLAT		0x50

%DEFINE	KERNEL_LINER_ADDRESS	 0x10000	;Start Kernel at this address(Liner Format)
%DEFINE	KERNEL_SEGMENT_ADDRESS	 0x1000		;Start Kernel at this address(Segment Format)
%DEFINE	KERNEL_SECTOR_TOTAL		 1000		;Total Sector To Read and Load on to ram

%DEFINE CHARACTER_LOAD			 04			;shekle Lozei

%macro JMP_F 2
		mov	eax,[%2]
		mov [%%jmp_offset],eax
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		DB	0xEA		;opcode		1 byte	;
	%%jmp_offset:							;
		DD  0x00000000	;offset		4 byte	;simulate far jump=>[jmp	SEL_16CODE:pmode16]
		DW	%1			;base/sel	2 byte	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%endmacro

%macro CHG_CS 1
		push %1
		push %%change_cs	
		retf
	%%change_cs:
%endmacro

;;;;;;;;;;;;;;;;;;;;;
%macro CMP_BIT 2	;
	mov al,%1		;
	and al,%2		;compare bit  for example CPM_BIT 01111011,01000000 is true
	cmp al,%2		;
%endmacro			;
;;;;;;;;;;;;;;;;;;;;;

%endif

