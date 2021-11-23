
#ifndef __MMU_H
#define	__MMU_H

#ifdef __cplusplus
extern "C"
{
#endif 	
	
	#include <io.h>
	#include <system.h>


	#define MASSERT1(MemoryName,ErrorMessage,ReturnValue)	\
	if(MemoryName==NULL){									\
		kprintf("ErrorMessage.");							\
		return ReturnValue;									\
	}														\

	#define MASSERT2(MemoryName,ErrorMessage)				\
	if(MemoryName==NULL){									\
		kprintf("ErrorMessage.");							\
		return;												\
	}														\


	#define MEMMAN_FREES		4090  
	#define MEMMAN_ADDR			0x003c0000

	#define EFLAGS_AC_BIT       0x00040000
	#define CR0_CACHE_DISABLE   0x60000000
	
	typedef struct _freeinfo {
		unsigned int addr, size;  
	} FREEINFO, *PFREEINFO;

	typedef struct memmanTag {	
	int frees, maxfrees, lostsize, losts;
	FREEINFO free[MEMMAN_FREES];
	} MEMMAN, *PMEMMAN;

	struct MAM{//MemoryAllocManagementTag
		struct MAM *Next;
		unsigned int addr, size;
		char *Tag;
	};
	
	extern unsigned int Kernel_TotalMemory;
	extern PMEMMAN memman;
	extern struct MAM *RootMAM;
	unsigned int memtest(unsigned int start, unsigned int end);
	void memman_init(PMEMMAN man);
	unsigned int memman_total(PMEMMAN man);
	unsigned int memman_alloc(PMEMMAN man, unsigned int size);
	int memman_free(PMEMMAN man, unsigned int addr, unsigned int size);
	unsigned int memman_alloc_4k(PMEMMAN man, unsigned int size);
	int memman_free_4k(PMEMMAN man, unsigned int addr, unsigned int size);	
	void Memory_Install();
	void * malloc(unsigned int size);	
	void * kmalloc(unsigned int size,char *Tag);
	int free(void * addr);
	void DumpMAM(struct MAM *MAM);

#ifdef __cplusplus
}
#endif

#endif

