;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Comment Place;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;comment
;bp refer to [ss:]bp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Boot Sector & Loader ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "nasm.inc"

[BITS 16]       ; We need 16-bit intructions for Real mode
[ORG 0x7C00]    ; The BIOS loads the boot sector into memory location 0x7C00

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Reading Kernel From Floopy;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		xor ax,ax
		mov ds,ax		
		mov DWORD[PMODE_16BIT_ADDRESS],pmode16
		mov si,_read_kernel_msg
		call puts

		;read rest of kernel to 0x10000
        mov ax, KERNEL_SEGMENT_ADDRESS
        mov es, ax
        xor ebx, ebx					; Destination address = 1000:0000(liner address:100000)       
        mov si,1						; 0 is boot sector        
        mov WORD[Total_Sector],KERNEL_SECTOR_TOTAL        

SET_CX_BEGIN:     
		mov cx,128
		cmp WORD[Total_Sector],128	;each segment such es is limit from 0000 to 0xFFFF so can addressing 64kBayte(128*512=64kbayte)		
		jnbe SET_CX_END
		mov cx, WORD[Total_Sector]     		
		
SET_CX_END:		
		mov al,CHARACTER_LOAD					;Show Progress		
		sub WORD[Total_Sector],cx		
		call putc
read_kernel:
		call read_sect
		add	 ebx,512
		inc	 si				

		loop read_kernel
		
		mov ax, es				;save es to ax to add with bx
		shr ebx,4				;shift to right to set es(becuse bx at this line become 0xFE00 and can not continue to read rest of sector)
		add ax,bx
		mov es, ax
		xor ebx, ebx
		cmp WORD[Total_Sector],0
		jnz SET_CX_BEGIN
		 
        cli                     ; Disable interrupts, we want to be alone        
        xor ax, ax
        mov ds, ax              ; Set DS-register to 0 - used by lgdt
        lgdt [gdt_desc]         ; Load the GDT descriptor
        mov eax, cr0            ; Copy the contents of CR0 into EAX
        or eax, 1               ; Set bit 0
        mov cr0, eax            ; Copy the contents of EAX into CR0

        jmp SEL_32CODE:clear_pipe      ; Jump to code segment, offset clear_pipe To Understand why code segement is 08 go to help(Bona Fide OS Development - Protected Mode.htm)becuse of cs is index to the descriptor register and gdt_code is just after gdt_null and each gdt is 8byte therefore cs=number of gdt(code)*8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Real Mode subroutine(interupt);;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; name:			read_sect									;
; action:		Read One Sector and save it					;
; in:			si: location of reading sector and 			;
;				save at es:bx								;
;				ax: LBA address, starts from 0				;
; out:			(nothing)									;
; modifies:		(nothing)									;
; minimum CPU:		8088									;
; notes:													;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
read_sect:
	push ax
	push cx
	push dx
	push bx
	
	mov ax,si
	xor dx,dx
	mov bx,18		; 18 sectors per track for floppy disk
	
	div bx
	inc dx
	mov cl,dl		; cl=sector number
	xor dx,dx
	mov bx,2		; 2 headers per track for floppy disk
	
	div bx
	
	mov dh,dl		; head
	xor dl,dl		; driver
	mov ch,al		; cylinder
	pop bx			; save to es:bx
