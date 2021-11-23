#ifndef __ZFS_H
#define	__ZFS_H

#ifdef __cplusplus
extern "C"
{
#endif

	#include <string.h>
	#include <stdio.h>
	#include <time.h>
	#include <HDDIO.h>
	#include <math.h>
	#include <fs.h>
	#include <system.h>  
	#include <conio.h>
	#include <io.h>

	#define FILE_NAME_SIZE		11
	#define FILE_EXTENTION_SIZE	3
	#define FST_FS		0x2e		/* normal partition */
	#define FST_SW		0x2f		/* swap partition */ 

	#define BOOTABLE	0x80		/* Predicate Bootable in Disk Partition*/
	#define SECTOR_SIZE 0x200

	#define CByteToSectro(x) (x/SECTOR_SIZE)
	#define CByteToKilo(x)	 (x/1024)
	#define CByteToMega(x)	 (x/1048576)
	#define CSectroToByte(x) (x*SECTOR_SIZE)
	#define CKiloToByte(x)	 (x*1024)
	#define CMegaToByte(x)	 (x*1048576)
	#define CSectroToKilo(x) (x/2)
	#define CSectroToMega(x) (x/2048)

	
	#define SOP					0x01	//Start Of Packet(Directory)
	#define EOP					0x02	//End Of Packet(Directory)
	#define IsEOP(PacketHeader)	((PacketHeader & EOP )==EOP ? true : false)
	#define IsSOP(PacketHeader)	((PacketHeader & SOP )==SOP ? true : false)
	
	#define FramePerPaket	16	//Total Frame At One Directory Paket

	/// <summary>		
	///This Function Is Frame Delete.			
	///Return True/False
	/// </summary> 
	#define DELETE_FLAG		0x10
	#define IsFrameDelete(Flag)			(Flag & DELETE_FLAG)>>4

	

	//#define FIRST_DISK_PARTITION  0x1BE
	//#define SECOND_DISK_PARTITION 0x1CE
	//#define THIRD_DISK_PARTITION  0x1DE
	//#define FOURTH_DISK_PARTITION 0x1EE

	typedef enum PartitionTag{
		FIRST_DISK_PARTITION  = 0x1BE,
		SECOND_DISK_PARTITION = 0x1CE,
		THIRD_DISK_PARTITION  = 0x1DE,
		FOURTH_DISK_PARTITION = 0x1EE,
		}partition;

	enum PartitionEnum{
		PARTITION_FIRST  = 0x00,
		PARTITION_SECOND = 0x01,
		PARTITION_THIRD  = 0x02,
		PARTITION_FOURTH = 0x03,
		};
    

	enum FrameFlagsPropertyEnum{
		PropertyNormalFile	 = 0x00,
		PropertySystemFile	 = 0x01,
		PropertyReadOnlyFile = 0x02,
		PropertyHidenFile	 = 0x04,
		PropertySubDirectory = 0x08,
		};
	
	enum FrameFlagsAuthenticationEnum{
		AuthenticationNone			= 0x00,
		AuthenticationRead			= 0x20,
		AuthenticationWrite			= 0x40,
		AuthenticationFolderDir		= 0x80,		
		AuthenticationExecute		= AuthenticationFolderDir | AuthenticationRead,
		AuthenticationFullControl	= AuthenticationRead | AuthenticationWrite | AuthenticationFolderDir,
		};

	enum FileOpenModeEnum{
		FOpenModeRead		= 0x1,
		FOpenModeWrite		= 0x2,
		FOpenModeAppend		= 0x4,
		FOpenModeText		= 0x8,		
		FOpenModeBinary		= 0x10,
		FOpenModeRDWRText	= FOpenModeWrite | FOpenModeRead | FOpenModeText,
		FOpenModeRDWRBinary	= FOpenModeWrite | FOpenModeRead | FOpenModeBinary,
		FOpenModeRDText		= FOpenModeRead | FOpenModeText,
		FOpenModeRDBinary	= FOpenModeRead | FOpenModeBinary,
		FOpenModeWRText		= FOpenModeWrite | FOpenModeText,
		FOpenModeWRBinary	= FOpenModeWrite | FOpenModeBinary,
		};

	enum WhenceEnum{
		SEEK_SET,
		SEEK_CUR,
		SEEK_END,
	};

	enum CreatDirResult{
		CreatDir_Success,
		CreatDir_DriveNotExist,
		CreatDir_InvalidPath,
		CreatDir_AlreadyExist,
		CreatDir_Exception,
		CreatDir_InComplate,
	};

	enum CreatFileResult{
		CreatFile_Success,
		CreatFile_DriveNotExist,
		CreatFile_PathNotExist,
		CreatFile_FileNotExist,
		CreatFile_InvalidPath,
		CreatFile_AlreadyExist,
		CreatFile_Exception,		
	};
	
	struct PartitionFormat{
		unsigned char bootable;
		unsigned char BeginCHS[3];
		unsigned char PartitionType;
		unsigned char SizeCHS[3];
		unsigned int  BeginLBA;
		unsigned int  SizeLBA;					// Bar Hasbe Sector Byte		
	}__attribute__((packed)) ;					//info: sizeof(PartitionFormat)=16	

	struct BootSector{
		unsigned char jumpBoot[3];
		unsigned char SysName[8];
		unsigned int  StartSectorBitmap;
		unsigned int  SizeSectorBitmap;		// How Many Sector used for SBITMAP
		unsigned int  StartRoot;		
		unsigned char DriveName;
		unsigned char Reserve1[422];
		struct PartitionFormat DiskPartition[4];
		unsigned char Signacuer[2];			//0xAA55
	}__attribute__((packed));

	struct FileDate{
		unsigned char	Date[3];			// Year=2000+byte[0] | Month=byte[1] | Day=byte[2]
		unsigned char	Time[3];			// Hour=Byte[0] | Minute=Byte[1] | second=Byte[2]	
	}__attribute__((packed));				//info: sizeof(FileDate)=6
	
	struct Frame{
		unsigned char	FileName[FILE_NAME_SIZE];
		unsigned char	FileExtention[3];
		struct FileDate CreatFile;
		struct FileDate ModifiedFile;
		unsigned char	Flags;				// Bit[0,1,2,3]=File Properties | Bit[4]=Delete | Bit[5,6,7]=Authentication
		unsigned int    PaketAddress;			// Address Of Content File(FirstPacket) Or Directory
	}__attribute__((packed));				//info: sizeof(Frame)=31	

	struct HeaderDir{
		unsigned short	FrameBitmap;		
		unsigned char	PacketHeader;		//Bit[0]=SOP | Bit[1]=EOP | Bit[2,3,4,5,6,7]=Rezerve
		unsigned int	ParentAddress;
		unsigned int	ChildsAddress;
		unsigned char	ParentOffSetFrame;	//Link From Child To Offset Need ParentAddress And Offset To Find It.
	}__attribute__((packed));				//info: sizeof(Header)=12


	struct PaketEntry{		//For Directory
		struct HeaderDir Header;
		unsigned short   Reserve[2];
		struct Frame Frame[16] ;	
	}__attribute__((packed));				//info: sizeof(PaketEntry)=12+3*2+16*31=512

	///////////////////////////just file	
	#define EOFP									(unsigned short)0x01	//End Of File Packet (Must Be 2Byte To ~Not Need 2Byte)
	#define IsEOFP(FileHeader)						((FileHeader & EOFP)==EOFP ? true : false)
	
	#define TOTAL_SECTOR_ADDRESS					126
	//////////////////////////////////////////////////////////////////
	//For 	PaketFile												//
	struct FirstPacket{												//
				unsigned int TotalSector;							//
				unsigned int SectorAddress[TOTAL_SECTOR_ADDRESS-1];	//
			}__attribute__((packed));								//
																	//
	struct OtherPacket{												//
				unsigned int SectorAddress[TOTAL_SECTOR_ADDRESS];	//
			}__attribute__((packed));								//
	//////////////////////////////////////////////////////////////////
	#define CONTENT_TOTAL_BYTE						510	
	struct SectorContent{
		short	Offset;						//0 to sizeof(SectorContent)-2
		char	Content[CONTENT_TOTAL_BYTE];//Content Of File
	}__attribute__((packed));				//info: sizeof(SectorContent)=512

	struct HeaderFile{
		unsigned short	FileHeader;			//Bit[0]=EOFP | bit[1..15]=Reserve		
		unsigned int	Next;				
	}__attribute__((packed));				//info: sizeof(Header)=6

	struct PaketFile{	//For File				
		struct HeaderFile Header;				
		unsigned short	 AddressOffset;				
		union{
			struct FirstPacket Size;
			struct OtherPacket Address;
		};
	}__attribute__((packed));				//info: sizeof(PaketContent)=10+502=512

	struct SeekAddress{
			unsigned int Sector ;
			unsigned short OffsetByte ;
	}__attribute__((packed));

	typedef struct FILETag{
		enum FileOpenModeEnum Mode;
		struct PaketFile *FirstPaketFile;
		unsigned int FirstLBA;
		struct PaketFile *BufferAddress;/*en Be Akharen buffer Paket Address Eshare Mecone*/
		unsigned int LBA;/*Address Be Akharen Paket Address Eshare Mecone*/	
		struct SectorContent *BufferRW;
		struct SeekAddress Seek;
		short DriveNumber;
		
	}FILE __attribute__((packed));


	void ZFS_Init();
	unsigned int CalculatSBitmapSize(unsigned int PartitionSize);
	void ReadMBR(struct BootSector *bs);
	void MakeBootSector(char Sector[512],struct BootSector *bs);
	void FormatDisk(void);
	int FormatPartition(char *DriveName ,short DriveNumber ,unsigned int Size ,char PartitionType ,bool IsBootable );
	void CFileDateToString(char StrFormat[] ,struct FileDate DateTime ,bool IsFullDateYear ,bool IsTime ,bool IsSecond );
	struct FileDate CDateTimeToFileDate(TIME time ,DATE date );	
	void Framecpy(struct Frame *Dest ,struct Frame *Src);
	void DumpSBitmap(short DriveNumber ,unsigned short Part);
	short GetDriveNumber(char Name);
	unsigned int GetRootLBA(short DriveNumber);//Get Absolute Address
	unsigned int GetSectorBitmapLBA(short DriveNumber );
	unsigned int GetSectorBitmapSize(short DriveNumber );
	bool IsAllocateSBitmapAtDrive(short DriveNumber ,unsigned int SectorNumber );//is start from 0 to ...
	unsigned int AllocateSBitmapAtDrive(short DriveNumber );
	void UnAllocateSector(short DriveNumber ,unsigned int SectorNumber );//is start from 0 to ...
	bool IsValidDriveName(char DriveName);
	unsigned short MakeHeaderFile(bool IsEOF,unsigned short OffSet);
	unsigned char MakeFlagsFrame(enum FrameFlagsPropertyEnum Property,bool Delete ,enum FrameFlagsAuthenticationEnum Authentication);
	bool IsFrameProperty(unsigned char	Flags ,enum FrameFlagsPropertyEnum Property );
	short FindFrameAtLBA(short DriveNumber ,unsigned int LBA ,char *Name ,short StartOfFrame ,bool IsDirectory ,unsigned int *HowManyPaketLoad ,struct Frame *Frame);	
	unsigned int CreatFrameAtLBA(short DriveNumber ,unsigned int LBAPaket,struct Frame Frame);
	unsigned int CreatPacketAtDrive(short DriveNumber );
	unsigned int AllocateDirPacketAtDrive(short DriveNumber ,struct HeaderDir *Header );
	unsigned int AllocateFilePacketAtDrive(short DriveNumber ,struct HeaderFile *Header );
	short GetDestinationPath(char *Path ,char *Name);
	short GetFileExtention(char *FileName ,char *Extention);
	short GetDirInPath(const char *Path ,short Start ,char *Dir );
	unsigned int GetStartOfFrame(char *Path,char *FrameName ,bool IsDirectory ,short *FrameNO ,unsigned *PE_LBA);
	enum CreatDirResult CreatDir(const char *FullPath ,bool IsSystem ,bool IsHiden ,bool IsReadOnly ,enum FrameFlagsAuthenticationEnum Authentication );
	enum CreatFileResult CreatFile(const char *FilePath ,bool IsSystem ,bool IsHiden ,bool IsReadOnly ,enum FrameFlagsAuthenticationEnum Authentication);
	short Delete(char *Path ,bool ToRecycle ,bool IsDirectory);
	bool FindFileInPath(const char *Path,const char *FileName);
	bool FindFolderInPath(const char *Path,const char *FolderName);
	FILE *fopen(const char *FilePath ,enum FileOpenModeEnum Mode);
	unsigned int fread(char* Buffer ,short Size ,int Count ,FILE* Stream);
	unsigned int fwrite(char *Buffer ,short Size ,int Count ,FILE *Stream);
	unsigned int GetLastAddressPaketFile(struct HeaderFile FileHeader ,struct PaketFile *Buf);
	unsigned int GetNAddressPaketFile(struct HeaderFile FileHeader ,unsigned int n,struct PaketFile *Buf);
	short fclose(FILE *Stream);
	short fflush(FILE *Stream);
	unsigned int AllocateContentSector(FILE *Stream);
	void DeleteContent(short DriveNumber ,unsigned int ContentAddress);
	int CreatRootDirectoryAtDrive(char DriveName ,unsigned int Root ,unsigned int BeginLBA);
	void PrintBootSector(struct BootSector *bs);
	void PrintPaketEntry(short DriveNumber ,unsigned int PaketEntryLBA,bool IsReserve);
	void PrintPaketContent(short DriveNumber ,unsigned int PaketContentLBA);
	void PrintAddressOfContentPaket(short DriveNumber ,unsigned int PaketContentAddressLBA,bool IsFirstPaketFile);
	void ViewFSMethod();


#ifdef __cplusplus
}
#endif

#endif

