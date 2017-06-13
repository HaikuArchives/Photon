#include "RangeWindow.h"
#include <Button.h>
RangeWindow::RangeWindow(BRect frame, share *sh)
				: BWindow(frame, Language.get("SELECT_RANGE"), 
				B_FLOATING_WINDOW, B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
shared=sh;

c_view= new RangeView(Bounds(),shared,B_WILL_DRAW);			
c_view->SetViewColor(216,216,216);
AddChild(c_view);

util.mainWin->PostMessage(DRAW_ME);


}


RangeWindow::~RangeWindow()
{
	ThePrefs.range_selection_open = false;
    util.mainWin->PostMessage(CANCEL_FILTERING);


}


void RangeWindow::MessageReceived(BMessage *msg)
{

	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;
		
		/*
			case SLIDER_CHANGED:
		sprintf(str,"%ld",int32(c_view->levels->Value()));
		c_view->levels->SetLimitLabels(Language.get("LIMIT_LEVELS"), str);	
		x.AddInt32("levels",int32(c_view->levels->Value()));
		util.mainWin->PostMessage(&x);
		break;
		*/
		
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
//util.mainWin->PostMessage(new BMessage(Range_CHANGED)); 


}//fin message received




void RangeView::MouseDown(BPoint point)
{
beep();
		rgb_color col;
		col.red = 255;
		col.green = 255;
		col.blue = 255;
		shared->act_img->SetMaskFromColorZone(col,64, MODE_REPLACE_SELECTION);
		util.mainWin->PostMessage(UPDATE_ME);
}

//----------------------
	
RangeView::RangeView(BRect rec, share *sh, long flags):
	BView(rec, "Range view", flags, B_WILL_DRAW)
{
shared=sh;
//---------------
BRect rect;

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

}//fin Rangeview


void RangeView::Draw(BRect zone)
{
}


