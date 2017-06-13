#ifndef PAPER_WINDOW_H
#define PAPER_WINDOW_H

#include "share.h"

class OnePaperView : public BView 
{
public:
	OnePaperView(const char *name,BRect r, BBitmap *pic, share *sh, int32 le_id);
	virtual ~OnePaperView();
	share *shared;
	BBitmap *img;
	virtual void Draw(BRect);
	virtual void MouseDown(BPoint point);
	int32 id;
	bool active;
	char name[256];
	bool dragging;
	BScrollView *parent_scroller;
};

class AllPaperView : public BView 
{

public:
AllPaperView(BRect r, share *sh);
//virtual	~AllPaperView();

share *shared;

	void AddPaper();
	OnePaperView *tab_paper_views[255];
 	BScrollView *tab_paper_scroll_views[255];
	int32 last_paper;
};


class PaperWindow : public BWindow 
{
public:

	share *shared;
	
	AllPaperView *paper_view;		
	PaperWindow(BRect frame, char *title, share *sh); 
	virtual ~PaperWindow();
	BScrollView *paper_scroll_view;
	virtual void FrameMoved(BPoint screenPoint);
	
	virtual void FrameResized(float x, float y);


	virtual void MessageReceived(BMessage *msg);
   
};

#endif