#include <hddio.h>
#include <ide.h>
//#define IDE_DEBUG
short (*HDD_RW)(unsigned int lba ,enum HardDiskCMDEnum command,unsigned short NumSector ,void *buf );
void HDD_Install()
{
	if(Init_IDE()!=0)
	{
		HDD_RW=(short (*)(unsigned int lba ,enum HardDiskCMDEnum command,unsigned short NumSector ,void *buf ))IDE_RW;  
	}
	/* Place Here Other Hard Disk Driver Like SATA...*/
}



