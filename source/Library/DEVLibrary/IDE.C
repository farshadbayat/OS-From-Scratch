#include <ide.h>
#include <mmu.h>
#include <system.h>
//#define IDE_DEBUG
//#define IDE_BOCHS2  //This Code Must Place Here If Useing Bochs !

static int numDrives;
static IDEDisk drives[IDE_MAX_DRIVES];

/// <summary>		
	///This Function Is Read And Write From HD.
	///LBA: Logical Block Addressing
	///command:	IDE_READ	 Or IDE_WRITE
	//NumSector: How Many Sector Is Read Or Write
	///Buf: Data To Read To Or Write From .
	///Importand: lba:starts from 0 And start_sect: starts from 1
	///If Return  IDE_ERROR_NO_ERROR  IDE_ERROR_BAD_DRIVE   IDE_ERROR_INVALID_BLOCK	 IDE_ERROR_DRIVE_ERROR
/// </summary> 
short IDE_RW(unsigned int lba ,enum IDECMDEnum command,unsigned short NumSector ,void *buf ) 
{
	unsigned char driveNum=0;
	bool reEnable = false;	
	// now compute the head, cylinder, and sector (lba to chs)
	unsigned short cyl  = lba / (drives[driveNum].num_Heads * 	drives[driveNum].num_SectorsPerTrack);
	unsigned short head = (lba / drives[driveNum].num_SectorsPerTrack) %  drives[driveNum].num_Heads;
	unsigned short sect = lba % drives[driveNum].num_SectorsPerTrack + 1;
	//printf("cyl=%d	head=%d		sect=%d",cyl,head,sect);getch();//puts ('\r');
	if (lba >= IDE_getNumBlocks(driveNum))
		return IDE_ERROR_INVALID_BLOCK;	 

	switch(command)
	{
		case HD_READ: 
			command= IDE_READ;
			break;
		case HD_WRITE: 
			command= IDE_WRITE;
			break;
		default:
			kprintf("IDE_RW:IDE Command Is Not Valid.");
			return -1;
	}

#ifdef IDE_DEBUG
	if (command == IDE_WRITE)
		printf("[w");
	else
		printf("[r");
	printf("%d]", lba);
#endif
	
	if (IsInterruptsEnabled()) 
	{		
		disable();
		reEnable = true;
    }
	while ((inportb(IDE_STATUS_REGISTER)&0xc0)!=0x40);
	outportb(IDE_SECTOR_COUNT_REGISTER ,NumSector);	
	outportb(IDE_SECTOR_NUMBER_REGISTER ,sect);	
	outportb(IDE_CYLINDER_LOW_REGISTER ,cyl);
	outportb(IDE_CYLINDER_HIGH_REGISTER, cyl>>8);
	outportb(IDE_DRIVE_HEAD_REGISTER, 0xa0|head);
	outportb(IDE_COMMAND_REGISTER, command);

	while (! (inportb(IDE_STATUS_REGISTER)&0x8));

	if (command == IDE_READ)
		insl(IDE_DATA_REGISTER, buf, NumSector<<7);
	else if (command == IDE_WRITE)
		outsl(buf, NumSector<<7, IDE_DATA_REGISTER);

	if (reEnable==true) enable();
	return IDE_ERROR_NO_ERROR;
}

/// <summary>		
	///This Function Is Read Drive Configeration(CHS Parameter)
	///drive Is 0(First HD) or 1(Second HD).
	//Private Function(Or Static Function)
