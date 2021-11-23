#include <string.h>
#include <math.h>
#include <system.h>
#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <time.h>
#include <mmu.h>
#include <keyboard.h>
#include <sheet.h>
#include <fat.h>
#include <fs.h>
#include <hddio.h>
#include <mouse.h>
#include <io.h>
#include <timer.h>
#include <console.h>
#include <sound.h>
#include <serial.h>
#include <pnp.h>
#include <dev.h>
#include <pcnet32.h>
#include <draw2d.h>
#include <draw3d.h>
#include <component.h>
#include <ImageBmp.h>
#include <component1.h>



char *ConfigStr="#\n"
"# krnl.ini\n"
"#\n"
"# Copyright (c) 2001 Michael Ringgaard. All rights reserved.\n"
"#\n"
"# Kernel configuration\n"
"#\n"
"\n"
"[kernel]\n"
"libpath=/bin\n"
"keyboard=us\n"
"\n"
"[bindings]\n"
"pci unit 10222000=pcnet32.sys\n";

///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
////////////////////
static long task1_stack0[1] = {0xf, };
static long task1_stack3[1] = {0xf, };
static long task2_stack0[1] = {0xf, };
static long task2_stack3[1] = {0xf, };





static void new_task(struct TASK_STRUCT *task, unsigned int eip, unsigned int stack0, unsigned int stack3) {
	memcpy(task, &TASK0, sizeof(struct TASK_STRUCT));
	task->tss.esp0 = stack0;
	task->tss.eip = eip;
	task->tss.eflags = 0x3202;
	task->tss.esp = stack3;

	task->priority = INITIAL_PRIO;

	task->state = TS_STOPPED;
	task->next = current->next;
	current->next = task;
	task->state = TS_RUNABLE;
}
extern "C" void task1_run(void);
extern "C" void task2_run(void);


void do_task1(void) 
{
	//gotoxy(17, 4);
	printf(" God");	
	__asm__ ("incb 0xb8000+160*24+2");	
}

void do_task2(void) 
{
	//gotoxy(17, 4);
	printf(" God");
	__asm__ ("incb 0xb8000+160*24+4");
}

//! output sound to speaker
void soundp(unsigned frequency) {

	//! sets frequency for speaker. frequency of 0 disables speaker
	outportb(0x61, 3 | (unsigned char)(frequency<<2) );
}


class FATInfo2
	{private:
		char DPF[7];
		
	public:
		char op;
		int x;
	};



