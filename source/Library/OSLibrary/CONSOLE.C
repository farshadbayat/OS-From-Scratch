#include <console.h>
#include <system.h>
#include <mmu.h>
#include <zfs.h>
#include <string.h>
#include <hddio.h>
#include <graphics.h>
#include <conio.h>
#include <stdio.h>

void Console()
{
	char Command[COMMAND_BUFFER]={0};
	char Paramter1[90]={0},Paramter2[90]={0},CMD[30]={0},CurrentDir[4][100]={"C:\\","D:\\","E:\\","F:\\"};
	struct Frame *CurDIR=(struct Frame *)malloc(MAX_DIR_LIST * sizeof(struct Frame ));
	short TotalFrame=0;
	short DriveNO=0;
	int Start;
	TIME time;
	DATE date;

	do{
		Paramter1[0]=0;Paramter2[0]=0;
		cprintf(GREEN,"\n%s>",CurrentDir[DriveNO]);
		GetString((char *)Command ,0 ,false );		
		if(strlen(Command)==0)continue;
		Start=0;
		Start=GetParameter(Command,Start,CMD);
		StrToLower(CMD,CMD);
		
		if(Strncmp(Command,"format",6)==0)
		{			
			DoFormat(Command ,Paramter1 ,Start );			
		}		
		
		else if(Strncmp(CMD,"dumpm",5)==0)
		{
			DumpMAM(RootMAM);
		}
		else if(Strncmp(CMD,"restart",7)==0)
		{
			reboot();
		}
		else if(Strncmp(CMD,"mkdir",5)==0)
		{
			Start=GetParameter(Command,Start,Paramter1);
			CreatDir(Paramter1,false ,false ,false ,AuthenticationFullControl );
		}		
		else if(Strncmp(CMD,"gmod",4)==0)
		{
			initgraph(SVGA_640x480_64k);
		}
		else if(Strncmp(CMD,"tmod",4)==0)
		{
			TextMode(TEXT_80x50);
		}
		else if(Strncmp(CMD,"time",4)==0)
		{
			gettime(&time);
			getdate(&date);				
			CFileDateToString(Paramter1 ,CDateTimeToFileDate(time,date) ,true ,true ,true );
			cprintf(LIGHTCYAN ,"Date And Time= %s.",Paramter1 );
		}		
		else if(Strncmp(CMD,"dir",3)==0)
		{
			unsigned int TotalFile=0,TotalSize=0;
			TotalFrame=0;
			DoDIR(Command ,Paramter1 ,Paramter2 ,CurrentDir[DriveNO] ,CurDIR ,&TotalFrame ,Start );
			printf("  Date      Time               Size(KB)          Name     \n");
			printf("---------- ------       -------------------- -------------\n");
			printf("%s  %-25s  %s\n","0000/00/00 00:00" ,"<DIR>" ,"." );
			printf("%s  %-25s  %s\n","0000/00/00 00:00" ,"<DIR>" ,".." );
			for(Start=0;Start<TotalFrame;Start++)
			{
				CFileDateToString(Paramter1 ,CurDIR[Start].CreatFile ,true ,true ,false);								
				if(IsFrameProperty(CurDIR[Start].Flags ,PropertySubDirectory)==true)
					printf("%s  %-25s  %s\n",Paramter1 ,"<DIR>" ,CurDIR[Start].FileName );
				else
				{
					SetNull(Paramter2 ,15);					
					strcat(Paramter2 ,(const char*)CurrentDir[DriveNO] );
					strcat(Paramter2 ,(const char*)CurDIR[Start].FileName );					
					strcat(Paramter2 ,"." );					
					Strncat(Paramter2 ,(const char*)CurDIR[Start].FileExtention ,FILE_EXTENTION_SIZE );					
					TotalSize+=CalculatFileSize(Paramter2);
					TotalFile++;
					printf("%s  %25d  %s.%c%c%c\n",Paramter1 ,CalculatFileSize(Paramter2),CurDIR[Start].FileName ,CurDIR[Start].FileExtention[0],CurDIR[Start].FileExtention[1],CurDIR[Start].FileExtention[2] );
				}				
			}
			printf("\t\t %d File(s) %d Mega Byte\n",TotalFile ,TotalSize);
			printf("\t\t %d Dir(s) %d Mega Byte Total Size\n",TotalFrame-TotalFile ,CSectroToMega(mbr.DiskPartition[DriveNO].SizeLBA));
		}
		else if(Strncmp(CMD,"c:",2)==0 || Strncmp(CMD,"d:",2)==0 || Strncmp(CMD,"e:",2)==0 || Strncmp(CMD,"f:",2)==0)
		{
			DriveNO=GetDriveNumber(CMD[0]); 
		}	
		else if(Strncmp(CMD,"cd..",4)==0)
		{				
			if(strlen(CurrentDir[DriveNO])>3)
			{			
				GetDestinationPath(CurrentDir[DriveNO] ,Paramter1 );				
				CurrentDir[DriveNO][strlen(CurrentDir[DriveNO])-strlen(Paramter1)+0]=0;				
			}
		}
		else if(Strncmp(CMD,"cd\\",3)==0)
		{
			CurrentDir[DriveNO][3]=0;
		}
		else if(Strncmp(CMD,"cd.",3)==0)
		{
			printf("%s",CurrentDir[DriveNO]);
		}
		else if(Strncmp(CMD,"cd",2)==0)//Must Be After cd. and cd..
		{
			DoCD(Command ,Paramter1 ,Paramter2 ,CurrentDir[DriveNO] ,Start );
		}		
		else if(Strncmp(CMD,"test",4)==0)
		{
			char op;
			printf("1)To Test File System Basic Opration.\n");
			printf("2)To Test Write File System.\n");
			printf("3)To Test Read File System.\n");
			puts("Select:");
			op=getch();
			switch(op)
			{
			case '1':   DoTeastBasicCreation();break;
			case '2':   DoTeastWrite();break;
			case '3':   DoTeastRead();break;
			default :   printf("\nSelection Is Not Correct.");
			}
		}
		else if(Strncmp(CMD,"cls",3)==0)
		{
			clrscr();
		}
		else if(Strncmp(CMD,"del",3)==0)
		{
			DoDelete(Command ,Paramter1 ,Paramter2 ,CurrentDir[DriveNO] ,Start );
		}
		else if(Strncmp(CMD,"rd",2)==0)
		{
			DoRD(Command ,Paramter1 ,Paramter2 ,CurrentDir[DriveNO] ,Start );
		}
		else if(Strncmp(CMD,"fs",2)==0)
		{
			ViewFSMethod();
		}		
		else if(Strncmp(CMD,"help",4)==0)
		{
			DoHelp();
		}
		else cprintf(RED,"Bad Command, '%s' is not recognized.",CMD);
	}while(true);
}
/////////////////////////////////////Useful Function/////////////////////////////////////////
short GetParameter(char *Str,int Start ,char *Buffer)
{
	int i=Start,j=0;
	int len=strlen(Str);
	while(Str[i]==' ')i++;//Skip Space

	while(true)
		{		
			if(Str[i]==' ' || len<=i)
			{
				Buffer[j]=0;				
				return i;
			}
			else
				Buffer[j++]=Str[i];
			i++;
		}	
	return 0;
}

