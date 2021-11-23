;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Template Place To Test;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "nasm.inc"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;16-BITS Entering point;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SECTION .text
[BITS 32]


global start
start: 		
		call A20_ENABLE
		mov DWORD[PMODE_32BIT_ADDRESS],gobak_pmode		
		jmp clear_pipe
stublet:	
		cli							; Disable interrupts, we want to be alone
        xor ax, ax
        mov ds, ax					; Set DS-register to 0 - used by lgdt
        lgdt [gdt_desc]				; Load the GDT descriptor
        mov eax, cr0				; Copy the contents of CR0 into EAX
        or eax, 1					; Set bit 0
        mov cr0, eax				; Copy the contents of EAX into CR0
        
        jmp SEL_32CODE:clear_pipe   ; Jump to code segment, offset clear_pipe
        							;To Understand why code segement is 08 go to help(Bona Fide OS Development - Protected Mode.htm)
        							;becuse of cs is index to the descriptor register and gdt_code is just after gdt_null and each
        							;gdt is 8byte therefore cs=number of gdt(code)*8

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Preparing To Go Real Mode;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        					
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	 return to real mode procedure, modified from section 14.5 of 386INTEL.TXT
;	 1. Disable interrupts (already done)
;	 2. If paging is enabled... (it isn't)
;	 3. Jump to a segment with limit 64K-1. CS must have this limit
;	 before you return to real mode.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
[BITS 32]                       ; We now need 32-bit instructions
ds_content:
dw	0
cs_content:
dw	0
es_content:
dw	0
ss_content:
dw	0
esp_content:
dd	0
ebp_content:
dd	0

global _realmode
_realmode:	
	mov  [ebp_content],ebp
	mov  ax,ds
	mov  [ds_content],ax	
	mov  ax,cs
	mov  [cs_content],ax
	mov  ax,es
	mov  [es_content],ax	
	mov  ax,ss
	mov  [ss_content],ax
	mov  [esp_content],esp	
	JMP_F SEL_16CODE,PMODE_16BIT_ADDRESS	
gobak_pmode:
	mov  ebp,[ebp_content]	
	mov  esp,[esp_content]	
	mov  ax,[es_content]
	mov  es,ax			
	mov  ax,[ss_content]
	mov  ss,ax	
	mov  ax,[ds_content]
	mov  ds,ax	
	CHG_CS DWORD[cs_content]	
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Clear Pipeline;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

clear_pipe:
        mov ax, SEL_32DATA      ; Save data segment identifyer
        mov ds, ax              ; Move a valid data segment into the data segment register
        mov ss, ax              ; Move a valid data segment into the stack segment register
        						;Note ds is equal to 2*8=16D or 10H;8 is size of one global descriptor of TABLE(GDT)     
        mov esp, _sys_stack     ; This points the stack to our new stack area
	    extern _StartCKernel	    
		call _StartCKernel
		jmp $

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Initial PM Struct(TEMP);;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

gdt:							; Address for the GDT
gdt_null:						; Null Segment
        dd 0
        dd 0
gdt_code:						; Code segment, read/execute, nonconforming
        dw 0FFFFh
        dw 0
        db 0
        db 10011010b
        db 01001111b
        db 0
gdt_data:						; Data segment, read/write, expand down
        dw 0FFFFh
        dw 0
        db 0
        db 10010010b
        db 01001111b
        db 0      
gdt_end:						; Used to calculate the size of the GDT
gdt_desc:                       ; The GDT descriptor
        dw gdt_end - gdt - 1    ; Limit (size)
        dd gdt                  ; Address of the GDT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Global Function Load New GDT;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;        
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This will set up our new segment registers. We need to do
; something special in order to set CS. We do what is called a
; far jump. A jump that includes a segment as well as an offset.
; This is declared in C as 'extern void gdt_flush();'
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global _gdt_flush
extern _gp
_gdt_flush:
    lgdt [_gp]
    mov ax, SEL_32DATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp SEL_32CODE:flush2
flush2:
    ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Global Function Load New IDT;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;        

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Loads the IDT defined in '_idtp' into the processor.
; This is declared in C as 'extern void idt_load();'
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _idt_load
extern _idtp
_idt_load:
    lidt [_idtp]
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Global Function ISR0..31;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; In just a few pages in this tutorial, we will add our Interrupt
; Service Routines (ISRs) right here!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global _isr0 , _isr1 , _isr2 , _isr3 , _isr4 , _isr5 , _isr6 , _isr7
global _isr8 , _isr9 , _isr10, _isr11, _isr12, _isr13, _isr14, _isr15
global _isr16, _isr17, _isr18, _isr19, _isr20, _isr21, _isr22, _isr23
global _isr24, _isr25, _isr26, _isr27, _isr28, _isr29, _isr30, _isr31

_isr0:	;  0: Divide By Zero Exception
    cli
    push byte 0
    push byte 0
    jmp isr_common_stub

_isr1:	;  1: Debug Exception
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

_isr2:	;  2: Non Maskable Interrupt Exception
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

_isr3:	;  3: Int 3 Exception
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

_isr4:	;  4: INTO Exception
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

_isr5:	;  5: Out of Bounds Exception
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

_isr6:	;  6: Invalid Opcode Exception
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

_isr7:	;  7: Coprocessor Not Available Exception
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub

_isr8:	;  8: Double Fault Exception (With Error Code!)
    cli
    push byte 8
    jmp isr_common_stub

_isr9:	;  9: Coprocessor Segment Overrun Exception
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

_isr10:	; 10: Bad TSS Exception (With Error Code!)
    cli
    push byte 10
    jmp isr_common_stub

_isr11:	; 11: Segment Not Present Exception (With Error Code!)
    cli
    push byte 11
    jmp isr_common_stub

_isr12:	; 12: Stack Fault Exception (With Error Code!)
    cli
    push byte 12
    jmp isr_common_stub

_isr13:	; 13: General Protection Fault Exception (With Error Code!)
    cli
    push byte 13
    jmp isr_common_stub

_isr14:	; 14: Page Fault Exception (With Error Code!)
    cli
    push byte 14
    jmp isr_common_stub

_isr15:	; 15: Reserved Exception
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

_isr16:	; 16: Floating Point Exception
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

_isr17:	; 17: Alignment Check Exception
    cli
    push byte 0
    push byte 17
    jmp isr_common_stub

_isr18:	; 18: Machine Check Exception
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

_isr19:	; 19: Reserved
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

_isr20:	; 20: Reserved
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

_isr21:	; 21: Reserved
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub

_isr22:	; 22: Reserved
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

_isr23:	; 23: Reserved
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

_isr24:	; 24: Reserved
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

_isr25:	; 25: Reserved
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

_isr26:	; 26: Reserved
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

_isr27:	; 27: Reserved
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

_isr28:	; 28: Reserved
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

_isr29:	; 29: Reserved
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub

_isr30:	; 30: Reserved
    cli
    push byte 0
    push byte 30
    jmp isr_common_stub

_isr31:	; 31: Reserved
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Global Function Comman ISR;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; We call a C function in here. We need to let the assembler know
; that '_fault_handler' exists in another file
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
extern _fault_handler

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, _fault_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Global Function IRQ0..15;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global _irq0, _irq1, _irq2 , _irq3 , _irq4 , _irq5 , _irq6 , _irq7
global _irq8, _irq9, _irq10, _irq11, _irq12, _irq13, _irq14, _irq15

_irq0:	; 32: IRQ0	
    cli
    push byte 0
    push byte 32
    jmp irq_common_stub

_irq1:	; 33: IRQ1 [KeyBoard IRQ]
    cli
    push byte 0
    push byte 33
    jmp irq_common_stub

_irq2:	; 34: IRQ2
    cli
    push byte 0
    push byte 34
    jmp irq_common_stub

_irq3:	; 35: IRQ3
    cli
    push byte 0
    push byte 35
    jmp irq_common_stub

_irq4:	; 36: IRQ4
    cli
    push byte 0
    push byte 36
    jmp irq_common_stub

_irq5:	; 37: IRQ5
    cli
    push byte 0
    push byte 37
    jmp irq_common_stub

_irq6:	; 38: IRQ6
    cli
    push byte 0
    push byte 38
    jmp irq_common_stub

_irq7:	; 39: IRQ7
    cli
    push byte 0
    push byte 39
    jmp irq_common_stub

_irq8:	; 40: IRQ8
    cli
    push byte 0
    push byte 40
    jmp irq_common_stub

_irq9:	; 41: IRQ9
    cli
    push byte 0
    push byte 41
    jmp irq_common_stub

_irq10:	; 42: IRQ10
    cli
    push byte 0
    push byte 42
    jmp irq_common_stub

_irq11:	; 43: IRQ11
    cli
    push byte 0
    push byte 43
    jmp irq_common_stub

_irq12:	; 44: IRQ12 [Mouse IRQ]
    cli
    push byte 0
    push byte 44
    jmp irq_common_stub

_irq13:	; 45: IRQ13
    cli
    push byte 0
    push byte 45
    jmp irq_common_stub

_irq14:	; 46: IRQ14
    cli
    push byte 0
    push byte 46
    jmp irq_common_stub

_irq15:	; 47: IRQ15
    cli
    push byte 0
    push byte 47
    jmp irq_common_stub

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Global Function Comman IRQ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

extern _irq_handler

irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, SEL_32DATA		;0x10		
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp

    push eax
    mov eax, _irq_handler
    call eax
    pop eax

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Global Function Other ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  ENABLE A20 LINE AND DISABLE INTS				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
A20_ENABLE:	
		cli
		call .discard_out
		call .command_wait
		mov  al,0xd1
		out  0x64,al
		call .command_wait
		mov  al,0xdf
		out  0x60,al
		call .command_wait
		jmp  .done

.command_wait:	in al,0x64
		test al,0x02
		jnz  .command_wait
		ret
.discard_out:	in al,0x64
		test al,0x01
		jnz  .read
		ret
.read:	in  al,0x60
		jmp .discard_out
.done:
		ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Function To reterive registers in C
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global __AX,__BX,__CX,__DX,__EAX,__EBX,__ECX,__EDX,__EDI,__ESI,__ESP,__ES,__DS,__CS,__SS
__AX:    ;; int _AX(void);    
	;MOV    EDX,[ESP+4]
     AND    EAX,0xffff
    ;pop    EAX
    RET
__BX:    ;; int _BX(void);    
	;MOV    EDX,[ESP+4]
    MOV    EAX,0
    MOV    AX,BX
    ;pop    EAX
    RET
__CX:    ;; int _CX(void);    
	;MOV    EDX,[ESP+4]
    MOV    EAX,0
    MOV    AX,CX
    ;pop    EAX
    RET
__DX:    ;; int _DX(void);    
	;MOV    EDX,[ESP+4]
    MOV    EAX,0
    MOV    AX,DX
    ;pop    EAX
    RET
__EAX:    ;; int _EAX(void);    
	;MOV    EDX,[ESP+4]
    ;MOV    EAX,0
    ;pop    EAX
    RET
__EBX:    ;; int _EBX(void);    
	;MOV    EDX,[ESP+4]
    ;MOV    EAX,0
    MOV    EAX,EBX
    ;pop    EAX
    RET
__ECX:    ;; int _ECX(void);    
	;MOV    EDX,[ESP+4]
    ;MOV    EAX,0
    MOV    EAX,ECX
    ;pop    EAX
    RET
__EDX:    ;; int _EDX(void);    
	;MOV    EDX,[ESP+4]
    ;MOV    EAX,0
    MOV    EAX,EDX
    ;pop    EAX
    RET
__EDI:    ;; int _EDI(void);    
	;MOV    EDX,[ESP+4]
    ;MOV    EAX,0
    MOV    EAX,EDI
    ;pop    EAX
    RET    
__ESI:    ;; int _ESI(void);    
	;MOV    EDX,[ESP+4]
    ;MOV    EAX,0
    MOV    EAX,ESI
    ;pop    EAX
    RET            
__ESP:    ;; int _ESP(void);    
	;MOV    EDX,[ESP+4]
    ;MOV    EAX,0
    MOV    EAX,ESP
    ;pop    EAX
    RET    
    
__ES:    ;; int _ES(void);    
	;MOV    EDX,[ESP+4]
    MOV    EAX,0
    MOV    EAX,ES
    ;pop    EAX
    RET
__DS:    ;; int _DS(void);    
	;MOV    EDX,[ESP+4]
    MOV    EAX,0
    MOV    EAX,DS
    ;pop    EAX
    RET
__CS:    ;; int _CS(void);    
	;MOV    EDX,[ESP+4]
    MOV    EAX,0
    MOV    EAX,CS
    ;pop    EAX
    RET
__SS:    ;; int _SS(void);    
	;MOV    EDX,[ESP+4]
    MOV    EAX,0
    MOV    EAX,SS    
    RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;int pop16(); in c prototype
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global _pop16

_pop16:    ;; int pop16(void);    
	;MOV    EDX,[ESP+4]
    ;MOV    EAX,0
    ;pop    EAX
    RET
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;IO Function
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global _inp8 ,_inp16 ,_inp32 ,_outp8 ,_outp16 ,_outp32 ,_load_eflags ,_store_eflags ,_load_cr0 ,_store_cr0, _memtest_sub ,_GetCurrentEFLAGS

_inp8:		;; int io_in8(int port);
    MOV  EDX,[ESP+4]
    MOV  EAX,0
    IN   AL,DX
    RET
    
_inp16:		;; int io_in16(int port);
    MOV  EDX,[ESP+4]
    MOV  EAX,0
    IN   AX,DX
    RET

_inp32:		;; int io_in32(int port);
    MOV  EDX,[ESP+4]
    IN   EAX,DX
    RET

_outp8:		;; void io_out8(int port, int data);
    MOV  EDX,[ESP+4] 
    MOV  AL,[ESP+8]  
    OUT  DX,AL
    RET

_outp16:	;; void io_out16(int port, int data);
    MOV  EDX,[ESP+4]  
    MOV  EAX,[ESP+8]  
    OUT  DX,AX
    RET

_outp32:	;; void io_out32(int port, int data);
    MOV  EDX,[ESP+4]  
    MOV  EAX,[ESP+8]  
    OUT  DX,EAX
    RET    

_load_eflags: ;; int load_eflags(void);
    PUSHFD            
    POP    EAX        
    RET               

_store_eflags: ;; void store_eflags(int eflags);
    MOV    EAX,[ESP+4] 
    PUSH   EAX         
    POPFD              
    RET    
    
 _load_cr0:    ;; int load_cr0(void);
    MOV    EAX,CR0     
    RET

_store_cr0:    ;; void store_cr0(int cr0);
    MOV    EAX,[ESP+4] 
    MOV    CR0,EAX
    RET 
    
_memtest_sub:  ;; unsigned int memtest_sub(unsigned int start, unsigned int end)
    PUSH  EDI  
    PUSH  ESI
    PUSH  EBX
    MOV    ESI,0xaa55aa55      ; pat0 = 0xaa55aa55;
    MOV    EDI,0x55aa55aa      ; pat1 = 0x55aa55aa;
    MOV    EAX,[ESP+12+4]      ; i = start;
mts_loop:
    MOV    EBX,EAX
    ADD    EBX,0xffc        ; p = i + 0xffc;
    MOV    EDX,[EBX]        ; old = *p;
    MOV    [EBX],ESI        ; *p = pat0;
    XOR    DWORD [EBX],0xffffffff  ; *p ^= 0xffffffff;
    CMP    EDI,[EBX]        ; if (*p != pat1) goto fin;
    JNE    mts_fin
    XOR    DWORD [EBX],0xffffffff  ; *p ^= 0xffffffff;
    CMP    ESI,[EBX]        ; if (*p != pat0) goto fin;
    JNE    mts_fin
    MOV    [EBX],EDX        ; *p = old;
    ADD    EAX,0x1000       ; i += 0x1000;
    CMP    EAX,[ESP+12+8]   ; if (i <= end) goto mts_loop;
    JBE    mts_loop
    POP    EBX
    POP    ESI
    POP    EDI
    RET
mts_fin:
    MOV    [EBX],EDX        ; *p = old;
    POP    EBX
    POP    ESI
    POP    EDI
    RET

; Return current contents of eflags register.
align 16
_GetCurrentEFLAGS:
	pushfd				; push eflags
	pop	eax				; pop contents into eax
	ret 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Global Function Multi Task ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    
;global _task1_run, _task2_run 	;
;extern _do_task1, _do_task2 	;
;
;_task1_run:
;	call	_do_task1
;	jmp		_task1_run
;	
;_task2_run:
;	call	_do_task2
;	jmp		_task2_run

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;32-BITS Global Function ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    
global _pnp_bios_call_asm


_pnp_bios_call_asm:    ;; int pnp_bios_call_asm(int func, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int *pnp_thunk_entrypoint);
	cli
    push ebp
    push edi
    push esi
    push ds
    push es
    push fs
    push gs
    pushf

    mov eax, [ESP+8]
    shl eax, 16
    or eax, [ESP+4]

    mov ebx, [ESP+16]
    shl ebx, 16
    or ebx, [ESP+12]

    mov ecx, [ESP+24]
    shl ecx, 16
    or ecx, [ESP+20]

    mov edx, [ESP+32]
    shl edx, 16
    or edx, [ESP+28]
	
    call [ESP+36]	;pnp_bios_thunk[]

    popf
    pop gs
    pop fs
    pop es
    pop ds
    pop esi
    pop edi
    pop ebp

    sti
    and eax, 0x0000FFFF
    ;mov [status], eax
    RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Data Section Constant Varibale;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;Stack Section Inconstant Varibale;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	  	
; Here is the definition of our BSS section. Right now, we'll use
; it just to store the stack. Remember that a stack actually grows
; downwards, so we declare the size of the data before declaring
; the identifier '_sys_stack'
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SECTION .bss
    resb 2097152               ; This reserves 2MBytes of memory here
_sys_stack:


