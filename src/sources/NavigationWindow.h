#ifndef NAV_WINDOW_H
#define NAV_WINDOW_H

#include "share.h"
#define ZOOM_WAS_SET 'zmws'


class MiniView : public BView 
{

public:
MiniView(BRect r, share *sh);
share *shared;
virtual void Draw(BRect update);
virtual void MouseDown(BPoint pt);

};


class NavigationView : public BView 
{

public:
NavigationView(BRect r, share *sh);
share *shared;

MiniView    *img_view;
BSlider	 *zoom_slider;
BStringView   *percent_view;
};


class NavigationWindow : public BWindow 
{
public:

	share *shared;
	
	NavigationView *navigation_view;		
	NavigationWindow(BRect frame, char *title, share *sh); 
	virtual ~NavigationWindow();
	virtual void MessageReceived(BMessage *msg);
   
};

#endif