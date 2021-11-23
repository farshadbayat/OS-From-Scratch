#ifndef __FS_H
#define	__FS_H

#ifdef __cplusplus
extern "C"
{
#endif

	#include <zfs.h>

	enum FSTypeEnum{
		ZFileSystem,
		FAT16FileSystem,
		FAT32FileSystem,
		};


	char* FileSystemType(unsigned char Type);
	void FileSystemMount(enum FSTypeEnum Type);


#ifdef __cplusplus
}
#endif

#endif

