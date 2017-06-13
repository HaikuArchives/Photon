#ifndef CREATE_WINDOW_H
#define CREATE_WINDOW_H

#include "share.h"

#define UNITS_CHANGED 	 'unch'
#define RES_CHANGED 	 'rsch'
#define RES_TYPE_CHANGED 'rstc'

#define ACTIVATE_FORMAT 'acft'


class PaperFormat 
{
public:
	PaperFormat(char the_name[64],int32 the_res_type,float the_res,
				int32 the_units,float the_x,float the_y);

		char name[64];
		int32 what_res_type;
		float res;
		int32 units;
		float x;
		float y;

};

class CreateView : public BView 
{
public:
	share *shared;

	CreateView(BRect frame,share *sh); 

	BTextControl *x,*y,*res,*name_field;
	BStringView *taille;
	BStringView *pixel_x,*pixel_y;
	int32 pix_x,pix_y;

	BMenuField *units,*format_field;
	BMenuField *res_type;
	virtual void MouseMoved(BPoint point, uint32 transit, const BMessage *message);

	PaperFormat *the_formats[255];
	int32 format_amount;
	
};



class CreateWindow : public BWindow 
{
public:

	share *shared;
	CreateWindow(BRect frame,share *sh); 
	~CreateWindow();
	
	void MessageReceived(BMessage *msg);

	CreateView *crt_view;
	void UpdatePixSize();
	int32 old_unit;
	int32 old_res_type;
	
};


#endif
