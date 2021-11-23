#include <fat.h>

volatile fs_request fsreqs[MAX_FS_REQ];
int fs_en_req = 0;
int fs_fr_req = 0;
volatile int no_fs_req = 0;
struct dir_des dd[MAX_PROC][MAX_DIR];
struct file_des fd[MAX_PROC][MAX_FILE];


int form_time()
 {
  int hh,mm,ss,tm;
  TIME t = {0,0,0};
  gettime(&t);
  hh = t.hour;
  mm = t.minute;
  ss = t.second/2;
  printf("%d %d %d\n",hh,mm,ss);
  tm =  (hh << 11) | (mm << 5) | ss;
  printf("tm %x\n",tm);
  return tm;
 }

void get_time(int ti,TIME *t)
{
	t->hour = (ti >> 11) & 0x1F;
	t->minute  = (ti >> 5) & 0x3F;
	t->second = ti & 0x1F;
}

int form_date()
 {
  int dd,mm,yy,dt;
  DATE d = {0,0,0,0};
  getdate(&d);
  yy = d.year;
  mm = d.month;
  dd = d.day;
  dt = ((yy-1980) <<9) | (mm << 5) | dd;
  printf("dt %x\n",dt);
  return dt;
 }

void get_date(int dt,DATE *d)
{
	d->day = dt & 0x1F;
	d->month = (dt >> 5) & 0xF;
	d->year = (dt >> 9) & 0x7F;
	d->year = d->year + 1980;
}

unsigned char ChkSum (unsigned char *name)
{
 short len;
 unsigned char sum;
 sum = 0;
 for (len=11; len!=0; len--)
 {
 sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *name++;
 }
 return (sum);
}

void parse_fat(int * fat_entry,int fat1,int fat2,int in)
 {
  int fat;
  if(in%2 == 0)
    fat = ((fat2 & 0x0f) << 8) | fat1;
  else
    fat = (fat2 << 4) | ((fat1 &0x0f0) >> 4);

   fat = fat & 0xFFF;
  *fat_entry = fat;

  //#ifdef DEBUG
   printf("\nfat1 = %d %x fat2 %d %x",fat1,fat1,fat2,fat2);
   printf("\nfat: %d %x\n",fat,fat);
  //#endif
 }

int read_fat(int *fat_entry,int in,int st_sec)
 {
  int fat_in,res;
  unsigned char a[512];
  int fat1,fat2;
  fat_in = (in*3)/2;
  st_sec = st_sec + fat_in/512;
  if(fat_in % 512 == 511)
   {
    //spans sectors
    res = read_sector(st_sec,a);
    if(res != 0)
     return E_DISK;
    fat1 = a[511];
    res = read_sector(st_sec+1,a);
    fat2 = a[0];
   }
  else
   {
    fat_in = fat_in%512;
    res = read_sector(st_sec,a);
    if(res != 0)
     return E_DISK;
    fat1 = a[fat_in] & 0xff;
    fat2 = a[fat_in+1] & 0xff;
   }
  parse_fat(fat_entry,fat1,fat2,in);
 //Even And Odd logical clusters

  return 0;
 }

int write_fat(int fat,int in,int st_sec)
 {
  int fat_in,res;
  unsigned char a[512],b[512];
  char fat1,fat2;


  //2fat index from cluster index

  fat_in = (in*3)/2;

  //Sector and index from fat_index

  st_sec = st_sec + fat_in/512;
  fat_in = fat_in%512;

  printf("writting fat %d to sec %d in %d sector_in %d",fat,st_sec,fat_in,in + DATA_SEC -2);

  if(fat_in == 511)
   {
    //spans sectors
    res = read_sector(st_sec,a);
    if(res != 0)
       return E_DISK;
    fat1 = a[511];
    res = read_sector(st_sec+1,b);
    if(res != 0)
      return E_DISK;
    fat2 = b[0];
   }
  else
   {
    res = read_sector(st_sec,a);
    if(res != 0)
       return E_DISK;
    fat1 = a[fat_in];
    fat2 = a[fat_in+1];
   }

   //Even and odd cluster

    if(in%2 == 0)
     {
      fat1 = fat &0x0ff;
      fat2 = (fat2 & 0xf0) | ((fat & 0xf00) >> 8);
     }
    else
     {
      fat1 = (fat1 &0x0f) | ( (fat & 0x00f) << 4);
      fat2 = (fat  & 0xff0) >>4;
     }

    //Writting back

    if(fat_in == 511)
     {
      a[511]=fat1;
      b[0] = fat2;
      res = write_sector(st_sec,a);
      if(res != 0)
       return E_DISK;
      res = write_sector(st_sec+1,b);
      if(res != 0)
       return E_DISK;
     }
    else
     {
      a[fat_in] = fat1;
      a[fat_in+1] = fat2;
      res = write_sector(st_sec,a);
      if(res != 0)
       return E_DISK;
     }
     return 0;
 }

int write_dir(struct dir_entry *rs,int in,int st_sec)
 {
   unsigned char a[512];
   int i,j;
   st_sec = st_sec + in/DIR_ENTRIES;

   if(read_sector(st_sec,a) != 0)
    return E_DISK;
   printf("writting dir to sec %d in %d\n",st_sec,in);
   i = (in % DIR_ENTRIES) *32;

   for(j=0;j<8;j++,i++)
    a[i] = rs->Filename[j];

   for(j=0;j<3;j++,i++)
    a[i] = rs->Extension[j];

   a[i++] = rs->Attributes;
   a[i++] = rs->NTRes;
   a[i++] = rs->CrtTimeTenth;

   a[i] = BYTE1(rs->CrtTime);

   a[i+1] = BYTE2(rs->CrtTime);
   i+= 2;

   a[i] = BYTE1(rs->CrtDate);
   a[i+1] = BYTE2(rs->CrtDate);
   i+= 2;

   a[i] = BYTE1(rs->LstAccDate);
   a[i+1] = BYTE2(rs->LstAccDate);
   i+=2;

   a[i] = BYTE1(rs->FstClusHI);
   a[i+1] = BYTE2(rs->FstClusHI);
   i+=2;

   a[i] = BYTE1(rs->WrtTime);
   a[i+1] = BYTE2(rs->WrtTime);
   i+=2;

   a[i] = BYTE1(rs->WrtDate);
   a[i+1] = BYTE2(rs->WrtDate);
   i+=2;

   a[i] = BYTE1(rs->FstClusLO);
   a[i+1] = BYTE2(rs->FstClusLO);
   i+=2;

   a[i] = BYTE1(rs->FileSize);
   a[i+1] = BYTE2(rs->FileSize);
   a[i+2] = BYTE3(rs->FileSize);
   a[i+3] = BYTE4(rs->FileSize);
   i+=4;

   return write_sector(st_sec,a);
 }
void print_dir(struct dir_entry *rs)
 {
  int j;
   printf("File Name : ");
    for(j=0;j<8;j++)
     printf("%c",rs->Filename[j]);
    printf("\n");

   printf("Extension : ");
    for(j=0;j<3;j++)
     printf("%c",rs->Extension[j]);
    printf("\n");

   printf("Attributes = %c %d %x\n",rs->Attributes ,rs->Attributes ,rs->Attributes );
   printf("NTRes = %c %d %x\n",rs->NTRes ,rs->NTRes ,rs->NTRes);
   printf("CrtTimeTenth = %c %d %x\n",rs->CrtTimeTenth ,rs->CrtTimeTenth ,rs->CrtTimeTenth );
   printf("CrtTime = %c %d %x\n",rs->CrtTime ,rs->CrtTime ,rs->CrtTime );
   printf("CrtDate = %c %d %x\n",rs->CrtDate ,rs->CrtDate ,rs->CrtDate );
   printf("LstAccDate = %c %d %x\n",rs->LstAccDate ,rs->LstAccDate ,rs->LstAccDate );
   printf("FstClusHI = %c %d %x\n",rs->FstClusHI ,rs->FstClusHI ,rs->FstClusHI );
   printf("WrtTime = %c %d %x\n",rs->WrtTime ,rs->WrtTime ,rs->WrtTime );
   printf("WrtDate = %c %d %x\n",rs->WrtDate ,rs->WrtDate ,rs->WrtDate );
   printf("FstClusLO = %c %d %x\n",rs->FstClusLO ,rs->FstClusLO ,rs->FstClusLO );
   printf("FileSize = %c %d %x\n",(unsigned int)rs->FileSize ,(unsigned int)rs->FileSize ,(unsigned int)rs->FileSize );

 }
void parse_dir(unsigned char *a,int in,struct dir_entry *rs)
 {
   int i,j;

   i = (in %DIR_ENTRIES) * 32;

   for(j=0;j<8;j++,i++)
    rs->Filename[j] = a[i];

   for(j=0;j<3;j++,i++)
    rs->Extension[j] = a[i];

   rs->Attributes = a[i++];
   rs->NTRes = a[i++];
   rs->CrtTimeTenth = a[i++];

   rs->CrtTime = FORM_INT(a[i],a[i+1]);
   i+= 2;

   rs->CrtDate = FORM_INT(a[i],a[i+1]);
   i+= 2;

   rs->LstAccDate = FORM_INT(a[i],a[i+1]);
   i+=2;

   rs->FstClusHI = FORM_INT(a[i],a[i+1]);
   i+=2;

   rs->WrtTime = FORM_INT(a[i],a[i+1]);
   i+=2;

   rs->WrtDate = FORM_INT(a[i],a[i+1]);
   i+=2;

   rs->FstClusLO = FORM_INT(a[i],a[i+1]);
   i+=2;

   rs->FileSize = FORM_LONG(a[i],a[i+1],a[i+2],a[i+3]);
   i+=4;
 }


int read_dir(struct dir_entry *rs,int in,int st_sec)
 {
   unsigned char a[512];   
   st_sec = st_sec + in/DIR_ENTRIES;
   if(read_sector(st_sec,a) != 0)
    return E_DISK;
   parse_dir(a,in,rs);

   #ifdef DEBUG
    print_dir(rs);
   #endif
   return 0;
 }

int write_long_dir(struct long_dir_entry *rs,int in,int st_sec)
 {
   unsigned char a[512];
   int i,j;
   if(read_sector(st_sec,a) != 0)
    return E_DISK;
   i = in * 32;

   a[i++] = rs->LDIR_Ord;

   for(j=0;j<10;j++,i++)
    a[i] = rs->LDIR_Name1[j];

   a[i++] = rs->LDIR_Attr;
   a[i++] = rs->LDIR_Type;
   a[i++] = rs->LDIR_Chksum;

   for(j=0;j<12;j++,i++)
    a[i] = rs->LDIR_Name2[j];

   a[i] = BYTE1(rs->LDIR_FstClusLO);
   a[i+1] = BYTE2(rs->LDIR_FstClusLO);
   i+= 2;

   for(j=0;j<4;j++,i++)
    a[i] = rs->LDIR_Name3[j];

   return write_sector(st_sec,a);
 }

