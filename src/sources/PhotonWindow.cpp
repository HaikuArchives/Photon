#include "PhotonWindow.h"

#define PAINT_MARGIN 0 //de chaque coté

#define  MENU_HSV 'mhsv'
#define  MENU_SELECT_ALL 'msla'
#define  MENU_SELECT_RANGE 'slrn'

#define  MENU_ROTATE_180 'r180'
#define  MENU_ROTATE_FREE 'rfre'
#define  MENU_ROTATE_PLUS_90 'r90p'
#define  MENU_ROTATE_MINUS_90 'r90m'
#define  MENU_SYMETRY_H  'symh'
#define  MENU_SYMETRY_V  'symv'


#define  MENU_LIMIT_LEVELS  'ltlv'

PhotonWindow::PhotonWindow(BRect frame, share *sh)
				: BWindow(frame, "      ", B_DOCUMENT_WINDOW, B_WILL_DRAW | B_ASYNCHRONOUS_CONTROLS)
{


shared=sh;

util.mainWin = this;
all_win_hidden = false;

rgWindow_visible = false;
		
//SetPulseRate(500*1000);

	BRect rect;

/// BARRE DE MENUS
	BMenu		*a_menu;  
	BMenuItem	*item;

	Lock();
	
	rect.Set( 0, 0,Bounds().right-1, 15);
	shared->menubar = new BMenuBar(rect, "MB");
	
	a_menu = new BMenu(Language.get("FILE"));
	a_menu->AddItem(new BMenuItem(Language.get("NEW"), 	new BMessage(MENU_NEW),'N',B_COMMAND_KEY));
	a_menu->AddSeparatorItem();
	a_menu->AddItem(new BMenuItem(Language.get("OPEN"), new BMessage(MENU_LOAD),'O',B_COMMAND_KEY));
	a_menu->AddItem(shared->close_item  = new BMenuItem(Language.get("CLOSE"), new BMessage(MENU_CLOSE),'W',B_COMMAND_KEY));
	a_menu->AddItem(shared->revert_item = new BMenuItem(Language.get("REVERT"), new BMessage(MENU_REVERT)));
	a_menu->AddItem(shared->save_item = new BMenuItem(Language.get("SAVE"), new BMessage(MENU_SAVE_DIRECT),'S',B_COMMAND_KEY));
	a_menu->AddItem(shared->save_as_item = new BMenuItem(Language.get("SAVE_AS"), new BMessage(MENU_SAVE),'S',B_COMMAND_KEY | B_SHIFT_KEY));
	a_menu->AddSeparatorItem();
	a_menu->AddItem(new BMenuItem(Language.get("PREFERENCES"), 		new BMessage(MENU_PREFS)));
	a_menu->AddSeparatorItem();
	a_menu->AddItem(new BMenuItem(Language.get("QUIT"), 		new BMessage(MENU_FILE_QUIT)));
	shared->menubar->AddItem(a_menu);
	
	
	
	//------

	a_menu = new BMenu(Language.get("EDIT"));
	a_menu->AddItem(new BMenuItem(Language.get("UNDO"), new BMessage(MENU_EDIT_UNDO),'Z',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("REDO"), new BMessage(MENU_EDIT_REDO),'Z',B_COMMAND_KEY | B_SHIFT_KEY));
	a_menu->AddSeparatorItem();

	a_menu->AddItem(new BMenuItem(Language.get("CUT"), new BMessage(MENU_EDIT_CUT),'X',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("COPY"), new BMessage(MENU_EDIT_COPY),'C',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("PASTE"), new BMessage(MENU_EDIT_PASTE),'V',B_COMMAND_KEY));
	a_menu->AddSeparatorItem();
	
/*
	a_menu->AddItem(new BMenuItem(Language.get("PR_BRUSH"), new BMessage(BRUSH_PREVIOUS), '1',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("NX_BRUSH"), 	new BMessage(BRUSH_NEXT),	  '2',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("PR_P_BRUSH"), new BMessage(BRUSH_PERSO_PREVIOUS), '3',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("NX_P_BRUSH"), 	new BMessage(BRUSH_PERSO_NEXT),	  '4',B_COMMAND_KEY));
	a_menu->AddSeparatorItem();
*/
/*
	a_menu->AddItem(new BMenuItem(Language.get("PR_PAPER"), new BMessage(PAPER_PREVIOUS), '5',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("NX_PAPER"),	new BMessage(PAPER_NEXT),	  '6',B_COMMAND_KEY));
	a_menu->AddSeparatorItem();
*/
	
	a_menu->AddItem(new BMenuItem(Language.get("ZOOM_IN"),   new BMessage(ZOOM_IN),  '+',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("ZOOM_OUT"),  new BMessage(ZOOM_OUT), '-',B_COMMAND_KEY));
	a_menu->AddSeparatorItem();


	a_menu->AddItem(mask_item = new BMenuItem(Language.get("PURGE_UNDO"),   new BMessage(EDIT_PURGE_UNDO)));
	a_menu->AddItem(mask_item = new BMenuItem(Language.get("PICK_BRUSH"),   new BMessage(PICK_BRUSH_SELECT),'b',B_COMMAND_KEY | B_SHIFT_KEY));
	a_menu->AddItem(mask_item = new BMenuItem(Language.get("USE_AS_PAPER"),   new BMessage(USE_SEL_AS_PAPER)));
	shared->menubar->AddItem(a_menu);

	//------
	
	a_menu = new BMenu(Language.get("IMAGE"));
	a_menu->AddItem(new BMenuItem(Language.get("CURVES"), new BMessage(MENU_CURVES),'M',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("HSV"), new BMessage(MENU_HSV),'U',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("INVERT"), new BMessage(MENU_INVERT),'I',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("LIMIT_LEVELS"), new BMessage(MENU_LIMIT_LEVELS)));
	a_menu->AddSeparatorItem();
	a_menu->AddItem(new BMenuItem(Language.get("CROP"), new BMessage(MENU_CROP)));
	a_menu->AddItem(new BMenuItem(Language.get("CANVAS_SIZE"), new BMessage(MENU_CANVAS)));
	a_menu->AddItem(new BMenuItem(Language.get("RESIZE_IMAGE"), new BMessage(MENU_RESIZE)));
	
	BMenu *sub_rotate = new BMenu(Language.get("ROTATION_SYMETRY"));
	sub_rotate->AddItem(new BMenuItem(Language.get("ROTATE_180"), new BMessage(MENU_ROTATE_180)));
	sub_rotate->AddItem(new BMenuItem(Language.get("ROTATE_PLUS_90"), new BMessage(MENU_ROTATE_PLUS_90)));
	sub_rotate->AddItem(new BMenuItem(Language.get("ROTATE_MINUS_90"), new BMessage(MENU_ROTATE_MINUS_90)));
	sub_rotate->AddItem(new BMenuItem(Language.get("ROTATE_FREE"), new BMessage(MENU_ROTATE_FREE)));
	sub_rotate->AddSeparatorItem();
	sub_rotate->AddItem(new BMenuItem(Language.get("HORIZONTAL_SYMETRY"), new BMessage(MENU_SYMETRY_H)));
	sub_rotate->AddItem(new BMenuItem(Language.get("VERTICAL_SYMETRY"), new BMessage(MENU_SYMETRY_V)));
	a_menu->AddItem(sub_rotate);
	a_menu->AddSeparatorItem();
	
	shared->convert_menu = new BMenu(Language.get("CONVERT_IMAGE"));
	shared->manip_menu = new BMenu(Language.get("MANIPULATE_IMAGE"));
	a_menu->AddItem(shared->convert_menu);
	a_menu->AddItem(shared->manip_menu);
	
	//DESACTIVé en attendant de clarifier les questions de licences GPL
	//pour l'instant en plus çA merdait mais bon.. peu i mporte on verra plus tard
	
	/*
	//FOR GIMP FILTERS SUPPORT
	fFilterMenu = new BMenu ("Gimp");
	RegisterGimpFilters ();
	a_menu->AddItem(fFilterMenu);
	*/
	
	shared->menubar->AddItem(a_menu);



	//------
	
	a_menu = new BMenu(Language.get("MASK"));
	
	a_menu->AddItem(new BMenuItem(Language.get("SELECT_ALL"),   new BMessage(MENU_SELECT_ALL),  'A',B_COMMAND_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("DELETE_MASK"),   new BMessage(MASK_DELETE),  'D',B_COMMAND_KEY));
	a_menu->AddSeparatorItem();
	a_menu->AddItem(new BMenuItem(Language.get("INVERT_MASK"), new BMessage(MENU_INVERT_MASK),'I',B_COMMAND_KEY | B_SHIFT_KEY));
	a_menu->AddItem(new BMenuItem(Language.get("SELECT_RANGE"), new BMessage(MENU_SELECT_RANGE)));
	a_menu->AddItem(mask_item = new BMenuItem(Language.get("DISABLE_MASK"),   new BMessage(MASK_ENABLE_DISABLE),  'h',B_COMMAND_KEY));
	shared->menubar->AddItem(a_menu);

	//------
	
	//PAS CHANGER L'ORDRE!!!!!! DE CEUX-CI!!!
	//PAS CHANGER L'ORDRE!!!!!! DE CEUX-CI!!!
	//PAS CHANGER L'ORDRE!!!!!! DE CEUX-CI!!!
	shared->display_menu = new BMenu(Language.get("DISPLAY"));
	//shared->display_menu->SetRadioMode(TRUE);//pour avoir un menu d'options à choix (coche)
	shared->display_menu->AddItem(new BMenuItem(Language.get("BRUSHES"),   new BMessage(SHOW_BRUSH_WIN),'1',B_COMMAND_KEY));
	shared->display_menu->AddItem(new BMenuItem(Language.get("PAPERS"),   new BMessage(SHOW_PAPER_WIN),'2',B_COMMAND_KEY ));
	shared->display_menu->AddItem(new BMenuItem(Language.get("LAYERS"),   new BMessage(SHOW_LAYER_WIN),'3',B_COMMAND_KEY ));
	shared->display_menu->AddItem(new BMenuItem(Language.get("INFOS"),   new BMessage(SHOW_INFO_WIN),'4',B_COMMAND_KEY));
	shared->display_menu->AddItem(new BMenuItem(Language.get("OPTIONS"),   new BMessage(SHOW_OPTION_WIN),'5',B_COMMAND_KEY));
	shared->display_menu->AddItem(new BMenuItem(Language.get("NAVIGATION"),   new BMessage(SHOW_NAVIGATION_WIN),'6',B_COMMAND_KEY ));
	shared->display_menu->AddItem(new BMenuItem(Language.get("TOOLS"),   new BMessage(SHOW_TOOL_WIN),'7',B_COMMAND_KEY ));



	shared->display_menu->AddSeparatorItem();

	
	shared->display_menu->AddItem(new BMenuItem(Language.get("HIDE_ALL"),   new BMessage(HIDE_ALL_WIN),'H',B_COMMAND_KEY | B_SHIFT_KEY ));

	shared->menubar->AddItem(shared->display_menu);

	//------

	
	shared->win_menu = new BMenu(Language.get("WINDOW"));
	shared->win_menu->SetRadioMode(TRUE);//pour avoir un menu d'options à choix (coche)
	//Il y a rien de fixe la dedans les images sont créées selon besoin
	shared->menubar->AddItem(shared->win_menu);

	//------
				

	a_menu = new BMenu(Language.get("HELP"));
	a_menu->AddItem(item = new BMenuItem(Language.get("ABOUT"), new BMessage(MENU_HELP_ABOUT)));

	shared->menubar->AddItem(a_menu);
	AddChild(shared->menubar); //ajoute la barre de menus complète à la fenêtre
	
	Unlock();
//FIN BARRE DE MENUS


//-------------------------------------------------------------------------------------------------

shared->mb_height = shared->menubar->Bounds().Height();

BRect back_rect = Bounds();

back_rect.top     = shared->mb_height+1;
back_rect.right  -= B_V_SCROLL_BAR_WIDTH;
back_rect.bottom -= B_H_SCROLL_BAR_HEIGHT;

back_view = new BView(back_rect,"", B_FOLLOW_ALL, B_WILL_DRAW);
back_view->SetViewColor(150,150,150);

rect.Set (0,0, 16,16);
inside_view = new PicView(rect, shared, B_FOLLOW_H_CENTER | B_FOLLOW_V_CENTER); //le B_WILL_DRAW est déjà dans le constructeur
back_view->AddChild(inside_view);
scroll_view = new BScrollView("scroll view", back_view, B_FOLLOW_ALL, B_WILL_DRAW,TRUE, TRUE);

//int16 marge_x = int16 ((back_view->Bounds().Width()-shared->act_img->pix_per_line)/2);
//int16 marge_y = int16 ((back_view->Bounds().Height()-shared->act_img->pix_per_row)/2);

AddChild(scroll_view); //ajoute la vue du texte à la window  

/*
back_bmp = BTranslationUtils::GetBitmap('RAWT',"back_text.png");
if (back_bmp!=NULL) inside_view->SetViewBitmap(back_bmp);
*/

//pointeur sur back, scroll view et 
//deux pointeurs sur les scrollbars de la scroll view
shared->scroll_view_ptr = scroll_view;
//shared->back_view_ptr   = back_view;
shared->scroll_hori 	= scroll_view->ScrollBar(B_HORIZONTAL);
shared->scroll_verti 	= scroll_view->ScrollBar(B_VERTICAL);


//POINT DE SELECTION DANS LE SELECTEUR DE COULEUR (carré et barre spectrale)
shared->ori_back_slide   = BPoint(260,0);
shared->ori_front_slide  = BPoint(260,0);
shared->ori_back_square  = BPoint(0,0); //noir
shared->ori_front_square = BPoint(255,255); //noir
DisableAll();




PostMessage(SHOW_TOOL_WIN);

full_screen =FULL_SCREEN_NONE;
menu_bar_shown=true;


}

