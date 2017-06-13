#ifndef TOOL_WINDOW_H
#define TOOL_WINDOW_H

#include "share.h"
#include "ColWindow.h"

#define SET_FULL_NONE 'sfnn'
#define SET_FULL_HALF 'sfhf'
#define SET_FULL_TOTAL 'sftt'
#define SET_TO_COLOR 'stcl'

class ForeView : public BView 
{
public:
	share *shared;
ForeView(BRect r,share *sh);
virtual void MouseDown(BPoint pt);
virtual void MouseUp(BPoint pt);
virtual void MouseMoved(BPoint where, uint32 transit, const BMessage* dragDropMsg);
virtual void MessageReceived(BMessage *ms);

BPoint old_pos;
bool dragging;
bool pressed;
};

class BackView : public BView 
{
public:
	share *shared;
BackView(BRect r,share *sh);
virtual void MouseDown(BPoint pt);
virtual void MouseUp(BPoint pt);
virtual void MouseMoved(BPoint where, uint32 transit, const BMessage* dragDropMsg);
virtual void MessageReceived(BMessage *ms);

BPoint old_pos;
bool dragging;
bool pressed;
};

class BrushView : public BView 
{
public:
	share *shared;
BrushView(BRect r,share *sh);
virtual void MouseDown(BPoint pt);
virtual void Draw(BRect);
};

class PaperView : public BView 
{
public:
share *shared;
PaperView(BRect r,share *sh);
virtual void MouseDown(BPoint pt);
virtual void Draw(BRect);
BScrollView *parent_scroller;
};



class ToolWindow : public BWindow 
{
public:

	share *shared;
	
	ToolWindow(BRect frame,const char *name, share *sh); 
	~ToolWindow();

	ColWindow		*foreWindow;
	ColWindow		*backWindow;
	
	
	BBitmap* downBitmap;
	BBitmap* upBitmap;
	char name[2048];
	char temp_name[2048];
   	BPicture *on,*off;
 	void CreateButton(char [255], char nm2[255]);
	void MessageReceived(BMessage *msg);
	void DeselectAll();
	
	virtual void FrameMoved(BPoint screenPoint);
	//--------------------
	BPictureButton *button_01,*button_02,*button_03,*button_04,*button_05,*button_06,*button_07,*button_08;
	BPictureButton *button_b_01,*button_b_02,*button_b_03,*button_b_04,*button_b_05,*button_b_06,*button_b_07,*button_b_08;
	BPictureButton *full_none, *full_half, *full_total;
	
	ForeView *fore_view;
	BackView *back_view;
	PaperView *paper_view;
	BrushView *brush_view;		
};


#endif
