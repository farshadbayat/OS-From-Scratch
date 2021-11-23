#include <system.h>

/* Our GDT, with 3(TOTAL_GDT) entries, and finally our special GDT pointer */
struct dt_entry gdt[TOTAL_GDT];
struct gdt_ptr gp;

/* This is in start.asm. We use this to properly reload
*  the new segment registers */
extern void gdt_flush();

/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
    /* Setup the descriptor base address */
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

/* Should be called by main. This will setup the special GDT
*  pointer, set up the first 3(TOTAL_GDT) entries in our GDT, and then
*  finally call gdt_flush() in our assembler file in order
*  to tell the processor where the new GDT is and update the
*  new segment registers */
void GDT_Install()
{
    /* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct dt_entry) * TOTAL_GDT) - 1;
    gp.base = (unsigned int) &gdt;/// errorrrrrrrrrrrrrrr1

    /* Our NULL descriptor */
    gdt_set_gate(GDT_NULL, 0x000000000, 0x000000000, 0x00, 0x00);

    /* The second entry is our Code Segment. The base address
    *  is 0, the limit is 4GBytes, it uses 4KByte granularity,
    *  uses 32-bit opcodes, and is a Code Segment descriptor.
    *  Please check the table above in the tutorial in order
    *  to see exactly what each value means */
    gdt_set_gate(GDT_TEXT, 0x000000000, 0xFFFFFFFF, 0x9A, 0xCF);

    /* The third entry is our Data Segment. It's EXACTLY the
    *  same as our code segment, but the descriptor type in
    *  this entry's access byte says it's a Data Segment */
    gdt_set_gate(GDT_DATA, 0x000000000, 0xFFFFFFFF, 0x92, 0xCF);

	gdt_set_gate(GDT_USER_DATA, 0x000000000, 0x0000FFFFF, 0xF2, 0xCF);//USER_DATA_SEL
	gdt_set_gate(GDT_USER_CODE, 0x000000000, 0x0000FFFFF, 0xFA, 0xCF);//USER_CODE_SEL

	gdt_set_gate(5, 0x000000000, 0x0000FFFFF, 0x92, 0xCF);//LINEAR_DATA_SEL	
	gdt_set_gate(6, 0x000000000, 0x0000FFFFF, 0x9A, 0xCF);//LINEAR_CODE_SEL

	gdt_set_gate(7, 0x000000000, 0x0000FFFFF, 0x92, 0);//_16BIT_DATA_SEL	
	gdt_set_gate(8, 0x000000000, 0x0000FFFFF, 0x9A, 0);//_16BIT_CODE_SEL

	gdt_set_gate(9, 0x000000000, 0x0000FFFF, 0x89, 0);//tss

	gdt_set_gate(10, 0x000000000, 0x000FFFFF, 0x92, 0x8F);//flat	

	gdt_set_gate(11, 0x000000400, 0x00000300-1,0xF2, 0x00);//BOIS	


    gdt_flush();
	kprintf("GDT(Global Description Table) Driver Is Install\t\t[ OK ]");	   
}

// Converts a selector corresponding to GDT segment to a base 32-bit address
unsigned long gdt_sel_to_base (unsigned long _sel) {
  unsigned long base;
  GDT_REG gdtr;
  SEG_DESCRIPTOR descr;

  SGDT (&gdtr);
  movedata (flat_sel, gdtr.base + (_sel & 0xFFFFFFF8),data_sel, (unsigned long) &descr, sizeof(SEG_DESCRIPTOR));
  base = descr.base0_15 + (descr.base16_23<<16) + (descr.base24_31<<24);

  return base;
}

// Converts a selector corresponding to GDT segment to a 32-bit limit
unsigned long gdt_sel_to_limit (unsigned long _sel) {
  unsigned long limit;
  GDT_REG gdtr;
  SEG_DESCRIPTOR descr;

  SGDT (&gdtr);
  movedata (flat_sel, gdtr.base + (_sel & 0xFFFFFFF8), data_sel, (unsigned long) &descr, sizeof(SEG_DESCRIPTOR));

  limit = descr.limit + ((unsigned int)(descr.attrs8_15 & 0xF)<<16);
 // printf("\ndescr.limit:	%X	%X	%X\n",descr.limit,((unsigned int)(descr.attrs8_15 & 0xF)<<16),limit);
  if ((descr.attrs8_15 & ATTR_GRANUL) == ATTR_GRANUL) {
    limit<<=12;
    limit|=0xFFF;
  };

  return limit;
}

// Converts a selector corresponding to GDT segment to a 8-bit access
unsigned long gdt_sel_to_access (unsigned long _sel) {
  unsigned char access;
  GDT_REG gdtr;
  SEG_DESCRIPTOR descr;

  SGDT (&gdtr);
  movedata (flat_sel, gdtr.base + (_sel & 0xFFFFFFF8), data_sel, (unsigned long) &descr, sizeof(SEG_DESCRIPTOR));

  access = descr.attrs0_7;
  return access;
}

// Converts a selector corresponding to GDT segment to a 8-bit granularity
unsigned long gdt_sel_to_gran (unsigned long _sel) {
  unsigned char gran;
  GDT_REG gdtr;
  SEG_DESCRIPTOR descr;

  SGDT (&gdtr);
  movedata (flat_sel, gdtr.base + (_sel & 0xFFFFFFF8), data_sel, (unsigned long) &descr, sizeof(SEG_DESCRIPTOR));

  gran = descr.attrs8_15;
  return gran;
}

unsigned long long GDT_TO_LONG(struct dt_entry gdt_)
{
	unsigned long long gdt_entry = 0x0000000000000000ULL;
	gdt_entry |=gdt_.base_high;//for 8bit base high

	gdt_entry <<=4;
	gdt_entry |=(gdt_.granularity & 0xF0);//for 4bit granularity
	
	gdt_entry <<=4;
	gdt_entry |=(gdt_.granularity & 0x0F);//for 4bit limit high
	
	gdt_entry <<=8;
	gdt_entry |=gdt_.access;//for 8bit access high
	
	gdt_entry <<=8;
	gdt_entry |=gdt_.base_middle;//for 8bit Base Middle

	gdt_entry <<=16;
	gdt_entry |=gdt_.base_low;//for 16bit Base Low

	gdt_entry <<=16;
	gdt_entry |=gdt_.limit_low;//for 16bit Limit Low

	return gdt_entry;
}

struct dt_entry LONG_TO_GDT(unsigned long long gdt_)
{
	struct dt_entry gdt_type;
	
	// Setup the descriptor base address 
	gdt_type.base_low = (unsigned short)((gdt_ & 0x00000000FFFF0000)>>16);
    gdt_type.base_middle = (unsigned char)((gdt_ & 0x000000FF00000000)>>32);
    gdt_type.base_high =(unsigned char)((gdt_ & 0xFF00000000000000)>>56);

    // Setup the descriptor limits 
    gdt_type.limit_low =(unsigned short)(gdt_ & 0x000000000000FFFF);
    gdt_type.granularity =(unsigned char)((gdt_ & 0x00FF000000000000)>>48);//set up the high granularity and low limit field

    // Finally, set up the access flags    
    gdt_type.access = ((gdt_ & 0x0000FF0000000000)>>40);

	return gdt_type;
}

