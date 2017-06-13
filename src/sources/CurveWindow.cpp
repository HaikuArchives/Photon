#include "CurveWindow.h"

CurveWindow::CurveWindow(BRect frame, share *sh)
				: BWindow(BRect(100,100,100+256,100+256+64), Language.get("CURVES"), 
				B_FLOATING_WINDOW, B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
shared=sh;

c_view= new CurveView(BRect(0,0,255,255),shared,B_WILL_DRAW);			
c_view->SetViewColor(216,216,216);
AddChild(c_view);


}


void CurveWindow::MessageReceived(BMessage *msg)
{

	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;
	}

//envoie messge à la fenêtre mère
//util.mainWin->PostMessage(new BMessage(CURVE_CHANGED)); 


}//fin message received




void CurveWindow::MouseDown(BPoint point)
{
 //pour ramener au front la pic dessin
c_view->DrawCurve();
}

//----------------------
	
CurveView::CurveView(BRect rec, share *sh, long flags):
	BView(rec, "curve view", flags, B_WILL_DRAW)
{
shared=sh;


int16 i=0;

while (i!=255+1)
{
ori[i]  = i;
dest[i] = i;
i++;
}

DrawCurve();

}//fin curveview


void CurveView::Draw(BRect zone)
{
DrawCurve();
}

void CurveView::DrawCurve()
{
int16 i=0;
SetPenSize(1);


SetPenSize(1);
SetHighColor(100,100,100);
StrokeLine(BPoint(0,64),BPoint(255,64),B_SOLID_HIGH);
StrokeLine(BPoint(0,128),BPoint(255,128),B_SOLID_HIGH);
StrokeLine(BPoint(0,192),BPoint(255,192),B_SOLID_HIGH);

StrokeLine(BPoint(64,0),BPoint(64,255),B_SOLID_HIGH);
StrokeLine(BPoint(128,0),BPoint(128,255),B_SOLID_HIGH);
StrokeLine(BPoint(192,0),BPoint(192,255),B_SOLID_HIGH);

SetHighColor(255,0,0);
BPoint pt;
while (i!=255+1)
{
pt.Set(ori[i],255-dest[i]);
StrokeLine(pt,pt,B_SOLID_HIGH);

i++;
}

//---------------------------------------------------

//-------------------------------

/*
BPoint courbes[4];

courbes[1] = BPoint (0,0);
courbes[2] = BPoint (16,16);
courbes[3] = BPoint (100,222);
courbes[4] = BPoint (300,300);

StrokeBezier(courbes,B_SOLID_HIGH);
*/
//-------------------------------

}

