#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include "share.h"


#define DISPLAY_UNITS_CHANGED 	 'duch'

class CoordView : public BStringView
{
public:
	CoordView(BRect rect, const char *txt, long follow, share *sh); 
	share *shared;
	
	virtual void MouseDown(BPoint pt);

	BPopUpMenu* pm;
	
};


class InfoView : public BView 
{

public:
InfoView(BRect r, share *sh);
share *shared;

CoordView 		*x_pos,*y_pos;
BStringView     *col;

};


class InfoWindow : public BWindow 
{
public:

	share *shared;
	
	InfoView *info_view;		
	InfoWindow(BRect frame, char *title, share *sh); 
	virtual ~InfoWindow();
	virtual void MessageReceived(BMessage *msg);
   
};

#endif