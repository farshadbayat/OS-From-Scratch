#include <component.h>
#include <graphics.h>
#include <draw2d.h>
#include <draw3d.h>
#include <stdio.h>
#include <mmu.h>
#include <string.h>
#include <system.h>
#include <image.h>
#include "GraphicTheme.C"
//#include <mmu.h>
//#include <stdio.h>

#define TOP_BORDER_HIGHT	50
#define BORDER_SIZE			20

struct WINDLIST **wm_handles=NULL;//for direct access By Handler
static int wm_num_windows;
image *IMGFormTopLeft=NULL,*IMGFormTopRight=NULL,*IMGFormCloseNormal=NULL,*IMGFormClosePush=NULL;
image *IMGFormTopMidBar=NULL/*,*IMGFormLeftLine=NULL,*IMGFormRightLine=NULL*/,*IMGFormRightCoordinate=NULL,*IMGFormLeftCoordinate=NULL;

image *IMGFormMaximizeNormal=NULL,*IMGFormMaximizePush=NULL,*IMGFormMinimizeNormal=NULL,*IMGFormMinimizePush=NULL;


static Control* InitControl(Point CPoint ,Size CSize ,enum ControlType CType )
{
	Control* NewControl=(Control *)kmalloc(sizeof(Control),"Control Allocation");
	if(NewControl==NULL)
	{
		kprintf("ControlInit :MMU can not allocate memory.");
		return NULL;
	}
	NewControl->CGraphicArea.BitsPerPixel=MainGraphicArea.BitsPerPixel; 
	NewControl->CGraphicArea.BytePerPixel=MainGraphicArea.BytePerPixel;
	NewControl->CPoint=CPoint;
	NewControl->CGraphicArea.width =CSize.Width+(CType==ControlTypeForm ? 2*BORDER_LINE_SIZE : 0);
	NewControl->CGraphicArea.hight =CSize.Hight+(CType==ControlTypeForm ? BORDER_LINE_SIZE+BORDER_BAR_SIZE : 0);
	NewControl->CType=CType;
	
	NewControl->CGraphicArea.Buffer=(unsigned short*)kmalloc(NewControl->CGraphicArea.hight*NewControl->CGraphicArea.width*MainGraphicArea.BytePerPixel,"VideoBuffer Allocation");	
	//printf("InitControl:NewControl->CGraphicArea.Buffer=%p Size=%d (%d,%d)",NewControl->CGraphicArea.Buffer,CSize.Hight*CSize.Width *MainGraphicArea.BytePerPixel,CSize.Width ,CSize.Hight);getch();
	if(NewControl->CGraphicArea.Buffer==NULL)
	{
		kprintf("ControlInit :MMU can not allocate memory.");
		return NULL;
	}
	return NewControl;
}
FormControl* InitForm(FormControl *NewForm ,Point CPoint ,Size CSize)
{	
	NewForm->CPoint =CPoint;
	NewForm->CSize =CSize;
	NewForm->FControl=InitControl(CPoint ,SIZE(CSize.Width ,CSize.Hight) ,ControlTypeForm );
	//printf("InitForm:CGraphicArea.Buffer=%p",NewForm->FControl->CGraphicArea.Buffer);getch();
	return NewForm;
}
static bool InitFormImage()
{
	IMGFormTopLeft=(image *) kmalloc(sizeof(FormLeftTopBMP) ,"Left Bar Theme");
	//printf("ptr=%p size=%d",IMGFormTopLeft,sizeof(FormLeftTopBMP));getch();
	IMGFormTopRight=(image *) kmalloc(sizeof(FormRightTopBMP) ,"Right Bar Theme");	
	IMGFormCloseNormal=(image *) kmalloc(sizeof(CloseNormalBMP) ,"Close Normal Theme");	
	IMGFormClosePush=(image *) kmalloc(sizeof(ClosePushBMP) ,"Close Push Theme");	
	IMGFormTopMidBar=(image *) kmalloc(sizeof(FormTopMidBarBMP) ,"MidBar Theme");	
	/*IMGFormLeftLine=(image *) kmalloc(sizeof(FormLeftLineBMP) ,"Left Line Theme");	
	IMGFormRightLine=(image *) kmalloc(sizeof(FormRightLineBMP) ,"Right Line Theme");*/		
	IMGFormLeftCoordinate=(image *) kmalloc(sizeof(FormLeftCoordinateBMP) ,"Left Coordinate Theme");	
	IMGFormRightCoordinate=(image *) kmalloc(sizeof(FormRightCoordinateBMP) ,"Right Coordinate Theme");	

	IMGFormMaximizeNormal=(image *) kmalloc(sizeof(MaximizeNormalBMP) ,"Maximize Normal Theme");	
	IMGFormMaximizePush=(image *) kmalloc(sizeof(MaximizePushBMP) ,"Maximize Push Theme");	

	IMGFormMinimizeNormal=(image *) kmalloc(sizeof(MinimizeNormalBMP) ,"Minimize Normal Theme");	
	IMGFormMinimizePush=(image *) kmalloc(sizeof(MinimizePushBMP) ,"Minimize Push Theme");	

	if( IMGFormMaximizeNormal==NULL || IMGFormMaximizePush==NULL || IMGFormMinimizeNormal==NULL || IMGFormMinimizePush==NULL || IMGFormTopMidBar==NULL || /*IMGFormLeftLine==NULL || IMGFormRightLine==NULL  ||*/ IMGFormRightCoordinate==NULL || IMGFormLeftCoordinate==NULL || IMGFormTopLeft==NULL || IMGFormTopRight==NULL || IMGFormCloseNormal==NULL || IMGFormClosePush==NULL)
	{
		kprintf("InitFormImage :Can Not Allocate Memory");
		free(IMGFormTopLeft);
		free(IMGFormTopRight);
		free(IMGFormCloseNormal);
		free(IMGFormClosePush);
		free(IMGFormTopMidBar);
		/*free(IMGFormLeftLine);
		free(IMGFormRightLine);	*/	
		free(IMGFormLeftCoordinate);
		free(IMGFormRightCoordinate);
		free(IMGFormMaximizeNormal);
		free(IMGFormMaximizePush);		
		free(IMGFormMinimizeNormal);
		free(IMGFormMinimizePush);
		return false;
	}
	if(BMPLoad(FormLeftTopBMP ,sizeof(FormLeftTopBMP) ,IMGFormTopLeft)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}
	if(BMPLoad(FormRightTopBMP ,sizeof(FormRightTopBMP) ,IMGFormTopRight)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}
	if(BMPLoad(CloseNormalBMP ,sizeof(CloseNormalBMP) ,IMGFormCloseNormal)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}
	if(BMPLoad(ClosePushBMP ,sizeof(ClosePushBMP) ,IMGFormClosePush)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}

	if(BMPLoad(FormTopMidBarBMP ,sizeof(FormTopMidBarBMP) ,IMGFormTopMidBar)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}
	/*if(BMPLoad(FormLeftLineBMP ,sizeof(FormLeftLineBMP) ,IMGFormLeftLine)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}
	if(BMPLoad(FormRightLineBMP ,sizeof(FormRightLineBMP) ,IMGFormRightLine)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}*/
	if(BMPLoad(FormRightCoordinateBMP ,sizeof(FormRightCoordinateBMP) ,IMGFormRightCoordinate)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}
	if(BMPLoad(FormLeftCoordinateBMP ,sizeof(FormLeftCoordinateBMP) ,IMGFormLeftCoordinate)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}

	if(BMPLoad(MaximizeNormalBMP ,sizeof(MaximizeNormalBMP) ,IMGFormMaximizeNormal)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}

	if(BMPLoad(MaximizePushBMP ,sizeof(MaximizePushBMP) ,IMGFormMaximizePush)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}

	if(BMPLoad(MinimizeNormalBMP ,sizeof(MinimizeNormalBMP) ,IMGFormMinimizeNormal)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}

	if(BMPLoad(MinimizePushBMP ,sizeof(MinimizePushBMP) ,IMGFormMinimizePush)!=0){
		kprintf("InitFormImage :Can Not Load Image Theme!");
		return false;
	}
	return true;
}
 