void parse_long_dir(struct long_dir_entry *rs,unsigned char *a,int i)
{
   int j;
   i = i*32;

   rs->LDIR_Ord =  a[i++];

   for(j=0;j<10;j++,i++)
    rs->LDIR_Name1[j] =  a[i];

   rs->LDIR_Attr =  a[i++];
   rs->LDIR_Type =  a[i++];
   rs->LDIR_Chksum = a[i++];

   for(j=0;j<12;j++,i++)
     rs->LDIR_Name2[j] = a[i];

   rs->LDIR_FstClusLO = FORM_INT(a[i],a[i+1]);
   i+= 2;

   for(j=0;j<4;j++,i++)
    rs->LDIR_Name3[j] = a[i] ;
}

void print_long_dir(struct long_dir_entry *rs)
 {
   int i,j;
    printf("Long Name : ");
    for(j=0;j<10;j++,i++)
     printf("%c",rs->LDIR_Name1[j]);
    printf(" ");
    for(j=0;j<12;j++,i++)
      printf("%c",rs->LDIR_Name2[j]);
    printf(" ");
    for(j=0;j<4;j++,i++)
      printf("%c",rs->LDIR_Name3[j]);
    printf("\n");

    printf("LDIR_Attr : %d %x\n",rs->LDIR_Attr,rs->LDIR_Attr);
	printf("LDIR_Ord : %d %x\n",rs->LDIR_Ord,rs->LDIR_Ord);
    printf("LDIR_Type :%d %x\n",rs->LDIR_Type,rs->LDIR_Type);
    printf("LDIR_Chksum : %d %x\n",rs->LDIR_Chksum,rs->LDIR_Chksum);
    printf("LDIR_FstClusLO : %d %x\n",rs->LDIR_FstClusLO,rs->LDIR_FstClusLO);
 }

int read_long_dir(struct long_dir_entry *rs,int in,int st_sec)
 {
   unsigned char a[512];
   int i;
   st_sec = st_sec + in/DIR_ENTRIES;
   if(read_sector(st_sec,a) != 0)
    return E_DISK;
   i = (in %DIR_ENTRIES) * 32;
   if(a[i+11] == ATTR_LONG_NAME)
   {
    parse_long_dir(rs,a,i);

   #ifdef DEBUG
    print_long_dir(rs);
   #endif
	return 1;
   }
   else
    return E_NOT_LONG;   
}

void print_boot_sector(struct boot_sector *bs)
{
  printf("jumpBoot= %s\n",bs->jumpBoot);
  printf("SysName = %s\n",bs->SysName);
  printf("charsPerSector = %d\n",bs->charsPerSector);
  printf("SectorsPerCluster = %d\n",bs->SectorsPerCluster);
  printf("ReservedSectors = %d\n",bs->ReservedSectors);
  printf("FATcount = %d\n",bs->FATcount);
  printf("MaxRootEntries = %d\n",bs->MaxRootEntries);
  printf("TotalSectors1 = %d\n",bs->TotalSectors1);
  printf("MediaDescriptor = %d\n",bs->MediaDescriptor);
  printf("SectorsPerFAT = %d\n",bs->SectorsPerFAT);
  printf("SectorsPerTrack = %d\n",bs->SectorsPerTrack);
  printf("HeadCount = %d\n",bs->HeadCount);
  printf("HiddenSectors = %d\n",(unsigned int)bs->HiddenSectors);
  printf("TotalSectors2 = %d\n",(unsigned int)bs->TotalSectors2);
  printf("DriveNumber = %d\n",bs->DriveNumber);
  printf("Reserved1 = %d\n",bs->Reserved1);
  printf("ExtBootSignature = %d\n",bs->ExtBootSignature);
  printf("VolumeSerial = %d\n",(unsigned int)bs->VolumeSerial);
  printf("VolumeLabel = %s\n",bs->VolumeLabel);
  printf("Reserved2 = %s\n",bs->VolumeLabel);
}

int read_boot_sector(struct boot_sector *bs)
 {
  int i,j;

  unsigned char a[512];

  if(read_sector(0,a) != 0)
   return E_DISK;

  i=0;
  for(j=0;j<3;j++)
    bs->jumpBoot[j]=(char)a[j];

  i+= 3;
  for(j=0;j<8;j++,i++)
    bs->SysName[j] = a[i];

  bs->charsPerSector = FORM_INT(a[i],a[i+1]);
  i+=2;

  bs->SectorsPerCluster = a[i++];

  bs->ReservedSectors = FORM_INT(a[i],a[i+1]);
  i+=2;

  bs->FATcount = a[i++];

  bs->MaxRootEntries = FORM_INT(a[i],a[i+1]);
  i+=2;

  bs->TotalSectors1 =  FORM_INT(a[i],a[i+1]);
  i+=2;

  bs->MediaDescriptor = a[i++];

  bs->SectorsPerFAT = FORM_INT(a[i],a[i+1]);
  i+=2;

  bs->SectorsPerTrack = FORM_INT(a[i],a[i+1]);
  i+=2;

  bs->HeadCount = FORM_INT(a[i],a[i+1]);
  i+=2;

  bs->HiddenSectors = FORM_LONG(a[i],a[i+1],a[i+2],a[i+3]);
  i+=4;


  bs->TotalSectors2 = FORM_LONG(a[i],a[i+1],a[i+2],a[i+3]);
  i+=4;

  bs->DriveNumber = a[i++];
  bs->Reserved1 = a[i++];
  bs->ExtBootSignature = a[i++];
  bs->VolumeSerial = FORM_LONG(a[i],a[i+1],a[i+2],a[i+3]);
  i+=4;

  for(j=0;j<11;j++,i++)
   bs->VolumeLabel[j] = a[i];
  for(j=0;j<8;j++,i++)
   bs->Reserved2[j] = a[i];

  #ifdef DEBUG
   print_boot_sector(bs);
  #endif
  return 0;
 }

int write_boot_sector(struct boot_sector *bs)
 {
  int i,j;
  unsigned char a[512];

  i=0;
  for(j=0;j<3;j++)
    a[j]=bs->jumpBoot[j];

  i+= 3;
  for(j=0;j<8;j++,i++)
    a[i]= bs->SysName[j];

  a[i] = BYTE1(bs->charsPerSector);
  a[i+1] =BYTE2(bs->charsPerSector);
  i+=2;

  a[i++] = bs->SectorsPerCluster;

  a[i] = BYTE1(bs->ReservedSectors);
  a[i+1] =BYTE2(bs->ReservedSectors);
  i+=2;

  a[i++] = bs->FATcount;

  a[i] = BYTE1(bs->MaxRootEntries);
  a[i+1] =BYTE2(bs->MaxRootEntries);
  i+=2;

  a[i] = BYTE1(bs->TotalSectors1);
  a[i+1] =BYTE2(bs->TotalSectors1);
  i+=2;

  a[i++] = bs->MediaDescriptor;

  a[i] = BYTE1(bs->SectorsPerFAT);
  a[i+1] =BYTE2(bs->SectorsPerFAT);
  i+=2;

  a[i] = BYTE1(bs->SectorsPerTrack);
  a[i+1] =BYTE2(bs->SectorsPerTrack);
  i+=2;

  a[i] = BYTE1(bs->HeadCount);
  a[i+1] =BYTE2(bs->HeadCount);
  i+=2;

  a[i] = BYTE1(bs->HiddenSectors);
  a[i+1] =BYTE2(bs->HiddenSectors);
  a[i+2] = BYTE3(bs->HiddenSectors);
  a[i+3] =BYTE4(bs->HiddenSectors);
  i+=4;

  a[i] = BYTE1(bs->TotalSectors2);
  a[i+1] =BYTE2(bs->TotalSectors2);
  a[i+2] = BYTE3(bs->TotalSectors2);
  a[i+3] =BYTE4(bs->TotalSectors2);
  i+=4;

  a[i++] = bs->DriveNumber;
  a[i++] = bs->Reserved1;
  a[i++] = bs->ExtBootSignature;

  a[i] = BYTE1(bs->VolumeSerial);

  a[i+1] =BYTE2(bs->VolumeSerial);
  a[i+2] = BYTE3(bs->VolumeSerial);
  a[i+3] =BYTE4(bs->VolumeSerial);
  i+=4;

  for(j=0;j<11;j++,i++)
   a[i] = bs->VolumeLabel[j];

  for(j=0;j<8;j++,i++)
  a[i] = bs->Reserved2[j];

  a[510]=0x55;
  a[511]=0xAA;
  return write_sector(0,a);

 }

int read_sector(int offs,unsigned char *a)
 {
	 HDD_RW(offs, HD_READ, 1, a);
	return 0;
 }



int write_sector(int offs,unsigned char *a)
 {
	HDD_RW(offs, HD_WRITE, 1, a); 
	return 0;
 }
int clear_sec(int sec)
{

  int i;
  unsigned char a[512];

  if(sec < 0 || sec > MAX_SECTOR)
   return E_DISK;
  for(i=0;i<512;i++)
   a[i] = 0;
  write_sector(sec,a);

  return 0;

}

void print_DIRENT(DIRENT *ds)
 {
   printf("File Name : %s",ds->name);

   printf("Attributes = %c %d %x\n",ds->attribute ,ds->attribute ,ds->attribute );
   printf("CrtDt d %d m %d y %d\n",ds->crt_dt.day ,ds->crt_dt.month ,ds->crt_dt.year );
   printf("CRttime h %d min %d sec %d\n",ds->crt_time.hour ,ds->crt_time.minute ,ds->crt_time.second );
   printf("AccDt d %d m %d y %d\n",ds->acc_dt.day ,ds->acc_dt.month ,ds->acc_dt.year );
   printf("WrtDt d %d m %d y %d\n",ds->mod_dt.day ,ds->mod_dt.month ,ds->mod_dt.year );
   printf("Wrttime h %d min %d sec %d\n",ds->mod_time.hour ,ds->mod_time.minute ,ds->mod_time.second );
   printf("Clus = %d \n",(unsigned int)ds->clu);
   printf("FileSize = %d \n",(unsigned int)ds->size);
 }

