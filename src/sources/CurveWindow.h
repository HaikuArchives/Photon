#ifndef CURVE_WINDOW_H
#define CURVE_WINDOW_H

#include "share.h"


class CurveView: public BView
{
public:
share *shared;
CurveView(BRect rec, share *sh, long flags);
virtual void Draw(BRect zone);
void DrawCurve();

int16 ori[255],dest[255];


};

//-----------------


class CurveWindow : public BWindow 
{
public:
	share *shared;
	CurveWindow(BRect frame,share *sh); 
	
	void MessageReceived(BMessage *msg);
	virtual void MouseDown(BPoint pt);

	CurveView *c_view;			
};


#endif
