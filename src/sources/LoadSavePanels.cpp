#include "LoadSavePanels.h"
#include <Button.h>

PhotonOpenPanel::PhotonOpenPanel(share *sh):
	 BFilePanel(B_OPEN_PANEL,NULL,NULL,B_FILE_NODE,true,NULL,NULL,false,true)
{
	shared = sh ;	

	SetPanelDirectory(util.dossier_app);
	SetButtonLabel(B_DEFAULT_BUTTON,Language.get("OPEN"));
	SetButtonLabel(B_CANCEL_BUTTON,Language.get("CANCEL"));

	
	Window()->SetTitle(Language.get("OPEN"));


changed_by_thumb=false;


  		Window()->Lock();
  		BView *background = Window()->ChildAt(0); //trouve view backgd c'est la premiere (0)

//Thumbnail size is 96 x 96 MAX

  		Window()->ResizeTo(350+16+((96+8)*3),Window()->Bounds().Height());
		background->ResizeTo(350,background->Bounds().Height()); //un peu de place en haut... pour la Preview
		
		//faut la recentrer après l'agrandissement
		BRect x = util.find_win_pos(350+16+((96+8)*3),Window()->Bounds().Height(),util.mainWin);
		Window()->MoveTo(x.left,x.top);


		//-B_V_SCROLL_BAR_WIDTH
		
		BRect rect = background->Frame();
		rect.left = background->Frame().right+1;
		rect.right = rect.left+ ((96+8)*3);
		
  		fond_all_thumb = new BView(rect,"", B_FOLLOW_ALL, B_WILL_DRAW);
		fond_all_thumb->SetViewColor(216,216,216);
		scv = new BScrollView("th_sc_view", fond_all_thumb, B_FOLLOW_RIGHT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW,false,true);
		

		Window()->AddChild(scv);

		BRect r;
		r.Set(180,18,180+100,18+12);
		check_thumb = new BCheckBox(r,"check_th",Language.get("SHOW_THUMBNAIL"),new BMessage(SHOW_THUMB_CHANGED),B_FOLLOW_TOP | B_FOLLOW_LEFT,B_WILL_DRAW);
		check_thumb->SetValue(ThePrefs.show_thumbs);		//par défaut save_with_thumb = B_CONTROL_ON; //par défaut on sauve le thumb
		check_thumb->SetTarget(util.mainWin);
		background->AddChild(check_thumb);
		
		r.OffsetBy(0,15);
		check_only_images = new BCheckBox(r,"check_th",Language.get("ONLY_IMG"),new BMessage(ONLY_IMG_CHANGED),B_FOLLOW_TOP | B_FOLLOW_LEFT,B_WILL_DRAW);
		check_only_images->SetValue(ThePrefs.show_only_images);		//par défaut save_with_thumb = B_CONTROL_ON; //par défaut on sauve le thumb
		check_only_images->SetTarget(util.mainWin);
		background->AddChild(check_only_images);
		
		r.left =4;
		r.right = 140;
		r.bottom = background->Bounds().bottom-14;
		r.top = r.bottom-20;
		
		BButton *create_thumbnail_button = new BButton(r,"crt",Language.get("CREATE_THUMBNAIL"), new BMessage(CREATE_THUMBS),B_FOLLOW_BOTTOM | B_FOLLOW_LEFT,B_WILL_DRAW);
		create_thumbnail_button->SetTarget(util.mainWin);
		background->AddChild(create_thumbnail_button);

  		background->Invalidate();
//		background->Draw(background->Bounds());
		Window()->Unlock();
  		
		
SetRefFilter(new ImgRefFilter(shared));
thumb_amount=-1;

}			


		 
PhotonSavePanel::PhotonSavePanel(share *sh):
	 BFilePanel(B_SAVE_PANEL,NULL,NULL,B_FILE_NODE,false,NULL,NULL,false,true)
{

shared=sh;

//----------------------------

		 SetPanelDirectory(util.dossier_app);
		 Window()->SetTitle(Language.get("SAVE_AS"));
		 SetButtonLabel(B_DEFAULT_BUTTON,Language.get("SAVE"));
		 SetButtonLabel(B_CANCEL_BUTTON,Language.get("CANCEL"));

ThePrefs.no_pictures_left=ON; //pour stop display
shared->thumbnail = new DThumbnail(shared->act_img->display_bitmap, true); // the thumbnail will be dithered 
ThePrefs.no_pictures_left=OFF; //pour réactiver affich...

float height=0;
//DISABLED FOR POWERPC (PPC) since thumbnail doesn't work?
height = shared->thumbnail->Bitmap()->Bounds().Height()+4;

if (height < 74) height=74; //pour afficher les infos textes.. 
							//si la pic est plus petite que la taille du thumbnail
							//la thumbnaillib le met à sa taille ori


  		Window()->Lock();
  		BView *background = Window()->ChildAt(0); //trouve view backgd c'est la premiere (0)

		background->ResizeBy(0,height*-1); //un peu de place en haut... pour la Preview
		background->MoveBy(0,height);
		
		BRect limit = background->Bounds();
		limit.bottom=height;
		fond_thumb = new ThumbView(limit,shared);
  		Window()->AddChild(fond_thumb);
		
//		Window()->Bounds().bottom+=200;
  		
	

	BMenuField		*format;  
	BMenu			*format_menu;
	BMenuItem *it;


	format_menu = new BMenu("Img Format");

//---------------------------------

		util.trans_roster = BTranslatorRoster::Default(); 
   		int32 num_translators, i; 
   		translator_id *trans = util.translators; 
   		const char *trans_name, *trans_info; 
   		int32 trans_version; 
   
		BMessage *the_msg;
//-------------------------------
		const translation_format * output_formats = NULL;
		int32 num_output = 0;
		


		
   		util.trans_roster->GetAllTranslators(&trans,&num_translators); 
		bool first_time = true;
   		for (i=0;i!=num_translators+1;i++)
   		{ 
      	util.trans_roster->GetTranslatorInfo(trans[i],&trans_name, &trans_info, &trans_version); 
		util.trans_roster->GetOutputFormats(trans[i],&output_formats, &num_output);
			
		
		the_msg = new BMessage(TRANSLATOR_SELECTED); 
	    the_msg->AddInt32("id",i);      //dans le message il y a pas l'id du translator mais le numéro dans la liste
		
     		if (first_time==true) 	//pour pouvoir selecter le premier avec SetMarked et selecter son format dans shared->active_translator
	 			{
	 			format_menu->AddItem(it = new BMenuItem(trans_name, the_msg));
    		 	util.mainWin->PostMessage(the_msg); 
    			it->SetMarked(true); //select le premier
				format_menu->SetLabelFromMarked(true); //pour mettre nom actif en titre
				first_time=false;
    		 	}
    	 	else format_menu->AddItem(new BMenuItem(trans_name, the_msg));
		}      	
      	
      	//printf("%s: %s (%.2f)\\n", translator_name, translator_info,translator_version/100.); 
			

	format_menu->SetTargetForItems(util.mainWin); //FAIT QUE LES MESSAGES sont envoyés à MAINWIN et pas à la win save...
	

	BRect rect;
	//je me base sur les coordonnées de ces view pour trouver ou caser mon menu déroulant
	BRect taille=background->Bounds();
	float x_center=taille.right-100;
	rect.Set(x_center,   taille.top+20,
			 x_center+96,    taille.top+20+12  );
	rect.OffsetBy(-20,2);	
	format = new BMenuField(rect,"",NULL,format_menu,B_FOLLOW_TOP | B_FOLLOW_RIGHT ,B_WILL_DRAW);
	background->AddChild(format);
	fond_thumb->UpdateText();
	SetSaveText(shared->act_img->name);
	Window()->Unlock();

SetRefFilter(new ImgRefFilter(shared));

}//fin SavePanel


