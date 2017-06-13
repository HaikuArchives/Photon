#include "ToolWindow.h"
#include "Dragger.h"


ToolWindow::ToolWindow(BRect frame, const char *the_win_name, share *sh)
				: BWindow(frame, the_win_name, B_FLOATING_WINDOW,
				B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_NOT_ZOOMABLE | 
				B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
shared=sh;
//------------------------
BView *fond = new BView(Bounds(),"",B_FOLLOW_ALL,B_WILL_DRAW);
fond->SetViewColor(216,216,216);
AddChild(fond);
#define BUTTON_X 24
#define BUTTON_Y 23 //eh, oui.. maintenant c'est fait, c'est fait...

			//DOSSIER DE BASE DANS NAME (utilisé par create button)
			sprintf(name,util.dossier_app);
			strcat(name,"/data/");

			BRect rect;			
			
			//BT_01
			rect.Set(0,0, BUTTON_X, BUTTON_Y );
			CreateButton("paintbrush.png","XXpaintbrush.png");
			button_01 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_01_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_01,Language.get("PAINTBRUSH"));

			
			//BT_02
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("bucket.png","XXbucket.png");
			button_02 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_02_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_02,Language.get("BUCKET"));

			//BT_03
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("line.png","XXline.png");
			button_03 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_03_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_03,Language.get("LINE"));

			//BT_04
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("spline.png","XXspline.png");
			button_04 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_04_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_04,Language.get("SPLINE"));

			//BT_05
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("rectangle.png","XXrectangle.png");
			button_05 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_05_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_05,Language.get("RECTANGLE"));

			//BTB_06
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("ellipse.png","XXellipse.png");
			button_06 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_06_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_06,Language.get("ELLIPSE"));

			//BT_07
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("text.png","XXtext.png");
			button_07 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_07_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_07,Language.get("TEXT"));

			//BT_08
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("zoom.png","XXzoom.png");
			button_08 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_08_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_08,Language.get("ZOOM"));

