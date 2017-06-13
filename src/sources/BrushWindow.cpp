#include "BrushWindow.h"

AllBrushesView::AllBrushesView(BRect r, share *sh) : 
	BView(r, "allbrushesview", B_FOLLOW_ALL, B_WILL_DRAW)
{
shared = sh;
//------------------------

SetViewColor(216,216,216);

}


OneBrushView::~OneBrushView()
{
	delete img;
}

OneBrushView::OneBrushView(const char *name, BRect r,  BBitmap *pic, share *sh, int32 le_id) : 
	BView(r, name, B_FOLLOW_NONE, B_WILL_DRAW)
{
shared = sh;
//------------------------
SetViewColor(216,216,216);

//resize image to shared->perso_brush_sizexshared->perso_brush_size!



//img=pic;


char str[200];
img= new BBitmap(Bounds(),B_RGB32,true);
	
if (pic!=NULL)
	{
		util.RescaleBitmap(pic,img,true);//center_if_smaller=true ne resize pas si pic est plus petit
		sprintf(str,"%ld x %ld pixels", int32(pic->Bounds().Width()), int32 ( pic->Bounds().Height()));
	}
else { 
		sprintf(str,"ERROR loading this brush");
	 }
	 
	bb_help.SetHelp(this, str);
	



id = le_id;
active=false;

}


void OneBrushView::Draw(BRect update_rect)
{

	DrawBitmap(img);

	if (active==true)
	{ 	
		SetHighColor(0,0,200);
		SetPenSize(1);
		BRect rect = Bounds();
		rect.InsetBy(1,1);
		StrokeRect(rect, B_SOLID_HIGH);
	}
	
	
}		



void OneBrushView::MouseDown(BPoint point)
{
	
	active=true;
	shared->current_perso_brush = id;

	BMessage msg_x(ACTIVATE_PERSO_BRUSH);
	msg_x.AddInt32("num",id);
	util.mainWin->PostMessage(&msg_x);

	BMessage msg(DRAW_PERSO_BRUSHES);
	msg.AddInt32("active",id);
	Window()->PostMessage(&msg);
		
}


BrushWindow::BrushWindow(BRect frame, char *title, share *sh)
				: BWindow(frame, title, B_FLOATING_WINDOW,B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE| 
				B_NOT_H_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
shared=sh;

//------------------------

shared->display_menu->ItemAt(0)->SetMarked(true);
ThePrefs.brush_selector_open=true;
util.brushWin = this;
 					

BRect rect  = Bounds();
rect.right -= B_V_SCROLL_BAR_WIDTH;
pr_br_view = new AllBrushesView(rect, shared);

perso_scroll_view = new BScrollView(Language.get("PERSO_BRUSHES"), pr_br_view, 
										    B_FOLLOW_ALL, B_WILL_DRAW, false,true);


BView *set_view = new BView(Bounds(),Language.get("BRUSHES"),B_FOLLOW_ALL, B_WILL_DRAW);
set_view->SetViewColor(216,216,216);


BTabView *tab_fond = new BTabView(Bounds(), "", B_WIDTH_AS_USUAL, B_FOLLOW_ALL, B_WILL_DRAW); 	
tab_fond->AddTab(set_view, new BTab()); 
tab_fond->AddTab(perso_scroll_view, new BTab()); 


AddChild(tab_fond);

//AddChild(perso_scroll_view);

SetSizeLimits(ThePrefs.brushes_frame.Width(),ThePrefs.brushes_frame.Width(), 
			  ThePrefs.perso_brush_size+tab_fond->TabHeight(), 3000);	


		pr_br_view->AddBrushes();
		PostMessage(new BMessage(BRUSH_TITLE));

FrameResized(Bounds().Width(),Bounds().Height()); //pour resize scroll barr

//PostMessage(INIT_PERSO);

}


BrushWindow::~BrushWindow()
{
	shared->display_menu->ItemAt(0)->SetMarked(false);
	ThePrefs.brush_selector_open = false;
	ThePrefs.brushes_frame = Frame(); //in order to use it again later
}