/// </summary> 
static int readDriveConfig(int drive)
{
    int i;
    int status;     

    //printf("ide: about to read drive config for drive #%d\n", drive);
    outportb(IDE_DRIVE_HEAD_REGISTER, (drive == 0) ? IDE_DRIVE_0 : IDE_DRIVE_1);
    outportb(IDE_COMMAND_REGISTER, IDE_COMMAND_IDENTIFY_DRIVE);
    while (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);

    status = inportb(IDE_STATUS_REGISTER);	    
     // simulate failure
     // status = 0x50;
     
    if ((status & IDE_STATUS_DRIVE_DATA_REQUEST)) 
	{
       kprintf("HDD Probe Found ATA Drive\t\t\t\t[ OK ]");	   
	   
	   drives[drive].Information =(short *) memman_alloc_4k(memman, 256*2);
         /* drive responded to ATA probe */
		for (i=0; i < (256*1); i++)
			drives[drive].Information[i] = inportw(IDE_DATA_REGISTER);	
	
		drives[drive].num_Cylinders = drives[drive].Information[IDE_INDENTIFY_NUM_CYLINDERS];
		drives[drive].num_Heads = drives[drive].Information[IDE_INDENTIFY_NUM_HEADS];
		drives[drive].num_SectorsPerTrack = drives[drive].Information[IDE_INDENTIFY_NUM_SECTORS_TRACK];
		drives[drive].num_BytesPerSector = drives[drive].Information[IDE_INDENTIFY_NUM_BYTES_SECTOR];
    } 
	else 
	{
		/* try for ATAPI */
		outportb(IDE_FEATURE_REG, 0);		 /* disable dma & overlap */ 
		outportb(IDE_DRIVE_HEAD_REGISTER, (drive == 0) ? IDE_DRIVE_0 : IDE_DRIVE_1);
		outportb(IDE_COMMAND_REGISTER, IDE_COMMAND_ATAPI_IDENT_DRIVE);
		while (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);
		status = inportb(IDE_STATUS_REGISTER);		
		kprintf("HDD Probe Found ATAPI Drive\t\t\t\t[ OK ]");	   
		return -1;
	}
#ifdef IDE_DEBUG
	printf("\n ide%d: cyl=%d, heads=%d, sectors=%d\n", drive, drives[drive].num_Cylinders,	drives[drive].num_Heads, drives[drive].num_SectorsPerTrack);      
#endif    
	//dump(drives[drive].Information,512,true); 
    return 0;
}



/// <summary>		
	///This Function Is Initializing IDE controller And Found Number Of IDE.	
	/// return number ide drive
/// </summary> 
short Init_IDE(void)
{
    int errorCode,i;    
    // Reset the controller and drives
	kprintf("Initializing IDE Controller...");	   
    outportb(IDE_DEVICE_CONTROL_REGISTER, IDE_DCR_NOINTERRUPT | IDE_DCR_RESET);
    for(i=1000;i>0;i--);
    outportb(IDE_DEVICE_CONTROL_REGISTER, IDE_DCR_NOINTERRUPT);

#ifndef IDE_BOCHS2	
	// FIXME: This code doesn't work on Bochs 2.0.
   while ((inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_READY) == 0) ;	
    // This code does work on Bochs 2.0.
    while (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);
#endif 

	 //puts("About to run drive Diagnosis\n");
    outportb(IDE_COMMAND_REGISTER, IDE_COMMAND_DIAGNOSTIC);
    while (inportb(IDE_STATUS_REGISTER) & IDE_STATUS_DRIVE_BUSY);
    errorCode = inportb(IDE_ERROR_REGISTER);    
#ifdef IDE_DEBUG
	printf("ide: ide error register = %x\n", errorCode);
#endif
    // Probe and register drives
    if (readDriveConfig(0) == 0)numDrives++;
    //if (readDriveConfig(1) == 0)numDrives++;
#ifdef IDE_DEBUG
     printf("Found %d IDE drives\n", numDrives);   
 #endif
	 return numDrives;
}

/// <summary>		
	///This Function Is return the number of logical blocks for a particular drive.
	///driveNum Is 0 or 1.	
/// </summary> 
unsigned int IDE_getNumBlocks(int driveNum)
{
    if (driveNum < 0 || driveNum > IDE_MAX_DRIVES) 
	{
        return IDE_ERROR_BAD_DRIVE;
    }
    return (drives[driveNum].num_Heads * drives[driveNum].num_SectorsPerTrack * drives[driveNum].num_Cylinders);
}