PhotonWindow::~PhotonWindow()
{

	if (ThePrefs.brush_selector_open == true)	{ brWindow->Lock();	brWindow->Close(); }
	if (ThePrefs.paper_selector_open == true)	{ ppWindow->Lock();	ppWindow->Close(); }
	if (ThePrefs.layer_selector_open == true)	{ lyWindow->Lock();	lyWindow->Close(); }
	if (ThePrefs.info_win_open 		 == true)	{ ifWindow->Lock();	ifWindow->Close(); }
	if (ThePrefs.option_win_open 	 == true)	{ opWindow->Lock();	opWindow->Close(); }
	if (ThePrefs.tool_win_open	 	 == true)	{ tlWindow->Lock();	tlWindow->Close(); }
	if (ThePrefs.navigation_win_open == true)	{ nvWindow->Lock();	nvWindow->Close(); }


}

bool PhotonWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(true);
}

void PhotonWindow::MessageReceived(BMessage *msg)
{

BRect to_update, r,rect;
BMenuItem *temp;
BAlert *alert;
BPoint pt(0,0);
uint32 button_index;
int32 number;
		
		
BMessage *msg_a;

	switch (msg->what)
	{
	
	
		//POUR GIMP FILTERS
		case 'Fltr':	
//		fSrcBitmap = new BBitmap(BRect(0,0,320,200),B_RGB32); 
//		fDstBitmap = new BBitmap(BRect(0,0,320,200),B_RGB32); 
		//resume_thread (spawn_thread (AsyncFilter, "AsyncFilter", B_NORMAL_PRIORITY, new BMessage (msg))); 
		break;

		case SET_CURSOR:
		msg->FindInt32("id",&number);
		switch(number)
				{
				
				case 0:		shared->cursor_ptr = cursor_pick;				break;
				case 1:		shared->cursor_ptr = cursor_cross_txt;			break;
				case 2:		shared->cursor_ptr = cursor_min_cross;			break;
				case 3:		shared->cursor_ptr = cursor_empty_pointer;		break;
				case 4:		shared->cursor_ptr = cursor_zoom_out;			break;
				case 5:		shared->cursor_ptr = cursor_zoom_in;			break;
				case 6:		shared->cursor_ptr = cursor_simple_cross;		break;
				case 7:		shared->cursor_ptr = (uint8*) B_HAND_CURSOR; 			break;
			
			
				case CURSOR_OF_ACTIVE_TOOL:
				//SetPointerForActiveTool();
				break;
				 
				}
				
		shared->active_cursor_id = number;
		be_app->SetCursor(shared->cursor_ptr);
		break;
		
		
		case SET_FULL_NONE:
   	 	full_screen = FULL_SCREEN_NONE;
   	 	SetFullScreen();
   	 	break;
   	 	
   	 	case SET_FULL_HALF:
   	 	full_screen = FULL_SCREEN_HALF;
   	 	SetFullScreen();
   	 	break;
   	 	
   	 	
   	 	case SET_FULL_TOTAL:
   	 	full_screen = FULL_SCREEN_TOTAL;
   	 	SetFullScreen();
   	 	break;
   	 	
   	 		
		case UPDATE_DISPLAY:
		msg->FindPoint("where",&pt);
		inside_view->UpdateDisplay(pt);
		break;
		
	
		case IMAGE_MANIP:
			if (!shared->mBitmapInUse)
			{
				shared->mAddonId = msg->FindInt32("addon_id");
				inside_view->PrepareFilter();
				shared->ImageManip(util.sel_pic);				
				inside_view->FilteringDone();
			}
			break;

		case IMAGE_CONV:
			if (!shared->mBitmapInUse)
			{
				shared->mAddonId = msg->FindInt32("addon_id");
				inside_view->PrepareFilter();
				shared->ImageConvert(util.sel_pic);				
				inside_view->FilteringDone();
			}
			break;

	
		case B_REFS_RECEIVED:
		beep();
		break; 
	
		case MENU_EDIT_CUT:
		beep();
		break;
		
		case MENU_EDIT_COPY:

  		if ( be_clipboard->Lock() ) 
  		 { 
  		 	beep();
      		be_clipboard->Clear(); 
      	    BMessage *clipper = be_clipboard->Data(); 
     	    clipper->AddString("text/plain", shared->act_img->name);
	      	be_clipboard->Commit(); 
     	 	be_clipboard->Unlock(); 
 		 }

		break;
		
		case MENU_EDIT_PASTE:
		beep();
		break;
	
		case DISABLE_ALL:	
		DisableAll();
		break;
		
		case ENABLE_ALL:	
		EnableAll();
		break;
		
		
		case MENU_CLOSE:
		
		if (ThePrefs.no_pictures_left==OFF)
		{
		alert = new BAlert("",Language.get("MODIFIED_SAVE"), Language.get("CANCEL"),Language.get("NO"), Language.get("YES"),
						   B_WIDTH_FROM_WIDEST,B_WARNING_ALERT); 
   				int32 button_index_aaa;
   
   				switch(button_index_aaa = alert->Go())
   					{
   					case 0:
   					break;
   					
   					case 1:	
					shared->DeleteImage(shared->active_image);
   					break;
					
					case 2:   
					PostMessage(MENU_SAVE);
					break;
					}
		}
		break;
		
		case MENU_PREFS:
		rect = ThePrefs.pref_frame;
		if (ThePrefs.pref_win_open==false)
		{
		prefWindow = new PrefWindow(rect,Language.get("PREFERENCES"),shared);   
 		prefWindow->Show();
		}
		break;


		case IMAGE_CHOSEN:
		if (ThePrefs.no_pictures_left==OFF)
		{
		temp = shared->win_menu->FindMarked();
		shared->active_image = shared->win_menu->IndexOf(temp);
		shared->initPic();
		PostMessage(ZOOM_CHANGED); 
		}
		PostMessage(UPDATE_TITLE); 

		break;
			
		case TRANSLATOR_SELECTED:
		msg->FindInt32("id",&number);
		shared->active_translator = util.translators[number];
    	break;
		
		case MASK_ENABLE_DISABLE:
		Lock();
		if (ThePrefs.mask_activated==ON) { mask_item->SetLabel(Language.get("ENABLE_MASK"));ThePrefs.mask_activated=OFF; }
		else {mask_item->SetLabel(Language.get("DISABLE_MASK")); ThePrefs.mask_activated=ON; }

		
		Unlock();
		break;

		case MENU_REVERT:

	    alert = new BAlert(NULL,Language.get("RELOAD_SAVED"), Language.get("YES"),Language.get("NO"),NULL,
        					B_WIDTH_FROM_WIDEST,B_WARNING_ALERT); 
   		
   		switch(button_index = alert->Go())
   		{
   		//les boutons ont toujours les valeurs 0,1,2 (trois boutons max)
   		case 1:	break;
		case 0:   
		//on reloade l'originale
		shared->act_img->Revert();	 //revert mode =ON !!
		break;
		}
		break;

		case MENU_HSV:
		inside_view->Filter_SlideHSV();
		break;
		
		case MENU_INVERT:
		inside_view->Filter_Invert();
		break;
		
		
		case MENU_LIMIT_LEVELS:
		inside_view->PrepareFilter();
		if (ThePrefs.limit_levels_open==false)
		{
		llWindow = new LimitLevelsWindow(util.find_win_pos(256+16,100,this),shared); 
 		llWindow->Show();
		}
		break;
		
		case PREVIEW_LIMIT_LEVELS:
		msg->FindInt32("levels",&number);
		inside_view->Filter_LimitLevels(uint8(number));
		PostMessage(UPDATE_ME);
		break;
		
		case APPLY_FILTER:
		inside_view->FilteringDone();
		break;
		
		case CANCEL_FILTERING:
		inside_view->filtering =false;
		PostMessage(DRAW_ME);
    	delete util.sel_pic; //on efface la pic temp...
		util.progress_win->Hide();
		break;
		
		case MENU_INVERT_MASK:
		inside_view->InvertSelection();
		break;

		case MENU_SELECT_RANGE:
		if (ThePrefs.range_selection_open==false)
		{
		rgWindow = new RangeWindow(util.find_win_pos(320,200,this),shared); 
 		rgWindow->Show();
		}
		break;
		

		case MENU_CROP:
		inside_view->CropSelected();
		
		break; 
		
		case PICK_BRUSH_SELECT:
		shared->previous_tool = shared->active_tool;
		shared->active_tool   = PICK_BRUSH;
		util.mainWin->PostMessage(TOOL_CHANGED);
		break;

		case USE_SEL_AS_PAPER:
		//prend la sélection et en fait un papier...
		r = 	shared->FindSelectedRect(); //on agit que sur des rects qui font partie de la sélection
   		shared->paper_24 = util.GrabRect(r,  shared->act_img->undo_bitmap); 
	   	shared->UseAsPaper(shared->paper_24);
   		break;

		case MENU_CANVAS:
   	  	inside_view->ResizeCanvas(32,64,100,200);
		break;

		case MENU_ROTATE_180:    
		inside_view->Filter_Rotate180();   
		break;
		
		case MENU_ROTATE_PLUS_90:  util.NotImplemented();    break;
		case MENU_ROTATE_MINUS_90:  util.NotImplemented();    break;
		case MENU_ROTATE_FREE:  util.NotImplemented();    break;
		
		case MENU_SYMETRY_H:  
		inside_view->Filter_FlipHori();
		break;
		
		case MENU_SYMETRY_V:  util.NotImplemented();    break;
	
		case MENU_RESIZE:
		util.NotImplemented(); 
		break;

		//this one does all layer calculations, call draw_me for simple screen redraws
   	  	case UPDATE_ME:
   	  	if (shared->act_img!=NULL) 
	   	{
   		  	if (  msg->FindRect("zone", &to_update)!=B_OK ) //Si pas de paramètres on update la full
   		  	{	
   		  		
	   		  	if (shared->act_img!=NULL)
   		  			to_update = shared->act_img->the_layers[0]->img->Bounds();
   		  	}

   		  //	to_update.PrintToStream(); 	
   		  	shared->act_img->UpdateDisplayImg(to_update);
		}
  	  	break;
   

		case DRAW_ME:
		  	if (  msg->FindRect("zone", &to_update)!=B_OK) //Si pas de paramètres on update la full
   		  	{
	   		  	if (shared->act_img!=NULL)
   		  			to_update = shared->act_lay->img->Bounds();
   		  	}

   		  	 inside_view->Draw(to_update);
	   		//inside_view->Draw(Bounds());
	   		
		break;
		 
	

   		case PIC_CREATED:
       //valeurs inverses par défaut, comme ça on peut revenir en arrière pour trouver bord
		inside_view->zone_to_update.top    = shared->act_img->pix_per_row-1;
		inside_view->zone_to_update.left   = shared->act_img->pix_per_line-1;
		inside_view->zone_to_update.bottom = 0;
		inside_view->zone_to_update.right  = 0;
		break;
	
   	  	case PAINT_RECT:
 
   		msg->FindRect("zone",&r);
		shared->act_img->UpdateDisplayImg(r);
		
		break;
   	  	
		case MENU_NEW:
		if (ThePrefs.create_win_open == false)
		{
			crt_win = new CreateWindow(util.find_win_pos(320,220,this),shared);
			crt_win->Show();
		}
		break;

	
	   	case MENU_LOAD:		
	   	OpenPanel(); //SAVE AS
		break;
			  	   	  	
		case MENU_SAVE:
		//SavePanel(); //SAVE AS
		panneau_save = new SaveAsPanel(this,shared);
	  	panneau_save->SaveAs(shared->act_img->display_bitmap);
	  	break;
   	  	
   	  	case MENU_SAVE_DIRECT:
   	  	
   	  	if ( strcmp(shared->act_img->full_path,"")==0) { SavePanel(); }
		else {
				BMessage *msg = new BMessage(SAVE_WITH_NAME);
				be_app->PostMessage(msg);
				}
		break;
	  	
		case MENU_EDIT_REDO:
		
			if (shared->act_img->undo_data->undo[shared->act_img->undo_data->undo_amount+1]!=NULL)
			{
			to_update = shared->act_img->undo_data->undo[shared->act_img->undo_data->undo_amount+1]->Bounds(); //+1 car on draw le suivant
			shared->act_img->Redo(); 
			shared->act_img->UpdateDisplayImg(to_update);
			}		
		
		if (ThePrefs.layer_selector_open==true)
		{		
			BMessage msg(UPDATE_ACTIVE_LAYER);
			msg.AddInt32("active",shared->act_lay->id);
			util.layerWin->PostMessage(&msg);		
		}	
		break;


		case MENU_EDIT_UNDO:
		if (shared->act_img->undo_data->undo_amount >= 0)		
		{	
		to_update = shared->act_img->undo_data->undo[shared->act_img->undo_data->undo_amount]->Bounds();
		shared->act_img->Undo();
		shared->act_img->UpdateDisplayImg(to_update);
		
		if (ThePrefs.layer_selector_open==true)
		{		
			BMessage msg(UPDATE_ACTIVE_LAYER);
			msg.AddInt32("active",shared->act_lay->id);
			util.layerWin->PostMessage(&msg);		
		}	
		
		} 
		break;
		
		case EDIT_PURGE_UNDO: 
		shared->act_img->PurgeUndo();
		break;
		
		case MENU_CURVES:
		curv_win = new CurveWindow(BRect(100,100,100+204,100+100),shared);
		curv_win->Show();
		break;

		case MENU_SELECT_ALL:
		shared->act_img->FillMask(); //memorize old mask in undo ON
		shared->act_img->UpdateDisplayImg(Bounds());
		break;
		

		case MASK_DELETE:
		shared->act_img->DeleteMask(ON); //memorize old mask in undo ON
		shared->act_img->UpdateDisplayImg(Bounds());
	//	inside_view->Draw(Bounds());
		break;

		case MASK_DELETE_INIT:
		shared->act_img->DeleteMask(OFF); //memorize PAS old mask in undo OFF
		//pas la peine de se faire chier à afficher, on le refait après...
		break;

		case BRUSH_PREVIOUS:
		shared->generateBrush(shared->current_brush-1);
		PostMessage(new BMessage(UPDATE_TITLE)); 
		PostMessage(new BMessage(TOOL_CHANGED)); 
		
		break;
		
		//pour l'instant personne n'envoie encore ce message
		case ACTIVATE_BRUSH:
		msg->FindInt32("num",&number); 
		shared->current_brush=number;
		shared->generateBrush(shared->current_brush);
		PostMessage(new BMessage(UPDATE_TITLE)); 
		PostMessage(new BMessage(TOOL_CHANGED)); 	
		break;
		
		case BRUSH_NEXT:
		shared->generateBrush(shared->current_brush+1);
		PostMessage(new BMessage(UPDATE_TITLE)); 
		PostMessage(new BMessage(TOOL_CHANGED)); 
		break;


		case ACTIVATE_PERSO_BRUSH:
		
		msg->FindInt32("num",&number); 
		shared->current_perso_brush=number;
		shared->loadBrush(shared->current_perso_brush);
		
		msg_a = new BMessage(DRAW_PERSO_BRUSHES);
		msg_a->AddInt32("active",shared->current_perso_brush);
		if (ThePrefs.brush_selector_open==true) util.brushWin->PostMessage(msg_a);
		
		break;


		case ACTIVATE_LAYER:

		msg->FindInt32("num",&number); 
		shared->act_lay = shared->act_img->the_layers[number];
		shared->act_img->active_layer = shared->act_lay->id;
		
		switch(shared->act_lay->layer_type)
			{	
				case LAYER_TYPE_BITMAP:		
				shared->initLayer();
				break;
				case LAYER_TYPE_TEXT:	break;
				case LAYER_TYPE_EFFECT:	break;
				case LAYER_TYPE_GUIDE:	break;
				default: beep(); beep(); printf("Layer has invalid type!!!!"); break;				
			}

		if (ThePrefs.layer_selector_open==true) 
		{
			msg_a = new BMessage(DRAW_LAYERS);
			msg_a->AddInt32("active",shared->act_lay->id);
			util.layerWin->PostMessage(msg_a);
		}

		break;

		case ACTIVATE_THUMB_IMG:
		msg->FindInt32("num",&number); 
		panneau_load->SelectFromThumb(number);
		break;


		case CHANGED_BY_THUMB_TRUE:
		panneau_load->changed_by_thumb=true;
		break;
				
		case DEACTIVATE_THUMB_IMG:
		panneau_load->ClearThumbSelection();
		break;
				
		case DONE_SELECTING_THUMB_IMG:
		panneau_load->SelectionDone();
		break;

		case BRUSH_PERSO_PREVIOUS:
		if (shared->current_perso_brush > 0)
			{
				shared->loadBrush(shared->current_perso_brush-1);
				msg_a = new BMessage(DRAW_PERSO_BRUSHES);
				msg_a->AddInt32("active",shared->current_perso_brush);
				if (ThePrefs.brush_selector_open==true) util.brushWin->PostMessage(msg_a);
			}
		break;
		
		case BRUSH_PERSO_NEXT:
		if (shared->current_perso_brush+1 < shared->max_brush)
			{
				shared->loadBrush(shared->current_perso_brush+1);
				msg_a = new BMessage(DRAW_PERSO_BRUSHES);
				msg_a->AddInt32("active",shared->current_perso_brush);
				if (ThePrefs.brush_selector_open==true) util.brushWin->PostMessage(msg_a);
			}
		break;

		case ACTIVATE_PAPER:
		msg->FindInt32("num",&number); 
		shared->current_paper=number;
		shared->loadPaper(shared->current_paper);
		
		msg_a = new BMessage(DRAW_PAPERS);
		msg_a->AddInt32("active",shared->current_paper);
		if (ThePrefs.paper_selector_open==true) util.paperWin->PostMessage(msg_a);
		break;
		

		case PAPER_PREVIOUS:
		if (shared->current_paper > 0)
			{
				shared->loadPaper(shared->current_paper-1);
				msg_a = new BMessage(DRAW_PAPERS);
				msg_a->AddInt32("active",shared->current_paper);
				if (ThePrefs.paper_selector_open==true) util.paperWin->PostMessage(msg_a);
			}
		break;

		case PAPER_NEXT:
			if (shared->current_paper+1 < shared->max_paper)
			{
				shared->loadPaper(shared->current_paper+1);
				msg_a = new BMessage(DRAW_PAPERS);
				msg_a->AddInt32("active",shared->current_paper);
				if (ThePrefs.paper_selector_open==true) util.paperWin->PostMessage(msg_a);
			}
		break;
		
		
		case MENU_HELP_ABOUT:
		be_app->PostMessage(MENU_HELP_ABOUT);
		break;
		
		case ZOOM_IN:

		if (shared->act_img->zoom_level + ZOOM_AMOUNT <=10) //max 1000%
			{
			shared->act_img->zoom_level += ZOOM_AMOUNT;
			ZoomChanged();
			}
		break;

		case ZOOM_OUT:
		if (shared->act_img->zoom_level - ZOOM_AMOUNT > 0) //faut pas que ce soit zero  
			{
			shared->act_img->zoom_level -= ZOOM_AMOUNT; 
			ZoomChanged();
			}
			
		else if (shared->act_img->zoom_level - ZOOM_AMOUNT/5 > 0) //increment minimum 
			{
			shared->act_img->zoom_level -= ZOOM_AMOUNT/5; 
			ZoomChanged();
			}
		break;
	
		case ZOOM_CHANGED:
		ZoomChanged();
		break;


		
				
		case TOOL_CHANGED:
		//Si tool ou taille brush a changé...
		UpdateToolInfo();
		Activate(); 
		break;		

		
		case UPDATE_TITLE:
		UpdateTitle();
		
		break;

		case UPDATE_WIN_MENU:
		Lock();
		if (shared->win_menu->ItemAt(shared->active_image)!=NULL)
		shared->win_menu->ItemAt(shared->active_image)->SetLabel(shared->act_img->name);
		Unlock();
		break;
		
		
		case SET_TRANSP:
		msg->FindInt32("value",&number);
		shared->paint_transparency=uint8(100-number); 
		if (ThePrefs.option_win_open==true) 	opWindow->PostMessage(TRANSP_CHANGED);
		break;
		
		case MODE_CHANGED:		
		if (ThePrefs.option_win_open==true) opWindow->PostMessage(msg);
		break;
		
		case B_KEY_DOWN:
		ShortCuts(msg);
		break;
		
		case B_MODIFIERS_CHANGED:
		//ShortCuts(msg);
		break;

		case SAVE_THUMB_CHANGED:
			//on inverse la valeur
			if (ThePrefs.save_with_thumb==B_CONTROL_OFF) ThePrefs.save_with_thumb = B_CONTROL_ON;
			   else ThePrefs.save_with_thumb = B_CONTROL_OFF;
		break;
		
		case SHOW_THUMB_CHANGED:
			//on inverse la valeur
			if (ThePrefs.show_thumbs==B_CONTROL_OFF) ThePrefs.show_thumbs = B_CONTROL_ON;
			   else ThePrefs.show_thumbs = B_CONTROL_OFF;
			panneau_load->UpdateThumbs();
		break;
	
		case ONLY_IMG_CHANGED: 
			Lock();
			//on inverse la valeur
			if (ThePrefs.open_panel_open == true) 
				{
				ThePrefs.show_only_images= panneau_load->check_only_images->Value();	
				panneau_load->Refresh();
				}
			if (ThePrefs.save_panel_open == true) 
				{
					ThePrefs.show_only_images= panneau_save->fond_thumb->only_img_check->Value();
					panneau_save->panel->Refresh();
				}
			Unlock();
			
			
		break;
		
		case CREATE_THUMBS:
		if (ThePrefs.open_panel_open == true) 
			{
				panneau_load->CreateThumbs();
			}
		
		break;


		case SHOW_PROGRESS_WIN:
		util.progress_win->Show();
		util.progress_exists = true;
		break;
		
		
		case HIDE_PROGRESS_WIN:
		util.progress_win->Hide();
		util.progress_exists = false;
		break;


		//CES MESSAGES SONT DANS l'ordre correspondant au menu.
		case SHOW_BRUSH_WIN:
			if (ThePrefs.brush_selector_open==true)	
				{ 
					brWindow->Lock(); brWindow->Close(); 
					shared->display_menu->ItemAt(0)->SetMarked(false);
				} 
			else
				{ 
					brWindow = new BrushWindow(ThePrefs.brushes_frame,Language.get("PERSO_BRUSHES"),shared); //ON = mode back 
	 				brWindow->Show();
					shared->display_menu->ItemAt(0)->SetMarked(true);

				}
		break;
		
		case SHOW_PAPER_WIN:
		if (ThePrefs.paper_selector_open==true)	
				{ 
					ppWindow->Lock(); ppWindow->Close();
					shared->display_menu->ItemAt(1)->SetMarked(false);

				} 
			else
				{ 
					ppWindow = new PaperWindow(ThePrefs.paper_frame,Language.get("PAPERS"),shared);  
 					ppWindow->Show();
					shared->display_menu->ItemAt(1)->SetMarked(true);
				}
		break;
		
		case SHOW_LAYER_WIN:
		if (ThePrefs.layer_selector_open==true)	
				{ 	
					lyWindow->Lock(); lyWindow->Close(); 
					shared->display_menu->ItemAt(2)->SetMarked(false);
				}
			else
				{ 
					lyWindow = new LayerWindow(ThePrefs.layer_frame,Language.get("LAYERS"),shared);  
 					lyWindow->Show();
					shared->display_menu->ItemAt(2)->SetMarked(true);

				}
		break;


	case SHOW_INFO_WIN:
		if (ThePrefs.info_win_open==true)	
				{ 
					ifWindow->Lock(); ifWindow->Close(); 
					shared->display_menu->ItemAt(3)->SetMarked(false);
				}
			else
				{ 
					ifWindow = new InfoWindow(ThePrefs.info_frame,Language.get("INFOS"),shared); 
 					ifWindow->Show();
					shared->display_menu->ItemAt(3)->SetMarked(true);

				}
		break;
		
	case SHOW_OPTION_WIN:
		if (ThePrefs.option_win_open==true)	
				{ 
					opWindow->Lock(); opWindow->Close(); 
					shared->display_menu->ItemAt(4)->SetMarked(false);
				}
			else
				{ 
					opWindow = new OptionWindow(ThePrefs.option_frame,Language.get("OPTIONS"),shared); 
 					opWindow->Show();
					shared->display_menu->ItemAt(4)->SetMarked(true);
				}
		break;
		
		case SHOW_NAVIGATION_WIN:
		if (ThePrefs.navigation_win_open==true)	
				{ 
					nvWindow->Lock(); nvWindow->Close(); 
					shared->display_menu->ItemAt(5)->SetMarked(false);
				}
			else
				{ 
					nvWindow = new NavigationWindow(ThePrefs.navigation_frame,Language.get("NAVIGATION"),shared); 
 					nvWindow->Show();
					shared->display_menu->ItemAt(5)->SetMarked(true);
				}
		break;
		
		case SHOW_TOOL_WIN:
		if (ThePrefs.tool_win_open==true)	
				{ 
					tlWindow->Lock(); tlWindow->Close(); 
					shared->display_menu->ItemAt(6)->SetMarked(false);
				}
			else
				{ 
					tlWindow = new ToolWindow(ThePrefs.tool_frame,Language.get("TOOLS"),shared); 
 					tlWindow->Show();
					shared->display_menu->ItemAt(6)->SetMarked(true);
				}
		break;
		
	
		/*
		#define SHOW_TOOL_WIN	'shtw'
#define SHOW_NAVIGATION_WIN	'shnw'

		*/
		
		case HIDE_ALL_WIN:
		if (all_win_hidden==true)
			{	
				//on réaffiche tout.
				all_win_hidden = false;
				shared->display_menu->ItemAt(8)->SetMarked(false);
				
				if (ThePrefs.brush_selector_open==true) brWindow->Show();
				if (ThePrefs.paper_selector_open==true) ppWindow->Show();
				if (ThePrefs.layer_selector_open==true) lyWindow->Show();
				if (ThePrefs.info_win_open==true) 		ifWindow->Show();
				if (ThePrefs.option_win_open==true) 	opWindow->Show();
				if (ThePrefs.tool_win_open==true)		util.toolWin->Show();
				if (ThePrefs.navigation_win_open==true)	util.navWin->Show();
				if (ThePrefs.back_selector_open==true)	util.foreWin->Show();
				if (ThePrefs.fore_selector_open==true)	util.backWin->Show();

			}
			else
			{
				//on cache tout
				all_win_hidden = true;
				shared->display_menu->ItemAt(8)->SetMarked(true);
	
				if (ThePrefs.brush_selector_open==true) brWindow->Hide();
				if (ThePrefs.paper_selector_open==true) ppWindow->Hide();
				if (ThePrefs.layer_selector_open==true) lyWindow->Hide();
				if (ThePrefs.info_win_open==true) 		ifWindow->Hide();
				if (ThePrefs.option_win_open==true) 	opWindow->Hide();
				if (ThePrefs.navigation_win_open==true)	util.navWin->Hide();
				if (ThePrefs.tool_win_open==true)		util.toolWin->Hide();
				if (ThePrefs.back_selector_open==true)	util.foreWin->Hide();
				if (ThePrefs.fore_selector_open==true)	util.backWin->Hide();
			}
		
		break;
		


		case DISPLAY_UNITS_CHANGED:
		msg->FindInt32("unit",&shared->act_img->units);
		
		break;
				
		
		default:
   			BWindow::MessageReceived(msg);
   			break;
	}
}	