int format(char *vlab)
 {
  int res;
  struct boot_sector b;
  struct dir_entry d;
  unsigned char a[512];
  int i,j,dt,tm;

  for(i=0;i<11;i++)
   if(vlab[i] == '\0')
    break;
  for(j=i;j<11;j++)
   vlab[j] = ' ';
  b.jumpBoot[0] = 0xeb;
  b.jumpBoot[1] = 0x3c;
  b.jumpBoot[2] = 0x90;
  b.SysName[0] = 'D';
  b.SysName[1] = 'Y';
  b.SysName[2] = 'N';
  b.SysName[3] = 'A';
  b.SysName[4] = 'C';
  b.SysName[5] = 'U';
  b.SysName[6] = 'B';
  b.SysName[7] = 'E';
  b.charsPerSector= 512;
  b.SectorsPerCluster = 1;
  b.ReservedSectors = 1;
  b.FATcount = 2;
  b.MaxRootEntries = 224;
  b.TotalSectors1 = 2880;
  b.MediaDescriptor = 0xF0;
  b.SectorsPerFAT = 9;
  b.SectorsPerTrack = 18;
  b.HeadCount = 2;
  b.HiddenSectors = 0;
  b.TotalSectors2 = 0;
  b.DriveNumber = 0;
  b.Reserved1 = 0;
  b.ExtBootSignature = 0x29;
  dt = form_date();
  tm = form_time();
  dt = ((dt & 0xff) << 8) | ((dt & 0xff00) >> 8);
  tm = ((tm & 0xff) << 8) | ((tm & 0xff00) >> 8);
  b.VolumeSerial = (tm << 16) + dt;
  for(i=0 ; i<11 ; i++)
   b.VolumeLabel[i] = vlab[i];
  b.Reserved2[0] = 'F';
  b.Reserved2[1] = 'A';
  b.Reserved2[2] = 'T';
  b.Reserved2[3] = '1';
  b.Reserved2[4] = '2';
  b.Reserved2[5] = ' ';
  b.Reserved2[6] = ' ';
  b.Reserved2[7] = ' ';
  res = write_boot_sector(&b);
  if(res != 0)
   return E_DISK;

  for(i=0;i<512;i++)
    a[i] = 0;
  printf(" 03 %% Format Completed\r");
  for(i=1;i<33;i++)
  {
   res = write_sector(i,a);
   if(res != 0)
    return E_DISK;

   printf(" %d %%\r",(int)i*100/38);
  }

  //Write two reserved FAT entries and others as 0

  a[0] = 0xf0;
  a[1] = 0xff;
  a[2] = 0xff;
  for(i=3;i<512;i++)
   a[i] = 0;
  write_sector(FAT1_SEC,a);
  write_sector(FAT2_SEC,a);

  a[0] = 0; a[1] =0; a[2] =0;
  for(i=FAT1_SEC+1;i<FAT2_SEC;i++)
   {
    res = write_sector(i,a);
	if(res != 0)
		return E_DISK;
    res = write_sector(i+9,a);
	if(res != 0)
		return E_DISK;
  }
  //form volume root dir entry
  for(i=0;i<8;i++)
    d.Filename[i] = vlab[i];
  for(i=0;i<3;i++)

    d.Extension[i] = vlab[8+i];
  d.Attributes = ATTR_VOLUME_ID | ATTR_ARCHIVE;
  d.NTRes = 0;
  d.CrtTimeTenth = 0;
  d.CrtTime=0;
  d.CrtDate=0;
  d.LstAccDate=0;
  d.FstClusHI=0;
  d.WrtTime=form_time();
  d.WrtDate=form_date();
  d.FstClusLO=0;
  d.FileSize=0;
  res = write_dir(&d,0,ROOT_SEC);
  if(res != 0)
   return E_DISK;
  printf(" %d %%\r",100);
  printf("\nFormat Complete.\n");
  return 0;
 }






int get_next_sector(int sec)
 {
  int cluster = sec - DATA_SEC + 2;
  int next_clu,next_sec;
  printf("sec : %d",sec);
  if(sec >= ROOT_SEC && sec < DATA_SEC)
  {
	  if(sec+1 < DATA_SEC)
		return sec+1;
	  else
		  return EOF1;
  }
  read_fat(&next_clu,cluster,FAT1_SEC);
  printf("cluster : %d\n",cluster);
  if(EOF(next_clu))
   return next_clu;
  next_sec = next_clu -2 + DATA_SEC;
  return next_sec;
 }
int get_free_sector(int prev)
 {
  unsigned char a[512];
  int fat1,fat2,fat_entry,fat_in,in,res,next_sec;
  if(prev < DATA_SEC)
	  return EOF1;
  if(!EOF(prev))
	prev = prev - DATA_SEC +2;
  else
	prev = 0;
  next_sec = prev*3/2;
  next_sec = FAT1_SEC+ next_sec/512;
  if(read_sector(next_sec,a) != 0)
   return EOF1;

  for(in=prev;in<MAX_BLOCK;in++)
   {
     fat_in = (in*3)/2;
     if(fat_in % 512 == 511)
     {
      //spans sectors
      fat1 = a[511];
      res = read_sector(next_sec+1,a);
      if(res != 0)
       return EOF1;
      next_sec++;
      fat2 = a[0];
     }
    else
     {
      fat_in = fat_in%512;
      fat1 = a[fat_in] & 0xff;
      fat2 = a[fat_in+1] & 0xff;
     }
     parse_fat(&fat_entry,fat1,fat2,in);
     if(fat_entry == 0)
      return (in + DATA_SEC -2);
   }
   return EOF1;
 }
int get_free_entry(int *sec,int no_ent)
{
  int st_sec,i,unused,u,u_sec,u_ent,prev_sec;
  unsigned char a[512];
  st_sec = *sec;
  unused = 0;
  u = 0;
  while(1)
  {
	  if(read_sector(*sec,a) != 0)
		  return E_DISK;
	  for(i=0;i<16;i++)

	  {
		  if(unused != 0 && a[i*32] != 0xE5)
		  	  unused =0;

		  if(a[i*32] == 0x0)
			  return i;
		  else if(a[i*32] == 0xE5)
		  {
			if(!unused)
			{
				u = 0;
				u_ent = i;
				u_sec = *sec;
				unused = 1;
			}
			u++;
			if(u >= no_ent)
			{
				*sec = u_sec;
				return u_ent;
			}
			printf("unused : %d u:%d u_ent : %d\n",unused,u,u_ent);
		  }
	  }
	  *sec = get_next_sector(*sec);
	  if(EOF(*sec))
	  {
		  if(st_sec == ROOT_SEC)
			break;
		  else
		  {
			prev_sec = *sec;
			*sec = get_free_sector(*sec);
			if(!EOF(*sec))
			{
			write_fat(*sec-DATA_SEC +2,prev_sec - DATA_SEC + 2,FAT1_SEC);
			write_fat(*sec-DATA_SEC +2,prev_sec - DATA_SEC + 2,FAT2_SEC);
			write_fat(EOF1,*sec-DATA_SEC +2,FAT1_SEC);
			write_fat(EOF1,*sec-DATA_SEC +2,FAT2_SEC);
			}
			else
				break;
    	  }
	  }
  }
  return E_DISK;
}

int remove_chain(long clu)
{
  long fat1,fat2,fat_entry,fat_in,in,res,next_sec;
  int prev_sec;
  unsigned char a[512];


  if(clu == 0 || EOF(clu))
	return 0;

  prev_sec = (clu*3)/2;
  prev_sec = FAT1_SEC+ prev_sec/512;

  in = clu;
  printf("next fat : %d prev_Sec %d",(unsigned int)in,prev_sec);

  if(read_sector(prev_sec,a) != 0)
        return E_DISK;

  while(!EOF(in))
   {
     fat_in = (in*3)/2;
     next_sec = FAT1_SEC+ fat_in/512;


	 if(next_sec != prev_sec)
	 {
	  prev_sec = next_sec;
      if(read_sector(next_sec,a) != 0)
        return E_DISK;
	 }

	 if(fat_in % 512 == 511)
     {
      //spans sectors
      fat1 = a[511];
      res = read_sector(next_sec+1,a);
      if(res != 0)
       return EOF1;
      prev_sec = next_sec+1;
      fat2 = a[0];
     }
    else
     {
      fat_in = fat_in%512;
      fat1 = a[fat_in] & 0xff;
      fat2 = a[fat_in+1] & 0xff;
     }
     parse_fat((int *)&fat_entry,fat1,fat2,in);
	 if(fat_entry == 0)
      return E_DISK;
	 write_fat(0,in,FAT1_SEC);
	 write_fat(0,in,FAT2_SEC);
	 printf("next fat : %d",(unsigned int)in);
	 in = fat_entry;
   }
   return 0;
}

int get_long_name(char *long_name,unsigned char *a,int j,int *prev_sec,int cur)
 {
  int i,k,sec,in_long,lo_len;
  struct long_dir_entry lo;
  struct dir_entry rs;
  unsigned char chksum,name[15];
  unsigned char b[512];
  sec = prev_sec[cur];
  parse_dir(a,j,&rs);
  for(i=0;i<8;i++)
           name[i] = rs.Filename[i];
  for(;i<12;i++)
           name[i] = rs.Extension[i-8];
  name[i] = '\0';
  printf("%s",name);
  for(i=0;i<512;i++)
	  b[i] = a[i];
  chksum = ChkSum(name);
  in_long = 0;
  i = j;
  printf("i = %d sec = %d chksum = %d",i,sec,chksum);
  lo_len = 0;
  long_name[lo_len] = '\0';
  while(1)
   {
    i--;
    if(i == -1)
	{
		cur--;
		sec = prev_sec[cur];
		printf("sec %d cur%d",sec,cur);
		if(cur < 0)
			return E_DISK;
		if(read_sector(sec,b) != 0)
			return E_DISK;
		i = 15;
	}
		parse_long_dir(&lo,b,i);
        print_long_dir(&lo);
		printf("long_len %d\n",lo_len);
        if((lo.LDIR_Chksum != chksum) || ((lo.LDIR_Attr & ATTR_LONG_NAME) != ATTR_LONG_NAME))
			return E_DISK;
		else
		{
            in_long++;
			if(lo.LDIR_Ord & 0x40)
			{
				for(k=0;k<10;k++)
				{
					if((lo.LDIR_Name1[k] == 0x00) && (lo.LDIR_Name1[k+1] == 0x00))
						break;
					if((lo.LDIR_Name1[k] == 0xFF) && (lo.LDIR_Name1[k+1] == 0xFF))
						break;
					else if(k%2 ==0)
						long_name[lo_len++] = lo.LDIR_Name1[k];
				}
				for(k=0;k<12;k++)
				{
					if((lo.LDIR_Name2[k] == 0x00) && (lo.LDIR_Name2[k+1] == 0x00))
						break;
					if((lo.LDIR_Name2[k] == 0xFF) && (lo.LDIR_Name2[k+1] == 0xFF))
						break;
					else if(k%2 ==0)
						long_name[lo_len++] = lo.LDIR_Name2[k];
				}
				for(k=0;k<4;k++)
				{
					if((lo.LDIR_Name3[k] == 0x00) && (lo.LDIR_Name3[k+1] == 0x00))
						break;
					if((lo.LDIR_Name3[k] == 0xFF) && (lo.LDIR_Name3[k+1] == 0xFF))
						break;
					else if(k%2 ==0)
						long_name[lo_len++] = lo.LDIR_Name3[k];
				}
				long_name[lo_len] = '\0';
				printf("%s %d",long_name,lo_len);
				return 0;
			}
          else if(lo.LDIR_Ord == in_long)
			{
				for(k=0;k<10;k++)
				{
					if(k%2 ==0)
						long_name[lo_len++] = lo.LDIR_Name1[k];

				}
				for(k=0;k<12;k++)
				{
					if(k%2 ==0)
						long_name[lo_len++] = lo.LDIR_Name2[k];
				}
				for(k=0;k<4;k++)
				{
					if(k%2 ==0)
						long_name[lo_len++] = lo.LDIR_Name3[k];
				}
			}
           else
             return E_DISK;
	}
   }
 }