/////////////////////////////////////Command Prompt/////////////////////////////////////////
void DoFormat(char *Command ,char *Paramter1,int Start)
{
	Start=GetParameter(Command,Start+1 ,Paramter1);
	StrToLower(Paramter1,Paramter1);

	if(Paramter1[0]=='c' && Paramter1[1]==':')
	{		
		FormatPartition((char *)mbr.SysName ,0 ,mbr.DiskPartition[0].SizeLBA ,mbr.DiskPartition[0].PartitionType ,(mbr.DiskPartition[0].bootable==BOOTABLE ? true:false));     
	}
	else if(Paramter1[0]=='d' && Paramter1[1]==':')
	{
		FormatPartition(mbr.SysName ,1,mbr.DiskPartition[1].SizeLBA ,mbr.DiskPartition[1].PartitionType ,(mbr.DiskPartition[1].bootable==BOOTABLE ? true:false));     
	}
	else if(Paramter1[0]=='e' && Paramter1[1]==':')
	{
		FormatPartition(mbr.SysName ,2,mbr.DiskPartition[2].SizeLBA ,mbr.DiskPartition[2].PartitionType ,(mbr.DiskPartition[2].bootable==BOOTABLE ? true:false));     
	}
	else if(Paramter1[0]=='f' && Paramter1[1]==':')
	{
		FormatPartition(mbr.SysName ,3,mbr.DiskPartition[3].SizeLBA ,mbr.DiskPartition[3].PartitionType ,(mbr.DiskPartition[3].bootable==BOOTABLE ? true:false));     
	}
	else cprintf(RED,"Invalid Parameter(s), '%s' is not recognized.",Paramter1);
}