/*
			//BT_09
			rect.Set(0,0,20,20);	
			SetHighColor(255,0,0);
			fore_rect = new BBitmap(rect,B_SOLID_HIGH);
*/			
//--------------------------------

			
			//changement de colonne
			rect.Set(BUTTON_X,0, (BUTTON_X)*2, BUTTON_Y );


			//BTB_01
			//pas d'offset pour le premier
			CreateButton("picker.png","XXpicker.png");
			button_b_01 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_B_01_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_b_01,Language.get("PICKER"));

			//BTB_02
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("stamp.png","XXstamp.png");
			button_b_02 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_B_02_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_b_02,Language.get("STAMP"));

			//BTB_03
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("wand.png","XXwand.png");
			button_b_03 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_B_03_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_b_03,Language.get("WAND"));

			//BTB_04
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("sel_lasso.png","XXsel_lasso.png");
			button_b_04 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_B_04_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_b_04,Language.get("LASSO"));

			//BTB_05
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("sel_rect.png","XXsel_rect.png");
			button_b_05 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_B_05_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_b_05,Language.get("SEL_RECT"));
			
			//BTB_06
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("sel_ellipse.png","XXsel_ellipse.png");
			button_b_06 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_B_06_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_b_06,Language.get("SEL_ELLIPSE"));

			//BTB_07
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("mask.png","XXmask.png");
			button_b_07 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_B_07_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_b_07,Language.get("MASKING"));

			//BTB_08
			rect.OffsetBy(0, BUTTON_Y);
			CreateButton("move.png","XXmove.png");
			button_b_08 = new BPictureButton(rect,"",off, on, new BMessage(BUTTON_B_08_MSG),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(button_b_08,Language.get("MOVE"));


			
		

			//BT_09
		//	rect.OffsetBy(0, BUTTON_Y);	
		//	StrokeRect(rect,B_SOLID_HIGH);

			fond->AddChild(button_01);
			fond->AddChild(button_02);
			fond->AddChild(button_03);
			fond->AddChild(button_04);
			fond->AddChild(button_05);
			fond->AddChild(button_06);
			fond->AddChild(button_07);
			fond->AddChild(button_08);


			fond->AddChild(button_b_01);
			fond->AddChild(button_b_02);
			fond->AddChild(button_b_03);
			fond->AddChild(button_b_04);
			fond->AddChild(button_b_05);
			fond->AddChild(button_b_06);
			fond->AddChild(button_b_07);
			fond->AddChild(button_b_08);
		


//button_01->SetValue(B_CONTROL_ON); //paintbrush par défaut au début


rect.OffsetBy((BUTTON_X*-1), BUTTON_Y+1);

rect.right-=2;
rect.bottom-=1;
rect.top+=1;

rect.left+=1;
BRect tep = rect;
BView *b_fond = new BView(rect,"",B_FOLLOW_ALL,B_WILL_DRAW);
tep = b_fond->Bounds();
tep.InsetBy(1,1);
back_view = new BackView(tep,shared);
b_fond->AddChild(back_view);

rect.OffsetBy(BUTTON_X,0);
BView *f_fond = new BView(rect,"",B_FOLLOW_ALL,B_WILL_DRAW);
tep = f_fond->Bounds();
tep.InsetBy(1,1);
fore_view = new ForeView(tep,shared);
f_fond->AddChild(fore_view);

b_fond->SetViewColor(0,0,0); //ca fait les cadres noirs...
f_fond->SetViewColor(0,0,0);


//-----------------------------------


rect.OffsetBy((BUTTON_X*-1), BUTTON_Y);
rect.left =0;
rect.right = Bounds().right;
rect.bottom +=BUTTON_Y;
rect.InsetBy(1,1);

BView *br_fond = new BView(rect,"",B_FOLLOW_ALL,B_WILL_DRAW);
tep = br_fond->Bounds();
tep.InsetBy(1,1);
brush_view = new BrushView(tep,shared);;
br_fond->AddChild(brush_view);


rect.OffsetBy(0, (BUTTON_Y*2)-1);
BView *pp_fond = new BView(rect,"",B_FOLLOW_ALL,B_WILL_DRAW);
tep = pp_fond->Bounds();
tep.InsetBy(1,1);

paper_view = new PaperView(tep,shared);;


BScrollView *paper_sc_view = new BScrollView("sc", paper_view,
									B_FOLLOW_NONE, B_WILL_DRAW, true,true);
pp_fond->AddChild(paper_sc_view);
paper_view->parent_scroller = paper_sc_view;



br_fond->SetViewColor(0,0,0); //ca fait les cadres noirs...
pp_fond->SetViewColor(0,0,0);

paper_view->SetViewColor(216,216,216);

/*
BDragger *drag_paper = new BDragger(BRect(0,0,16,16), paper_view, B_FOLLOW_ALL, B_WILL_DRAW);
paper_view->AddChild(drag_paper);

*/  
//  BDragger(BRect frame, BView *target, uint32 resizingMode = B_FOLLOW_NONE, uint32 flags = B_WILL_DRAW) 



fond->AddChild(f_fond);
fond->AddChild(b_fond);

fond->AddChild(br_fond);
fond->AddChild(pp_fond);

    
bb_help.SetHelp(fore_view,Language.get("FOREGROUND"));
bb_help.SetHelp(back_view,Language.get("BACKGROUND"));
bb_help.SetHelp(brush_view,Language.get("BRUSHES"));
bb_help.SetHelp(paper_view,Language.get("PAPERS"));


			//full_none
			rect.Set(0,0, 14-1, 13-1 );
			rect.OffsetBy(1,Bounds().Height()-14);
			CreateButton("full_none_up.png","full_none_down.png");
			full_none = new BPictureButton(rect,"",off, on, new BMessage(SET_FULL_NONE),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(full_none,Language.get("WINDOWED"));

			//full_half
			rect.OffsetBy(14+2,0);
			CreateButton("full_half_up.png","full_half_down.png");
			full_half = new BPictureButton(rect,"",off, on, new BMessage(SET_FULL_HALF),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(full_half,Language.get("NO_TITLE"));

			//full_none
			rect.OffsetBy(14+2,0);
			CreateButton("full_full_up.png","full_full_down.png");
			full_total = new BPictureButton(rect,"",off, on, new BMessage(SET_FULL_TOTAL),B_TWO_STATE_BUTTON);
			bb_help.SetHelp(full_total,Language.get("FULL_SCREEN"));
	
			fond->AddChild(full_none);
			fond->AddChild(full_half);
			fond->AddChild(full_total);

ThePrefs.tool_win_open=true;
util.toolWin = this;

}

ToolWindow::~ToolWindow()
{
	ThePrefs.tool_win_open=false;
	shared->display_menu->ItemAt(6)->SetMarked(false); //à partir de 0
	ThePrefs.tool_frame = Frame(); //in order to use it again later
	
	if (ThePrefs.fore_selector_open == true)	{ foreWindow->Lock();	foreWindow->Close(); }
	if (ThePrefs.back_selector_open == true)	{ backWindow->Lock();	backWindow->Close(); }

}

void ToolWindow::CreateButton(char nm[255],char nm2[255])
{
			BRect rect;
			rect.Set(0,0, BUTTON_X, BUTTON_Y );
	
					
			downBitmap = BTranslationUtils::GetBitmap('RAWT',nm);
			upBitmap = BTranslationUtils::GetBitmap('RAWT',nm2);

			//sécurité
			if (upBitmap==NULL) upBitmap = new BBitmap(rect, B_RGB32);
			if (downBitmap==NULL) downBitmap = new BBitmap(rect, B_RGB32);
		
		
			//tempview for creating the picture
			BView *tempView = new BView( rect, "temp", B_FOLLOW_NONE, B_WILL_DRAW );
			AddChild(tempView);
			//create on picture
   			tempView->BeginPicture(new BPicture); 
   			tempView->DrawBitmap(upBitmap);
   			on = tempView->EndPicture();
   			//create off picture
   			tempView->BeginPicture(new BPicture); 
   			tempView->DrawBitmap(downBitmap);
   			off = tempView->EndPicture();
   			//get rid of tempview
   			 
   			RemoveChild(tempView);
   			delete tempView;
			delete upBitmap;
   			delete downBitmap;
   			
   			
}

//--------------

void ToolWindow::MessageReceived(BMessage *msg)
{
BRect rect;

BMessage *tmp;

	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;
		
		
	   	case BUTTON_01_MSG:
	   	DeselectAll();  button_01->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=PAINTBRUSH;
   	 	break; 	

	   	case BUTTON_02_MSG:
	   	DeselectAll();  button_02->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=BUCKET;
   	 	break; 	

	   	case BUTTON_03_MSG:
	   	DeselectAll();  button_03->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=LINE;
   	 	break; 	

	   	case BUTTON_04_MSG:
	   	DeselectAll();  button_04->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=SPLINE;
   	 	break; 	

	   	case BUTTON_05_MSG:
	   	DeselectAll();  button_05->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=RECTANGLE;
   	 	break; 	

	   	case BUTTON_06_MSG:
	   	DeselectAll();  button_06->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=ELLIPSE;
   	 	break; 	

	   	case BUTTON_07_MSG:
	   	DeselectAll();  button_07->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=TEXT;
   	 	break; 	

	   	case BUTTON_08_MSG:
	   	DeselectAll();  button_08->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=ZOOM;
   	 	break; 	


//DEUXIüME colonne

	   	case BUTTON_B_01_MSG:
	   	DeselectAll();  button_b_01->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=PICKER;
   	 	break; 	

	   	case BUTTON_B_02_MSG:
	   	DeselectAll();  button_b_02->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=STAMP;
   	 	break; 	

	   	case BUTTON_B_03_MSG:
	   	DeselectAll();  button_b_03->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=WAND;
   	 	break; 	

	   	case BUTTON_B_04_MSG:
	   	DeselectAll();  button_b_04->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=LASSO;
   	 	break; 	

	   	case BUTTON_B_05_MSG:
	   	DeselectAll();  button_b_05->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=SEL_RECT;
   	 	break; 	

	   	case BUTTON_B_06_MSG:
	   	DeselectAll();  button_b_06->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=SEL_ELLIPSE;
   	 	break; 	

	   	case BUTTON_B_07_MSG:
	   	//quick mask
	   
		if (ThePrefs.mask_mode==ON) {ThePrefs.mask_mode=OFF; ThePrefs.mask_activated=ON; button_b_07->SetValue(B_CONTROL_OFF);}
		else {ThePrefs.mask_activated = OFF; ThePrefs.mask_mode=ON; button_b_07->SetValue(B_CONTROL_ON);}
		
		if (shared->act_img!=NULL) 
	   	{
   		  	tmp = new BMessage(UPDATE_ME);
			tmp->AddRect("zone",BRect(0,0,shared->act_img->pix_per_line-1,shared->act_img->pix_per_row-1));
			util.mainWin->PostMessage(tmp); 
   	 	}
   	 	util.mainWin->PostMessage(UPDATE_TITLE); 
   	 	break; 	 
		
		case SET_MASK_BUTTON_OFF:		
		button_b_07->SetValue(B_CONTROL_OFF);
		break;
		
		case BUTTON_B_08_MSG:
	   	DeselectAll();  button_b_08->SetValue(B_CONTROL_ON);
   	 	shared->active_tool=MOVE;
   	 	break;
   	 	 
   	 	 
   	 	case SET_FULL_NONE:
   	 	full_none->SetValue(B_CONTROL_ON);
   	 	full_half->SetValue(B_CONTROL_OFF);
   	 	full_total->SetValue(B_CONTROL_OFF);
   	 	util.mainWin->PostMessage(SET_FULL_NONE);
   	 	break;
   	 	
   	 	
   	 	case SET_FULL_HALF:
   	 	full_none->SetValue(B_CONTROL_OFF);
   	 	full_half->SetValue(B_CONTROL_ON);
   	 	full_total->SetValue(B_CONTROL_OFF);
   	 	util.mainWin->PostMessage(SET_FULL_HALF);
   	 	break;
   	 	
   	 	
   	 	case SET_FULL_TOTAL:
   	 	full_none->SetValue(B_CONTROL_OFF);
   	 	full_half->SetValue(B_CONTROL_OFF);
   	 	full_total->SetValue(B_CONTROL_ON);
   	 	util.mainWin->PostMessage(SET_FULL_TOTAL);
   	 	break;
   	 	
   	 	
   	 	case COL_SELECTED:
   	 	fore_view->SetViewColor(shared->fore_color); 	back_view->SetViewColor(shared->back_color);
		fore_view->Invalidate();	back_view->Invalidate();
		if (ThePrefs.info_win_open==true) util.infoWin->PostMessage(COL_CHANGED); //pour restaurer col si on sort du picker

		break;

		case FORE_CLICKED:
   	 	fore_view->SetViewColor(shared->fore_color);
		rect = ThePrefs.fore_color_frame;
		
		if (ThePrefs.fore_selector_open==true) 
		{
			foreWindow->Lock(); foreWindow->Close(); 
			ThePrefs.fore_selector_open=false; 
		}
		else
		{
		
			foreWindow = new ColWindow(ON,rect,Language.get("FOREGROUND"),shared); //OFF = mode fore
 			util.foreWin = foreWindow;
 			foreWindow->Show();
			ThePrefs.fore_selector_open=true;
 		}
 		
		break;

 		
 		
		break;

		case BACK_CLICKED:
   	 	back_view->SetViewColor(shared->back_color);
		rect = ThePrefs.back_color_frame;
		
		if (ThePrefs.back_selector_open==true) { backWindow->Lock(); backWindow->Close(); ThePrefs.back_selector_open=false; }
		else
		{
			backWindow = new ColWindow(OFF,rect,Language.get("BACKGROUND"),shared); //ON = mode back 
 			util.backWin = backWindow;
 			backWindow->Show();
 			ThePrefs.back_selector_open=true;
		}
		break;
		
		case BRUSH_CLICKED:
		util.mainWin->PostMessage(SHOW_BRUSH_WIN);
		break;
		
		case TEXTURE_CLICKED:
		util.mainWin->PostMessage(SHOW_PAPER_WIN);
		break;
		
		case BRUSH_CHANGED:
		brush_view->Invalidate();
		break;

		case PAPER_CHANGED:
		paper_view->Invalidate();
		break;


	default:
	BWindow::MessageReceived( msg);
   	 }

//envoie messge à la fenêtre mère
util.mainWin->PostMessage(new BMessage(TOOL_CHANGED)); 


}//fin message received


void ToolWindow::DeselectAll()
{ 

button_01->SetValue(B_CONTROL_OFF);
button_02->SetValue(B_CONTROL_OFF);
button_03->SetValue(B_CONTROL_OFF);
button_04->SetValue(B_CONTROL_OFF);
button_05->SetValue(B_CONTROL_OFF);
button_06->SetValue(B_CONTROL_OFF);
button_07->SetValue(B_CONTROL_OFF);
button_08->SetValue(B_CONTROL_OFF);
//pas les colors

button_b_01->SetValue(B_CONTROL_OFF);
button_b_02->SetValue(B_CONTROL_OFF);
button_b_03->SetValue(B_CONTROL_OFF);
button_b_04->SetValue(B_CONTROL_OFF);
button_b_05->SetValue(B_CONTROL_OFF);
button_b_06->SetValue(B_CONTROL_OFF);
//pas le mask
button_b_08->SetValue(B_CONTROL_OFF);
//pas les colors

}

ForeView::ForeView(BRect rec, share *sh) :
	BView(rec,NULL, B_FOLLOW_ALL, B_WILL_DRAW)
{
	shared=sh;
	SetViewColor(shared->fore_color);
	dragging=false;
	pressed=false;

}

BackView::BackView(BRect rec, share *sh) :
	BView(rec,NULL, B_FOLLOW_ALL, B_WILL_DRAW)
{
	shared=sh;
	SetViewColor(shared->back_color);
	dragging=false;
	pressed=false;
}

BrushView::BrushView(BRect rec, share *sh) :
	BView(rec, NULL, B_FOLLOW_ALL, B_WILL_DRAW)
{
	shared=sh;
	SetViewColor(216,216,216,216);
}

PaperView::PaperView(BRect rec, share *sh) :
	BView(rec,NULL, B_FOLLOW_ALL, B_WILL_DRAW)
{
	shared=sh;
	SetViewColor(216,216,216,216);
}

void BrushView::Draw(BRect update_rect)
{

	//DrawBitmap(new BBitmap(Bounds(),B_RGB32),shared->the_brush_24->Bounds(),Bounds()); //on efface l'ancienne (pour si plus grand)
	SetHighColor(255,255,255);
	FillRect(Bounds(),B_SOLID_HIGH); //on efface l'ancienne (pour si plus grand)


		if ((shared->the_brush_24->Bounds().Height() <  Bounds().Height() ) &&
			(shared->the_brush_24->Bounds().Width() <  Bounds().Width() )  )
		{
			 BPoint pt;
			 pt.x = (Bounds().Width() - shared->the_brush_24->Bounds().Width())/2;
 			 pt.y = (Bounds().Height() - shared->the_brush_24->Bounds().Height())/2;
			 DrawBitmap(shared->the_brush_24, BPoint(pt));
		}
		else  DrawBitmap(shared->the_brush_24,shared->the_brush_24->Bounds(),Bounds());

}

void PaperView::Draw(BRect update_rect)
{
	SetHighColor(255,255,255);
	FillRect(Bounds(),B_SOLID_HIGH); //on efface l'ancienne (pour si plus grand)

	DrawBitmap(shared->paper_24,BPoint(0,0));
}



void BrushView::MouseDown(BPoint point)
{
	Window()->PostMessage(new BMessage(BRUSH_CLICKED)); 

}

void PaperView::MouseDown(BPoint point)
{
	
uint32 buttons;
BPoint start,end;
GetMouse(&start, &buttons);

if (buttons==B_SECONDARY_MOUSE_BUTTON)
{ 
//pour que cette routine puisse fonctionner correctement faut que frameResized()
//ajuste les scrollbars...

//remet la taille possible des scrollbars au nécessaire
//dragging=true;


GetMouse(&start, &buttons);
BRect v_rect=Frame();
BPoint delta;

	//Bouton droit drag la win.
       do { 
       		GetMouse(&end, &buttons);
			if (start != end)
      	 		{

      	 		delta=start-end;      	 	

				//Pour EVITER BLOP quand on essaie de scroller out of limits...
				
				
				//si nég et barre tout en haut à gauche on scrolle pas...
				if (delta.x < 0 && parent_scroller->ScrollBar(B_HORIZONTAL)->Value() == 0)  delta.x=0;
				if (delta.y < 0 && parent_scroller->ScrollBar(B_VERTICAL)->Value()   == 0)  delta.y=0;


				//si pos
				if (delta.x > 0 && (parent_scroller->ScrollBar(B_HORIZONTAL)->Value() )   >= 
					shared->paper_24->Bounds().Width()- (v_rect.right )-1)  delta.x=0;
				
				if (delta.y > 0 && (parent_scroller->ScrollBar(B_VERTICAL)->Value() )  >= 
					shared->paper_24->Bounds().Height()-(v_rect.bottom ) -1) delta.y=0;

      	 		ScrollBy(delta.x,delta.y);
       			start=end;
				start.x += delta.x; //car quand on scrolle le BPoint change aussi!!!
				start.y += delta.y; //faut le redécaler d'autant
				//v_rect.PrintToStream();
				Draw(Bounds());

				}
		} while ( buttons ); 


	Invalidate();
	}
	else Window()->PostMessage(new BMessage(TEXTURE_CLICKED)); 
	
	
}

//----------------------------------------------------------------------------


void BackView::MessageReceived(BMessage *ms)
{
int32 r,g,b;

	switch(ms->what)
		{
			case SET_TO_COLOR:
			ms->FindInt32("r",&r);
			ms->FindInt32("g",&g);
			ms->FindInt32("b",&b);
			
			shared->back_color.red 	 = uint8 (r);
			shared->back_color.green = uint8 (g);
			shared->back_color.blue	 = uint8 (b);
			Window()->PostMessage(COL_SELECTED);
			break;
		}

}

void ForeView::MessageReceived(BMessage *ms)
{

int32 r,g,b;

	switch(ms->what)
		{
			case SET_TO_COLOR:
			ms->FindInt32("r",&r);
			ms->FindInt32("g",&g);
			ms->FindInt32("b",&b);
			
			shared->fore_color.red 	 = uint8 (r);
			shared->fore_color.green = uint8 (g);
			shared->fore_color.blue	 = uint8 (b);
			Window()->PostMessage(COL_SELECTED);
			break;
		}
}


void BackView::MouseUp(BPoint point)
{
	if (dragging == false) Window()->PostMessage(new BMessage(BACK_CLICKED)); 
}


void ForeView::MouseUp(BPoint point)
{
	if (dragging == false) 	Window()->PostMessage(new BMessage(FORE_CLICKED)); 
}


void BackView::MouseMoved(BPoint where, uint32 transit, const BMessage* dragDropMsg)
{
	if (pressed ==true && old_pos != where) { dragging = true; pressed = false; }
}

void ForeView::MouseMoved(BPoint where, uint32 transit, const BMessage* dragDropMsg)
{
	if (pressed ==true && old_pos != where) { dragging = true; pressed = false; }
}


void BackView::MouseDown(BPoint point)
{
	old_pos = point;
	dragging = false;
	pressed = true;
	 
	//drag
	BMessage message(SET_TO_COLOR);
	message.AddInt32("r",shared->back_color.red);
	message.AddInt32("g",shared->back_color.green);
	message.AddInt32("b",shared->back_color.blue);
	DragMessage(&message, Bounds()); 
}

void ForeView::MouseDown(BPoint point)
{
	old_pos = point;

	dragging = false;
	pressed = true;
	
	//drag
	BMessage message(SET_TO_COLOR);
	message.AddInt32("r",shared->fore_color.red);
	message.AddInt32("g",shared->fore_color.green);
	message.AddInt32("b",shared->fore_color.blue);
	DragMessage(&message, Bounds()); 
}

void ToolWindow::FrameMoved(BPoint screenPoint)
{
 //pour ramener au front la pic dessin
}