void AllBrushesView::AddBrushes()
{

//On efface les anciennes brushes
while (ChildAt(0) !=NULL) ChildAt(0)->RemoveSelf();

	Window()->SetTitle(Language.get("LOADING_BRUSHES"));
	char name[NAME_SIZE];
	char tmp[255];
	
	sprintf(name,util.dossier_app);
	strcat(name,"/brushes/");
	
	//trouver nb de brushes dans le dossier
	BDirectory the_dir; 
	the_dir.SetTo(name);	
	last_brush = the_dir.CountEntries();
//	printf("\nnb brushes: %d", last_brush);

	//pas dépasser les bornes!
	BEntry the_entry;	
	
	BRect rect;
	rect.Set(0,0,63,63);

	int16 col=0;
	char str[255];

    int32 i = 0;
	do { 
	
	the_dir.GetNextEntry(&the_entry,false);
	the_entry.GetName(tmp);
	
	
	//Faut le refaire
	sprintf(name,util.dossier_app);
	strcat(name,"/brushes/");
	strcat (name,tmp);


	
	sprintf(str,"brview_%ld",i);

	tab_brush_views[i]= new OneBrushView(str, rect, util.load_bmp(name), shared,i);
	
	sprintf(tab_brush_views[i]->name,tmp); //set name
	
	col++; rect.OffsetBy(ThePrefs.perso_brush_size,0); //horizontale
	if (col==3) { col=0; rect.OffsetBy(-3*ThePrefs.perso_brush_size,ThePrefs.perso_brush_size); }
	
	AddChild(tab_brush_views[i]);
	
	} while (i++!= last_brush-1);
		

}


void BrushWindow::FrameResized(float x, float y)
{

	if (pr_br_view!=NULL)
	{
		int32 val =  (int32) ( (ceil(float(pr_br_view->last_brush)/3) *ThePrefs.perso_brush_size) -y); // div par 3 et arrondi vers le haut
		if (val < 0) val=0;
		perso_scroll_view->ScrollBar(B_VERTICAL)->SetRange(0,val);
	}
	
}

void BrushWindow::MessageReceived(BMessage *msg)
{
int32 act;
int32 i;
char str[255];
	switch (msg->what)
	{
	
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;
		
		case DRAW_PERSO_BRUSHES:
		//clear old squares and draw the new one.
		i=0;		
		do {   
		
			pr_br_view->tab_brush_views[i]->active=false;
			pr_br_view->tab_brush_views[i]->Invalidate();
			pr_br_view->tab_brush_views[i]->Draw(pr_br_view->tab_brush_views[i]->Bounds());
			
		   } while(i++ != pr_br_view->last_brush-1);

		msg->FindInt32("active",&act);	
		
		if (act!=TEMP_BRUSH_VALUE)
		{
		pr_br_view->tab_brush_views[act]->active=true;
		pr_br_view->tab_brush_views[act]->Invalidate();
		pr_br_view->tab_brush_views[act]->Draw(pr_br_view->tab_brush_views[act]->Bounds());
		}
		pr_br_view->Draw(Bounds());
		
		PostMessage(new BMessage(BRUSH_TITLE));
		break;
		
		case BRUSH_TITLE:
		sprintf(str, Language.get("BRUSHES"));

		
		if (shared->brush_is_perso==true)
			{
				strcat(str," – ");
				if (act!=TEMP_BRUSH_VALUE)
					strcat(str,	pr_br_view->tab_brush_views[shared->current_perso_brush]->name);
				else strcat(str,	Language.get("TEMP_BRUSH"));
			}
			
		SetTitle(str);
		break;
		
		case INIT_PERSO:
		pr_br_view->AddBrushes();
		PostMessage(new BMessage(BRUSH_TITLE));
		break;
		
	default:
	BWindow::MessageReceived( msg );
   			   		   	
	 }
   	 	
}



void BrushWindow::FrameMoved(BPoint screenPoint)
{
	 //pour ramener au front la pic dessin
}