//--------------

OneThumbView::OneThumbView(const char *n, BRect r,  BBitmap *pic, share *sh, int32 le_id, entry_ref the_ref) : 
	BView(r, n, B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
shared = sh;

//-----------------
SetViewColor(216,216,216);
img=pic;
//img=new BBitmap(BRect(0,0,22,22),B_RGB32);

id = le_id;
active=false;
previous_active=true; //pour forcer l'update la première fois...
ref = the_ref;
sprintf(the_name,n);
bb_help.SetHelp(this,the_name);
}



OneThumbView::~OneThumbView()
{
	delete img;
}

void OneThumbView::MouseDown(BPoint point)
{

	//on efface pas les ancien si on appuie sur shift
	if (modifiers() & B_SHIFT_KEY) { }
				else  util.mainWin->PostMessage(DEACTIVATE_THUMB_IMG);
		
	previous_active=active;			
	if (active==false)	active = true;
	else active = false;

	util.mainWin->PostMessage(DONE_SELECTING_THUMB_IMG);
	util.mainWin->PostMessage(CHANGED_BY_THUMB_TRUE);
	
	//-------------------------------------------------------------------------------------------------------
	//apppliquer la modif de sélection à la liste Tracker
	BMessage selector(B_SET_PROPERTY);
	selector.AddSpecifier("Selection");
	selector.AddSpecifier("Poses");
	selector.AddSpecifier("Window", Window()->Title());
	selector.AddRef("data", &ref);
	
	BMessenger ph(be_app);
	ph.SendMessage(&selector);	
	
}

