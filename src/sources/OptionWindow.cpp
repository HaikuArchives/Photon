#include "OptionWindow.h"


CommonOptionsView::CommonOptionsView(BRect r, share *sh) : 
	BView(r, "common_option_view", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	shared = sh;
	//------------------------

	SetViewColor(216,216,216);

BRect rect;
rect = Bounds();
rect.right/=2;
rect.bottom = 24;

BMenuItem *it;
mode_menu = new BMenu("Paint Mode");

BMessage *a = new BMessage(MODE_CHANGED); a->AddInt32("mode",NORMAL);
mode_menu->AddItem(it = new BMenuItem(Language.get("PM_NORMAL"), a));

BMessage *b = new BMessage(MODE_CHANGED); b->AddInt32("mode",MULTIPLY);
mode_menu->AddItem(new BMenuItem(Language.get("PM_MULTIPLY"),  b));

BMessage *c = new BMessage(MODE_CHANGED); c->AddInt32("mode",LIGHTEN);
mode_menu->AddItem(new BMenuItem(Language.get("PM_LIGHTEN"),   c));

BMessage *d = new BMessage(MODE_CHANGED); d->AddInt32("mode",DARKEN);
mode_menu->AddItem(new BMenuItem(Language.get("PM_DARKEN"),    d));

BMessage *e = new BMessage(MODE_CHANGED); e->AddInt32("mode",COLORIZE);
mode_menu->AddItem(new BMenuItem(Language.get("PM_COLORIZE"),  e));

BMessage *f = new BMessage(MODE_CHANGED); f->AddInt32("mode",DIFFERENCE);
mode_menu->AddItem(new BMenuItem(Language.get("PM_DIFFERENCE"),f));

BMessage *g = new BMessage(MODE_CHANGED); g->AddInt32("mode",COMBINE);
mode_menu->AddItem(new BMenuItem(Language.get("PM_COMBINE"),  g));


it->SetMarked(true);

mode_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
paint_mode = new BMenuField(rect,"",NULL,mode_menu, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,B_WILL_DRAW);
mode_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
AddChild(paint_mode);

//--------------------------------------------------------------------

rect = Bounds();
rect.bottom = 36;
rect.OffsetBy(0,paint_mode->Bounds().Height());
opacity_slider = new BSlider(rect,NULL,"",new BMessage(TRANSP_WAS_SET),
							0,100,B_TRIANGLE_THUMB,B_FOLLOW_BOTTOM,B_WILL_DRAW);

rgb_color a_col={128,128,255}; 
rgb_color a_colb={0,0,155}; 
opacity_slider->SetBarColor(a_col);
opacity_slider->UseFillColor(true,&a_colb);
opacity_slider->SetHashMarks(B_HASH_MARKS_BOTTOM); 
opacity_slider->SetHashMarkCount(10+1);
opacity_slider->SetValue(100);

AddChild (opacity_slider);


}



OptionView::OptionView(BRect r, share *sh) : 
	BView(r, "option_view", B_FOLLOW_ALL, B_WILL_DRAW)
{
	shared = sh;
	//------------------------

	SetViewColor(216,200,216);
	

}

OptionWindow::OptionWindow(BRect frame, char *title, share *sh)
				: BWindow(frame, title, B_FLOATING_WINDOW,B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE| B_NOT_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
shared=sh;

//------------------------

shared->display_menu->ItemAt(4)->SetMarked(true); //à parti de 0
ThePrefs.option_win_open=true;
util.optionWin = this;


BRect rect  = Bounds();
option_view = new OptionView(rect, shared);

AddChild(option_view);


SetSizeLimits(ThePrefs.option_frame.Width(), ThePrefs.option_frame.Width() ,ThePrefs.option_frame.Width(), 3000);	

 //toujours la pic activée ...

PostMessage(UPDATE_OPTIONS);

BMessage *tr = new BMessage(SET_TRANSP);
tr->AddInt32("value",100-shared->paint_transparency); 
util.mainWin->PostMessage(tr); 


//pour les scroll bars
FrameResized(Bounds().Width(),Bounds().Height());

}


OptionWindow::~OptionWindow()
{
	shared->display_menu->ItemAt(4)->SetMarked(false); //à partir de 0
	ThePrefs.option_win_open = false;
	ThePrefs.option_frame = Frame(); //in order to use it again later

}

void OptionView::UpdateOptions()
{

	
	//on efface les anciens
	while (ChildAt(0) !=NULL) ChildAt(0)->RemoveSelf();

	BRect rect = Bounds();
	
	rect.bottom = COMMON_OPTION_HEIGHT;
	common_view = new CommonOptionsView(rect,shared);
	AddChild(common_view);
	
Window()->PostMessage(TRANSP_CHANGED);

}