extern "C" void StartCKernel()
{ 
	__asm__ __volatile__ ("cli");   
	GDT_Install();
	TextVideo_Install();	
    IDT_Install();
    ISRs_Install();
    IRQ_Install();    
	Timer_Install();
	Keyboard_Install();	
	InstallSerialPort();	
	HDD_Install();
	Memory_Install();	
	FileSystemMount(ZFileSystem);	



	char wheel[] = {'\\', '|', '/', '-'};
	int i = 0,j;
	struct TASK_STRUCT task1;
	struct TASK_STRUCT task2;
	set_tss(CURR_TASK_TSS,(unsigned long long)&TASK0.tss);
	set_ldt(CURR_TASK_LDT,(unsigned long long)&TASK0.ldt);
	__asm__ ("ltrw	%%ax\n\t"::"a"(TSS_SEL));//set sum bits in access(like busy bit when ltr is execute)
	__asm__ ("lldt	%%ax\n\t"::"a"(LDT_SEL));
	__asm__ __volatile__ ("sti");
	double t=123.456789;	
	printf("\nr=%f  ceil=%d floorl=%d",t,(int)ceill(t),(int)floorl(t));getch();
	//printf("rr=%d%c%d",(int)t,'.',snum*100);
	

	//new_task(&task1, (unsigned int)task1_run, (unsigned int)task1_stack0+sizeof task1_stack0, (unsigned int)task1_stack3+sizeof task1_stack3);
	//new_task(&task2, (unsigned int)task2_run, (unsigned int)task2_stack0+sizeof task2_stack0, (unsigned int)task2_stack3+sizeof task2_stack3);	
	//enum_host_bus();
	//krnlcfg = parse_properties(ConfigStr);
	//install_drivers();
	//Component Com1;		
	//CreatFile("C:\\3.bmp",true,true,true,AuthenticationFullControl ); 	
	//FILE *fp;	
	//fp=fopen("C:\\3.bmp",FOpenModeWRBinary);
	//fwrite(MyFile,1,304062,fp);
	//Console();

	FATInfo ff;
	ff.op='s';
	printf("gg=%d",ff.Get());getch();

	ColorRGB Color={0xFF,0xA0,0x10};
	ColorRGB Color2={0xF,0xFF,0xFF};		
	clrscr();
	initgraph(SVGA_640x480_64k);
	Mouse_Install(MainGraphicArea.width,MainGraphicArea.hight);	
	GUIInit();

	clrscr();
	FormControl *NewForm=(FormControl *)malloc(sizeof(FormControl));	  
	InitForm(NewForm ,POINT(50,100) ,SIZE(150,150));	
	CreatForm(NewForm );

	printf("Get Ready To Creat Form2");
	FormControl *NewForm1=(FormControl *)malloc(sizeof(FormControl));	  
	InitForm(NewForm1 ,POINT(220,100) ,SIZE(150,150));	
	CreatForm(NewForm1 );		
	UpdateWNDS(wm_handles[0]);	
//	//struct WINDLIST *LastWIND=wm_handles[0];
//	//getch();
//	//gotoxy(10,1);
//	/*clrscr();
//	printf("FirstName=%s",wm_handles[0]->First_Child->Form->CName);getch();
//	printf("Start Link List %p %s",LastWIND->First_Child ,LastWIND->First_Child->Form->CName );getch();
//	for(LastWIND=wm_handles[0]->First_Child ;LastWIND!=NULL;LastWIND=LastWIND->Next )
//	{printf("%s (%d,%d)",LastWIND->Form->CName ,LastWIND->Form->CPoint.X,LastWIND->Form->CPoint.Y );
//	getch();
//	}
//getch();
//DumpWNDS();
//printf("Point(%d,%d)",NewForm1->CPoint.X,NewForm1->CPoint.Y  );
//getch();*/
Console();
//	/*image loadImage;
//	printf("Load=%d",load(MyFile ,sizeof(MyFile),318,239, &loadImage) );getch();
//	printf("Load=%d",DrawImage(MainGraphicArea,&loadImage ,Draw_Translucent ,0 ,0,0,0,318,239));*/
//	while(1);
//
//	__asm__ ("movl %%esp,%%eax		\n\t"
//			 "pushl %%ecx			\n\t"
//			 "pushl %%eax			\n\t"
//			 "pushfl				\n\t"
//			 "pushl %%ebx			\n\t"
//			 "pushl $1f				\n\t"	//Far pointer to 1:
//			 "iret					\n\t"	//jmp to code_sel:eip
//			 "1:					\n\t"			 
//			 "movw %%cx,%%ds		\n\t"
//			 "movw %%cx,%%es		\n\t"
//			 "movw %%cx,%%fs		\n\t"
//			 "movw %%cx,%%gs			"
//			 ::"b"(USER_CODE_SEL),"c"(USER_DATA_SEL));
//
//	for (;;) 
//	{
//		__asm__ ("movb	%%al,	0xb8000+160*24"::"a"(wheel[i]));
//		if (i == sizeof wheel)
//			i = 0;
//		else
//			++i;
//	}
//	
//	for(i=0;i<=0x20;i+=8) {		
//		printf ("selector: %X, limit: %X	, base: %X,access: %X	,gran:	%X\r\n", i, gdt_sel_to_limit(i), gdt_sel_to_base(i),gdt_sel_to_access(i),gdt_sel_to_gran(i));
//		getch();
//	};
//	float f=12.3;
//	printf("\nFarshad Bayat %f\n",f);
//	long ll=15;
//   printf("Farshad Bayat %d",ll);    
  for(;;);
}