void OneThumbView::Draw(BRect update_rect)
{
	BPoint pt(0,0);
	

	pt.x = (Bounds().Width()-img->Bounds().Width())/2;
	pt.y = (Bounds().Height()-img->Bounds().Height())/2;
	

	if (img!=NULL) DrawBitmap(img,pt);

	if (active==true)
	{ 	

	   SetDrawingMode(B_OP_ALPHA); 
       SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_OVERLAY); 
       rgb_color color; 
       color.red = 0; color.green = 0; color.blue = 255; 
       color.alpha = 64; //presque tout transparent
       SetHighColor(color); 
       BRect r = Bounds();
       r.InsetBy(2,2);
       FillRect(r); 
	   SetDrawingMode(B_OP_COPY); 
    }
	
	
}




void PhotonOpenPanel::CreateThumbs()
{



		
	entry_ref active_ref;
	GetPanelDirectory(&active_ref);
	 
//-----------------------------------------------------------------------------------------

	BRect rect;
	BDirectory the_dir;
	BEntry the_entry(&active_ref);
	BPath patha;
	
	the_entry.GetPath(&patha);
	the_dir.SetTo(patha.Path());
	
	char tmp[B_FILE_NAME_LENGTH];
	char name[NAME_SIZE];
	
	int32 count = the_dir.CountEntries();
	the_dir.Rewind();

 	BWindow *prog_win = new BWindow(util.find_win_pos(200,32,util.mainWin),Language.get("CREATE_THUMBNAIL"),B_FLOATING_WINDOW, B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_NOT_RESIZABLE,B_CURRENT_WORKSPACE);
	BView *back = new BView(prog_win->Bounds(),"",B_FOLLOW_ALL,B_WILL_DRAW);
	back->SetViewColor(216,216,216);

	//Faut avoir une vue de fond pour que le texte en prenne la couleur...

	prog_win->AddChild(back);
	
	BStatusBar *stat = new BStatusBar(prog_win->Bounds(),NULL,NULL,NULL);
	stat->SetMaxValue(count-2);
	stat->SetViewColor(216,216,216);
	back->AddChild(stat);
	
	prog_win->Show();
	
	rect.Set(0,0,96+8,96+8);
	BPath path;
	
	char aaa[256];
	char bbb[256];
	int32 i = 0;
	do { 
	
	the_dir.GetNextEntry(&the_entry,false);
	the_entry.GetName(tmp);
	the_entry.GetPath(&path);
	
	sprintf(name,path.Path());
	
	BBitmap *img;
	DThumbnail *t;
  	BNode node(&the_entry); 
	BNodeInfo node_info(&node);
	char type[1024];
	node_info.GetType(type);
	BString s(type);

	if (s.FindFirst("image") >-1)
	{
		if ((img = util.FetchBitmap(name, B_READ_ONLY) )!=NULL )
		{
		t = new DThumbnail(img, true); // the thumbnail will be dithered 
	
		if (t->Bitmap())    // will be NULL if the thumbnail couldn't be created 
			{ 
   	 			t->WriteThumbnailAttribute(&node); 
   	 			t->WriteIconAttribute(&node); 
   	 			t->WriteMiniIconAttribute(&node); 
    			t->WriteResolutionAttributes(&node); 
			} 
		delete img;	
		}
	}
			sprintf(aaa,"%ld",i);	
			sprintf(bbb,"(");	strcat(bbb,tmp);  strcat(bbb,")");
			
			prog_win->Lock();		
			stat->Update(1, tmp, aaa);
			prog_win->Unlock();		
	
	
	} while (i++ < count-1);

	prog_win->Lock();//nécessaire avant de fermer
	prog_win->Close();

UpdateThumbs();
		
}

