#include <mmu.h> 
#include <system.h> 
#include <string.h> 
#include <stdio.h> 
#include <conio.h> 
#include <io.h>


unsigned int Kernel_TotalMemory;
PMEMMAN memman=NULL;
struct MAM *RootMAM=NULL,*LastMAM=NULL;

void memman_init(PMEMMAN man)
{
  man->frees    = 0; 
  man->maxfrees = 0; 
  man->lostsize = 0; 
  man->losts    = 0;   
} 

unsigned int memtest(unsigned int start, unsigned int end)
{
  char flg486 = 0;
  unsigned int eflg, cr0, i;

  eflg = load_eflags();
  eflg |= EFLAGS_AC_BIT; // AC-bit = 1
  store_eflags(eflg);
  eflg = load_eflags();
  if ((eflg & EFLAGS_AC_BIT) != 0) { 
    flg486 = 1;
  }
  eflg &= ~EFLAGS_AC_BIT; // AC-bit = 0
  store_eflags(eflg);

  if (flg486 != 0) {
    cr0 = load_cr0();
    cr0 |= CR0_CACHE_DISABLE; 
    store_cr0(cr0);
  }

  i = memtest_sub(start, end);

  if (flg486 != 0) {
    cr0 = load_cr0();
    cr0 &= ~CR0_CACHE_DISABLE; 
    store_cr0(cr0);
  }

  return i;
}



unsigned int memman_total(PMEMMAN man)
{
  unsigned int i, t = 0;
  for (i = 0; i < man->frees; i++)
    t += man->free[i].size;
  return t;
}

unsigned int memman_alloc(PMEMMAN man, unsigned int size)
{
  unsigned int i, a;
  for (i = 0; i < man->frees; i++) {
    if (man->free[i].size >= size) {    
      a = man->free[i].addr;
      man->free[i].addr += size;
      man->free[i].size -= size;
      if (man->free[i].size == 0) {        
        for (man->frees--; i < man->frees; i++)
          man->free[i] = man->free[i + 1]; 
      }
      return a;
    }
  }
  return 0; 
}

int memman_free(PMEMMAN man, unsigned int addr, unsigned int size)
{
  unsigned int i, j;  
  for (i = 0; i < man->frees; i++)
    if (man->free[i].addr > addr)
      break;
  // free[i - 1].addr < addr < free[i].addr  
  if (i > 0) {
    if (man->free[i - 1].addr + man->free[i - 1].size == addr) {      
      man->free[i - 1].size += size;
      if (i < man->frees) {        
        if (addr + size == man->free[i].addr) {          
          man->free[i - 1].size += man->free[i].size;          
          for (man->frees--; i < man->frees; i++)
            man->free[i] = man->free[i + 1]; 
        }
      }
      return 0; 
    }
  }

  if (i < man->frees) {
    if (addr + size == man->free[i].addr) {   
      man->free[i].addr = addr;
      man->free[i].size += size;
      return 0; 
    }
  }
  
  if (man->frees < MEMMAN_FREES) {   
    for (j = man->frees; j > i; j--) {
      man->free[j] = man->free[j - 1];
    }
    man->frees++;
    if (man->maxfrees < man->frees)
      man->maxfrees = man->frees; 

    man->free[i].addr = addr;
    man->free[i].size = size;
    return 0; 
  }

  man->losts++;
  man->lostsize += size;
  return -1; 
}


unsigned int memman_alloc_4k(PMEMMAN man, unsigned int size)
{
  unsigned int a;
  size = (size + 0xfff) & 0xfffff000;
  a = memman_alloc(man, size);
  return a;
}
int memman_free_4k(PMEMMAN man, unsigned int addr, unsigned int size)
{
  int i;
  size = (size + 0xfff) & 0xfffff000;
  i = memman_free(man, addr, size);
  return i;
}