void DoDelete(char *Command ,char *Paramter1 ,char *Paramter2 ,char *CurrentDir ,int Start)
{
	Start=GetParameter(Command,Start+1 ,Paramter1);
	//StrToLower(Paramter1,Paramter1);
	if(Strncmp(Paramter1,"c:\\",3)!=0 && Strncmp(Paramter1,"d:\\",3)!=0 && Strncmp(Paramter1,"e:\\",3)!=0 && Strncmp(Paramter1,"f:\\",4)!=0)
	{
		Paramter2[0]=0;
		Strcpy(Paramter2 ,CurrentDir);
		strcat(Paramter2 ,Paramter1);
		printf("Paramter2=%s",Paramter2);
	}	
	Delete(Paramter2,false,false);
}

void DoRD(char *Command ,char *Paramter1 ,char *Paramter2 ,char *CurrentDir ,int Start)
{
	Start=GetParameter(Command,Start+1 ,Paramter1);
	//StrToLower(Paramter1,Paramter1);
	if(Strncmp(Paramter1,"c:\\",3)!=0 && Strncmp(Paramter1,"d:\\",3)!=0 && Strncmp(Paramter1,"e:\\",3)!=0 && Strncmp(Paramter1,"f:\\",4)!=0)
	{
		Paramter2[0]=0;
		Strcpy(Paramter2 ,CurrentDir);
		strcat(Paramter2 ,Paramter1);
		printf("Paramter2=%s",Paramter2);
	}	
	Delete(Paramter2,false,true);
}
void DoCD(char *Command ,char *Paramter1,char *Paramter2,char *CurrentDir,int Start)
{	
	short CurrentDirLen=strlen(CurrentDir);
	CurrentDir[CurrentDirLen]=0;	
	int NextStart;
	Start=GetParameter(Command,Start+1 ,Paramter1);
	if(Start==0)
		printf("%s",CurrentDir);
	if(strlen(Paramter1)==1 && Paramter1[0]=='\\' )
		CurrentDir[3]=0;
	if(Paramter1[strlen(Paramter1)]!='\\')
	{
		Paramter1[strlen(Paramter1)+1]=0;//EOS
		Paramter1[strlen(Paramter1)]='\\';		
	}	
	Start=0;	
	while((NextStart=StrnFind(Paramter1 ,Start+1 ,"\\"))!=-1)
	{
		Strncpy(Paramter2 ,&Paramter1[Start] ,NextStart-Start );
		Paramter2[NextStart-Start]=0;
		Start=NextStart+1;
		if(FindFolderInPath(CurrentDir ,Paramter2 )==true)
		{	
			strcat(CurrentDir ,"\\" );			
			strcat(CurrentDir ,Paramter2 );
			//strcat(CurrentDir ,"\\" );			
		}
		else
		{
			strcat(CurrentDir ,"\\" );
			cprintf(RED,"Directory '%s' is not Exist.",Paramter1);getch();
			CurrentDir[CurrentDirLen]=0;
			return;
		}
	}
}