void PhotonOpenPanel::UpdateThumbs()
{

	return;

Window()->Lock();

//On efface les anciens thumbs
	int32 x=0;

		if (thumb_amount > -1)
		{
		while (x != thumb_amount)
			{	
 				if (tab_th_views[x]!=NULL) 
 					{
 						if (tab_th_views[x]->RemoveSelf());
 							delete tab_th_views[x];
 						
 					}
				x++; 
			} 
		}

//printf("\nTHumbamount: %ld / %ld \n",thumb_amount,x);


thumb_amount=-1;

Window()->Unlock();



//de toute façons on les enlève et on refait les nouveaux que si la cache est cochée
//ou alors si le cocher est casé mais enfin bref...		
		
if (ThePrefs.show_thumbs==B_CONTROL_ON)
{
		
	entry_ref active_ref;
	GetPanelDirectory(&active_ref);
	 
//-----------------------------------------------------------------------------------------

	BRect rect;
	BDirectory the_dir;
	BEntry the_entry(&active_ref);
	BPath patha;
	
	the_entry.GetPath(&patha);
	the_dir.SetTo(patha.Path());
	
	int32 col=0;
	char tmp[256];
	char aaa[256];
	char name[NAME_SIZE];
	
	int32 count = the_dir.CountEntries();
	the_dir.Rewind();

 	BWindow *prog_win = new BWindow(util.find_win_pos(200,32,util.mainWin),Language.get("LOADING_THUMB"),B_FLOATING_WINDOW, B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_NOT_RESIZABLE,B_CURRENT_WORKSPACE);
	BView *back = new BView(prog_win->Bounds(),"",B_FOLLOW_ALL,B_WILL_DRAW);
	back->SetViewColor(216,216,216);

	//Faut avoir une vue de fond pour que le texte en prenne la couleur...

	prog_win->AddChild(back);
	
	BStatusBar *stat = new BStatusBar(prog_win->Bounds(),NULL,NULL,NULL);
	stat->SetMaxValue(count-2);
	stat->SetViewColor(216,216,216);
	back->AddChild(stat);
	
	prog_win->Show();
	
	rect.Set(0,0,96+8,96+8);
	
	int32 i = 0;
	entry_ref temp_ref;
	
	do { 
	
	the_dir.GetNextEntry(&the_entry,false);
	the_entry.GetName(tmp);
	the_entry.GetRef(&temp_ref);
	sprintf(name,tmp);
//	printf("\n"); 	printf(name);
	
	BNode node(&the_entry);

	DThumbnail *t = new DThumbnail();
	
	if (t->ReadFromAttributes(&node)==B_OK
		&& i<MAX_THUMBS) //si on test pas c'est le crash direct
		{
	
			tab_th_views[i]= new OneThumbView(tmp, rect, (BBitmap*)t->Bitmap(), shared,i,temp_ref);

			Window()->Lock();
			if (fond_all_thumb!=NULL) fond_all_thumb->AddChild(tab_th_views[i]);
			Window()->Unlock();
		
			thumb_amount=i;


			col++; rect.OffsetBy(96+8,0); //horizontale
			if (col==3) { col=0; rect.OffsetBy(-3*(96+8),96+8); }
	
	
		}

			sprintf(aaa,"%ld",i);	
			prog_win->Lock();		
			stat->Update(1, Language.get("PLEASE_WAIT"), aaa);
			prog_win->Unlock();		


	} while (i++ < count -1);

	prog_win->Lock();
	prog_win->Close();
	
	
	
			Window()->Lock();
	if (scv!=NULL) 
		{
		int32 val =  (int32) ( (ceil(float(count)/3) *(96+8)) -Window()->Bounds().Height()); // div par 3 et arrondi vers le haut
		if (val < 0) val=0;
		BScrollBar *t=	scv->ScrollBar(B_VERTICAL);
		if (t!=NULL) t->SetRange(0,val);
	
		}
			Window()->Unlock();
		
  }//in if checkbox on



}