int rem_long_name(unsigned char *a,int j,int *prev_sec,int *c)
{
  int i,sec,in_long;
  struct long_dir_entry lo;
  struct dir_entry rs;
  unsigned char chksum,name[15];
  int cur = *c;
  unsigned char b[512];


  sec = prev_sec[cur];
  parse_dir(a,j,&rs);


  for(i=0;i<8;i++)
           name[i] = rs.Filename[i];
  for(;i<12;i++)
           name[i] = rs.Extension[i-8];
  name[i] = '\0';
  printf("%s",name);
  for(i=0;i<512;i++)
	  b[i] = a[i];
  chksum = ChkSum(name);
  in_long = 0;
  i = j;
  printf("i = %d sec = %d chksum = %d",i,sec,chksum);
  while(1)
   {
    i--;
    if(i == -1)
	{
		cur--;
		sec = prev_sec[cur];
		printf("sec %d cur%d",sec,cur);
		if(cur < 0)
			return 0;
		if(read_sector(sec,b) != 0)
			return E_DISK;
		i = 15;
	}
		parse_long_dir(&lo,b,i);
        print_long_dir(&lo);
        if((lo.LDIR_Chksum != chksum) || ((lo.LDIR_Attr & ATTR_LONG_NAME) != ATTR_LONG_NAME))
			return 0;
		else
		{
            in_long++;

			if(lo.LDIR_Ord & 0x40)
			{
				lo.LDIR_Ord = 0xE5;
				printf("\nfinal long: sec %d in %d",sec,i);
				if(write_long_dir(&lo,i,sec) != 0)
					return E_DISK;
				return 0;
			}
            else if(lo.LDIR_Ord == in_long)
			{

				lo.LDIR_Ord = 0xE5;

				printf("\nlong: sec %d in %d",sec,i);
				if(write_long_dir(&lo,i,sec) != 0)
					return E_DISK;
			}
           else
             return 0;
		}
  }
}


int search(char *name,struct sector sec,struct dir_entry *rs,int *root_sec,int *root_in,int *prev_sec,int *s_cur)
 {
   int j,res,next_sec,found = 0,p,no_sec,q;
   unsigned char a[512];
   char name1[15],long_name[255];
   int cur;
   if(strlen(name) <= 0)
	   return -1;
   no_sec = sec.base;
   next_sec = sec.base;
   cur = 0;
   prev_sec[cur] = sec.base;
   cur++;
   printf("search:");
   while(!found)
    {
     printf("next_sec: %d",next_sec);
     res = read_sector(next_sec,a);
     if(res != 0)
      return E_DISK;
     for(j=0;j<16 && !found;j++)
      {
	   printf("j = %d a = %d %x first: %x\n",j,a[j*32+11],a[j*32+11],a[j*32]);
       if(a[j*32] == 0xE5)
        continue;
       else if(a[j*32] == 0)
        return E_FS_NEXISTS;
       else
        {
         if(((a[j*32+11] & ATTR_LONG_NAME) == 0) && ((a[j*32+11] & ATTR_VOLUME_ID) == 0))
          {
           printf("yes %d\n",strlen(name));
           parse_dir(a,j,rs);
    		   print_dir(rs);
           if(strlen(name) <= 12)
            {
             Strcpy(name1,rs->Filename );
             for(p=0;p<8;p++)
              if(name1[p]== 32) // ' '
               break;
             name1[p++] = '.';
             for(q=0;q<3;q++,p++)
		    	   {
				   if(rs->Extension[q] == ' ')
					   break;
				   name1[p] = rs->Extension[q];
			       }
               	if(q == 0)
        					p--;
               name1[p] = '\0';
               clrscr();
              printf("Name1 : %s\n",name1);
              printf("Name : %s\n",name);
             if(Strcmpi(name,(char *)name1) == 0)
              {
                found = 1;
                *root_sec = next_sec;
                *root_in = j;
				        *s_cur = cur - 1;
                break;
              }
            }
              if(get_long_name(long_name,a,j,prev_sec,cur-1) == 0)
        			  {
                clrscr();
                printf("Name : %s \n",name);
				        printf("LongName : %s\n",long_name);
        				if(Strcmpi(long_name,name) == 0)
        				{
                                    found = 1;
                                    *root_sec = next_sec;
                                    *root_in = j;
				                  					*s_cur = cur-1;
                                    break;
				}
			  }
        }//if
      }//else

    }//for
      if(found)
       break;
      next_sec = get_next_sector(next_sec);
      if(EOF(next_sec))
          break;
	  prev_sec[cur++] = next_sec;
   }//while
   if(!found)
    return E_FS_NEXISTS;
   return 0;
 }

int get_part(char *name,int i)
 {
  int j;
  for(j=i;j<strlen(name);j++)
   if(name[j] == '\\')
    return j;
  return j;
 }

int get_dir_entry(char * dirname,struct dir_entry *rs,int *root_sec,int *root_in,int *prev,int *cur)
 {
  int len,i,j,k,res;
  struct sector sec;
  char name[256];
  len = strlen(dirname);
  if(dirname[0] =='\\') //if(dirname[0] ='\\')
   {
    sec.base = ROOT_SEC;
    sec.length = 14;
   }
  else
   {
	return E_FS_NEXISTS;
   }
   i=0;
   printf("len :%d ",len);
  while(i < len)
   {
    j=i+1;
    i = get_part(dirname,i+1);
    printf("i = %d",i);
    for(k=0;j<i;j++,k++)
      name[k]=dirname[j];
    name[k] = '\0';
    cli();
    printf("name : %s",name);
//    delay(0xFFFF);
    sti();
    res = search(name,sec,rs,root_sec,root_in,prev,cur);

    if(res != 0)
      return res;
    else
     {
	  if(i < len)
	  {
		  if(rs->Attributes != ATTR_DIRECTORY)
			  return E_FS_NEXISTS;
	      sec.base = rs->FstClusLO +DATA_SEC - 2;
		  if(sec.base == 0)
			sec.base = ROOT_SEC;
		  sec.length = rs->FileSize/512;
	  }
     }
   }
   print_dir(rs);
   	printf("cur %d\n",*cur);
	printf("res %d\n",res);
   return 0;
 }

int do_open(int pid,char *fname,int mode)
 {
  int i,res,root_sec,root_in,cur;
  struct dir_entry rs;
  int prev[14];
  for(i=0;i<MAX_FILE;i++)
   if(fd[pid][i].avl == 1)
    break;
  if(i == MAX_FILE)
    return E_FS_BUSY;
  Strcpy(fd[pid][i].fname,fname);
  fd[pid][i].mode = mode;
  res = get_dir_entry(fname,&rs,&root_sec,&root_in,prev,&cur);
  if(res != 0)
  {
   fd[pid][i].avl = 1;
   return res;
  }
  fd[pid][i].sec_offset= (rs.FstClusHI << 16 | rs.FstClusLO);
  if(fd[pid][i].sec_offset == 0)
	fd[pid][i].sec_offset = EOF1;
  else
  {
	fd[pid][i].sec_offset= DATA_SEC -2 + fd[pid][i].sec_offset;
	if(read_sector(fd[pid][i].sec_offset,fd[pid][i].buffer) != 0)
	{
		fd[pid][i].avl = 1;
		return E_DISK;
	}
  }
  fd[pid][i].offset= 0;
  fd[pid][i].tot_offset=0;
  fd[pid][i].length = rs.FileSize;
  fd[pid][i].rs = rs;
  fd[pid][i].root_sec = root_sec;
  fd[pid][i].root_in  = root_in;
  fd[pid][i].avl = 0;
  cli();
  printf("root_sec %d root_in %d",fd[pid][i].root_sec,fd[pid][i].root_in);
//  delay(0xFFFF);
  sti();
  return i;
 }

int do_opendir(int pid,char *fname)
{
  int i,res,root_sec,root_in,cur;
  struct dir_entry rs;
  int prev[14];
  for(i=0;i<MAX_FILE;i++)
   if(dd[pid][i].avl == 1)
    break;
  if(i == MAX_DIR)
    return E_FS_BUSY;
  Strcpy(dd[pid][i].fname,fname);
  res = get_dir_entry(fname,&rs,&root_sec,&root_in,prev,&cur);
  if(res != 0 || ((rs.Attributes & ATTR_DIRECTORY) != ATTR_DIRECTORY))
  {
   if(Strcmp(fname,"\\") == 0)
   {
	   dd[pid][i].offset = 0;
	   dd[pid][i].sec_offset = ROOT_SEC;
	   dd[pid][i].avl = 0;
	   if(read_sector(dd[pid][i].sec_offset,dd[pid][i].buffer) != 0)
	   {
		dd[pid][i].avl = 1;
		return E_DISK;
	   }
	   return i;
   }
   else
   {
    dd[pid][i].avl = 1;
    return E_DISK;
   }
  }
  dd[pid][i].sec_offset= (rs.FstClusHI << 16) | rs.FstClusLO;
  printf("sec %d\n",dd[pid][i].sec_offset);

  if(dd[pid][i].sec_offset == 0)
	dd[pid][i].sec_offset = EOF1;
  else
  {
	dd[pid][i].sec_offset= DATA_SEC -2 + dd[pid][i].sec_offset;
	if(read_sector(dd[pid][i].sec_offset,dd[pid][i].buffer) != 0)
	{
		dd[pid][i].avl = 1;
		return E_DISK;
	}
  }
  dd[pid][i].offset= 0;
  dd[pid][i].avl = 0;
  return i;
}