void Memory_Install()
{
	bool reDisable=false;
	if (!IsInterruptsEnabled()) 
	{		
		enable();
		reDisable = true;
    }
	//Memory initialize
	memman = (PMEMMAN ) MEMMAN_ADDR;
	Kernel_TotalMemory = memtest(0x00400000, 0xbfffffff);
	kprintf("Detecting Memory:%d Byte!",Kernel_TotalMemory);
	memman_init(memman);
	//memman_free(memman, 0x00001000, 0x0009e000); // 0x00001000 - 0x0009efff
	memman_free(memman, 0x00400000, Kernel_TotalMemory - 0x00400000);
	kprintf("MMU(Memory Management Unit) Driver Is Install\t[ OK ]");	   
	if (reDisable==true) disable();	
}
/////////////////////////////Memory Management Interface////////////////////////////////

void * malloc(unsigned int size)
{
	return kmalloc(size,""); 
}

void *kmalloc(unsigned int size,char *Tag)
{
	if(memman==NULL)
	{
		kprintf("MMU Is Not Install!");
		return 0;
	}

	struct MAM *NewMAM=(struct MAM *)memman_alloc_4k(memman,sizeof(struct MAM));
	if(NewMAM==0)
	{
		KASSERT();
		kprintf("Can Not Allocate Memory.");
		return 0;//Can Not Allocate Memory
	}
	NewMAM->addr=(unsigned int)memman_alloc_4k(memman,size);
	if(NewMAM->addr==0)
	{
		memman_free_4k(memman ,(unsigned int)NewMAM ,sizeof(struct MAM) );		
		KASSERT();
		kprintf("Can Not Allocate Memory.");
		return 0;
	}
	NewMAM->Tag =(char *)memman_alloc_4k(memman,strlen(Tag));
	if(NewMAM->Tag==0)
	{
		memman_free_4k(memman ,(unsigned int)NewMAM ,sizeof(struct MAM) );
		memman_free_4k(memman ,(unsigned int)NewMAM->addr ,size );
		KASSERT();
		kprintf("Can Not Allocate Memory.");
		return 0;
	}
	Strcpy(NewMAM->Tag ,Tag); 
	NewMAM->size=size;
	NewMAM->Next=NULL;

	if(RootMAM==NULL)	
	{
		RootMAM=NewMAM;
		LastMAM=NewMAM;
	}
	else{
		LastMAM->Next=NewMAM;
		LastMAM=NewMAM;
	}
	//printf("LastMAM:%p RootMAM=%p Size=%d %s\r",NewMAM,RootMAM,NewMAM->size ,Tag);getch();
	return (void *)NewMAM->addr;
}

int free(void * addr)
{
	//printf("\n Free:addr=%p",addr);getch();while(1);
	struct MAM *CurrMAM=RootMAM,*PrevMAM=NULL;
	if(memman==NULL)
	{
		kprintf("MMU Is Not Install!");
		return 0;
	}
	if(addr==(unsigned int)NULL)return 0;

	while(CurrMAM)
	{
		if((void *)CurrMAM->addr==addr)//Find It
		{			
			if(PrevMAM==NULL)
				RootMAM=NULL;
			else{
				if(CurrMAM->Tag!=NULL)
					memman_free_4k(memman ,(unsigned int)CurrMAM->Tag ,strlen(CurrMAM->Tag));
				memman_free_4k(memman ,(unsigned int)CurrMAM ,sizeof(struct MAM) );
				PrevMAM->Next=CurrMAM->Next;
			}
			return memman_free_4k(memman ,(unsigned int)addr ,CurrMAM->size);
		}
		PrevMAM=CurrMAM;
		CurrMAM=CurrMAM->Next;
	}	
	return 0;
}

void DumpMAM(struct MAM *MAM)
{
	unsigned short count=0;
	//printf("MAM=%p  RootMAM=%p",MAM,RootMAM);getch();
	printf(" MAM Address |  Address   |   Size   |    Next    |          Tag           \n");
	printf("-------------|------------|----------|------------|------------------------\n");
	while(MAM)
	{
		printf("  %08XH  |  %08XH | %-8d | %08XH  |%25s\n",MAM ,MAM->addr ,MAM->size ,MAM->Next ,MAM->Tag );
		MAM=MAM->Next;
		count++;
		if(count==20)
		{
			printf("\nDo You Want Print Rest Of MAM?[Y/N]");
			if(CharToUpper(getch())!='Y') 
				break;
			putch('\n');
			count=0;
		}
	}
	//printf("MAM=%p",RootMAM);getch();
}

