#ifndef __FAT_H
#define	__FAT_H

#ifdef __cplusplus
extern "C"
{
#endif

	#include <string.h>
	#include <system.h>
	#include <stdio.h>
	#include <time.h>
	#include <conio.h>
  
	//Proc  
  #define MAX_PROC 32
  #define MAX_Q  0x1000//0x9C800
  #define MAX_FILENAME 0x100
  #define MAX_PATHNAME 512
  #define MAX_OPEN_FILES 0xA

  #define BUF_SIZE 0x200

  #define MAX_PROC_SIZE 0x32000     //200Kb
  #define MAX_DEVICE 5

  #define FLOPPY_PID  2
  #define FS_PID      3



#define FORM_INT(a,b) ((b<<8)|a)
#define FORM_LONG(a,b,c,d) ((d<<24)|(c<<16)|(b<<8)|a)
#define MAX_FILE 10
#define MAX_DIR 10

#define BYTE1(a) (a&0xff)
#define BYTE2(a) ((a>>8)&0xff)
#define BYTE3(a) ((a>>16)&0xff)
#define BYTE4(a) ((a>>24)&0xff)

//Attributes in Root Directory

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_LONG_NAME 0xf
#define ATTR_LONG_NAME_MASK 0x3F

//Start Sector Values

#define BOOT_SEC 0
#define FAT1_SEC 1
#define FAT2_SEC 10
#define ROOT_SEC 19
#define DATA_SEC 33
#define MAX_BLOCK 2849
#define MAX_SECTOR 2880

//Number of Entries

#define DIR_ENTRIES 16


// FAT Entry Values

#define UNUSED 0
#define RESERVED1 0xff0
#define RESERVED2 0xff6
#define BADCLUSTER 0xff7
#define EOF1 0xFF8
#define EOF2 0xFFF
#define EOF(in) (in >= EOF1 && in <= EOF2)

//Root Directory Dir[0] values

#define FREE 0xE5
#define REST_FREE 0x0
#define KANJI 0x05

//Error

#define E_DISK			-1
#define E_FS_NEXISTS 	-2
#define E_FS_EXISTS		-3
#define E_FS_FNAME		-4
#define E_FS_SPACE		-5
#define E_FS_BUSY		-6
#define E_NOT_LONG		-7
#define E_FS_BADREQ		-8

#define O_RDONLY		1
#define O_RDWR			2

#define MAX_FS_REQ		50

//TYPES Of Requests

#define OPEN		1
#define CLOSE		2
#define READ		3
#define WRITE		4
#define CREATEFIL	5
#define OPENDIR		6
#define CREATEDIR	7
#define CLOSEDIR	8
#define READDIR		9
#define REMOVE		10
#define RENAME		11

#define LOAD 		12

struct boot_sector
 {									//Offset
 unsigned char	jumpBoot[3];		//0x00
 unsigned char	SysName[8];			//0x03
 unsigned short charsPerSector;		//0x0b
 unsigned char	SectorsPerCluster;	//0x0d
 unsigned short ReservedSectors;	//0x0e
 unsigned char	FATcount;			//0x10
 unsigned short MaxRootEntries;		//0x11
 unsigned short TotalSectors1;		//0x13
 unsigned char	MediaDescriptor;	//0x15
 unsigned short SectorsPerFAT;		//0x16
 unsigned short SectorsPerTrack;	//0x18
 unsigned short HeadCount;			//0x1a
 unsigned int	HiddenSectors;		//0x1c
 unsigned int	TotalSectors2;		//0x20
 unsigned char	DriveNumber;		//0x24
 unsigned char	Reserved1;			//0x25
 unsigned char	ExtBootSignature;	//0x26
 unsigned int	VolumeSerial;		//0x27
 unsigned char	VolumeLabel[11];	//0x2b
 unsigned char	Reserved2[8];		//0x36
}__attribute__((packed));									
struct dir_entry  
 {
  char Filename[8];
  char Extension[3];
  char Attributes;
  char NTRes;
  char CrtTimeTenth;
  int CrtTime;
  int CrtDate;
  int LstAccDate;
  int FstClusHI;
  int WrtTime;
  int WrtDate;
  int FstClusLO;
  long FileSize;
 }__attribute__((packed));

typedef struct
{
	char name[256];
	char attribute;
	DATE crt_dt;
	TIME crt_time;
	DATE acc_dt;
	long clu;
	long size;
	DATE mod_dt;
	TIME mod_time;
}__attribute__((packed)) DIRENT;

struct long_dir_entry
 {
  char LDIR_Ord;
  unsigned char LDIR_Name1[10];
  char LDIR_Attr;
  char LDIR_Type;
  char LDIR_Chksum;
  unsigned char LDIR_Name2[12];
  int  LDIR_FstClusLO;
  unsigned char LDIR_Name3[4];
 }__attribute__((packed));

struct sector
 {
  int base;
  int length;
 }__attribute__((packed));

struct file_des
 {
    int avl;
    char fname[256];
    int mode;
    int sec_offset;
    int offset;
    unsigned char buffer[512];
    long length;
    long tot_offset;
    struct dir_entry rs;
    int root_sec;
    int root_in;
 }__attribute__((packed));

 extern struct file_des fd[MAX_PROC][MAX_FILE];
struct dir_des
 {
    int avl;
    char fname[256];
    int sec_offset;
    int offset;
    unsigned char buffer[512];
 }__attribute__((packed));

extern struct dir_des dd[MAX_PROC][MAX_DIR];

typedef struct
{
  int type;
  int from_pid;
  union{
	struct{
		char *fname;
		int mode;
	}__attribute__((packed))op;
	struct{
		int fd_in;
	}__attribute__((packed))cl;
	struct{
		int fd_in;
		char *buf;
		int len;
	}__attribute__((packed))re;
	struct{
		int fd_in;
		char *buf;
		int len;
	}__attribute__((packed))wr;
	struct{
		char *name;
	}__attribute__((packed))cr;
	struct{
		char *dir_name;
	}__attribute__((packed))opd;
	struct{
		int dd_in;
	}__attribute__((packed))cld;
	struct{
		char *name;
	}__attribute__((packed))crd;
	struct{
		int dd_in;
		DIRENT *dir;
	}__attribute__((packed))red;
	struct{
		char *old_name;
		char *new_name;
	}__attribute__((packed))ren;
	struct{
		char *name;
	}__attribute__((packed))rem;
  struct{
    char *fname;
  }__attribute__((packed))load;
  };
}__attribute__((packed)) fs_request;

int form_time();
void get_time(int ti,TIME *t);
int form_date();

unsigned char ChkSum (unsigned char *name);
void parse_fat(int * fat_entry,int fat1,int fat2,int in);
int read_fat(int *fat_entry,int in,int st_sec);
int write_fat(int fat,int in,int st_sec);
int write_dir(struct dir_entry *rs,int in,int st_sec);
void print_dir(struct dir_entry *rs);
void parse_dir(unsigned char *a,int in,struct dir_entry *rs);
int read_dir(struct dir_entry *rs,int in,int st_sec);
int write_long_dir(struct long_dir_entry *rs,int in,int st_sec);
void parse_long_dir(struct long_dir_entry *rs,unsigned char *a,int i);
void print_long_dir(struct long_dir_entry *rs);
int read_long_dir(struct long_dir_entry *rs,int in,int st_sec);
void print_boot_sector(struct boot_sector *bs);
int read_boot_sector(struct boot_sector *bs);
int write_boot_sector(struct boot_sector *bs);
int read_sector(int offs,unsigned char *a);
int write_sector(int offs,unsigned char *a);
int clear_sec(int sec);
void print_DIRENT(DIRENT *ds);
int format(char *vlab);
int get_next_sector(int sec);
int get_free_sector(int prev);
int get_free_entry(int *sec,int no_ent);
int remove_chain(long clu);
int get_long_name(char *long_name,unsigned char *a,int j,int *prev_sec,int cur);
int rem_long_name(unsigned char *a,int j,int *prev_sec,int *c);
int search(char *name,struct sector sec,struct dir_entry *rs,int *root_sec,int *root_in,int *prev_sec,int *s_cur);
int get_part(char *name,int i);
int get_dir_entry(char * dirname,struct dir_entry *rs,int *root_sec,int *root_in,int *prev,int *cur);
int do_ls(int pid,char *dirname);
int do_load(char *fname,int pid);

int do_open(int pid,char *fname,int mode);
int do_opendir(int pid,char *fname);
int do_read(int pid,int fd_in,char *buf,int length);
int do_readdir(int pid,int dd_in,DIRENT *dir);
int do_write(int pid,int fd_in,char *buf,int length);
int do_close(int pid,int fd_in);
int do_closedir(int pid,int dd_in);
int do_create(int pid,char *name,int is_dir);
int fs_create(int pid,char *name);
int do_createdir(int pid,char *name);
int do_remove(int pid,char *name);
int do_rename(int pid,char *old_name,char *new_name);
void fs_init();
int do_copy(int pid,char *src,char *dest);
void runFS();
int addfsreq(fs_request *r);
void fscloseAll(dword pid);


#ifdef __cplusplus
}
#endif

#endif

