#ifndef __HDDIO_H
#define	__HDDIO_H

#ifdef __cplusplus
extern "C"
{
#endif
	
	#include "ide.h"

	enum HardDiskCMDEnum{
		HD_READ,
		HD_WRITE,
		};

	void HDD_Install();
	short (*HDD_RW)(unsigned int lba ,enum HardDiskCMDEnum command,unsigned short NumSector ,void *buf );

	
#ifdef __cplusplus
}
#endif

#endif
