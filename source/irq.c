/// <summary>		
	///Interrupt Requests or IRQs are interrupts that are raised by hardware devices.
	///Some devices generate an IRQ when they have data ready to be read, or when they finish a command like writing a buffer to disk, for example. 
	///It's safe to say that a device will generate an IRQ whenever it wants the processor's attention. IRQs are generated by everything from network cards and sound cards to your mouse, keyboard, and serial ports.
/// </summary>

#include <system.h>
#include <conio.h>
#include <io.h>
#include <stdio.h>


//#define IRQ_DEBUG
/// <summary>
	///These are own ISRs that point to our special IRQ handler
	///instead of the regular 'fault_handler' function 
/// </summary>

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/* This array is actually an array of function pointers. We use
*  this to handle custom IRQ handlers for a given IRQ */
void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* This installs a custom IRQ handler for the given IRQ */
void irq_install_handler(int irq, void (*handler)(struct regs *r))
{
    irq_routines[irq] = handler;
}

/* This clears the handler for a given IRQ */
void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
void irq_remap(void)
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}
 void asm_inthandler2c(void);
/* We first remap the interrupt controllers, and then we install
*  the appropriate ISRs to the correct entries in the IDT. This
*  is just like installing the exception handlers */
void IRQ_Install()
{
    irq_remap();

    idt_set_gate(32, (unsigned)irq0 , 0x08, 0x8E);
    idt_set_gate(33, (unsigned)irq1 , 0x08, 0x8E);
    idt_set_gate(34, (unsigned)irq2 , 0x08, 0x8E);
    idt_set_gate(35, (unsigned)irq3 , 0x08, 0x8E);
    idt_set_gate(36, (unsigned)irq4 , 0x08, 0x8E);
    idt_set_gate(37, (unsigned)irq5 , 0x08, 0x8E);
    idt_set_gate(38, (unsigned)irq6 , 0x08, 0x8E);
    idt_set_gate(39, (unsigned)irq7 , 0x08, 0x8E);

    idt_set_gate(40, (unsigned)irq8 , 0x08, 0x8E);
    idt_set_gate(41, (unsigned)irq9 , 0x08, 0x8E);
    idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
	kprintf("IRQ(Interrupt Requests) Driver Is Install\t\t[ OK ]");
}

/* Each of the IRQ ISRs point to this function, rather than
*  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
*  to be told when you are done servicing them, so you need
*  to send them an "End of Interrupt" command (0x20). There
*  are two 8259 chips: The first exists at 0x20, the second
*  exists at 0xA0. If the second controller (an IRQ from 8 to
*  15) gets an interrupt, you need to acknowledge the
*  interrupt at BOTH controllers, otherwise, you only send
*  an EOI command to the first controller. If you don't send
*  an EOI, you won't raise any more IRQs */
void irq_handler(struct regs *r)
{
    /* This is a blank function pointer */
    void (*handler)(struct regs *r);	
    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
	short x,y,z;
	x=wherex();
	y=wherey();
	gotoxy(80-10,1);
if(r->int_no!=0x20)	printf("Int=%d\n",r->int_no-32);
for(z=0;z<9000;z++);
	gotoxy(x,y);
    handler = irq_routines[r->int_no - 32];	
    if (handler)
    {		
       handler(r);
    }


    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (r->int_no >= 40)
    {
        outportb(0xA0, 0x20);
		outportb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
	outportb(0x20, 0x20);
	 
#ifdef IRQ_DEBUG
	if(r->int_no > 32)
	{
		cprintf(LIGHTRED,"\nRecive IRQ %d And Run IRQ Routines %d ",r->int_no,r->int_no - 32);
		cprintf(LIGHTBLUE,"\nIRQ MAPED ROUTINES:\n");
		cprintf(YELLOW,"%-8X ,%-8X ,%-8X ,%-8X ,%-8X ,%-8X ,%-8X ,%-8X \n",irq_routines[0],irq_routines[1],irq_routines[2],irq_routines[3],irq_routines[4],irq_routines[5],irq_routines[6],irq_routines[7]);
		cprintf(YELLOW,"%-8X ,%-8X ,%-8X ,%-8X ,%-8X ,%-8X ,%-8X ,%-8X \n",irq_routines[8],irq_routines[9],irq_routines[10],irq_routines[11],irq_routines[12],irq_routines[13],irq_routines[14],irq_routines[15]);
	}	
#endif

}