static void InitWindowManagment()
{
	Point CPoint={0,0};
	Size CSize={MainGraphicArea.width ,MainGraphicArea.hight };

	wm_handles = (struct WINDLIST **)kmalloc(sizeof(struct WINDLIST *)*MAX_WINDOW ,"Graphic Window Handler" );	
	if(wm_handles==NULL)
	{
		kprintf("InitWINList :MMU can not allocate memory.");
		return;
	}
	struct WINDLIST *wm_system_parent_window=(struct WINDLIST *)kmalloc(sizeof(struct WINDLIST) ,"Desktop Window" );		
	if(wm_system_parent_window==NULL)
	{
		kprintf("InitWINList :MMU can not allocate memory.");
		free(wm_handles);
		return;
	}
	//Creat Desktop Form
	wm_system_parent_window->Form=(FormControl *)kmalloc(sizeof(FormControl) ,"Desktop Form" );
	if(wm_system_parent_window->Form==NULL)
	{
		kprintf("InitWINList :MMU can not allocate memory.");
		free(wm_system_parent_window);
		free(wm_handles);
		return;
	}
	InitForm(wm_system_parent_window->Form ,CPoint ,CSize );
	//printf("InitW:CGraphicArea.Buffer=%p",wm_system_parent_window->Form->FControl->CGraphicArea.Buffer);getch();
	//printf("(%d,%d)(%d,%d)",wm_system_parent_window->Form->CPoint.X,wm_system_parent_window->Form->CPoint.Y,wm_system_parent_window->Form->CSize.Width,wm_system_parent_window->Form->CSize.Hight);getch();
	wm_system_parent_window->NeedsRepaint=true; 
	wm_system_parent_window->Handle=0;//Window 0
	wm_system_parent_window->Next=NULL;
	wm_system_parent_window->Prev=NULL;
	wm_system_parent_window->Parent=NULL;
	//Form Atribute
	wm_system_parent_window->Form->BackColor=DESKTOP_BACKGROUND;
	GraphicClearClip(wm_system_parent_window->Form->FControl->CGraphicArea ,0 ,0 ,wm_system_parent_window->Form->FControl->CGraphicArea.width ,wm_system_parent_window->Form->FControl->CGraphicArea.hight ,DESKTOP_BACKGROUND );
	wm_system_parent_window->Form->TopMost=false;
	Strcpy(wm_system_parent_window->Form->CName ,"MainDesktop" );

    wm_handles[0] = wm_system_parent_window;
    wm_num_windows = 1;
	
	UpdateWNDS(wm_system_parent_window);
}
void DumpWNDS()
{
	//gotoxy(1,1);	
	short c=0;
	while(true)
	{
		printf("Repaint=%d FormName=%s Point=(%d,%d) Size(%d,%d)\n",wm_handles[c]->NeedsRepaint ,wm_handles[c]->Form->CName ,wm_handles[c]->Form->CPoint.X,wm_handles[c]->Form->CPoint.Y ,wm_handles[c]->Form->CSize.Width,wm_handles[c]->Form->CSize.Hight );
		c++;
		if(wm_handles[c]==NULL)
			break;
	}
}
void UpdateWNDS(struct WINDLIST *WND)
{	
	//printf("In UpdateWNDS %s",WND->Form->CName);getch();
	//printf("UpdateWNDS:Buffer=%p Name=%s (%d,%d)(%d,%d)",WND->Form->FControl->CGraphicArea.Buffer,WND->Form->CName ,WND->Form->CPoint.X,WND->Form->CPoint.Y,WND->Form->CSize.Width,WND->Form->CSize.Hight );getch();
	struct WINDLIST *WNDLink=NULL,*WNDChild=NULL;
	for(WNDLink=WND ;WNDLink!= NULL ;WNDLink=WNDLink->Next )
	{		
		//printf("Buf=%p(%d,%d)(%d,%d)%s%pFC=%pFC=%s",WNDLink->Form->FControl->CGraphicArea.Buffer,WNDLink->Form->CPoint.X,WNDLink->Form->CPoint.Y,WNDLink->Form->CSize.Width ,WNDLink->Form->CSize.Hight ,WNDLink->Form->CName ,WNDLink->Next,WNDLink->First_Child ,WNDLink->Next,WNDLink->First_Child->Form->CName);getch();
		WindowGraphicMemoryCopy(MainGraphicArea ,WNDLink->Form->CPoint ,WNDLink->Form->FControl->CGraphicArea ,POINT(0,0) ,SIZE(WNDLink->Form->FControl->CGraphicArea.width,WNDLink->Form->FControl->CGraphicArea.hight));
		if(WNDLink->First_Child!=NULL)
			UpdateWNDS(WNDLink->First_Child);
		/*for(WNDChild=WND->First_Child ;WNDChild!= NULL ;WNDChild=WNDChild->Next )
		{
			WindowGraphicMemoryCopy(MainGraphicArea ,WNDChild->Form->CPoint ,WNDChild->Form->FControl->CGraphicArea ,POINT(0,0) ,SIZE(WNDChild->Form->FControl->CGraphicArea.width,WNDChild->Form->FControl->CGraphicArea.hight));
		}*/		
	}
}

