#include "InfoWindow.h"
#include <PopUpMenu.h>


CoordView::CoordView(BRect r, const char *text, long follow,  share *sh) :
	BStringView(r, "", text, follow, B_WILL_DRAW)
{
shared = sh;


//------------------------------------------------------------------------------------------------------------

pm = new  BPopUpMenu("", true, true, B_ITEMS_IN_COLUMN) ;


BMessage *a = new BMessage(DISPLAY_UNITS_CHANGED);   a->AddInt32("unit",UNIT_CM); 
pm->AddItem( new BMenuItem(Language.get("CENTIMETERS"),a) );
BMessage *b = new BMessage(DISPLAY_UNITS_CHANGED);   b->AddInt32("unit",UNIT_MM); 
pm->AddItem(new BMenuItem(Language.get("MILLIMETERS"),b));

BMessage *c = new BMessage(DISPLAY_UNITS_CHANGED);   c->AddInt32("unit",UNIT_INCH); 
pm->AddItem(new BMenuItem(Language.get("INCHES"),c));

BMessage *d = new BMessage(DISPLAY_UNITS_CHANGED);   d->AddInt32("unit",UNIT_PIXELS); 
pm->AddItem(new BMenuItem("pixels",d));

	
//pm->SetTargetForItems(this->Window());
pm->SetTargetForItems(util.mainWin); //ben ça ça marche et pas celle d'en dessus
										 //alors va savoir...
}

void CoordView::MouseDown(BPoint pt)
{
	if (ThePrefs.no_pictures_left==OFF)	pm->Go(ConvertToScreen(pt),true,false,false);
}


InfoView::InfoView(BRect r, share *sh) : 
	BView(r, "info_view", B_FOLLOW_ALL, B_WILL_DRAW)
{
	shared = sh;
	//------------------------

	SetViewColor(216,216,216);

BRect rect;
#define INFO_MARGIN 6
rect.Set(INFO_MARGIN,INFO_MARGIN-2,INFO_MARGIN+100,INFO_MARGIN-2+14);
x_pos = new CoordView(rect,"X: 0",  B_FOLLOW_LEFT | B_FOLLOW_BOTTOM ,shared);
rect.OffsetBy(0,14);
y_pos = new CoordView(rect,"Y: 0", B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, shared);

char str[255];
sprintf(str,"%d / %d / %d  (%d / %d / %d )",
		shared->fore_color.red,shared->fore_color.green, shared->fore_color.blue,
		shared->back_color.red,shared->back_color.green, shared->back_color.blue
		);
		
rect.OffsetBy(0,14);
rect.right+=100;
col = new BStringView(rect,"",str,  B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,B_WILL_DRAW);

AddChild(x_pos);
AddChild(y_pos);
AddChild(col);

}

InfoWindow::InfoWindow(BRect frame, char *title, share *sh)
				: BWindow(frame, title, B_FLOATING_WINDOW,B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE| B_NOT_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
shared=sh;

//------------------------

shared->display_menu->ItemAt(3)->SetMarked(true); //à parti de 0
ThePrefs.info_win_open=true;
util.infoWin = this;


BRect rect  = Bounds();
info_view = new InfoView(rect, shared);

AddChild(info_view);


SetSizeLimits(ThePrefs.info_frame.Width(), ThePrefs.info_frame.Width() ,ThePrefs.info_frame.Width(), 3000);	

 //toujours la pic activée ...

}


InfoWindow::~InfoWindow()
{
	shared->display_menu->ItemAt(3)->SetMarked(false); //à partir de 0
	ThePrefs.info_win_open = false;
	ThePrefs.info_frame = Frame(); //in order to use it again later

}


void InfoWindow::MessageReceived(BMessage *msg)
{
char str[255];
char str2[255];
char col_str[255];
float val_a,val_b;
	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;
		
		
		case MOUSE_MOVEMENT:
		
		
		msg->FindFloat("x_pos",&val_a);
		msg->FindFloat("y_pos",&val_b);
		
		val_a = util.ConvertUnits(val_a, UNIT_PIXELS, shared->act_img->units, 
								  shared->act_img->resolution,shared->act_img->res_units);
		val_b = util.ConvertUnits(val_b, UNIT_PIXELS, shared->act_img->units, 
								  shared->act_img->resolution,shared->act_img->res_units);
				
		switch (shared->act_img->units)
			{
			case UNIT_CM:
				sprintf(str,"X: %.3f cm",val_a);
				sprintf(str2,"Y: %.3f cm",val_b);
			break;
			
			case UNIT_MM:
				sprintf(str,"X: %.1f mm",val_a);
				sprintf(str2,"Y: %.1f mm",val_b);
			break;
			
			case UNIT_INCH:
				sprintf(str,"X: %.3f ''",val_a);
				sprintf(str2,"Y: %.3f ''",val_b);
			break;
			
			case UNIT_PIXELS:
				sprintf(str,"X: %.0f pixels",val_a);
				sprintf(str2,"Y: %.0f pixels",val_b);
			break;
			}
	
		info_view->x_pos->SetText(str);
		info_view->y_pos->SetText(str2);
		break;
		

		case COL_CHANGED:
		sprintf(col_str,"%d / %d / %d  (%d / %d / %d )",
			shared->fore_color.red,shared->fore_color.green, shared->fore_color.blue,
			shared->back_color.red,shared->back_color.green, shared->back_color.blue);
		
		info_view->col->SetText(col_str);
	
		break;

	default:
	BWindow::MessageReceived( msg );
   			   		   	
	 }
   	 	
}