int do_read(int pid,int fd_in,char *buf,int length)
 {
  long i,j,k,prev_sec;
  cli();
  printf("read : fd_in %d\n",fd_in);
//  delay(0xFFFF);
  sti();
  j=0;
  if(fd[pid][fd_in].avl == 1)
   return E_DISK;
  if(EOF(fd[pid][fd_in].sec_offset))
   return j;
  if(fd[pid][fd_in].offset == 512)
  {
     prev_sec = fd[pid][fd_in].sec_offset;
     if(fd[pid][fd_in].mode == O_RDWR)
       write_sector(prev_sec,fd[pid][fd_in].buffer);
     fd[pid][fd_in].sec_offset = get_next_sector(fd[pid][fd_in].sec_offset);
     printf("next: %d ",fd[pid][fd_in].sec_offset);
     if(fd[pid][fd_in].sec_offset >= EOF1 && fd[pid][fd_in].sec_offset <= EOF2)
       return j;
     read_sector(fd[pid][fd_in].sec_offset,fd[pid][fd_in].buffer);
     fd[pid][fd_in].offset = 0;
   }

  while(1)
  {
   for(k=fd[pid][fd_in].tot_offset,i=fd[pid][fd_in].offset;i<512 && j<length && k < fd[pid][fd_in].length;j++,i++,k++)
     buf[j] = fd[pid][fd_in].buffer[i];
   fd[pid][fd_in].offset = i;
   fd[pid][fd_in].tot_offset = k;
   if(k >= fd[pid][fd_in].length && j < length)
    {
     return j;
    }

   if(j < length )
   {
     prev_sec = fd[pid][fd_in].sec_offset;
     if(fd[pid][fd_in].mode == O_RDWR && !EOF(prev_sec))
       write_sector(prev_sec,fd[pid][fd_in].buffer);
     fd[pid][fd_in].sec_offset = get_next_sector(fd[pid][fd_in].sec_offset);
  //   printf("next: %d ",fd[fd_in].sec_offset);
     if(EOF(fd[pid][fd_in].sec_offset))
       return j;
     read_sector(fd[pid][fd_in].sec_offset,fd[pid][fd_in].buffer);
     fd[pid][fd_in].offset = 0;
   }
   else
    return j;
  }
 }
 
int do_readdir(int pid,int dd_in,DIRENT *dir)
 {
  long j,length;
  int cur,prev_sec[14],p,q;
  char name[15],long_name[255];
  struct dir_entry rs;
  j=0;
  length = 32;
  cur = 0;
  prev_sec[cur] = dd[pid][dd_in].sec_offset;
  cur++;

  if(dd[pid][dd_in].avl == 1)
    return E_DISK;
  if(EOF(dd[pid][dd_in].sec_offset))
    return E_DISK;
  if(dd[pid][dd_in].offset == 512)
  {
     dd[pid][dd_in].sec_offset = get_next_sector(dd[pid][dd_in].sec_offset);
     printf("next: %d ",dd[pid][dd_in].sec_offset);
     if(dd[pid][dd_in].sec_offset >= EOF1 && dd[pid][dd_in].sec_offset <= EOF2)
        return E_DISK;
     if(read_sector(dd[pid][dd_in].sec_offset,dd[pid][dd_in].buffer) != 0)
		 return E_DISK;
     dd[pid][dd_in].offset = 0;
  }
   while(1)
   {
	   j = dd[pid][dd_in].offset/32;

	   for(;j<16;j++)
	   {
		 printf("\n%d %d",(unsigned int)j,dd[pid][dd_in].buffer[j*32]);
		 if(dd[pid][dd_in].buffer[j*32] == 0xE5)
			continue;
		else if(dd[pid][dd_in].buffer[j*32] == 0)
		{
			dd[pid][dd_in].offset = j*32;
	        return E_DISK;
		}
		else
		{
		 if((dd[pid][dd_in].buffer[j*32+11] & ATTR_LONG_NAME) == 0) // && ((dd[pid][dd_in].buffer[j*32+11] & ATTR_VOLUME_ID) == 0))
			{
			 parse_dir(dd[pid][dd_in].buffer,j,&rs);
  		 	 Strcpy(name,rs.Filename);
			 for(p=0;p<8;p++)
			  if(name[p]== 32) // ' '
			   break;
             name[p++] = '.';
             for(q=0;q<3;q++,p++)
				{
				   if(rs.Extension[q] == ' ')
					   break;
				   name[p] = rs.Extension[q];
				}
               	if(q == 0)
					p--;
               name[p] = '\0';

			 Strcpy(dir->name,name);
			 dir->clu = (rs.FstClusHI << 16) + rs.FstClusLO;
			 dir->attribute = rs.Attributes;
			 get_date(rs.CrtDate,&dir->crt_dt);
			 get_time(rs.CrtTime,&dir->crt_time);
			 get_date(rs.LstAccDate,&dir->acc_dt);
			 get_date(rs.WrtDate,&dir->mod_dt);
			 get_time(rs.WrtTime,&dir->mod_time);
			 dir->size = rs.FileSize;

			if((dd[pid][dd_in].buffer[j*32+11] & ATTR_VOLUME_ID) == 0)
			{
 		     if(get_long_name(long_name,dd[pid][dd_in].buffer,j,prev_sec,cur-1) == 0)
			  {
				Strcpy(dir->name,long_name);
			  }
			}
			dd[pid][dd_in].offset = (j+1)*32;
			return 0;
	      }//if
		}//else
   }//for

	  dd[pid][dd_in].sec_offset =  get_next_sector(dd[pid][dd_in].sec_offset);
	  dd[pid][dd_in].offset = 0;
	  if(read_sector(dd[pid][dd_in].sec_offset,dd[pid][dd_in].buffer) != 0)
		  return E_DISK;
	  if(EOF(dd[pid][dd_in].sec_offset))
          break;
	  prev_sec[cur++] = dd[pid][dd_in].sec_offset;
   }
   dd[pid][dd_in].offset = (j+1)*32;
   return E_DISK;
 }

int do_write(int pid,int fd_in,char *buf,int length)
 {
  long i,j,k;
  int prev_sec;
  j=0;
  if(fd[pid][fd_in].avl == 1 || fd[pid][fd_in].mode == O_RDONLY)
   return E_DISK;
  prev_sec = fd[pid][fd_in].sec_offset;
  if(fd[pid][fd_in].offset == 512 && !EOF(fd[pid][fd_in].sec_offset))
  {
     fd[pid][fd_in].sec_offset = get_next_sector(fd[pid][fd_in].sec_offset);
     printf("next: %d %d\n",fd[pid][fd_in].sec_offset,prev_sec);
	 if(!EOF(prev_sec))
		write_sector(prev_sec,fd[pid][fd_in].buffer);
     if(!EOF(fd[pid][fd_in].sec_offset))
     {
       read_sector(fd[pid][fd_in].sec_offset,fd[pid][fd_in].buffer);
       fd[pid][fd_in].offset = 0;
     }
   }

  if(EOF(fd[pid][fd_in].sec_offset))
   {
    fd[pid][fd_in].sec_offset = get_free_sector(prev_sec);
    printf("next free: %d\n",fd[pid][fd_in].sec_offset);
    if(!EOF(fd[pid][fd_in].sec_offset))
       {
		 if(!EOF(prev_sec))
		 {
			write_fat(fd[pid][fd_in].sec_offset-DATA_SEC +2,prev_sec - DATA_SEC + 2,FAT1_SEC);
			write_fat(fd[pid][fd_in].sec_offset-DATA_SEC +2,prev_sec - DATA_SEC + 2,FAT2_SEC);
		 }
		 else
		 {
			 fd[pid][fd_in].rs.FstClusLO = (fd[pid][fd_in].sec_offset - DATA_SEC + 2) & 0xFFFF;
			 fd[pid][fd_in].rs.FstClusHI = (fd[pid][fd_in].sec_offset - DATA_SEC + 2) >> 16;
		 }
         write_fat(EOF1,fd[pid][fd_in].sec_offset-DATA_SEC +2,FAT1_SEC);
         write_fat(EOF1,fd[pid][fd_in].sec_offset-DATA_SEC +2,FAT2_SEC);
       }
     else
       {
		 if(!EOF(prev_sec) && prev_sec > DATA_SEC)
		 {
			write_fat(EOF1,prev_sec - DATA_SEC + 2,FAT1_SEC);
			write_fat(EOF1,prev_sec - DATA_SEC + 2,FAT2_SEC);
		 }
         return j;
       }
    fd[pid][fd_in].offset = 0;
   }
  while(1)
  {
   for(k=fd[pid][fd_in].tot_offset,i=fd[pid][fd_in].offset;i<512 && j<length;j++,i++,k++)
     fd[pid][fd_in].buffer[i] = buf[j];
   fd[pid][fd_in].offset = i;
   fd[pid][fd_in].tot_offset = k;
   if(fd[pid][fd_in].length < k)
    fd[pid][fd_in].length = k;

   if(j < length )
   {
     prev_sec = fd[pid][fd_in].sec_offset;
     fd[pid][fd_in].sec_offset = get_next_sector(fd[pid][fd_in].sec_offset);
     //printf("inside next: %d ",fd[fd_in].sec_offset);
	 if(!EOF(prev_sec))
		write_sector(prev_sec,fd[pid][fd_in].buffer);
     if(EOF(fd[pid][fd_in].sec_offset))
      {
            fd[pid][fd_in].sec_offset = get_free_sector(prev_sec);
       //     printf("inside next free: %d\n",fd[fd_in].sec_offset);
            if(!EOF(fd[pid][fd_in].sec_offset))
             {
				if(!EOF(prev_sec))
				{
					write_fat(fd[pid][fd_in].sec_offset-DATA_SEC +2,prev_sec - DATA_SEC + 2,FAT1_SEC);
					write_fat(fd[pid][fd_in].sec_offset-DATA_SEC +2,prev_sec - DATA_SEC + 2,FAT2_SEC);
				}
				else
				{
					fd[pid][fd_in].rs.FstClusLO = (fd[pid][fd_in].sec_offset - DATA_SEC + 2) && 0xFFFF;
					fd[pid][fd_in].rs.FstClusHI = (fd[pid][fd_in].sec_offset - DATA_SEC + 2) >> 16;
				}
               write_fat(EOF1,fd[pid][fd_in].sec_offset- DATA_SEC + 2,FAT1_SEC);
               write_fat(EOF1,fd[pid][fd_in].sec_offset- DATA_SEC + 2,FAT2_SEC);
             }
            else
             {
				 if(!EOF(prev_sec) && prev_sec > DATA_SEC)
				{
					write_fat(EOF1,prev_sec - DATA_SEC + 2,FAT1_SEC);
					write_fat(EOF1,prev_sec - DATA_SEC + 2,FAT2_SEC);
				}
               return j;
             }
      }
     else
       read_sector(fd[pid][fd_in].sec_offset,fd[pid][fd_in].buffer);
     fd[pid][fd_in].offset = 0;
   }
   else
    {
     return j;
    }
  }
 }

 int do_close(int pid,int fd_in)
 {
   int res;
   if(fd[pid][fd_in].avl == 1)
      return E_DISK;
     if(fd[pid][fd_in].mode == O_RDWR && !EOF(fd[pid][fd_in].sec_offset))
     {
	   if((res= write_sector(fd[pid][fd_in].sec_offset,fd[pid][fd_in].buffer)) != 0)
		   return E_DISK;
       printf("result of close : %d %d",fd[pid][fd_in].sec_offset,res);
       fd[pid][fd_in].rs.FileSize = fd[pid][fd_in].length;
	   fd[pid][fd_in].rs.WrtDate = form_date();
	   fd[pid][fd_in].rs.WrtTime = form_time();
     }
	 fd[pid][fd_in].rs.LstAccDate = form_date();
//   cli();
//   printf("fd_in %d root_sec %d root_in %d",fd_in,fd[pid][fd_in].root_sec,fd[pid][fd_in].root_in);
//   delay(0xFFFF);
//   sti();

	 write_dir(&fd[pid][fd_in].rs,fd[pid][fd_in].root_in,fd[pid][fd_in].root_sec);
     fd[pid][fd_in].avl = 1;
   return 0;
 }

