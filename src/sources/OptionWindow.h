#ifndef OPTION_WINDOW_H
#define OPTION_WINDOW_H

#define COMMON_OPTION_HEIGHT 64
#define SET_TRANSP		'sttr'
#define TRANSP_WAS_SET  'trws'

#include "share.h"

#define UPDATE_OPTIONS 'upop'

class CommonOptionsView : public BView 
{

public:
CommonOptionsView(BRect r, share *sh);
share *shared;

BMenuField		*paint_mode;  //mode multiply, normal...
BSlider			*opacity_slider;
BMenu *mode_menu;
};


class OptionView : public BView 
{

public:
OptionView(BRect r, share *sh);
share *shared;
CommonOptionsView *common_view;
void UpdateOptions();

};


class OptionWindow : public BWindow 
{
public:

	share *shared;
	
	OptionView *option_view;		
	OptionWindow(BRect frame, char *title, share *sh); 
	virtual ~OptionWindow();
	virtual void MessageReceived(BMessage *msg);
	virtual void FrameResized(float width, float height);

	void UpdateTitle();

};

#endif