void PhotonWindow::UpdateTitle()
{
		char str[4096]; //il en faut assez car au chargement de plusieurs pics des fois ça merde...
		char str2[4096]; //et pis bon c'est que 4 k...

		sprintf(str,util.version_txt);

		if (ThePrefs.no_pictures_left==OFF)
			{
			
				strcat(str," - [ ");
				strcat(str,shared->act_img->name);
				sprintf(str2," at %.0f %% ]",shared->act_img->zoom_level*100);
				strcat (str,str2);
				sprintf(str2," - %ld x",shared->act_img->pix_per_line);
				strcat (str,str2);
				sprintf(str2," %ld -",shared->act_img->pix_per_row);
				strcat (str,str2);
			
			}
			
			if (ThePrefs.mask_mode==ON) { strcat(str," - "); strcat(str,Language.get("MASKING")); }
				
//		sprintf(str2," | %d | %d",shared->active_image, shared->image_amount);
//		strcat(str,str2);
		
	SetTitle(str);
}		

void PhotonWindow::ZoomChanged()
{

if (shared->act_img!=NULL)
	{

	//scroll_view->SetScale(shared->act_img->zoom_level); 
	//back_view->SetScale(shared->act_img->zoom_level); 

	inside_view->SetScale(shared->act_img->zoom_level); 

	inside_view->ResizeTo(	(shared->act_img->pix_per_line) * shared->act_img->zoom_level,
							(shared->act_img->pix_per_row)  * shared->act_img->zoom_level);

	//inside_view->ScrollTo(100* shared->act_img->zoom_level,100* shared->act_img->zoom_level);
	inside_view->ScrollTo(0,0);
	}

FrameResized(Bounds().Width(),Bounds().Height());	//pour le placement pic

if (ThePrefs.navigation_win_open==true) nvWindow->PostMessage(ZOOM_CHANGED);

UpdateTitle();


}	


