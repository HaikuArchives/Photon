#include "PaperWindow.h"

AllPaperView::AllPaperView(BRect r, share *sh) : 
	BView(r, "allPaperview", B_FOLLOW_ALL, B_WILL_DRAW)
{
shared = sh;
//------------------------

SetViewColor(216,216,216);

}

OnePaperView::~OnePaperView()
{
delete img;
}


OnePaperView::OnePaperView(const char *name, BRect r,  BBitmap *pic, share *sh, int32 le_id) : 
	BView(r, name, B_FOLLOW_NONE, B_WILL_DRAW)
{
shared = sh;
//------------------------
SetViewColor(216,216,216);

//dans le brush on resize image to shared->paper_sizexshared->paper_size mais ici on SCROLLE DEDANS


//img=pic;


char str[200];
	
if (pic!=NULL)
	{
		img= new BBitmap(pic); //le constructeur qui fait à partir d'un bbitmap requiert la r4.5
		sprintf(str,"%ld x %ld pixels", int32(img->Bounds().Width()), int32 ( img->Bounds().Height()));
	}
else { 
		img= new BBitmap(Bounds(),B_RGB32,true);
		sprintf(str,"ERROR loading this brush");
	 }
	 
	 
	bb_help.SetHelp(this, str);
	



id = le_id;
active=false;
dragging=false;

}


void OnePaperView::Draw(BRect update_rect)
{

	DrawBitmap(img,update_rect,update_rect);

	if (active==true && dragging==false)
	{ 	
		SetHighColor(0,0,200);
		SetPenSize(1);
		BRect rect = Bounds();
		rect.InsetBy(1,1);
		StrokeRect(rect, B_SOLID_HIGH);
	}
	
}		



void OnePaperView::MouseDown(BPoint point)
{
uint32 buttons;
BPoint start,end;
GetMouse(&start, &buttons);

if (buttons==B_SECONDARY_MOUSE_BUTTON)
{ 
//pour que cette routine puisse fonctionner correctement faut que frameResized()
//ajuste les scrollbars...

//remet la taille possible des scrollbars au nécessaire
dragging=true;

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
					img->Bounds().Width()- (v_rect.right )-1)  delta.x=0;
				
				if (delta.y > 0 && (parent_scroller->ScrollBar(B_VERTICAL)->Value() )  >= 
					img->Bounds().Height()-(v_rect.bottom ) -1) delta.y=0;

      	 		ScrollBy(delta.x,delta.y);
       			start=end;
				start.x += delta.x; //car quand on scrolle le BPoint change aussi!!!
				start.y += delta.y; //faut le redécaler d'autant
				//v_rect.PrintToStream();
				Draw(Bounds());

				}
		} while ( buttons ); 
dragging=false;

Invalidate();
 
}//fin right button (secondary button).

else //ou on active le papier
{	
	active=true;
	shared->current_paper = id;

	
	BMessage msg_x(ACTIVATE_PAPER);
	msg_x.AddInt32("num",id);
	util.mainWin->PostMessage(&msg_x);


	BMessage msg(DRAW_PAPERS);
	msg.AddInt32("active",id);
	Window()->PostMessage(&msg);
}

		
}


PaperWindow::PaperWindow(BRect frame, char *title, share *sh)
				: BWindow(frame, title, B_FLOATING_WINDOW,
				B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE| 
				B_NOT_H_RESIZABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS)
{
shared=sh;

//------------------------

shared->display_menu->ItemAt(1)->SetMarked(true);
ThePrefs.paper_selector_open=true;
util.paperWin = this;


BRect rect  = Bounds();
rect.right -= B_V_SCROLL_BAR_WIDTH;
paper_view = new AllPaperView(rect, shared);

paper_scroll_view = new BScrollView(Language.get("PAPERS"), paper_view, 
										    B_FOLLOW_ALL, B_WILL_DRAW, false,true);
 
/*
BTabView *tab_fond = new BTabView(Bounds(), "", B_WIDTH_AS_USUAL, B_FOLLOW_ALL, B_WILL_DRAW); 	
tab_fond->AddTab(set_view, new BTab()); 
tab_fond->AddTab(perso_scroll_view, new BTab()); 
AddChild(tab_fond);
*/


AddChild(paper_scroll_view);

SetSizeLimits(ThePrefs.paper_frame.Width(), ThePrefs.paper_frame.Width() , ThePrefs.paper_size, 3000);	


		//PostMessage(INIT_PERSO);
		paper_view->AddPaper();
		PostMessage(new BMessage(PAPER_TITLE));
		
FrameResized(Bounds().Width(),Bounds().Height()); //pour resize scroll barr
 //toujours la pic activée ...

}


