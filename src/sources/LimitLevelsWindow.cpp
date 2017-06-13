#include "LimitLevelsWindow.h"
#include <Button.h>

LimitLevelsWindow::LimitLevelsWindow(BRect frame, share *sh)
				: BWindow(frame, Language.get("LIMIT_LEVELS"), 
				B_FLOATING_WINDOW, B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
shared=sh;

c_view= new LimitLevelsView(Bounds(),shared,B_WILL_DRAW);			
c_view->SetViewColor(216,216,216);
AddChild(c_view);

util.mainWin->PostMessage(DRAW_ME);
}

LimitLevelsWindow::~LimitLevelsWindow()
{
	ThePrefs.limit_levels_open = false;
	util.mainWin->PostMessage(CANCEL_FILTERING);

}



void LimitLevelsWindow::MessageReceived(BMessage *msg)
{

BMessage x(PREVIEW_LIMIT_LEVELS);
char str[255];
	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;
		
		case SLIDER_CHANGED:
		sprintf(str,"%ld",int32(c_view->levels->Value()));
	//	c_view->levels->SetTrailingText(str);
		x.AddInt32("levels",int32(c_view->levels->Value()));
		util.mainWin->PostMessage(&x);
		break;
		
		case OK_APPLY:
		util.mainWin->PostMessage(APPLY_FILTER);
		Lock();
		Close();
		break;
		
		case CANCEL_IT:
		util.mainWin->PostMessage(CANCEL_FILTERING);
		Lock();
		Close();
		break;

		
	}

//envoie messge à la fenêtre mère
//util.mainWin->PostMessage(new BMessage(LimitLevels_CHANGED)); 


}//fin message received




void LimitLevelsView::MouseDown(BPoint point)
{
beep();
util.mainWin->PostMessage(UPDATE_ME);
}

//----------------------
	
LimitLevelsView::LimitLevelsView(BRect rec, share *sh, long flags):
	BView(rec, "LimitLevels_view", flags, B_WILL_DRAW)
{
shared=sh;
///-------------------

BRect rect;
rect.top = 8;
rect.bottom = 64;
rect.left = (Bounds().Width()-256)/2;
rect.right = rect.left + 256;

levels = new BSlider(rect,"sss",Language.get("LIMIT_LEVELS"),new BMessage(SLIDER_CHANGED),
							1,256,B_TRIANGLE_THUMB,B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);


rgb_color a_col={128,128,255}; 
rgb_color a_colb={0,0,155}; 
levels->SetBarColor(a_col);
levels->UseFillColor(true,&a_colb);
//opacity->SetHashMarks(B_HASH_MARKS_BOTTOM); 
//opacity->SetHashMarkCount(10+1);
levels->SetValue(256);

AddChild(levels);

//BOUTons en bas à gauche
rect.Set(0,0,64,24);
rect.OffsetBy(Bounds().Width()-64-8,Bounds().Height()-24-8);
cancel_button = new BButton(rect,"cancelb",Language.get("CANCEL"),new BMessage (CANCEL_IT),
							B_WILL_DRAW, B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT);
rect.OffsetBy(-64-8,0);
ok_button = new BButton(rect,"okb",Language.get("OK"),new BMessage (OK_APPLY),
							B_WILL_DRAW, B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT);

AddChild(cancel_button);
AddChild(ok_button);

}//fin LimitLevelsview


void LimitLevelsView::Draw(BRect zone)
{
}


