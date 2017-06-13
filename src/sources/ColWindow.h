#ifndef COL_WINDOW_H
#define COL_WINDOW_H

#include "share.h"

class ColView : public BView 
{

public:

share *shared;
BPoint old_pt;
BBitmap *col_img,*slide_img;
BTextControl *red_box, *green_box, *blue_box, *alpha_box;
uint8 mode_fore;
rgb_color col_ori; 		//dernière couleur selectée dans spectre
uint8 color_selector_size;

bool pressed,updating;

ColView(BRect r, uint8 b_mode, share *sh, long the_flags);
virtual	~ColView();

	virtual	void Draw(BRect update_rect);

	void UpdatePanel(rgb_color col);
	void CreateSpectrum();
	rgb_color SelectColor(BPoint pix); 		//sel dans le spectre
	rgb_color PickColor(BPoint pix);	 	//sel dans le carré
	void ClickedInSquare(BPoint pt); 		//click DANS LE CARRE
	void ClickedInSpectrum(BPoint pt); 		//click DANS le spectrum

	virtual void MouseDown(BPoint point);
	virtual void MouseUp(BPoint point);
    virtual void MouseMoved(BPoint pt, uint32 transit, const BMessage *a_message);
	
};

class ColWindow : public BWindow 
{
public:

	share *shared;
	
	ColView *c_view;		
 
	ColWindow(uint8 back_m,BRect frame, char *title, share *sh); 
	virtual ~ColWindow();

	virtual void MessageReceived(BMessage *msg);
	 virtual void Zoom(BPoint leftTop, float width, float height);

void ResizeMe();
	
virtual void FrameMoved(BPoint screenPoint);
uint8 color_selector_size;
 
 	uint8 is_fore;

	BPoint last_picked;  
};

#endif