//------------------------------------------------------------------------------

void PhotonWindow::UpdateToolInfo()
{

	if (ThePrefs.option_win_open==true) opWindow->PostMessage(UPDATE_OPTIONS);
	if (ThePrefs.info_win_open==true) ifWindow->PostMessage(COL_CHANGED); //pour restaurer col si on sort du picker

	uint32 buttons;
	//ça s'update que quand la mouse bouge, donc.. on fake		
	inside_view->GetMouse(&shared->pos_actuelle,&buttons);
	inside_view->MouseMoved(shared->pos_actuelle,0,NULL);

		

}//Fin updateToolInfo

void PhotonWindow::SetPointerForActiveTool()
{

/*

case 0:		shared->cursor_ptr = cursor_pick;				break;
				case 1:		shared->cursor_ptr = cursor_cross_txt;			break;
				case 2:		shared->cursor_ptr = cursor_min_cross;			break;
				case 3:		shared->cursor_ptr = cursor_empty_pointer;		break;
				case 4:		shared->cursor_ptr = cursor_zoom_out;			break;
				case 5:		shared->cursor_ptr = cursor_zoom_in;			break;
				case 6:		shared->cursor_ptr = cursor_simple_cross;		break;
				case 7:		shared->cursor_ptr = B_HAND_CURSOR; 			break;
		
*/

//les valeurs de pointer_id sont définies dans messagereceived -- SetCursor

int32 pointer_id=6; //par défaut le simple....

switch (shared->active_tool)
		{		
				case PAINTBRUSH:	pointer_id = 2;		break;
				case ERASER:		pointer_id = 2;		break;
				case BUCKET:		pointer_id = 6;		break;
				case LINE:			pointer_id = 2;		break;
				case F_POLY:		pointer_id = 6;		break;
				case SPLINE:		pointer_id = 2;		break;
				case F_SPLINE_POLY:	pointer_id = 6;		break;
				case RECTANGLE:		pointer_id = 2;		break;
				case F_RECTANGLE:	pointer_id = 6;		break;
				case ELLIPSE:		pointer_id = 2;		break;
				case F_ELLIPSE:		pointer_id = 6;		break;
				case TEXT:			pointer_id = 1;		break;
				case ZOOM:			pointer_id = 5;		break;
				case PICKER:		pointer_id = 0;		break;
				case STAMP:			pointer_id = 2;		break;
				case WAND:			pointer_id = 6;		break; 
				case LASSO:			pointer_id = 6;		break; 
				case SEL_RECT:		pointer_id = 6;		break; 
				case SEL_ELLIPSE:	pointer_id = 6;		break; 
				case MOVE:			pointer_id = 6;		break; 
				case PICK_BRUSH:	pointer_id = 6;		break;
		}
		
		BMessage *mx = new BMessage(SET_CURSOR);
		mx->AddInt32("id",pointer_id); 
		PostMessage(mx);
		
					

}