static bool KernelCreatWindow(struct WINDLIST *parent ,FormControl *NewForm )
{	
	//printf("\nIn KernelCreatWindow");getch();
	struct WINDLIST *NewWND=NULL;
	//struct WINDLIST *LastWIND=NULL;
	unsigned short i;

	if(NewForm==NULL)
	{
		kprintf("InitWINList :Input Argumant Is NULL.");		
		return false;
	}	
	NewWND=(struct WINDLIST *)kmalloc(sizeof(struct WINDLIST) ,"Form Memory" );    
	
	//DumpMAM(RootMAM);getch();
	if(wm_handles==NULL)
	{
		kprintf("CreatWindow :MMU can not allocate memory.");
		return false;
	}	
	memset(NewWND, 0, sizeof(struct WINDLIST));	
	NewWND->Form = NewForm;
	wm_handles[wm_num_windows] = NewWND;
	NewWND->Handle = wm_num_windows++;
	char *FormName=(char *)kmalloc(MAX_FORM_NAME ,"Form Name");
	MASSERT1(FormName ,"CreatWindow :MMU can not allocate memory." ,false);	
	sprintf((unsigned char *)FormName ,"Form%d",NewWND->Handle );
	NewWND->Form->CName =FormName;
	NewWND->NeedsRepaint=true;
	NewWND->Parent=parent; 
	NewWND->First_Child=NewWND->Last_Child=NULL;
	
	
	//printf("NewForm->FControl->CGraphicArea.Buffer=%p",NewForm->FControl->CGraphicArea.Buffer);getch();
	//printf("KernelCreatWindow:CGraphicArea.Buffer=%p",NewWND->Form->FControl->CGraphicArea.Buffer);getch();     
	
	 //printf("parent=%p parentName=%s New Form=%s",parent,parent->Form->CName,NewWND->Form->CName);getch();	 
	 //for(LastWIND=parent->First_Child ;LastWIND->Next!=NULL && LastWIND !=NULL ;LastWIND=LastWIND->Next );	 
	 if(parent->Last_Child==NULL){//Parent Not Have Child
		parent->First_Child=parent->Last_Child=NewWND;
		NewWND->Prev =NewWND->Next=NULL;
		//printf("First Child Creat");getch();
	 }
	 else{//Parent Have  at Last a Child
		parent->Last_Child->Next =NewWND;//Last Item Link To The New Item
		NewWND->Prev =parent->Last_Child;//New Item Set Prev Link To The Old Last Item
		parent->Last_Child=NewWND;//Set New Last Item
		NewWND->Next=NULL;
		//printf("Next Child Creat");getch();
	 }

	 //printf("FirstChild=%p LastChild=%p",NewWND->First_Child->Form  ,NewWND->Last_Child->Form );getch();
	 //printf("LastWIND=%s FirstWIND=%s",parent->Last_Child->Form->CName ,parent->First_Child->Form->CName );getch();

	/* set window variables here... Fill them ALL IN */
	NewWND->Form->BackColor=FormDefultBackColor;
	//printf("bCreatWindow:Buffer=%p Name=%s (%d,%d)(%d,%d)",NewWND->Form->FControl->CGraphicArea.Buffer,NewWND->Form->CName ,NewWND->Form->CPoint.X,NewWND->Form->CPoint.Y,NewWND->Form->CSize.Width,NewWND->Form->CSize.Hight );getch();
	
	//DrawImage(MainGraphicArea/*NewWND->Form->FControl->CGraphicArea*/ ,IMGFormLeftLine ,Draw_Normal ,0 ,IMGFormTopLeft->height ,0 ,0 ,IMGFormLeftLine->width ,NewWND->Form->FControl->CGraphicArea.hight ,ImageMode_RepeatImage );	  		
	for(i=0;i<4;i++)
		DrawLine(NewWND->Form->FControl->CGraphicArea ,&FormBorderColor[i] ,Draw_Normal ,i ,IMGFormTopLeft->height ,i ,NewWND->Form->FControl->CGraphicArea.hight-1 );
	for(i=0;i<4;i++)
		DrawLine(NewWND->Form->FControl->CGraphicArea ,&FormBorderColor[4-i] ,Draw_Normal ,i+NewWND->Form->FControl->CGraphicArea.width-4 ,IMGFormTopLeft->height ,i+NewWND->Form->FControl->CGraphicArea.width-4 ,NewWND->Form->FControl->CGraphicArea.hight-1 );
	for(i=0;i<4;i++)
		DrawLine(NewWND->Form->FControl->CGraphicArea ,&FormBorderColor[4-i] ,Draw_Normal ,0 ,NewWND->Form->FControl->CGraphicArea.hight+i-4  ,NewWND->Form->FControl->CGraphicArea.width-1 ,NewWND->Form->FControl->CGraphicArea.hight+i-4 );
	DrawImage(NewWND->Form->FControl->CGraphicArea ,IMGFormTopLeft ,Draw_Normal ,0 ,0 ,0 ,0 ,IMGFormTopLeft->width ,IMGFormTopLeft->height ,ImageMode_Normal );  	
	DrawImage(NewWND->Form->FControl->CGraphicArea ,IMGFormTopMidBar ,Draw_Normal ,IMGFormTopLeft->width ,0 ,0 ,0 ,NewWND->Form->FControl->CGraphicArea.width-IMGFormTopRight->width ,IMGFormTopMidBar->height ,ImageMode_RepeatImage );  	
	DrawImage(NewWND->Form->FControl->CGraphicArea ,IMGFormTopRight ,Draw_Normal ,NewWND->Form->FControl->CGraphicArea.width-IMGFormTopRight->width ,0 ,0 ,0 ,NewWND->Form->FControl->CGraphicArea.width-IMGFormTopRight->width ,IMGFormTopRight->height ,ImageMode_Normal );	
	DrawImage(NewWND->Form->FControl->CGraphicArea ,IMGFormLeftCoordinate ,Draw_Normal ,0 ,NewWND->Form->FControl->CGraphicArea.hight-IMGFormLeftCoordinate->height ,0 ,0 ,IMGFormLeftCoordinate->width ,IMGFormLeftCoordinate->height ,ImageMode_Normal );	
	DrawImage(NewWND->Form->FControl->CGraphicArea ,IMGFormRightCoordinate ,Draw_Normal ,NewWND->Form->FControl->CGraphicArea.width-IMGFormRightCoordinate->width ,NewWND->Form->FControl->CGraphicArea.hight-IMGFormRightCoordinate->height ,0 ,0 ,IMGFormRightCoordinate->width ,IMGFormRightCoordinate->height ,ImageMode_Normal );
	//Clear Form With Defult Color
	GraphicClearClip(NewWND->Form->FControl->CGraphicArea ,BORDER_LINE_SIZE ,BORDER_BAR_SIZE ,NewWND->Form->CSize.Width ,NewWND->Form->CSize.Hight ,FormDefultBackColor );
	
	//DrawImage(MainGraphicArea/*NewWND->Form->FControl->CGraphicArea*/ ,IMGFormRightLine ,Draw_Normal ,NewWND->Form->CSize.Width-IMGFormRightLine->width ,IMGFormTopRight->height ,0 ,0 ,IMGFormRightLine->width ,NewWND->Form->CSize.Hight ,ImageMode_RepeatImage );

	//DrawImage(NewWND->Form->FControl->CGraphicArea ,IMGFormTopLeft ,Draw_Normal ,0 ,0 ,0 ,0 ,IMGFormTopLeft->width ,IMGFormTopLeft->height ,ImageMode_Normal );  
	//GraphicClipClear(NewWND->Form->FControl->CGraphicArea ,0 ,0 ,NewWND->Form->FControl->CGraphicArea.width ,NewWND->Form->FControl->CGraphicArea.hight ,COLOR_LIGHTBLUE );
	//printf("aCreatWindow:Buffer=%p Name=%s (%d,%d)(%d,%d)",NewWND->Form->FControl->CGraphicArea.Buffer,NewWND->Form->CName ,NewWND->Form->CPoint.X,NewWND->Form->CPoint.Y,NewWND->Form->CSize.Width,NewWND->Form->CSize.Hight );getch();
	//GraphicClipClear(NewWND->Form->FControl->CGraphicArea ,0 ,0 ,NewWND->Form->FControl->CGraphicArea.width ,NewWND->Form->FControl->CGraphicArea.hight  ,FORM_BACKGROUND );
	/*while(1);*/	
	
	//printf("KernelCreatWindow:CGraphicArea.Buffer=%p",NewWND->Form->FControl->CGraphicArea.Buffer);getch();
	//printf("Start Form Draw");getch();
	//printf("UpdateWNDS:Buffer=%p Name=%s (%d,%d)(%d,%d)",NewWND->Form->FControl->CGraphicArea.Buffer,NewWND->Form->CName ,NewWND->Form->CPoint.X,NewWND->Form->CPoint.Y,NewWND->Form->CSize.Width,NewWND->Form->CSize.Hight );getch();
	UpdateWNDS(NewWND);	
	//printf("Finish Form Draw");getch();
	return true;
}
bool CreatForm(FormControl *NewForm )
{
	return KernelCreatWindow(wm_handles[0]/*Parent For All Form On Desktop*/ ,NewForm );
}



void KernelRaiseMouseEvent()
{
}






void GUIInit()
 {
	//Init Windows Form Theme	
	InitFormImage();	
	//Init Windows List Management And Init Desktop
	InitWindowManagment();

 }


 




