int do_closedir(int pid,int dd_in)
 {
     dd[pid][dd_in].avl = 1;
     return 0;
 }

int do_create(int pid,char *name,int is_dir)
{
 int len,i,j,k,p,is_long,ord,lg,name_len,sh_len,pres,m,sh_in,temp_j,ext_len,dir_sec,dir_ent,no_dig,chksum,no_ent,no_sec,dir_sec_t[10],free_sec,res;
 char fname[255],sh_name[14],ext[4],sh_w[9],no[9],dirname[255],tmp_name[255];
 struct dir_entry rs,dot_ent,ddot_ent;
 struct long_dir_entry ld;
 int prev[14],cur;

 len = strlen(name);
 for(i=len-1;i>=0;i--)
	 if(name[i] == '\\')
		 break;
 for(j=0;j<i;j++)
	 dirname[j] = name[j];
 dirname[j] = '\0';
 if(strlen(dirname) == 0)
	 Strcpy(dirname,"\\");
 is_long = 0;
 for(j=i+1;j<len;j++)
 {
	fname[j-i-1] = name[j];
	if(name[j] == ' ')
		is_long =1;
 }
 fname[j-i-1] = '\0';

 if(strlen(fname) > 255)
	 return E_FS_FNAME;

 printf("%s %s %s %d\n",name,fname,dirname,is_long);
 if((p = do_open(pid,name,O_RDONLY)) >= 0)
 {
	 do_close(pid,p);
	 return E_FS_EXISTS;
 }

 if(Strcmp(dirname,"\\") == 0)
 {
	 dir_sec = ROOT_SEC;
 }
 else
 {
	 if((res= get_dir_entry(dirname,&rs,&dir_sec,&i,prev,&cur)) != 0)
		 return res;
	 if(rs.Attributes != ATTR_DIRECTORY)
		 return E_FS_NEXISTS;
	 dir_sec = DATA_SEC -2 + (rs.FstClusHI << 16 | rs.FstClusLO);
 }
 no_ent = strlen(fname);
 no_ent = no_ent%13 ? no_ent/13+1: no_ent/13;
 printf("no_ent : %d\n",no_ent);
 if( (dir_ent = get_free_entry(&dir_sec,no_ent+1)) == -1)
	 return E_FS_SPACE;
 printf("dir_ent : %d\n",dir_ent);
 for(k=0,j=0;k<8 && j<strlen(fname);j++)
 {
		 if(fname[j] == '.')
			 break;
		 if(fname[j] != ' ')
		 {
			 sh_name[k] = fname[j];
			 k++;
		 }
 }
 sh_in = k;
 sh_name[k] = '\0';
 for(j=strlen(fname);j>=0;j--)
 {
	 if(fname[j] == '.')
		 break;
 }
 if(j < 0)
	 j = strlen(fname) - 1;
 m=j;
 for(j++,k=0;k<3 && j<strlen(fname);j++)
 {
	 if(fname[j] != ' ')
	 {
		 ext[k] = fname[j];
		 k++;
	 }
 }
 for(;k<3;k++)
		 ext[k] = ' ';
 ext[k] ='\0';
 name_len = m;
 ext_len = strlen(fname) - m -1;
  if(name_len > 8)
	  is_long = 1;
  else
  {
	  if(ext_len > 4)
		  is_long =1;
  }
  printf("\n%d %s %s %s %d %d",is_long,sh_name,ext,fname,name_len,ext_len);
  if(is_long == 1)
  {
	  pres =1;
	  j=1;
	  while(pres)
	  {
		pres =1;
		if(j > 256)
			return E_FS_FNAME;
		temp_j = j;
		for(m=0;temp_j > 0;m++)
		{
			no[m] = '0' + temp_j%10;
			temp_j = temp_j/10;
		}
		no[m] = '\0';
		no_dig = strlen(no);
		sh_len= 8 - no_dig -1;
		for(k=0,m=0;k<sh_len && k < sh_in;k++)
			sh_w[k] = sh_name[k];
		sh_w[k++] = '~';
		for(m=strlen(no) - 1;m>=0;m--,k++)
			sh_w[k] = no[m];

		sh_w[k] = '\0';
		j++;
		Strcpy(tmp_name,"");
		strcat(tmp_name,dirname);
		strcat(tmp_name,sh_w);
		if(ext[0] != ' ')
		{
			strcat(tmp_name,".");
			strcat(tmp_name,ext);
		}
		printf("\ntmp = %s %s\n",tmp_name,sh_w);
		if((p = do_open(pid,tmp_name,O_RDONLY)) >= 0)
			do_close(pid,p);
		else
			pres = 0;
		printf("p = %d pres = %d\n",p,pres);

		printf("\nShort name : %s",sh_w);
	  }
  }
  else
  {
	  sh_name[sh_in] = '\0';
	  Strcpy(sh_w,sh_name);
  }
  rs.CrtDate	= form_date();
  rs.CrtTime	= form_time();
  rs.LstAccDate = form_date();
  rs.WrtDate	= rs.CrtDate;
  rs.WrtTime	= rs.CrtTime;
  rs.NTRes		= 0;
  rs.FileSize	= 0;
  rs.CrtTimeTenth = 0;

  if(is_dir)
  {
	    free_sec = get_free_sector(EOF1);
		if(clear_sec(free_sec) != 0)
			return E_DISK;
		if(EOF(free_sec))
			return E_FS_SPACE;
		if(write_fat(EOF2,free_sec - DATA_SEC + 2,FAT1_SEC) != 0)
			return E_DISK;
		if(write_fat(EOF2,free_sec - DATA_SEC + 2,FAT2_SEC) != 0)
			return E_DISK;

		rs.Attributes = ATTR_DIRECTORY;
		rs.FstClusLO = (free_sec - DATA_SEC + 2) & 0xFFFF;
		rs.FstClusHI = (free_sec - DATA_SEC + 2) >> 16;
		//Form entries for . and ..
		dot_ent = rs;
		dot_ent.Filename[0] = '.';
		for(i=1;i<8;i++)
			dot_ent.Filename[i] = ' ';
		for(i=0;i<3;i++)
			dot_ent.Extension[i] = ' ';
		if(write_dir(&dot_ent,0,free_sec) != 0)
			return E_DISK;

		ddot_ent = rs;
		if(dir_sec >= ROOT_SEC && dir_sec < DATA_SEC)
		{
			ddot_ent.FstClusHI = 0;
			ddot_ent.FstClusLO =0;
		}
		else
		{
			ddot_ent.FstClusLO = (dir_sec -DATA_SEC + 2) & 0xFFFF;
			ddot_ent.FstClusHI = (dir_sec -DATA_SEC +2) >> 16;

		}

		ddot_ent.Filename[0] = '.';
		ddot_ent.Filename[1] = '.';
		for(i=2;i<8;i++)
			ddot_ent.Filename[i] = ' ';
		for(i=0;i<3;i++)
			ddot_ent.Extension[i] = ' ';
		if(write_dir(&ddot_ent,1,free_sec) != 0)
			return E_DISK;

		print_dir(&dot_ent);
		print_dir(&ddot_ent);
  }
  else
  {
		rs.Attributes = ATTR_ARCHIVE;
		rs.FstClusHI	= 0;
		rs.FstClusLO	= 0;
  }

  for(i=0;i<strlen(sh_w);i++)
  {
	  if(sh_w[i] == ' ')
		break;
	  else
		  rs.Filename[i] = toupper(sh_w[i]);
  }
  for(;i<8;i++)
	  rs.Filename[i] = ' ';

  for(i=0;i<strlen(ext);i++)
  {
	  if(sh_w[i] == ' ')
		break;
	  else
		rs.Extension[i] = toupper(ext[i]);
  }
  for(;i<3;i++)
	  rs.Extension[i] = ' ';

  no_sec = dir_ent+no_ent;
  dir_sec_t[0] = dir_sec;
  i=0;
  while(no_sec >= 16)
  {
	 i++;

	 dir_sec_t[i] = get_next_sector(dir_sec_t[i-1]);
	 no_sec = no_sec -16;
  }
  print_dir(&rs);
  printf("sh_ent %d %d %d %d",(dir_ent+no_ent)%16,dir_ent,no_ent,dir_sec_t[i]);
  if(write_dir(&rs,(dir_ent+no_ent)%16,dir_sec_t[i]) != 0)
	  return E_DISK;
  j=0;
  for(k=0;k<8;k++)
	  sh_name[k] = rs.Filename[k];
  for(;k<12;k++)
	  sh_name[k] = rs.Extension[k-8];
  sh_name[k] = '\0';
  chksum = ChkSum(sh_name);

  ld.LDIR_Attr = ATTR_LONG_NAME;
  ld.LDIR_Chksum = chksum;
  ld.LDIR_FstClusLO = 0;
  ld.LDIR_Type = 0;
  ord = 1;
  lg =0;
  while(j < strlen(fname))
  {
	if(strlen(fname) - j <= 13)
		ord = ord | 0x40;

	ld.LDIR_Ord = ord;
	for(k=0;k<5 && j<strlen(fname);k++,j++)
	{
		ld.LDIR_Name1[k*2] = fname[j];
		ld.LDIR_Name1[k*2+1] = 0;
	}

	for(;k<5;k++)
	{
		if(lg == 1)
		{
			ld.LDIR_Name1[k*2] = 0xFF;
			ld.LDIR_Name1[k*2+1] = 0xFF;
		}
		else
		{
		lg = 1;
		ld.LDIR_Name1[k*2] = 0;
		ld.LDIR_Name1[k*2+1] = 0;
		}
	}
	for(k=0;k<6 && j<strlen(fname);k++,j++)
	{
		ld.LDIR_Name2[k*2] = fname[j];
		ld.LDIR_Name2[k*2+1] = 0;
	}

	for(;k<6;k++)
	{
		if(lg == 1)
		{
			ld.LDIR_Name2[k*2] = 0xFF;
			ld.LDIR_Name2[k*2+1] = 0xFF;
		}
		else
		{
			ld.LDIR_Name2[k*2] = 0;
			ld.LDIR_Name2[k*2+1] = 0;
			lg = 1;
		}
	}
	for(k=0;k<2 && j<strlen(fname);k++,j++)
	{
		ld.LDIR_Name3[k*2] = fname[j];
		ld.LDIR_Name3[k*2+1] = 0;
	}
	for(;k<2;k++)
	{

		if(lg == 1)
		{
			ld.LDIR_Name3[k*2] = 0xFF;
			ld.LDIR_Name3[k*2+1] = 0xFF;
		}
		else
		{
			ld.LDIR_Name3[k*2] = 0;
			ld.LDIR_Name3[k*2+1] = 0;
			lg = 1;
		}
	}
		ord++;
		dir_ent--;
		if(dir_ent+no_ent > 16)
			i--;
	if(write_long_dir(&ld,(dir_ent+no_ent)%16,dir_sec_t[i]) != 0)
		return E_DISK;
	printf("%d %d",dir_sec_t[i],dir_ent+no_ent);
	print_long_dir(&ld);
  }
  return 0;
}

