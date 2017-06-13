#include "NavigationWindow.h"

MiniView::MiniView(BRect r, share *sh) : 
	BView(r, "mini_view", B_FOLLOW_ALL, B_WILL_DRAW)
{
	shared = sh;
	//------------------------

	SetViewColor(216,216,216);
	
}

void MiniView::Draw(BRect update_rect)
{
	if (shared->act_img!=NULL)
	{
	BRect s_rect = shared->act_img->display_bitmap->Bounds();
	BRect dest_rect = Bounds();
	float ratio = s_rect.Width() / s_rect.Height();
	
	if (ratio >1)  
	{
		dest_rect.bottom = dest_rect.right / ratio;  
		dest_rect.OffsetBy(0,(Bounds().Height()-dest_rect.bottom)/2);
	}
	else
	{
		dest_rect.right = dest_rect.bottom * ratio;  
		dest_rect.OffsetBy((Bounds().Width()-dest_rect.right)/2,0);
	}
	
	 DrawBitmap(shared->act_img->display_bitmap, s_rect,dest_rect);
	
	 if (shared->act_img->zoom_level > 1) //seulement quand le zoom est positif...
	 {
	 //dessin du cadre... complètement bidon...
	 float x = dest_rect.Width()  / shared->act_img->zoom_level;
	 float y = dest_rect.Height() / shared->act_img->zoom_level;
	 dest_rect.Set(0,0,x,y);
	 
	 //bon là pour des raisons esthétiques c'est centré mais faudra trouver la
	 //solution pour placer juste par rapport à la vision...
	 dest_rect.OffsetBy((Bounds().Width()-dest_rect.Width()) /2,
	 					(Bounds().Height()-dest_rect.Height()) /2); 
	 
	 SetPenSize(2);
	 SetDrawingMode(B_OP_INVERT);
	 StrokeRect(dest_rect,B_SOLID_HIGH);
	 SetDrawingMode(B_OP_COPY);
	 }
		
	}
	
	 //Cadre noir
	 SetPenSize(1); SetHighColor(0,0,0);
	 StrokeRect(Bounds(),B_SOLID_HIGH);
	 
	
	
}

void MiniView::MouseDown(BPoint pt)
{
	uint32 buttons;
	GetMouse(&pt,&buttons);
	if (buttons==B_SECONDARY_MOUSE_BUTTON) 
		{
		if (shared->act_img!=NULL)
			{
				shared->act_img->zoom_level = 1;
				util.mainWin->PostMessage(ZOOM_CHANGED);
			}
			
		}

}


NavigationView::NavigationView(BRect r, share *sh) : 
	BView(r, "navigation_view", B_FOLLOW_ALL, B_WILL_DRAW)
{
	shared = sh;
	//------------------------

	SetViewColor(216,216,216);

BRect rect   = Bounds();
rect.bottom -= 24;
rect.InsetBy(2,2);

img_view = new MiniView (rect,shared);

rect = Bounds();
rect.top = rect.bottom -24+1;
rect.left = 48;
zoom_slider = new BSlider(rect,NULL,NULL,new BMessage(ZOOM_WAS_SET),
							5,1000,B_TRIANGLE_THUMB,B_FOLLOW_BOTTOM,B_WILL_DRAW | B_ASYNCHRONOUS_CONTROLS);
							
rect.left =0; rect.right = 48-1;
rect.bottom-=5;							
percent_view = new BStringView(rect,"percent_view","0xx%",B_FOLLOW_ALL,B_WILL_DRAW);
percent_view->SetAlignment(B_ALIGN_CENTER);

AddChild(img_view);
AddChild(zoom_slider);
AddChild(percent_view);

	
}

NavigationWindow::NavigationWindow(BRect frame, char *title, share *sh)
				: BWindow(frame, title, B_FLOATING_WINDOW,B_NOT_MINIMIZABLE 
				| B_NOT_ZOOMABLE| B_NOT_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK 
				| B_AVOID_FOCUS| B_ASYNCHRONOUS_CONTROLS)
{
shared=sh;

//------------------------

shared->display_menu->ItemAt(5)->SetMarked(true); //à parti de 0
ThePrefs.navigation_win_open=true;
util.navWin = this;


BRect rect  = Bounds();
navigation_view = new NavigationView(rect, shared);

AddChild(navigation_view);

PostMessage(ZOOM_CHANGED);

 //toujours la pic activée ...

}


NavigationWindow::~NavigationWindow()
{
	shared->display_menu->ItemAt(5)->SetMarked(false); //à partir de 0
	ThePrefs.navigation_win_open = false;
	ThePrefs.navigation_frame = Frame(); //in order to use it again later

}


void NavigationWindow::MessageReceived(BMessage *msg)
{
float val_a;
char str[255];
	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;

			
		case ZOOM_CHANGED:
		if (shared->act_img!=NULL) 
		{
			navigation_view->zoom_slider->SetValue(shared->act_img->zoom_level*100);
			sprintf(str,"%.0f%%",shared->act_img->zoom_level*100);
			navigation_view->percent_view->SetText(str);
			navigation_view->img_view->Draw(Bounds());
		}
		break;
		
		
		case ZOOM_WAS_SET:
		val_a = navigation_view->zoom_slider->Value();
		sprintf(str,"%.0f%%",val_a);
		navigation_view->percent_view->SetText(str);
		if (shared->act_img!=NULL) shared->act_img->zoom_level = val_a/100;
		util.mainWin->PostMessage(ZOOM_CHANGED);
		//util.mainWin->PostMessage(UPDATE_TITLE);
		
		break;
	
			
		
	default:
	BWindow::MessageReceived( msg );
   			   		   	
	 }
   	 	
}