void DoDIR(char *Command ,char *Paramter1,char *Paramter2,char *CurrentDir,struct Frame *CurDIR,short *TotalFrame,int Start)
{
	unsigned int PaketEntryLBA,i;
	struct PaketEntry DirPaket={{0}};
	Strcpy(Paramter1 ,CurrentDir ); 
	StrToLower(Paramter1 ,Paramter1 );		
	if(strlen(Paramter1)>3)
	{
		if(GetDestinationPath(Paramter1,Paramter2)==-1)
		{
			cprintf(RED,"Invalid Parameter(s), '%s' is not Correct Format.",Paramter1);
			return;
		}
		Paramter1[strlen(Paramter1)-strlen(Paramter2)]=0;		
		PaketEntryLBA=GetStartOfFrame(Paramter1 ,Paramter2 ,true ,NULL,NULL);
		if(PaketEntryLBA==0)
		{
			cprintf(RED,"BUG File System, Path :'%s' .",Paramter1);
			return;
		}
	}
	else 
		PaketEntryLBA=GetRootLBA(CharToLower(GetDriveNumber(Paramter1[0])));
	do
	{
		HDD_RW(PaketEntryLBA, HD_READ, 1,(void *)&DirPaket);
		for(i=0;i<FramePerPaket;i++)
		{			
			if(IsSetBit((long)DirPaket.Header.FrameBitmap ,i ,false)==true)//Check That Bit i Is 0(False) Then Go Next Frame.
				continue;
			else
			{
				/*Bytencpy((char *)CurDIR[*TotalFrame] ,(char *)DirPaket.Frame[i] ,sizeof(struct Frame));*/				
				Framecpy(&CurDIR[*TotalFrame] ,&DirPaket.Frame[i] );
				//printf("FileName=%s",DirPaket.Frame[i].FileName);getch();
				//printf("FileNAme=%s Orginal=%s %c",CurDIR[*TotalFrame].FileExtention ,DirPaket.Frame[i].FileExtention ,DirPaket.Frame[i].FileExtention[2]);getch();
				(*TotalFrame)++;
			}			
		}
		PaketEntryLBA=DirPaket.Header.ChildsAddress;		
	}while(IsEOP(DirPaket.Header.PacketHeader)!=true);	
}

void DoFDisk()
{

}
void DoTeastRead()
{
	char *Path=(char *)malloc(100);
	char *Buffer=(char *)malloc(510);
	enum CreatFileResult;
	printf("\nPlaese Enter Your Path To Read File?");
	GetString(Path ,0 ,true); 	
	
	FILE *fp;
	fp=fopen(Path,FOpenModeRDText);
	if(fp!=NULL)	
	{
		fread(Buffer ,sizeof(char) ,510 ,fp);
		printf("Your Message:\n%s",Buffer);	
	}
	else 
		cprintf(RED,"In Opening File Is Occur Exception.");
	fclose(fp);		
	free(Path);
	free(Buffer);
}
void DoTeastWrite()
{
	char *Path=(char *)malloc(100);
	char *Buffer=(char *)malloc(510);
	enum CreatFileResult Result;
	printf("\nPlaese Enter Your Path To Creat File?");
	GetString(Path ,0 ,true); 
	Result=CreatFile(Path,false ,false ,false ,AuthenticationFullControl ); 
	printf("\nPath=%s",Path);getch();
	if(Result==CreatFile_Success || Result==CreatFile_AlreadyExist)
	{
		FILE *fp;
		fp=fopen(Path,(enum FileOpenModeEnum)(FOpenModeRDText|FOpenModeAppend));
		if(fp!=NULL)	
		{
			printf("Please Enter Your Message To Store Or Append?");
			GetString(Buffer,510,false);
			fwrite(Buffer ,sizeof(char) ,strlen(Buffer) ,fp);
			fclose(fp);
		}
		else 
			cprintf(RED,"In Opening File Is Occur Exception.");		
	}
	else
		cprintf(RED,"In Creating File Is Occur Exception.");
	free(Path);
	free(Buffer);
}
void DoTeastBasicCreation()
{
	FormatPartition("ZanjanOS",0,160*1024*2,FST_FS,true); 
	FormatPartition("ZanjanOS",1,100*1024*2,FST_SW,false); 
	FormatPartition("ZanjanOS",2,100*1024*2,FST_SW,false);
	FormatPartition("ZanjanOS",3,100*1024*2,FST_SW,false);
	
	CreatFile("D:\\MyWork.TXT",true,true,true,AuthenticationFullControl); 
	CreatDir("D:\\Unix\\",true,true,true,AuthenticationFullControl ); 			
	CreatFile("D:\\Unix\\Hello.jpg",true,true,true,AuthenticationFullControl ); 		
	CreatFile("D:\\Unix\\ReadMe.TXT",true,true,true,AuthenticationFullControl ); 		
	CreatDir("D:\\Unix\\Document\\",true,true,true,AuthenticationFullControl ); 
	
	CreatFile("C:\\MyOs.TXT",true,true,true,AuthenticationFullControl); 	
	CreatDir("C:\\ProgramFile\\MPlayer\\",true,true,true,AuthenticationFullControl ); 
	CreatDir("C:\\ProgramFile\\Notpade\\",true,true,true,AuthenticationFullControl ); 
	CreatDir("C:\\ProgramFile\\Work\\",true,true,true,AuthenticationFullControl ); 
	CreatDir("C:\\Windows\\Folder1\\",true,true,true,AuthenticationFullControl ); 
	CreatDir("C:\\SwapFile\\Folder2\\",true,true,true,AuthenticationFullControl ); 
	CreatDir("C:\\Shared\\Work\\",true,true,true,AuthenticationFullControl ); 
	CreatDir("C:\\ParsOS\\Work1\\",true,true,true,AuthenticationFullControl ); 
	CreatDir("C:\\Driver\\Work\\",true,true,true,AuthenticationFullControl ); 
	CreatDir("C:\\Sample\\Work\\",true,true,true,AuthenticationFullControl ); 
	CreatDir("C:\\Sample\\WorkBKup\\",true,true,true,AuthenticationFullControl );	
}