int fs_create(int pid,char *name)
{
	return do_create(pid,name,0);
}

int do_createdir(int pid,char *name)
{
	return do_create(pid,name,1);
}

int do_remove(int pid,char *name)
{
        int sec,in,prev[14],cur,dir;
		long clu;
		struct dir_entry rs;
		DIRENT ds;
		unsigned char a[512],name1[255];


	    if(get_dir_entry(name,&rs,&sec,&in,prev,&cur) != 0)
                return E_DISK;

		if((rs.Attributes & ATTR_DIRECTORY) == ATTR_DIRECTORY)
		{
			dir = do_opendir(pid,name);
			printf("dir : %d",dir);
			if(dir < 0)
				return E_DISK;

			while(do_readdir(pid,dir,&ds) == 0)
			{
				if((Strcmp(ds.name,".") != 0) && (Strcmp(ds.name,"..") != 0))
				{
				 print_DIRENT(&ds);
				 Strcpy(name1,name);
				 strcat(name1,"\\");
				 strcat(name1,ds.name);
				 do_remove(pid,name1);
				}
			}
			do_closedir(pid,dir);
		}

		if(read_sector(sec,a) != 0)
			return E_DISK;

		printf("\nsector is %d",sec);
		printf("\ncur %d",cur);

		if(rem_long_name(a,in,prev,&cur) != 0)
			return E_DISK;

		//deleting short entry

        if(read_dir(&rs,in,sec) != 0)
			return E_DISK;

		clu = (rs.FstClusHI << 16) | rs.FstClusLO;

		rs.Filename[0] = 0xE5;
		if(write_dir(&rs,in,sec) != 0)
			return E_DISK;

		if(remove_chain(clu) != 0)
			return E_DISK;
    return 0;

}

int do_rename(int pid,char *name1,char *name)
{
	int prev[14],cur;
	int rsec,rin,rprev[14],rcur;
	unsigned char a[512];
	int len,i,j,k,p,is_long,ord,lg,name_len,sh_len,pres,m,sh_in,temp_j,ext_len,dir_sec,dir_ent,no_dig,chksum,no_ent,no_sec,dir_sec_t[10];
	char fname[255],sh_name[14],ext[4],sh_w[9],no[9],dirname[255],tmp_name[255];
	struct dir_entry rs;
	struct long_dir_entry ld;


	    if(get_dir_entry(name1,&rs,&rsec,&rin,rprev,&rcur) != 0)
                return E_FS_NEXISTS;

		len = strlen(name);
		for(i=len-1;i>=0;i--)
		 if(name[i] == '\\')
		  break;
		for(j=0;j<i;j++)
			dirname[j] = name[j];
		dirname[j] = '\0';
		if(strlen(dirname) == 0)
			Strcpy(dirname,"\\");

		is_long = 0;

		for(j=i+1;j<len;j++)
		{
		 fname[j-i-1] = name[j];
		 if(name[j] == ' ')
		  is_long =1;
		}
		fname[j-i-1] = '\0';

		if(strlen(fname) > 255)
			return E_FS_FNAME;

		printf("%s %s %s %d\n",name,fname,dirname,is_long);
		if((p = do_open(pid,name,O_RDONLY)) >= 0)
		 {
		  do_close(pid,p);
		  return E_FS_EXISTS;
		}

		if(Strcmp(dirname,"\\") == 0)
		{
		 dir_sec = ROOT_SEC;
		}
		else
		{
		if( get_dir_entry(dirname,&rs,&dir_sec,&i,prev,&cur) != 0)
		  return E_FS_NEXISTS;
		if(rs.Attributes != ATTR_DIRECTORY)
		  return E_FS_NEXISTS;
		dir_sec = DATA_SEC -2 + (rs.FstClusHI << 16 | rs.FstClusLO);
		}

		no_ent = strlen(fname);
		no_ent = no_ent%13 ? no_ent/13+1: no_ent/13;
		printf("no_ent : %d\n",no_ent);
		if((dir_ent = get_free_entry(&dir_sec,no_ent+1)) == -1)
			return E_FS_SPACE;
		printf("dir_ent : %d\n",dir_ent);
		for(k=0,j=0;k<8 && j<strlen(fname);j++)
		{
		 if(fname[j] == '.')
			 break;
		 if(fname[j] != ' ')
		 {
			 sh_name[k] = fname[j];
			 k++;
		 }
		}
		sh_in = k;
		sh_name[k] = '\0';
		for(j=strlen(fname);j>=0;j--)

		{
		if(fname[j] == '.')
		  break;
		}
		if(j < 0)
			j = strlen(fname) - 1;
		m=j;
		for(j++,k=0;k<3 && j<strlen(fname);j++)
		{
		if(fname[j] != ' ')
		{
		 ext[k] = fname[j];
		 k++;
		}
		}
		for(;k<3;k++)
		 ext[k] = ' ';
		ext[k] ='\0';
		name_len = m;
		ext_len = strlen(fname) - m -1;
		if(name_len > 8)
			is_long = 1;
		else
		{
		if(ext_len > 4)
		  is_long =1;
		}
		printf("\n%d %s %s %s %d %d",is_long,sh_name,ext,fname,name_len,ext_len);
		if(is_long == 1)
		{
			pres =1;
			j=1;
			while(pres)
			{
				pres =1;
				if(j > 256)
					return E_FS_EXISTS;
				temp_j = j;
				for(m=0;temp_j > 0;m++)
				{
					no[m] = '0' + temp_j%10;
					temp_j = temp_j/10;
				}
				no[m] = '\0';
				no_dig = strlen(no);
				sh_len= 8 - no_dig -1;
				for(k=0,m=0;k<sh_len && k < sh_in;k++)
					sh_w[k] = sh_name[k];
				sh_w[k++] = '~';
				for(m=strlen(no) - 1;m>=0;m--,k++)
					sh_w[k] = no[m];
				sh_w[k] = '\0';
				j++;
				Strcpy(tmp_name,"");

				strcat(tmp_name,dirname);
				strcat(tmp_name,sh_w);
				if(ext[0] != ' ')
				{
				strcat(tmp_name,".");
				strcat(tmp_name,ext);
				}
				printf("\ntmp = %s %s\n",tmp_name,sh_w);
				if((p = do_open(pid,tmp_name,O_RDONLY)) >= 0)
					do_close(pid,p);
				else
	  				pres = 0;
				printf("p = %d pres = %d\n",p,pres);
				printf("\nShort name : %s",sh_w);
			}
		}
		else
		{
		sh_name[sh_in] = '\0';
		Strcpy(sh_w,sh_name);
		}
  		if(read_sector(rsec,a) != 0)
			return E_DISK;

		printf("\nsector is %d",rsec);
		printf("\ncur %d",rcur);

		if(rem_long_name(a,rin,rprev,&rcur) != 0)
			return E_DISK;

		//deleting short entry

        if(read_dir(&rs,rin,rsec) != 0)
			return E_DISK;

		rs.Filename[0] = 0xE5;
		if(write_dir(&rs,rin,rsec) != 0)
			return E_DISK;
		rs.LstAccDate = form_date();

  for(i=0;i<strlen(sh_w);i++)
  {
	  if(sh_w[i] == ' ')
		break;
	  else
		  rs.Filename[i] = toupper(sh_w[i]);
  }
  for(;i<8;i++)
	  rs.Filename[i] = ' ';

  for(i=0;i<strlen(ext);i++)
  {
	  if(sh_w[i] == ' ')
		break;
	  else
		rs.Extension[i] = toupper(ext[i]);
  }
  for(;i<3;i++)
	  rs.Extension[i] = ' ';
  no_sec = dir_ent+no_ent;
  dir_sec_t[0] = dir_sec;
  i=0;
  while(no_sec >= 16)
  {
	 i++;
	 dir_sec_t[i] = get_next_sector(dir_sec_t[i-1]);
	 no_sec = no_sec -16;
  }
  print_dir(&rs);
  printf("sh_ent %d %d %d %d",(dir_ent+no_ent)%16,dir_ent,no_ent,dir_sec_t[i]);
  if(write_dir(&rs,(dir_ent+no_ent)%16,dir_sec_t[i]) != 0)
	  return E_DISK;
  j=0;
  for(k=0;k<8;k++)
	  sh_name[k] = rs.Filename[k];
  for(;k<12;k++)
	  sh_name[k] = rs.Extension[k-8];
  sh_name[k] = '\0';
  chksum = ChkSum(sh_name);

  ld.LDIR_Attr = ATTR_LONG_NAME;
  ld.LDIR_Chksum = chksum;
  ld.LDIR_FstClusLO = 0;
  ld.LDIR_Type = 0;
  ord = 1;
  lg =0;
  while(j < strlen(fname))
  {
	if(strlen(fname) - j <= 13)
		ord = ord | 0x40;

	ld.LDIR_Ord = ord;
	for(k=0;k<5 && j<strlen(fname);k++,j++)
	{
		ld.LDIR_Name1[k*2] = fname[j];
		ld.LDIR_Name1[k*2+1] = 0;
	}

	for(;k<5;k++)
	{
		if(lg == 1)
		{
			ld.LDIR_Name1[k*2] = 0xFF;
			ld.LDIR_Name1[k*2+1] = 0xFF;
		}
		else
		{
		lg = 1;
		ld.LDIR_Name1[k*2] = 0;
		ld.LDIR_Name1[k*2+1] = 0;
		}
	}
	for(k=0;k<6 && j<strlen(fname);k++,j++)
	{
		ld.LDIR_Name2[k*2] = fname[j];
		ld.LDIR_Name2[k*2+1] = 0;
	}

	for(;k<6;k++)
	{
		if(lg == 1)
		{
			ld.LDIR_Name2[k*2] = 0xFF;
			ld.LDIR_Name2[k*2+1] = 0xFF;
		}
		else
		{
			ld.LDIR_Name2[k*2] = 0;
			ld.LDIR_Name2[k*2+1] = 0;
			lg = 1;
		}
	}
	for(k=0;k<2 && j<strlen(fname);k++,j++)
	{
		ld.LDIR_Name3[k*2] = fname[j];
		ld.LDIR_Name3[k*2+1] = 0;
	}
	for(;k<2;k++)
	{
		if(lg == 1)
		{
			ld.LDIR_Name3[k*2] = 0xFF;
			ld.LDIR_Name3[k*2+1] = 0xFF;
		}
		else
		{
			ld.LDIR_Name3[k*2] = 0;
			ld.LDIR_Name3[k*2+1] = 0;
			lg = 1;
		}
	}
		ord++;
		dir_ent--;
		if(dir_ent+no_ent > 16)
			i--;
	if(write_long_dir(&ld,(dir_ent+no_ent)%16,dir_sec_t[i]) != 0)
		return E_DISK;
	printf("%d %d",dir_sec_t[i],dir_ent+no_ent);
	print_long_dir(&ld);
  }

  return 0;
}