void PhotonWindow::OpenPanel()
{
	if (ThePrefs.open_panel_open == false)
	{	
		 ThePrefs.open_panel_open = true;	
  	 	 panneau_load = new PhotonOpenPanel(shared);
	}

	Lock();
	panneau_load->check_only_images->SetValue(ThePrefs.show_only_images);
	Unlock();
	panneau_load->Show();

}



void PhotonWindow::SavePanel()
{

/*
	ThePrefs.no_pictures_left=ON; //pour arrêter l'affichage pendant le save

if (ThePrefs.save_panel_open == false)
	{	
	 ThePrefs.save_panel_open = true;	
   	 panneau_save = new PhotonSavePanel(shared);
	}
else 

{   //si la window a déjà été créée faut juste refaire le thumbnail...
	shared->thumbnail = new DThumbnail(shared->act_lay->img, true); // the thumbnail will be dithered 
}

Lock();
panneau_save->fond_thumb->only_img_check->SetValue(ThePrefs.show_only_images);
Unlock();

panneau_save->Show();
*/
}







void PhotonWindow::ShortCuts(BMessage *msg)
 {
#define SH 	 0
#define SHCT 2

 
		   int8 bytes[3];
           int32 key;
           int32 the_modifiers;

           msg->FindInt32 ("modifiers", &the_modifiers); 
           msg->FindInt32 ("key", &key); 
           msg->FindInt8  ("byte", &bytes[0]); 


			rgb_color a= {0,0,0,0};
			rgb_color b= {255,255,255,0};
			

			//PUTAIN COMMENT ON TESTE CES SHIFT ____ET____ les caractères?
			//CA ça teste s'il y a un shift mais le case '1' fonctionne plus...						
			//Keys with shift	
			
			/*
            	switch (key)
            	{ 
            	
                //	case '1' || the_modifiers:	shared->paint_transparency = 1; 	i_view->opacity_slider->SetValue(1); 	  break; 
            		case '2' & B_SHIFT_KEY:	beep(); break;
            	}
            */
            
            
       /*     
                case '1':
                	if (key & B_SHIFT_KEY)
                		 {beep(); PostMessage(new BMessage(BRUSH_PREVIOUS));} 
                	else if (key & B_SHIFT_KEY) 
                		 { shared->paint_transparency = 1;  i_view->opacity_slider->SetValue(1); }  
                	else {shared->paint_transparency = 10;  i_view->opacity_slider->SetValue(10); }	
                break; 
       
       */
/* 
 	bool shift_is_on = false;
	bool control_is_on = false;
 
//	if (the_modifiers & B_SHIFT_KEY)   beep();
	if (the_modifiers & B_SHIFT_KEY)   shift_is_on   = true;
	if (the_modifiers & B_CONTROL_KEY) control_is_on = true;
*/			 
 
bool tool_has_changed=false;

			
			BMessage *tr = new BMessage(SET_TRANSP);
			//POUR KEYS SIMPLES
	        switch ( bytes[0] )
            { 
				
				
				//pourcentages	
				case '1':	tr->AddInt32("value",10); PostMessage(tr); break;
				case '2':	tr->AddInt32("value",20); PostMessage(tr); break;
				case '3':	tr->AddInt32("value",30); PostMessage(tr); break;
				case '4':	tr->AddInt32("value",40); PostMessage(tr); break;
				case '5':	tr->AddInt32("value",50); PostMessage(tr); break;
				case '6':	tr->AddInt32("value",60); PostMessage(tr); break;
				case '7':	tr->AddInt32("value",70); PostMessage(tr); break;
				case '8':	tr->AddInt32("value",80); PostMessage(tr); break;
				case '9':	tr->AddInt32("value",90); PostMessage(tr); break;
				case '0':	tr->AddInt32("value",100); PostMessage(tr); break;
				
				//ECHANGE BACK AND FOREGROUND
				case 'x': 	rgb_color temp=shared->fore_color;
							shared->fore_color = shared->back_color;
							shared->back_color = temp;
							util.toolWin->PostMessage(new BMessage(COL_SELECTED));
				break;
				 		   
				//COULEURS PAR Défaut (black+white)
				case 'd': 	
							shared->fore_color = a;
							shared->back_color = b;
							util.toolWin->PostMessage(new BMessage(COL_SELECTED));
				break;
			
				

 				case 'f':
 					//on cycle entre full et autres...
					if (full_screen==FULL_SCREEN_TOTAL) full_screen = FULL_SCREEN_NONE;
					else full_screen++;
					
					if (ThePrefs.tool_win_open==true)
					{
					switch (full_screen) //on appelle le menu qui sette les boutons...
						{
						 case FULL_SCREEN_NONE:		tlWindow->PostMessage(SET_FULL_NONE);	 break;
						 case FULL_SCREEN_HALF:		tlWindow->PostMessage(SET_FULL_HALF);	 break;
						 case FULL_SCREEN_TOTAL:	tlWindow->PostMessage(SET_FULL_TOTAL); break;
						}
					}
					else SetFullScreen();
				break;

				

				/*   T    O    O    L    S*/
				 
				//GAUCHE		   
 		   		case 'b': util.toolWin->PostMessage(new BMessage(BUTTON_01_MSG));	tool_has_changed=true;	break; //BRUSH TOOL
 		   		case 'k': util.toolWin->PostMessage(new BMessage(BUTTON_02_MSG));	tool_has_changed=true;	break; 	//bucket
				case 'n':	util.toolWin->PostMessage(new BMessage(BUTTON_03_MSG));	tool_has_changed=true;	break;	//line
				case 'N':	util.toolWin->PostMessage(new BMessage(BUTTON_04_MSG));	tool_has_changed=true;break;	//spline
				case 'r':	util.toolWin->PostMessage(new BMessage(BUTTON_05_MSG));	tool_has_changed=true;break;	//Rectangle
				case 'e':	util.toolWin->PostMessage(new BMessage(BUTTON_06_MSG));	tool_has_changed=true;break;	//ELLIPSE
				case 't':	util.toolWin->PostMessage(new BMessage(BUTTON_07_MSG));	tool_has_changed=true;break;	//TEXT
				case 'z':	util.toolWin->PostMessage(new BMessage(BUTTON_08_MSG));	tool_has_changed=true;break;	//ZOOM

				//DROITE	   
 		   		case 'i': util.toolWin->PostMessage(new BMessage(BUTTON_B_01_MSG));		tool_has_changed=true;	break; //PICKER
 		   		case 's': util.toolWin->PostMessage(new BMessage(BUTTON_B_02_MSG));		tool_has_changed=true;	break; 	//Stamp
				case 'w':	util.toolWin->PostMessage(new BMessage(BUTTON_B_03_MSG));	tool_has_changed=true;		break;	//wand
				case 'l':	util.toolWin->PostMessage(new BMessage(BUTTON_B_04_MSG));	tool_has_changed=true;	break;	//Lasso
				case 'm':	util.toolWin->PostMessage(new BMessage(BUTTON_B_05_MSG));	tool_has_changed=true;	break;	//Rect Sel
				case 'M':	util.toolWin->PostMessage(new BMessage(BUTTON_B_06_MSG));	tool_has_changed=true;	break;	//Ellipse sel
				case 'q':	util.toolWin->PostMessage(new BMessage(BUTTON_B_07_MSG));	tool_has_changed=false;	break;	//QUICK MASK
				case 'h':	util.toolWin->PostMessage(new BMessage(BUTTON_B_08_MSG));	tool_has_changed=true;	break;	//HAND_TOOL

 		      }//fin switch 

      	     	   
           if (tool_has_changed == true) PostMessage(new BMessage(TOOL_CHANGED));	  //pour si on a changé de tool
		
		
		
		
		
           //POUR MODIFIER KEYS
           switch ( key )
            { 
            BMessage *ms;
            
            	//MODES
           		case B_F1_KEY:
           		ms = new BMessage(MODE_CHANGED); ms->AddInt32("mode",NORMAL); 
				PostMessage(ms);
           		break; 
      			
      			case B_F2_KEY: 
           		ms = new BMessage(MODE_CHANGED); ms->AddInt32("mode",MULTIPLY);
				PostMessage(ms);
                break; 
                
                case B_F3_KEY: 
           		ms = new BMessage(MODE_CHANGED); ms->AddInt32("mode",LIGHTEN);
				PostMessage(ms);
                break; 
                
                case B_F4_KEY: 
           		ms = new BMessage(MODE_CHANGED); ms->AddInt32("mode",DARKEN);
				PostMessage(ms);
                break; 
                    
                case B_F5_KEY: 
           		ms = new BMessage(MODE_CHANGED); ms->AddInt32("mode",COLORIZE);
				PostMessage(ms);
                break; 
                
                case B_F6_KEY: 
           		ms = new BMessage(MODE_CHANGED); ms->AddInt32("mode",DIFFERENCE);
				PostMessage(ms);
                break; 
            
            	case B_F7_KEY: 
           		ms = new BMessage(MODE_CHANGED); ms->AddInt32("mode",COMBINE);
				PostMessage(ms);
                break; 
 				
 				//ARROWS
 				case B_PAGE_DOWN: break; 
 				case B_PAGE_UP: break; 
 				
           		case B_ENTER:	break;
           		case B_SPACE:	break;
           		case B_UP_ARROW:	break;	
           		case B_DOWN_ARROW:	break;
           		case B_RIGHT_ARROW:	break;
           		case B_LEFT_ARROW:	break;
           		            
 		      }//fin switch 
 }
 