rp_read:
	mov al,0x1		; read 1 sector
	mov ah,0x2
	int 0x13
	jc rp_read
	pop dx
	pop cx
	pop ax
	ret
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; return to real mode procedure, modified from section 14.5 of 386INTEL.TXT
; (...continued from below...)
; 4. load SS with selector to descriptor that is "appropriate for real mode":
;	Limit = 64K (FFFFh)	Byte-granular (G=0)
;	Expand-up (E=0)		Writable (W=1)
;	Present (P=1)		Base address = any value
; You may leave the other data segment registers with limits >64K if you want 'unreal mode', otherwise load them with a similar selector.
; 5. Clear the PE bit in register CR0
; 6. Jump to a 16:16 real-mode far address
; 7. Load all other segment registers (SS, DS, ES, FS, GS)
; 8. Use the LIDT instruction to load an IDT appropriate for real mode, with base address = 0 and limit = 3FFh. Use the 32-bit operand size override prefix so all 32 bits of the IDT base are set (otherwise only the bottom 24 bits will be set).
; 9. Zero the high 16 bits of 32-bit registers. If the register value is not important, just zero the entire 32-bit register, otherwise use 'movzx'
; 10. Enable interrupts
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;					
pmode16:
	;4
	mov ax,SEL_16DATA	;_16BIT_DATA_SEL
	mov ss,ax
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax	
	mov eax,cr0			;5
	and al,0FEh
	mov cr0,eax	
	jmp far [_real_ip]	;6
real:   
	mov ax,cs			;7
	mov ss,ax
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax			
	o32 lidt [_real_idt];8				
	xor eax,eax			;9
	xor ebx,ebx
	xor ecx,ecx
	xor edx,edx
	xor esi,esi
	xor edi,edi
	;movzx ebp,bp
	;movzx esp,sp	
	mov esp, 090000h
	sti					;10
	;simulate interupt instruction	becuse of interupt used at this form [INT imm8],and we change int_no(imm8) dynamicly
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	cmp WORD[INTERUPT_NO_ADDRESS],03h	;
	jz jmp_not_change					;
	mov al,0xCD							;(0x00CD)==>00:interupt number and 'CD' is interupt op code
	mov ah,BYTE[INTERUPT_NO_ADDRESS]	;
	mov word[int_opcode],ax				;this block is for simulat interupt code becuse we can not use "int nn" instruction directive
jmp_not_change:							;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
	CMP_BIT BYTE[IS_SEG_ADDRESS],00000001b	;
	jnz not_load_ds							;if [IS_SEG_ADDRESS]=00000001b then ds register not must loaded with new value
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov ax, [SEG_REGS_ADDRESS+00]			;ds
	mov ds,ax
not_load_ds:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
	CMP_BIT BYTE[IS_SEG_ADDRESS],00000010b	;
	jnz not_load_ss							;if [IS_SEG_ADDRESS]=00000010b then ss register not must loaded with new value
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov ax, [SEG_REGS_ADDRESS+02]			;ss	
	mov ss,ax
not_load_ss:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
	CMP_BIT BYTE[IS_SEG_ADDRESS],00000100b	;
	jnz not_load_es							;if [IS_SEG_ADDRESS]=00000100b then es register not must loaded with new value
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov ax, [SEG_REGS_ADDRESS+04]			;es	
	mov es,ax
not_load_es:
;	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
;	CMP_BIT BYTE[IS_SEG_ADDRESS],00001000b	;
;	jnz not_load_cs							;if [IS_SEG_ADDRESS]=00001000b then cs register not must loaded with new value
;	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	CHG_CS WORD[SEG_REGS_ADDRESS+06]		;cs	
;not_load_cs:

	mov ax, [IN_OUT_REGS_ADDRESS+00]	;ax	
	mov bx, [IN_OUT_REGS_ADDRESS+02]	;bx
	mov cx, [IN_OUT_REGS_ADDRESS+04]	;cx
	mov dx, [IN_OUT_REGS_ADDRESS+06]	;dx
	mov si, [IN_OUT_REGS_ADDRESS+08]	;si
	mov di, [IN_OUT_REGS_ADDRESS+10]	;di