//-----

void PhotonOpenPanel::ClearThumbSelection()
{

		if (thumb_amount>-1)
		{
			for (int32 i = 0;  i!=thumb_amount+1; i++)  
			{
				tab_th_views[i]->previous_active = tab_th_views[i]->active;
				tab_th_views[i]->active=false;
			}
		}
	
}
void PhotonOpenPanel::SelectFromThumb(int32 number)
{

		be_app->Lock();
		Window()->Lock();
		//clear old squares and draw the new one.

		tab_th_views[number]->previous_active = tab_th_views[number]->active;
		tab_th_views[number]->active = true;
		
		Window()->Unlock();
		be_app->Unlock();
}

void PhotonOpenPanel::SelectionDone()
{

		if (thumb_amount>-1)
		{
			for (int32 i = 0;  i!=thumb_amount+1; i++) 
			{
				Window()->Lock();
				
				//on update que ceux qui ont été modfiées
				if (tab_th_views[i]->active!=tab_th_views[i]->previous_active) 
				tab_th_views[i]->Invalidate();
				Window()->Unlock();
			}
		}
	
}




void PhotonOpenPanel::SelectionChanged()
{
	GetPanelDirectory(&the_active_ref);
	if (the_active_ref!=old_dir_ref) UpdateThumbs(); //seulement si on changé de dir
	GetPanelDirectory(&old_dir_ref);

	 
 	if (changed_by_thumb==false) {

	
	
	util.mainWin->PostMessage(DEACTIVATE_THUMB_IMG);
					
	entry_ref tmp_ref;
	
	Rewind();
	while (GetNextSelectedRef(&tmp_ref) == B_OK)
	{   
	
		if (thumb_amount>-1)
		{
			for (int32 i = 0;  i!=thumb_amount+1; i++)
			{

				if (tab_th_views[i]->ref == tmp_ref)
				{
					tab_th_views[i]->previous_active = tab_th_views[i]->active;
					tab_th_views[i]->active=true;
				}

			}
		}//fin if y a des thumbs 

	}//fin des refs

	util.mainWin->PostMessage(DONE_SELECTING_THUMB_IMG);
	}//fin if changed by thumb = false
	
	changed_by_thumb=false;

}

ImgRefFilter::ImgRefFilter(share *sh)  :  BRefFilter() 
{ 
	shared = sh ;	
} 

bool ImgRefFilter::Filter(const entry_ref *ref, BNode *node, struct stat *st, const char *filetype)
{

BString s(filetype);
BString name(ref->name);
name.ToLower();	//so that we don't have to check for extensions in both modes
	
	if (
	ThePrefs.show_only_images==true) //checkbox à rajouter!
	{
	 
			
	if ( s.FindFirst("x-vnd.Be-directory") < 0
		 && s.FindFirst("x-vnd.Be-volume") < 0
		 && s.FindFirst("x-vnd.Be-symlink") < 0
		) //seulement si pas directory ou Volume
		{	
			bool is_valid=false;
			if (s.FindFirst("image") >-1) is_valid = true;
			if (s.FindFirst("video") >-1)  is_valid = true;
		
			//if not on a Be filesystem (specially for Fat) 
			//or in case the files weren't "identified "
			//we should check for extensions
			if (
				name.FindFirst("tga")	> -1
				|| name.FindFirst("jpeg")	> -1
				|| name.FindFirst("tif")	> -1
				|| name.FindFirst("tiff")	> -1
				|| name.FindFirst("jpg")	> -1
				|| name.FindFirst("pcx")	> -1
				|| name.FindFirst("png")	> -1
				|| name.FindFirst("ppm")	> -1
				|| name.FindFirst("gif")	> -1
				|| name.FindFirst("bmp")	> -1
				|| name.FindFirst("iff")	> -1
				|| name.FindFirst("lbm")	> -1
				|| name.FindFirst("eps")	> -1
				|| name.FindFirst("pict")	> -1
				|| name.FindFirst("pct")	> -1

				
				) is_valid=true; 		
				
			
			if (is_valid ==false) return is_valid; //only if false!
		}
	}


	//par défaut
	return true;
		

}


