#ifndef __CONSOLE_H
#define	__CONSOLE_H

#ifdef __cplusplus
extern "C"
{
#endif
	
	#define COMMAND_BUFFER 100
	#define MAX_DIR_LIST   200
	void Console();
	short GetParameter(char *Str,int Start ,char *Buffer);
	void DoFormat(char *Command ,char *Paramter1,int Start);
	void DoDelete(char *Command ,char *Paramter1,char *Paramter2 ,char *CurrentDir ,int Start);
	void DoRD(char *Command ,char *Paramter1 ,char *Paramter2 ,char *CurrentDir ,int Start);
	void DoCD(char *Command ,char *Paramter1,char *Paramter2,char *CurrentDir,int Start);
	void DoDIR(char *Command ,char *Paramter1,char *Paramter2,char *CurrentDir,struct Frame *CurDIR,short *TotalFrame,int Start);
	void DoTeastRead();
	void DoTeastWrite();
	void DoTeastBasicCreation();
	void DoHelp();
	

#ifdef __cplusplus
}
#endif

#endif

