#ifndef RANGE_WINDOW_H
#define RANGE_WINDOW_H

#include "share.h"

#define SLIDER_CHANGED 'slch'
#define PREVIEW_RANGE_SELECTION 'pvrs'
#define OK_APPLY 'okap'
#define CANCEL_IT 'ccit'

class RangeView: public BView
{
public:
share *shared;
RangeView(BRect rec, share *sh, long flags);
virtual void Draw(BRect zone);
	virtual void MouseDown(BPoint pt);
BButton *ok_button;
BButton *cancel_button;
};

//-----------------


class RangeWindow : public BWindow 
{
public:
	share *shared;
	RangeWindow(BRect frame,share *sh); 
	~RangeWindow();

	void MessageReceived(BMessage *msg);

	RangeView *c_view;			
};


#endif
