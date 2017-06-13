#include "PrefWindow.h"


LangView::LangView(BRect r, share *sh) : 
	BView(r, Language.get("LANGUAGE"), B_FOLLOW_ALL, B_WILL_DRAW)
{
shared = sh;
//------------------------

SetViewColor(216,216,216);

BRect rect = Bounds();
rect.right-= B_V_SCROLL_BAR_WIDTH+1;
rect.bottom-= B_H_SCROLL_BAR_HEIGHT+1;
rect.bottom-=8; //comprends pas mais bon...

rect.InsetBy(8,8);
rect.bottom-=24;

   lang_list = new BListView(rect, "", B_SINGLE_SELECTION_LIST); 
   lang_list->SetSelectionMessage(new BMessage(PREFS_CHANGED));

   AddLanguages();

   	AddChild(new BScrollView("", lang_list, B_FOLLOW_ALL, 0, false, true));

rect.bottom+=24+4;
rect.top = rect.bottom-24;

BTextView* warn = new BTextView(rect,"",rect, B_FOLLOW_ALL, B_WILL_DRAW);
warn->SetTextRect(warn->Bounds());
warn->MakeEditable(false);
warn->MakeSelectable(false);
warn->Insert(Language.get("CHANGES_ON_NEXT_LAUNCH"));
warn->SetViewColor(216,216,216);
AddChild(warn);

}


void LangView::AddLanguages()
{

	lang_list->MakeEmpty();
	
	char name[NAME_SIZE];
	char tmp[255];
	
	sprintf(name,util.dossier_app);
	strcat(name,"/languages/");
	
	//trouver nb de filez dans le dossier
	BDirectory the_dir; 
	the_dir.SetTo(name);	
	int32 last_lang = the_dir.CountEntries();

	//pas dépasser les bornes!
	BEntry the_entry;	
	
	BRect rect;
	rect.Set(0,0,63,63);



	
	lang_list ->AddItem( new BStringItem("English"));
	
    int32 i = 0;
	do { 
	
	the_dir.GetNextEntry(&the_entry,false);
	the_entry.GetName(tmp);

	lang_list ->AddItem( new BStringItem(tmp));
	
	/*
	BMessage ms(PREFS_CHANGED);
	ms.AddString("lang",tmp);
	itm.SetSelectionMessage(&ms);
	*/
	
	} while (i++!= last_lang-1);
		

}



PrefView::PrefView(BRect r, share *sh) : 
	BView(r, Language.get("MAIN"), B_FOLLOW_ALL, B_WILL_DRAW)
{
shared = sh;
//------------------------

SetViewColor(216,216,216);


BMenuItem *it;
BRect rect(4,4,4+200,4+16);

BMenu *mode_menu = new BMenu("");
mode_menu->AddItem(new BMenuItem(Language.get("CENTIMETERS"),   new BMessage(PREFS_CHANGED)));
mode_menu->AddItem(new BMenuItem(Language.get("MILLIMETERS"),    new BMessage(PREFS_CHANGED)));
mode_menu->AddItem(it = new BMenuItem(Language.get("INCHES"), new BMessage(PREFS_CHANGED)));
mode_menu->AddItem(new BMenuItem("pixels", new BMessage(PREFS_CHANGED))); //pixels cannot be translated in any language...

it->SetMarked(true);

mode_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
units = new BMenuField(rect,"",Language.get("UNITS"),mode_menu, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW);
mode_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
AddChild(units);

//DPI Dots/cm

rect.OffsetBy(0,24);
BMenuItem *xit;
BMenu *res_menu = new BMenu("");

char str_a[256];
char str_b[256];
sprintf(str_a, "pixels/"); strcat(str_a,Language.get("INCH")); 		 strcat(str_a," (dpi)"); 
sprintf(str_b, "pixels/"); strcat(str_b,Language.get("CENTIMETER")); strcat(str_b," (dpcm)");

res_menu->AddItem(xit = new BMenuItem(str_a, new BMessage(PREFS_CHANGED)));
res_menu->AddItem(new BMenuItem(str_b, new BMessage(PREFS_CHANGED)));

xit->SetMarked(true);

res_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
res_type = new BMenuField(rect,"",Language.get("RES_UNITS"),res_menu, B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW);
res_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
AddChild(res_type);



}