void DoHelp()
{

	cprintf(LIGHTRED ,"format ");
	cprintf(WHITE,"for format partion for example format d:\\ (formated d).\n");

	cprintf(LIGHTRED ,"restart ");
	cprintf(WHITE,"for restart system.\n");

	cprintf(LIGHTRED ,"mkdir ");
	cprintf(WHITE,"for make directory in path for example c:\\mkFarshad (make Farshad in c).\n");

	cprintf(LIGHTRED ,"gmod ");
	cprintf(WHITE,"change to graphic mode.\n");

	cprintf(LIGHTRED ,"tmod ");
	cprintf(WHITE,"change to txt mode.\n");

	cprintf(LIGHTRED ,"time ");
	cprintf(WHITE,"for show system time.\n");

	cprintf(LIGHTRED ,"dir ");
	cprintf(WHITE,"for show directory content.\n");

	cprintf(LIGHTRED ,"<DriveName> ");
	cprintf(WHITE,"for go To selected drive for example d:(go To d:\\).\n");

	cprintf(LIGHTRED ,"cd.. ");
	cprintf(WHITE,"for change directory to parent for example  cd.. in d:\\Farshad\\ali go d:\\Farshad.\n");

	cprintf(LIGHTRED ,"cd\\ ");
	cprintf(WHITE,"for change directory  to root for example  cd\\ in d:\\Farshad\\ali go d:\\.\n");

	cprintf(LIGHTRED ,"cd ");
	cprintf(WHITE,"for show dirtectory.\n");

	cprintf(LIGHTRED ,"test ");
	cprintf(WHITE,"for  write and read and test file system: 1) for test file system ,2) for write in path,3) for read in path.\n");

	cprintf(LIGHTRED ,"del ");
	cprintf(WHITE,"for delete file for example  del c:\\Farshad.txt  ( DELETE Farshad.TXT IN C).\n");

	cprintf(LIGHTRED ,"RD ");
	cprintf(WHITE,"for  delete directory for example rd c:\\Farshad(delet Farshad in c).\n");

	cprintf(LIGHTRED ,"fs ");
	cprintf(WHITE,"for show Menu To Work In Low Level In File System.\n");

	cprintf(LIGHTRED ,"dumpM ");
	cprintf(WHITE,"for Print Memory Allocation.\n");

	cprintf(LIGHTRED ,"help ");
	cprintf(WHITE,"for help system.\n");

}
