#include <system.h>

static unsigned long TASK0_STACK[256] = {0xf};

struct TASK_STRUCT TASK0 = {
	/* tss */
	{	/* back_link */
		0,
		/* esp0                                    ss0 */
		(unsigned)&TASK0_STACK+sizeof TASK0_STACK, DATA_SEL, 
		/* esp1 ss1 esp2 ss2 */
		0, 0, 0, 0, 
		/* cr3 */
		0, 
		/* eip eflags */
		0, 0, 
		/* eax ecx edx ebx */
		0, 0, 0, 0,
		/* esp ebp */
		0, 0,
		/* esi edi */
		0, 0, 
		/* es          cs             ds */
		USER_DATA_SEL, USER_CODE_SEL, USER_DATA_SEL, 
		/* ss          fs             gs */
		USER_DATA_SEL, USER_DATA_SEL, USER_DATA_SEL, 
		/* ldt */
		0x20,
		/* trace_bitmap */
		0x00000000},	
	/* tss_entry */
	{	//limit
		0,
		//base0_15
		0,
		//base16_23
		0,
		//attrs0_7
		0,
		//attrs8_15
		0,
		//base24_31
		0},
	/* idt[2] */
	{DEFAULT_LDT_CODE, DEFAULT_LDT_DATA},
	/* idt_entry */
	{	//limit
		0,
		//base0_15
		0,
		//base16_23
		0,
		//attrs0_7
		0,
		//attrs8_15
		0,
		//base24_31
		0},
	/* state */
	TS_RUNNING,
	/* priority */
	INITIAL_PRIO,
	/* next */
	0,
};

struct dt_entry set_tss(int num, unsigned long tss) 
{
	unsigned long base=tss,  limit=0;
	unsigned char access=0x89, gran=0x80;

	gdt_set_gate(num, base, limit, access, gran);	
	return gdt[num];//= tss_entry;
}

struct dt_entry set_ldt(int num, unsigned long ldt)
{
	unsigned long base=ldt,  limit=0x0F;
	unsigned char access=0x82, gran=0x80;	

	gdt_set_gate(CURR_TASK_LDT, base, limit, access, gran);	
	return gdt[num];
}

struct dt_entry get_tss(void) 
{
	return gdt[CURR_TASK_TSS];
}

struct dt_entry get_ldt(void) {
	return gdt[CURR_TASK_LDT];
}

struct TASK_STRUCT *current = &TASK0;

void scheduler(void) {
	struct TASK_STRUCT *v = &TASK0, *tmp = 0;
	int cp = current->priority;

	for (; v; v = v->next) {
		if ((v->state==TS_RUNABLE) && (cp>v->priority)) {
			tmp = v;
			cp = v->priority;
		}
	}
	if (tmp && (tmp!=current)) {
		current->tss_entry = get_tss();
		current->ldt_entry = get_ldt();
		tmp->tss_entry = set_tss(CURR_TASK_TSS,(unsigned long long)((unsigned int)&tmp->tss));
		tmp->ldt_entry = set_ldt(CURR_TASK_LDT,(unsigned long long)((unsigned int)&tmp->ldt));
		current->state = TS_RUNABLE;
		tmp->state = TS_RUNNING;
		current = tmp;
		__asm__ __volatile__("ljmp	$" TSS_SEL_STR ",	$0\n\t");
	}
}