void PhotonWindow::SetFullScreen()
{
	BRect screenRect = BScreen(this).Frame();

		switch(full_screen)
						{
							case FULL_SCREEN_NONE:
							Lock();
							SetLook(B_DOCUMENT_WINDOW_LOOK);
							ResizeTo(windowed_frame.Width(),windowed_frame.Height()); 
							MoveTo(windowed_frame.left,windowed_frame.top);
							scroll_view->SetBorder(B_PLAIN_BORDER);

							if (menu_bar_shown==false) 
								{
									AddChild(shared->menubar);
									menu_bar_shown=true; 
								}
							scroll_view->ResizeTo(Bounds().Width(),
												  Bounds().Height() 
												  - shared->menubar->Bounds().Height());
							scroll_view->MoveTo(0,shared->menubar->Bounds().Height());	
							back_view->SetViewColor(150,150,150);
							Unlock();
							break;
							
							case FULL_SCREEN_HALF:
							
							SetLook(B_NO_BORDER_WINDOW_LOOK);
							scroll_view->SetBorder(B_PLAIN_BORDER);

							ResizeTo(screenRect.Width(),screenRect.Height()); MoveTo(0,0);

							Lock();
								if (menu_bar_shown==false) 
								{
									AddChild(shared->menubar);
									menu_bar_shown=true; 
								}
							
							
							scroll_view->ResizeTo(Bounds().Width(),
												  Bounds().Height() 
												  - shared->menubar->Bounds().Height());
							scroll_view->MoveTo(0, shared->menubar->Bounds().Height());	
							back_view->SetViewColor(150,150,150);
							Unlock();
							break;
		
	
							case FULL_SCREEN_TOTAL:
							Lock();
							SetLook(B_NO_BORDER_WINDOW_LOOK);

							ResizeTo(screenRect.Width(),screenRect.Height()); MoveTo(0,0);
							if (menu_bar_shown==true) 
								{
									shared->menubar->RemoveSelf();
									menu_bar_shown=false; 
								}
							
							
							//on met les scrollbars en dehors de l'écan
							scroll_view->SetBorder(B_NO_BORDER);
							scroll_view->ResizeTo(Bounds().Width()+4 + B_V_SCROLL_BAR_WIDTH,
												 Bounds().Height()+4 + B_H_SCROLL_BAR_HEIGHT);
							scroll_view->MoveTo(-2,-2);	
							back_view->SetViewColor(0,0,0);

							Unlock();
							break;
						
						default: 
						break;
							
						}

}
 
 
void PhotonWindow::FrameResized(float width, float height)
{
//s'il est la view est plus petite on la move au centre...
	
	BPoint point = inside_view->LeftTop(); //pos_originale

	if (inside_view->Bounds().Width() < back_view->Bounds().Width())
		point.x = ((back_view->Bounds().Width()-inside_view->Bounds().Width() ) /2 );
	else point.x +=PAINT_MARGIN;
	
	if (inside_view->Bounds().Height() < back_view->Bounds().Height())
		point.y = ((back_view->Bounds().Height()-inside_view->Bounds().Height() ) /2) ;
	else point.y +=PAINT_MARGIN;
	
	inside_view->MoveTo(point);


	//taille des scrollbars--------------------------------------------------------------------------------------------------
	
	if (shared->act_img!=NULL)
	{
	int32 to_add_hori =0;
	int32 to_add_verti =0;
	
	switch(full_screen)	{
							case FULL_SCREEN_NONE:
							case FULL_SCREEN_HALF:
							to_add_hori  +=(int32) B_V_SCROLL_BAR_WIDTH; 
							to_add_verti += (int32) B_H_SCROLL_BAR_HEIGHT;
							to_add_verti += (int32) shared->menubar->Bounds().Height();
							break;
		
							case FULL_SCREEN_TOTAL:
							break;					
						}
	
	if (shared->image_amount>0)
	{	
	BScrollBar *sch =  scroll_view->ScrollBar(B_HORIZONTAL);
	if (sch!=NULL ) 
		{
		sch->SetRange(0, ((shared->act_img->pix_per_line) *  shared->act_img->zoom_level)-width + (PAINT_MARGIN*2*  shared->act_img->zoom_level)+ to_add_hori);
		}
				 
	BScrollBar *scv =  scroll_view->ScrollBar(B_VERTICAL);
	if (scv!=NULL) 
		{
		scv->SetRange(0, ((shared->act_img->pix_per_row) *  shared->act_img->zoom_level)-height + (PAINT_MARGIN*2*  shared->act_img->zoom_level)+ to_add_verti);
		}
	}		

	}
	//--------------------------------------------------------------------------------------------------

	if (full_screen == FULL_SCREEN_NONE) windowed_frame = Frame(); //pour après le retour du mode full_screen
							
}
 
void PhotonWindow::DisableAll()
{
 //disable tous les menus sauf le premier (file) et le dernier help
 int16 i=1;
 do
 { shared->menubar->ItemAt(i)->SetEnabled(false);
 } while (i++!=shared->menubar->CountItems()-4);

 shared->close_item->SetEnabled(false);
 shared->revert_item->SetEnabled(false);
 shared->save_item->SetEnabled(false);
 shared->save_as_item->SetEnabled(false);

 inside_view->ResizeTo(0,0); //pour pas que la vue s'affiche en blanc avec la taille de la dernière pic
	
}


void PhotonWindow::EnableAll()
{
 //réactive tout
 int16 i=1;
 do
 { shared->menubar->ItemAt(i)->SetEnabled(true);
 } while (i++!=shared->menubar->CountItems()-2);

 //elements de file à enabler sépareemmnt
 shared->close_item->SetEnabled(true);
 shared->revert_item->SetEnabled(true);
 shared->save_item->SetEnabled(true);
 shared->save_as_item->SetEnabled(true);

}