ThumbView::ThumbView(BRect rrr, share *sh) :
	BView(rrr, "thumb_view", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE  )
{
	//DONNEES PARTAGEES DANS SHARED
	shared = sh ;	
	SetViewColor(216,216,216);
	
	#define LARGEUR 200
	#define HAUTEUR 32
	BRect r;
	r.Set(0,4,LARGEUR,Bounds().Height()-4);
	info_container = new BView(r, "", B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW);
	info_container->SetViewColor(216,216,216);

	r.Set(0,0,LARGEUR,HAUTEUR);
	info = new BTextView(r, "text", BRect(0,0,LARGEUR,HAUTEUR), B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW);
	info->SetViewColor(216,216,216);
	info->MakeEditable(false);	info->MakeSelectable(false);
	info->SetFont(be_plain_font, B_FONT_ALL);
	
	r.OffsetBy(0,HAUTEUR);
	r.bottom-=16;
	thumb_check = new BCheckBox(r,"checkit",Language.get("SAVE_THUMBNAIL"),new BMessage(SAVE_THUMB_CHANGED),B_FOLLOW_TOP | B_FOLLOW_LEFT,B_WILL_DRAW);
	thumb_check->SetValue(ThePrefs.save_with_thumb);		//par défaut save_with_thumb = B_CONTROL_ON; //par défaut on sauve le thumb

	r.OffsetBy(0,14);
	only_img_check = new BCheckBox(r,"checkimg",Language.get("ONLY_IMG"),new BMessage(ONLY_IMG_CHANGED),B_FOLLOW_TOP | B_FOLLOW_LEFT,B_WILL_DRAW);
	only_img_check->SetValue(ThePrefs.show_only_images);		
	
	info_container->AddChild(info);
	info_container->AddChild(thumb_check);
	info_container->AddChild(only_img_check);
	
	AddChild(info_container);
	
	thumb_check->SetTarget(util.mainWin);
	only_img_check->SetTarget(util.mainWin);
	
}


void ThumbView::UpdateText()
{



if (shared->thumbnail != NULL)
	{ 
	
	  //décalage selon largeur du thumb

    	Window()->Lock();
    	//DISABLED FOR POWERPC (PPC) since thumbnail doesn't work?
	  	info_container->MoveTo(shared->thumbnail->Bitmap()->Bounds().Width()+8,4);
		
		
		thumb_check->MoveTo(info->Frame().LeftBottom());
	
		info->MakeEditable(true);	info->MakeSelectable(true);
    	info->SelectAll(); info->Delete();

	  char str[255];
	  sprintf(str, "%ld pixels",shared->act_img->pix_per_line);
	  info->Insert(Language.get("WIDTH")); 
	  info->Insert(":\t");
	  info->Insert(str);
	
	  info->Insert("\n");
	  sprintf(str, "%ld pixels",shared->act_img->pix_per_row);
	  info->Insert(Language.get("HEIGHT"));  
	  info->Insert(":\t");
	  info->Insert(str);

	  info->MakeEditable(false);	info->MakeSelectable(false);
      Window()->Unlock();


	}


}


void ThumbView::Draw(BRect update_rect)
{
//DISABLED FOR POWERPC (PPC) since thumbnail doesn't work?
if (shared->thumbnail!=NULL) DrawBitmap(shared->thumbnail->Bitmap(),BPoint(4,4) );
}
//--------------------------------------------------------------------------------------------------------