void fs_init()
 {
  int i,j;
  for(j=0;j<MAX_PROC;j++)
    for(i=0;i<MAX_FILE;i++)
     fd[j][i].avl = 1;
  for(j=0;j<MAX_PROC;j++)
  for(i=0;i<MAX_DIR;i++)
	  dd[j][i].avl = 1;
 }

int do_ls(int pid,char *dirname)
 {
  int p;
  DIRENT ds;
  p = do_opendir(pid,dirname);
  if(p != -1)
  {
   while(do_readdir(pid,p,&ds) == 0)
	  print_DIRENT(&ds);
   return 0;
  }
  return -1;
 }

int do_copy(int pid,char *src,char *dest)
 {
  int p,q,res;
  char buf[1024];
  p = do_open(pid,src,O_RDONLY);
  q = do_open(pid,dest,O_RDWR);
  if(p < 0 || q < 0)
   return -1;
  else
   {
    printf("p = %d  q=%d\n",p,q);
    while((res = do_read(pid,p,buf,1024)) > 0)
     {
//      putchar(buf);
//      printf("res %d\n",res);
      if(do_write(pid,q,buf,res) < 0)
       return -1;
     }
     printf("res %d",res);
     do_close(pid,p);
     do_close(pid,q);
     return 0;
   }
 }

void runFS()
{
  int ret_val;
  while(1)
  {
  // sti();
//    printf("FS running %d",no_fs_req);

    if(no_fs_req != 0)
    {
     printf("\nFS running %d",no_fs_req);
     printf("type %d fname %s\n",fsreqs[fs_fr_req].type,fsreqs[fs_fr_req].op.fname);
     
//     rect(390,0,50,50);
//     ginfo(400,10,itoa(no_fs_req,10));
     
  	 switch(fsreqs[fs_fr_req].type)
	   {
		 case OPEN :
				ret_val = do_open(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].op.fname,fsreqs[fs_fr_req].op.mode);
				break;
		 case CLOSE :
				ret_val = do_close(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].cl.fd_in);
				break;
		 case READ :
				ret_val = do_read(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].re.fd_in,fsreqs[fs_fr_req].re.buf,fsreqs[fs_fr_req].re.len);
				break;
		 case WRITE :
				ret_val = do_write(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].wr.fd_in,fsreqs[fs_fr_req].wr.buf,fsreqs[fs_fr_req].wr.len);
				break;
		 case CREATEFIL:
				ret_val = fs_create(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].cr.name);
				break;
		 case OPENDIR:
				ret_val = do_opendir(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].opd.dir_name);
				break;
		 case CREATEDIR:
				ret_val = do_createdir(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].crd.name);
				break;
		 case CLOSEDIR:
				ret_val = do_closedir(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].cld.dd_in);
				break;
		 case READDIR :
				ret_val = do_readdir(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].red.dd_in,fsreqs[fs_fr_req].red.dir);
				break;
		 case REMOVE :
				ret_val = do_remove(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].rem.name);
				break;
		 case RENAME :
				ret_val = do_rename(fsreqs[fs_fr_req].from_pid,fsreqs[fs_fr_req].ren.old_name,fsreqs[fs_fr_req].ren.new_name);
				break;
     case LOAD :
        ret_val = do_load(fsreqs[fs_fr_req].load.fname,fsreqs[fs_fr_req].from_pid);
        break;
		 default:
				ret_val = E_FS_BADREQ;

	 }
   cli();
    printf("ret %d name %s\n",ret_val,fsreqs[fs_fr_req].load.fname);
//    delay(0xFFFF);
    sti();
  	 asm("pushl %%eax"::"a"(fsreqs[fs_fr_req].type));
  	 asm("pushl %%eax"::"a"(ret_val));
     asm("pushl %%eax"::"a"(fsreqs[fs_fr_req].from_pid));
     asm("int $0x30"::"a"(12));

     fs_fr_req = (fs_fr_req +1) %MAX_FS_REQ;
     no_fs_req--;
    }
  }
}


int addfsreq(fs_request *r)

{
  if(no_fs_req == MAX_FS_REQ)
    return -1;
  memcpy((void *)&fsreqs[fs_en_req],(const void *)r,sizeof(fsreqs[fs_en_req]));
  printf("\n no_req %d id %d",no_fs_req,fsreqs[fs_en_req].cl.fd_in);
  fs_en_req = (fs_en_req +1) %MAX_FS_REQ;
  no_fs_req++;
  return 0;
}

void fscloseAll(dword pid)
 {
   dword i;
   for(i = 0;i<MAX_FILE;i++)
    do_close(pid,i);

   for(i = 0;i<MAX_DIR;i++)
    do_closedir(pid,i);
 }

/*main()
 {
  int i,j,res,len;
  int dir_ent,dir_sec;
  struct boot_sector b;
  struct dir_entry rs;
  struct long_dir_entry ld;
  DIRENT ds;
  char name[255];
  int p;
  char buf[1024];
  DATE dt;
  TIME ti;

  fs_init();
  p = do_opendir("\\");
  printf("p = %d",p);
  if(p != -1)
  {
  while(do_readdir(p,&ds) == 0)
	  print_DIRENT(&ds);
  }

  exit(0);
  read_sector(FAT1_SEC,buf);
  exit(0);
  printf("rename :%d",do_rename("\\test\\text1.txt","\\test\\text3.txt"));
  exit(0);
  printf("rename1 :%d",do_rename("\\text1.txt","\\text2.txt"));
  exit(0);
 do_createdir("\\source1");
 do_createdir("\\source1\\s1");
 do_createdir("\\source1\\s1\\s3");
 do_createdir("\\source1\\s1\\s4");
 create("\\source1\\s5");



   do_remove("\\text3.txt");
   exit(0);
   do_remove("\\CallByRef1");
  do_remove("\\Bochs Dynacube1");
  do_remove("\\os material1");
  do_remove("\\source1");
  exit(0);





  do_createdir("\\os material1");
  do_createdir("\\Bochs Dynacube1");
//  do_createdir("\\source1");


  do_createdir("\\CallByRef1");
  create("\\source.txt");

  buf[0] = 'b';
  buf[1] = 'y';
  buf[2] = 'e';
  buf[3] = ' ';

  p = do_open("\\source.txt",O_RDWR);
  printf("p = %d",p);
  if(p >= 0)
        do_write(p,buf,4);
  do_close(p);


  exit(0);

  for(i=ROOT_SEC;i<ROOT_SEC + 2;i++)
	read_sector(i,buf);

 for(i=FAT1_SEC;i<FAT1_SEC + 2;i++)
	read_sector(i,buf);

  exit(0);


  Strcpy(name,"\\creating a long directory2");
  len = strlen(name);
  for(i= 1;i<2;i++)
  {
          name[len] = i + '0';
		  name[len + 1] = '\0';


          printf(" create %d\n",do_create(name,0));
  }

  exit(0);
  buf[0] = ' ';
  buf[1] = ' ';
  buf[2] = ' ';
  buf[3] = ' ';

  p = do_open("\\creating a long directory21\\newfile123.txt",O_RDWR);
  printf("p = %d",p);
  if(p != -1)
        do_write(p,buf,4);

  do_close(p);

  exit(0);

  for(i=ROOT_SEC;i<ROOT_SEC + 8;i++)
	read_sector(i,buf);
  exit(0);


  read_long_dir(&ld,5,ROOT_SEC);
  print_long_dir(&ld);
  read_long_dir(&ld,6,ROOT_SEC);
  print_long_dir(&ld);
  read_long_dir(&ld,7,ROOT_SEC);
  print_long_dir(&ld);
  exit(0);
  read_fat(&i,0,FAT2_SEC);
  read_fat(&i,1,FAT2_SEC);
  read_fat(&i,2,FAT2_SEC);
  read_fat(&i,3,FAT2_SEC);

  get_dir_entry("\\hi",&rs,&j,&res);
  get_dir_entry("\\hi\\1",&rs,&j,&res);
  return -1;
//  printf("copy %d",do_copy("\\books1.txt","\\books.txt"));
 //p = do_open("\\a long directory - a trial must be long\\a long file what to do.txt",O_RDONLY);
 p = do_open("\\floppy.h.txt",O_RDONLY);




  printf("p = %d",p);
  if(p >= 0)
  {
  while((res = do_read(p,buf,512)) > 0)
   {
	 for(i=0;i<res;i++)
     putchar(buf[i]);
   }
  printf("res : %d",res);
  }
  do_close(p);
  get_dir_entry("\\",&rs,&i,&j);
  printf("%d %d\n",i,j);
  print_dir(&rs);

//  printf(" create %d\n",do_create("\\creating a very long directory",1));
//  printf(" create %d\n",do_create("\\creating a very long directory\\hello",0));
//  printf(" create %d\n",do_create("\\creating a very long directory\\trial",0));
//  printf(" create %d\n",do_create("\\creating a very long directory\\hi",1));
  //printf(" create %d\n",do_create("\\creating a long directory\\hi\\newfile12.tob",0));

//  p = do_open("\\creating a long directory\\hi\\newfile12.tob",O_RDWR);


//  printf("%d\n",do_open("\\no",O_RDONLY));
Strcpy(name,"\\my_directoru\\hi.bat");
  i = strlen(name);
  j = j%13?j/13:j/13+1;
  j++;
  //dir_sec = DATA_SEC + 2 - 2;
  dir_sec = ROOT_SEC;
  dir_ent = get_free_entry(&dir_sec,2);
  printf("dir_ent : %d dir_sec %d\n",dir_ent,dir_sec);
 } */



































int do_load(char *fname,int pid)
{

return 1;
}

