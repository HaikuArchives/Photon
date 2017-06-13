#ifndef LIMIT_LEVELS_WINDOW_H
#define LIMIT_LEVELS_WINDOW_H

#include "share.h"

#define SLIDER_CHANGED 'slch'
#define PREVIEW_LIMIT_LEVELS 'pvll'
#define OK_APPLY 'okap'
#define CANCEL_IT 'ccit'


class LimitLevelsView: public BView
{
public:
share *shared;
LimitLevelsView(BRect rec, share *sh, long flags);
virtual void Draw(BRect zone);
virtual void MouseDown(BPoint pt);

BButton *ok_button;
BButton *cancel_button;
BSlider *levels;


};

//-----------------


class LimitLevelsWindow : public BWindow 
{
public:
	share *shared;
	LimitLevelsWindow(BRect frame,share *sh); 
	~LimitLevelsWindow();
	void MessageReceived(BMessage *msg);

	LimitLevelsView *c_view;			
};


#endif