;	pop word[IN_OUT_REGS_ADDRESS+12]	;flags
;	pushfw
int_opcode:	dw 0xCC00
	mov  [IN_OUT_REGS_ADDRESS+00],ax	;ax	
	mov  [IN_OUT_REGS_ADDRESS+02],bx	;bx
	mov  [IN_OUT_REGS_ADDRESS+04],cx	;cx
	mov  [IN_OUT_REGS_ADDRESS+06],dx	;dx
	mov  [IN_OUT_REGS_ADDRESS+08],si	;si
	mov  [IN_OUT_REGS_ADDRESS+10],di	;di
	pushfw								;flags
	pop word[IN_OUT_REGS_ADDRESS+12]	;""""	
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	cli                     ; Disable interrupts, we want to be alone	;
    xor ax, ax															;
    mov ds, ax              ; Set DS-register to 0 - used by lgdt		;
    lgdt [gdt_desc]         ; Load the GDT descriptor					;
    mov eax, cr0            ; Copy the contents of CR0 into EAX			; gobak to protected mode
    or eax, 1               ; Set bit 0									;
    mov cr0, eax            ; Copy the contents of EAX into CR0			;    
    jmp SEL_32CODE:clear_pipe_goback									;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Real Mode subroutine(putc charachter);;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; name:			putc										;
; action:		displays one character on stdout			;
; in:			character in AL. [_dos] must be set			;
; out:			(nothing)									;
; modifies:		(nothing)									;
; minimum CPU:		8088									;
; notes:													;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

putc:
	push ax		
		push bx
			mov ah,0Eh	; INT 10h: teletype output
			xor bx,bx	; video page 0
			int 10h
		pop bx
	pop ax
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Real Mode subroutine(puts string);;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; name:			puts										;
; action:		displays text on screen or stdout			;
; in:			0-terminated string at DS:SI				;
; out:			(nothing)									;
; modifies:		(nothing)									;
; minimum CPU:		8088									;
; notes:													;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

puts:
	push si
	push ax
		cld
		jmp short puts_2
puts_1:
		call putc
puts_2:
		lodsb
		or al,al
		jne puts_1
	pop ax
	pop si
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Data Section;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_read_kernel_msg:
	db	"kernel is reading:";, 10, 13, 0
;_runing_msg:
;	db	"kernel is runing!", 10, 13, 0
_error_occur_msg:
;	db	"Occur Error in reading kernel ", 10, 13, 0
;_cpu_msg:
;	db "32-bit CPU required", 10, 13, 0
Total_Sector dw 0


_real_idt:
	dw 1023
	dd 0

_real_ip:
	dw real
_real_cs:
	dw 0	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;preparing 32 bits protected mode;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
[BITS 32]
clear_pipe:
        mov ax, SEL_32DATA					; Save data segment identifyer
        mov ds, ax						    ; Move a valid data segment into the data segment register
        mov ss, ax							; Move a valid data segment into the stack segment register Note ds is equal to 2*8=16D or 10H;8 is size of one global descriptor of TABLE(GDT)                
        mov esp, 090000h					; Move the stack pointer to 090000h
        jmp SEL_32CODE:KERNEL_LINER_ADDRESS ; Jump to section 08h (code), offset 01000h

clear_pipe_goback:
        mov ax, SEL_32DATA   
        mov ds, ax           
        mov ss, ax           
        mov esp, 090000h           
        JMP_F SEL_32CODE,PMODE_32BIT_ADDRESS
gdt:							; Address for the GDT
gdt_null:						; Null Segment
        dd 0
        dd 0
gdt_code:			            ; Code segment, read/execute, nonconforming
        dw 0FFFFh
        dw 0
        db 0
        db 10011010b
        db 11001111b
        db 0
gdt_data:					    ; Data segment, read/write, expand down
        dw 0FFFFh
        dw 0
        db 0
        db 10010010b
        db 11001111b
        db 0        
gdt_end:						; Used to calculate the size of the GDT
gdt_desc:                       ; The GDT descriptor
        dw gdt_end - gdt - 1    ; Limit (size)
        dd gdt                  ; Address of the GDT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Sector signacuer;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
times 510-($-$$) db 0           ; Fill up the file with zeros
       dw 0AA55h                ; Boot sector identifyer