PaperWindow::~PaperWindow()
{
	shared->display_menu->ItemAt(1)->SetMarked(false);
	ThePrefs.paper_selector_open = false;
	ThePrefs.paper_frame = Frame(); //in order to use it again later

}

void AllPaperView::AddPaper()
{
//On efface les anciens papers
while (ChildAt(0) !=NULL) ChildAt(0)->RemoveSelf();


	Window()->SetTitle(Language.get("LOADING_PAPERS"));
	char name[NAME_SIZE];
	char tmp[255];
	
	sprintf(name,util.dossier_app);
	strcat(name,"/papers/");
	
	//trouver nb de Paper dans le dossier
	BDirectory the_dir; 
	the_dir.SetTo(name);	
	last_paper = the_dir.CountEntries();
	//printf("\nnb Paper: %d", last_paper);

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
	strcat(name,"/papers/");
	strcat (name,tmp);

	
	
	sprintf(str,"ppview_%ld",i);
	
	tab_paper_views[i]= new OnePaperView(str, rect, util.load_bmp(name), shared,i);
	tab_paper_scroll_views[i] = new BScrollView("sc", tab_paper_views[i],
									B_FOLLOW_NONE, B_WILL_DRAW, true,true);
	
	tab_paper_views[i]->parent_scroller=tab_paper_scroll_views[i];

	sprintf(tab_paper_views[i]->name,tmp); //set name
	

//B_H_SCROLL_BAR_HEIGHT
//B_V_SCROLL_BAR_WIDTH
 
	float diff = tab_paper_views[i]->img->Bounds().Width() -ThePrefs.paper_size;
//	if (diff <0) diff=0;
	tab_paper_scroll_views[i]->ScrollBar(B_HORIZONTAL)->SetRange(0, diff); 

	diff = tab_paper_views[i]->img->Bounds().Height() -ThePrefs.paper_size ;
//	if (diff <0) diff=0;
	tab_paper_scroll_views[i]->ScrollBar(B_VERTICAL)->SetRange(0, diff);
	
	/*
	tab_paper_scroll_views[i]->ScrollBar(B_HORIZONTAL)->MoveBy(B_V_SCROLL_BAR_WIDTH,0); 
	tab_paper_scroll_views[i]->ScrollBar(B_VERTICAL)->MoveBy(0,B_H_SCROLL_BAR_HEIGHT); 
	*/
	
	tab_paper_scroll_views[i]->ScrollBar(B_HORIZONTAL)->Hide(); 
	tab_paper_scroll_views[i]->ScrollBar(B_VERTICAL)->Hide(); 
	
	
	col++; rect.OffsetBy(ThePrefs.paper_size,0); //horizontale
	if (col==3) { col=0; rect.OffsetBy(-3*ThePrefs.paper_size,ThePrefs.paper_size); }
	
	//AddChild(tab_paper_views[i]);
	AddChild(tab_paper_scroll_views[i]);
	
	} while (i++!= last_paper-1);
		

}


void PaperWindow::FrameResized(float x, float y)
{

	if (paper_view!=NULL)
	{
		int32 val =  int32 ((ceil(float(paper_view->last_paper)/3) * ThePrefs.paper_size) -y); // div par 3 et arrondi vers le haut
		if (val < 0) val=0;
		paper_scroll_view->ScrollBar(B_VERTICAL)->SetRange(0,val);
	}
	
}

void PaperWindow::MessageReceived(BMessage *msg)
{
int32 act;
int32 i;
char str[255];
	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;
		
		case DRAW_PAPERS:
		//clear old squares and draw the new one.
		i=0;		
		do {   
		
			paper_view->tab_paper_views[i]->active=false;
			paper_view->tab_paper_views[i]->Invalidate();
			paper_view->tab_paper_views[i]->Draw(paper_view->tab_paper_views[i]->Bounds());
			
		   } while(i++ != paper_view->last_paper-1);

		msg->FindInt32("active",&act);	
		paper_view->tab_paper_views[act]->active=true;
		paper_view->tab_paper_views[act]->Invalidate();
		paper_view->tab_paper_views[act]->Draw(paper_view->tab_paper_views[act]->Bounds());
		
		paper_view->Draw(Bounds());
		
		PostMessage(new BMessage(PAPER_TITLE));
		break;
		
		case PAPER_TITLE:
		sprintf(str, Language.get("PAPERS"));
		
			strcat(str," – ");
			strcat(str,	paper_view->tab_paper_views[shared->current_paper]->name);
			
		SetTitle(str);
		break;
		
		case INIT_PERSO:
		paper_view->AddPaper();
		PostMessage(new BMessage(PAPER_TITLE));
		break;
		
	default:
	BWindow::MessageReceived( msg );
   			   		   	
	 }
   	 	
}



void PaperWindow::FrameMoved(BPoint screenPoint)
{
	 //pour ramener au front la pic dessin
}