void OptionWindow::UpdateTitle()
{

char the_name[255];
char the_name2[255];

sprintf(the_name,Language.get("OPTIONS"));
strcat(the_name,": ");
switch (shared->active_tool)
		{		
				case PAINTBRUSH:	strcat(the_name,Language.get("PAINTBRUSH"));	break;
				case ERASER:		strcat(the_name,Language.get("ERASER"));		break;
				case BUCKET:		strcat(the_name,Language.get("BUCKET"));		break;
				case LINE:			strcat(the_name,Language.get("LINE"));			break;
				case F_POLY:		strcat(the_name,Language.get("F_POLY"));		break;
				case SPLINE:		strcat(the_name,Language.get("SPLINE"));		break;
				case F_SPLINE_POLY:	strcat(the_name,Language.get("F_SPLINE_POLY"));	break;
				case RECTANGLE:		strcat(the_name,Language.get("RECTANGLE"));		break;
				case F_RECTANGLE:	strcat(the_name,Language.get("F_RECTANGLE"));	break;
				case ELLIPSE:		strcat(the_name,Language.get("ELLIPSE"));		break;
				case F_ELLIPSE:		strcat(the_name,Language.get("F_ELLIPSE"));		break;
				case TEXT:			strcat(the_name,Language.get("TEXT"));			break;
				case ZOOM:			strcat(the_name,Language.get("ZOOM"));			break;
				case PICKER:		strcat(the_name,Language.get("PICKER"));		break;
				case STAMP:			strcat(the_name,Language.get("STAMP"));			break;
				case WAND:			strcat(the_name,Language.get("WAND"));			break; 
				case LASSO:			strcat(the_name,Language.get("LASSO"));			break; 
				case SEL_RECT:		strcat(the_name,Language.get("SEL_RECT"));		break; 
				case SEL_ELLIPSE:	strcat(the_name,Language.get("SEL_ELLIPSE"));	break; 
				case MOVE:			strcat(the_name,Language.get("MOVE"));			break; 
				case PICK_BRUSH:	strcat(the_name,Language.get("PICK_BRUSH"));	break;


		}

sprintf(the_name2,""); //chaine vide par défaut
switch (shared->active_tool) //ouais c'est le même switch mais c'est plus clair et pis bon on a pas besoin d'optimiser ici
		{		
				case PAINTBRUSH:	
				case ERASER:		
				case BUCKET:	
				case LINE:			
				case SPLINE:		
				case RECTANGLE:		
				case ELLIPSE:		
				case STAMP:		
				//pour tous ceux-ci on affiche la taille brush (ceux qui tirent des traits)
				sprintf(the_name2," (%d x %d)",shared->brush_x,shared->brush_y);		
				break;	

				case F_POLY:		
				case F_SPLINE_POLY:	
				case F_RECTANGLE:	
				case F_ELLIPSE:	
				case TEXT:		
				case ZOOM:		
				case PICKER:	
				case WAND:			
				case LASSO:			
				case SEL_RECT:		
				case SEL_ELLIPSE:	 
				case MOVE:			
				case PICK_BRUSH:
				
				break;
		}
strcat(the_name,the_name2);
SetTitle(the_name);	



}

void OptionWindow::MessageReceived(BMessage *msg)
{
char str[255];
char str2[255];
float val_a;
int32 number;

	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;

		case UPDATE_OPTIONS:
		Lock();
		UpdateTitle();
		option_view->UpdateOptions();
		Unlock();
		break;	
		
		case MODE_CHANGED:		
		msg->FindInt32("mode",&number);

		if (ThePrefs.option_win_open==true)
			{
				option_view->common_view->paint_mode->Menu()->ItemAt(number)->SetMarked(true);
				option_view->common_view->paint_mode->Menu()->SetLabelFromMarked(true);
			}
			shared->paint_mode=number;
		break;
		
		
		case TRANSP_WAS_SET: //using the slider
		val_a = option_view->common_view->opacity_slider->Value();
		shared->paint_transparency=uint8(100-floor(val_a)); //because opacity
		PostMessage(TRANSP_CHANGED);
		break;
		
		case TRANSP_CHANGED: //value of paint_transparency changed
		val_a = uint8(100-shared->paint_transparency); //because opacity
		
				sprintf(str,Language.get("OPACITY"));
				sprintf(str2," %.0f%%",val_a);
				strcat(str,str2);
				sprintf (str2," (-> %.1f%%)", shared->pressure*100);
				strcat(str,str2);

		option_view->common_view->opacity_slider->SetLabel(str);
		option_view->common_view->opacity_slider->SetValue(val_a);

		break;
		
	default:
	BWindow::MessageReceived( msg );
   			   		   	
	 }
   	 	
}

void OptionWindow::FrameResized(float width, float height)
{
BMessage *ms = new BMessage(SET_TRANSP);
ms->AddInt8("value",shared->paint_transparency);
util.mainWin->PostMessage(ms);

beep();
beep();
beep();
}