PrefWindow::PrefWindow(BRect frame, char *title, share *sh)
				: BWindow(frame, title, B_FLOATING_WINDOW,
				B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE| B_NOT_RESIZABLE)
{
shared=sh;

//------------------------


BRect rect  = Bounds();
///rect.OffsetBy(0,x.TabHeight());



BTabView x(rect,"", B_WIDTH_AS_USUAL, B_FOLLOW_ALL, B_WILL_DRAW); //nécessaire pour avoir la taille par défaut

////rect.bottom-= x.TabHeight();

pref_view = new PrefView(rect, shared);
lang_view = new LangView(rect, shared);


BTabView *tab_fond = new BTabView(rect, "", B_WIDTH_AS_USUAL, B_FOLLOW_ALL, B_WILL_DRAW); 	

//on ajoute les tabs...
tab_fond->AddTab(pref_view, new BTab()); 
tab_fond->AddTab(lang_view, new BTab()); 


//tab_fond->AddTab(perso_scroll_view, new BTab()); 

AddChild(tab_fond);


//AddChild(perso_scroll_view);

ThePrefs.pref_win_open  = true;

LoadPrefs();
}


PrefWindow::~PrefWindow()
{
	ThePrefs.pref_win_open = false;
	ThePrefs.pref_frame = Frame(); //in order to use it again later

	SavePrefs(); //before closing

}

//------------------------------------------------------------------------------------------------

void PrefWindow::SavePrefs()
{


BAlert *alert;
//alert = new BAlert("",Language.get("NEED_ONE_LAYER"),"Oops"); 
alert = new BAlert("","Saving Prefs...","Oops"); 
alert->Go(NULL);


	
	//------------------------------- COPY Settings into Memory----------------------------------//	
		
	//UNITS
	ThePrefs.pref_units = pref_view->units->Menu()->IndexOf(pref_view->units->Menu()->FindMarked());   

	//RES_type
	ThePrefs.pref_res_type = pref_view->res_type->Menu()->IndexOf(pref_view->res_type->Menu()->FindMarked());   

	//language (attention on sauve une string avec le nom et non pas le numéro de l'index!
	//comme ça quand on relit les prefs il y a pas de prob s'il y a de nouveaux filez..
	BListView* ls = lang_view->lang_list;
//	BListItem *sti = ls->ItemAt( ls->CurrentSelection() );

	//we need a cast...
	BStringItem *ptr =	(BStringItem*)(ls->ItemAt(ls->CurrentSelection()));
	if (ptr!=NULL)	sprintf(ThePrefs.lang_name, ptr->Text());
	else beep();
	
	//----------------------------------------------------------------
	
	ThePrefs.Save();

alert->Lock();
alert->Close();


}


//------------------------------------------------------------------------------------------------

void PrefWindow::LoadPrefs()
{

	//pas besoin de loader, c'est déjà fait au démarrage..
	//ThePrefs.Load();
	
	//----------------------- UPDATE DISPLAY TO REFLECT SETTINGS LOADED---------------------//	

	//units
	pref_view->units->Menu()->ItemAt(ThePrefs.pref_units)->SetMarked(true);
	pref_view->units->Menu()->SetLabelFromMarked(true); //pour mettre nom actif en titre
		
	//res_tpye
	pref_view->res_type->Menu()->ItemAt(ThePrefs.pref_res_type)->SetMarked(true);
	pref_view->res_type->Menu()->SetLabelFromMarked(true); //pour mettre nom actif en titre
		
	//language
	
	BListView* ls = lang_view->lang_list;
	int32 i = 0;	int32 total = ls->CountItems()+1;
	BStringItem *ptr ;
	while (i!=total)
	{
		ptr =	(BStringItem*)(ls->ItemAt(i));
		if (ptr!=NULL)
		{
			if ( strcmp(ThePrefs.lang_name, ptr->Text())==0)
				{
				ls->ItemAt(i)->Select(); 
				//ls->Select(i, false); //(false = on déselectionne les autres
				ls->ScrollToSelection();
				}
		}
		i++;
	}
	

	//----------------------------------------------------------------------------//	
	
}
	
void PrefWindow::MessageReceived(BMessage *msg)
{
	

	
	switch (msg->what)
	{	
	
	case PREFS_CHANGED:
	//SavePrefs();
	beep();
	break;
	

	default:
	BWindow::MessageReceived( msg );
   			   		   	
	 }
   	 	
}



void PrefWindow::FrameMoved(BPoint screenPoint)
{
	 //pour ramener au front la pic dessin
}
