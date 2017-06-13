#ifndef BRUSH_WINDOW_H
#define BRUSH_WINDOW_H

#include "share.h"


class OneBrushView : public BView 
{
public:
	OneBrushView(const char *name, BRect r, BBitmap *pic, share *sh, int32 le_id);
	virtual ~OneBrushView();
	share *shared;
	BBitmap *img;
	virtual void Draw(BRect);
	virtual void MouseDown(BPoint point);
	int32 id;
	bool active;
	char name[256];
};

class AllBrushesView : public BView 
{

public:
AllBrushesView(BRect r, share *sh);
//virtual	~AllBrushesView();

share *shared;

	void AddBrushes();
	OneBrushView *tab_brush_views[255];
 	
	int32 last_brush;
};


class BrushWindow : public BWindow 
{
public:

	share *shared;
	
	AllBrushesView *pr_br_view;		
	BrushWindow(BRect frame, char *title, share *sh); 
	virtual ~BrushWindow();
	BScrollView *perso_scroll_view;
	virtual void FrameMoved(BPoint screenPoint);
	
	virtual void FrameResized(float x, float y);


	virtual void MessageReceived(BMessage *msg);
   
};

#endif