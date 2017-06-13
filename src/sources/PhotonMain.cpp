#include "PhotonMain.h"

main()
{	

	PhotonApplication	myApplication;
	
	myApplication.Run();

	return(0);
}

PhotonApplication::PhotonApplication()
		  		  : BApplication("application/x-vnd.beunited-photon")
{

	//--------------------------------
	//Avant tout, loader les prefs et charger le language file

	ThePrefs.ResetPrefs(); 
	ThePrefs.Load(); //faut le reset d'abord, on en charge que certains...
	
	Language.InitLang(); //Chargement language...

	//PostMessage(MENU_HELP_ABOUT);
	//on crée direct la win car le message arrive trop tard pour le chargement qui est pas long...
	BRect r= util.find_win_pos_on_main_screen(400,300+12);
	ab_win = new AboutWindow(r,shared);
	ab_win->Show();
	
	//--------------------------------


	shared = new share(); //pour sous fonctions 

	BRect rect;
	// set up a rectangle and instantiate a new window
	
	rect = util.find_win_pos_on_main_screen(256,48);
	progWindow = new ProgressWindow(rect,"Progress...");
	util.progress_win = progWindow;
	progWindow->Show();
	
	rect.Set(8, 24, 8+START_X_SIZE-1, 24+START_Y_SIZE-1);
	mainWindow = new PhotonWindow(rect,shared);
 	mainWindow->PostMessage(new BMessage(TOOL_CHANGED)); 
	mainWindow->PostMessage(new BMessage(TRANSP_CHANGED)); 	
	mainWindow->Zoom();
	mainWindow->Show();
	
	mainWindow->PostMessage(HIDE_PROGRESS_WIN);

	
	//CA SE FAIT DANS Mainwin même sinon la premiere pic créée peut pas envoyer des messages à la win..
	util.mainWin=mainWindow; //pointeur vers fenetre principale histoire de pouvoir lui envoyer des msg
	
	if (Image_Init(NULL) != B_OK)
	{
	
		BAlert *alert = new BAlert(NULL,Language.get("NO_IMAGE_MANIP_LIB"), 
                     Language.get("YES"),NULL,NULL,B_WIDTH_FROM_WIDEST,B_WARNING_ALERT); 
   		alert->Go(NULL); //null pour pas qu'il arrête le reste
	}

//ZOOMCHANGED FAIT TOUT PLANTER!!!!!!!!!!
//	mainWindow->ZoomChanged(); 

	
	mainWindow->PostMessage(UPDATE_TITLE);
	
	
		BMessage *mx = new BMessage(SET_CURSOR);
		mx->AddInt32("id",6); //6 = cursor_simple_cross
		mainWindow->PostMessage(mx);
		mainWindow->Activate();
	
	ab_win->Lock();
	ab_win->Close();
}

void PhotonApplication::ArgvReceived(int32 argc, char **argv)
{
//argv is a pointer to the strings and argc is the 
//number of strings in the array
int32 i = 1; 	  argv++; //on saute le premier c'est le nom de l'exe!

char nm[NAME_SIZE];

	  //ATTENTION TANT QU'ON a pas crée une pic vide on peut pas charger les autres
	  //c'est un bug, mais pour l'instant on en crée une..
//	  shared->CreateNewImage(BRect(0,0,64,64),NULL,NULL,OFF);	
	  	 
	 do
	 {
	  sprintf(nm,*argv);
//	  printf("\n\n**** ");  printf(*argv);
	  shared->LoadNewImage(nm);	
	  argv++;
      i++;
	 }while (i < argc); //pas egal! comme ça il fait just le nécessaire

	
			mainWindow->Lock();
			mainWindow->ZoomChanged();
			mainWindow->Unlock();

	// Shutdown the image manipulation library
	Image_Shutdown();
}
      
void PhotonApplication::RefsReceived(BMessage *le_message)
{

			//**************************************************************
			// LOAD
			//**************************************************************
	
	
	   uint32 type; 
       int32 count; 
       entry_ref ref; 
       
       le_message->GetInfo("refs", &type, &count); 
	   				
	  		 
			for ( long i = --count; i >= 0; i-- )
	  		{
			le_message->FindRef("refs", i, &ref); 
			   			  		
			BEntry entry(&ref);
			BPath the_path; 
			char txt[NAME_SIZE];
			entry.GetPath(&the_path);
			sprintf(txt,the_path.Path());
			
			//on vérifie si l'image est pas déjà chargée somewhere
			int16 ii=-2; //si on commence à zero et que le num de la pic est zéro la boucle se termine jamais alors on fait deux tours dans le vide
			int16 exists= -100; //aucune pic a ce chiffre
			do {
				//si l'image existe on avertit the user...
				if (shared->les_images[i] != NULL)
				{if (strcmp(shared->les_images[i]->full_path,txt)==0) exists =i; }
			} while (ii++ != shared->image_amount);
			
			if (exists != -100) //si on en a trouvé une
			{
				
				
				BAlert *alert = new BAlert(NULL,Language.get("ALREADY_LOADED"), 
                     Language.get("YES"),  Language.get("NO"),NULL,B_WIDTH_FROM_WIDEST,B_WARNING_ALERT); 
   				int32 button_index;
   				
   
   				switch(button_index = alert->Go())
   					{
   					//les boutons ont toujours les valeurs 0,1,2 (trois boutons max)
   					case 1:	//on active celle qui est déjà chargée
   					shared->win_menu->ItemAt(exists)->SetMarked(true);
   					shared->active_image = exists;
					shared->initPic();
   					break;
					
					case 0:   //on charge une nouvelle copie
					util.show_progress_load = ON;
				//	util.mainWin->PostMessage(SHOW_PROGRESS_WIN);

					shared->LoadNewImage(txt);	 //revert mode =OFF
					break;
					}
			}
			
			else 
			{
			util.show_progress_load = ON;
			//util.mainWin->PostMessage(SHOW_PROGRESS_WIN);

			shared->LoadNewImage(txt);	 //revert mode =OFF
			}

			mainWindow->Lock();
			mainWindow->ZoomChanged();
			mainWindow->Unlock();
					
	}					
}

//-------------------------------------------------

void PhotonApplication::MessageReceived(BMessage *msg)
{
BRect r;

	switch (msg->what)
	{
		case B_REFS_RECEIVED:
		beep();
		break;
		
		case B_SAVE_REQUESTED:
		SaveImg(msg); //recupère le nom du BMessage et le sauve
		break;
		
		case SAVE_WITH_NAME:
		SaveImg(NULL); //si NULL on sauve sous le même nom
		break;


	
		case B_KEY_DOWN:
		mainWindow->PostMessage(msg);
		break;
		
		case MENU_HELP_ABOUT:
		//320x240 au centre de l'écran
		r= util.find_win_pos_on_main_screen(400,300+12);
		ab_win = new AboutWindow(r,shared);
		ab_win->Show();
		break;
			
	}
	
}


//-------------------------------------------------


//-------------------------------------------------------------

void PhotonApplication::SaveImg(BMessage *msg)
{

char txt[NAME_SIZE];

if (msg != NULL) //si msg est pas null c'est qu'on a reçu un nouvel emplacement
		{
				
		const char *Filename; // Nom du fichier a sauver CONST IMPORTANT
		entry_ref Directory; // Ref sur le r�pertoire dans lequel sauver

		msg->FindRef( "directory", &Directory );
		msg->FindString( "name",  &Filename );
		
		BEntry entry(&Directory);
		BPath the_path; 
		entry.GetPath(&the_path);
		
		sprintf(txt,the_path.Path());
		strcat(txt,"/");
		strcat(txt,Filename);
		
		//si on choisi un titre (nouveau fichier faut updater)
		sprintf(shared->act_img->full_path,txt);
		mainWindow->PostMessage(new BMessage(UPDATE_TITLE)); 
		}
		
else //sinon c'est qu'on update le fichier existant
		{
		sprintf(shared->act_img->full_path,txt);
		}	

	
		//faut récupérer le ref à partir du path...
		BEntry full_path;
		full_path.SetTo(txt);
		entry_ref ref;
		full_path.GetRef(&ref);
		
		BPath pth(txt);

		sprintf(shared->act_img->full_path,txt);
		sprintf(shared->act_img->name,pth.Leaf());


	//ON SAUVE	
	//	util.StoreTranslatorBitmap(shared->act_lay->img, txt,shared->active_translator) ;


//ThePrefs.no_pictures_left=ON; //pour désactiver affich...

//on peut pas utiliser direct le display bitmap sinon crash... alors LOCK
shared->act_img->display_bitmap->Lock();

if (util.StoreTranslatorBitmap(shared->act_img->display_bitmap, 
				txt, shared->active_translator) == B_OK) 
{

	if (ThePrefs.save_with_thumb==B_CONTROL_ON) //si on acoché la case
		  {
			if (shared->thumbnail->Bitmap())    // will be NULL if the thumbnail couldn't be created 
				{ 
	 		  	BNode node(&ref); 
    			shared->thumbnail->WriteThumbnailAttribute(&node); 
    			shared->thumbnail->WriteIconAttribute(&node); 
    			shared->thumbnail->WriteMiniIconAttribute(&node); 
    			shared->thumbnail->WriteResolutionAttributes(&node); 
				} 
		  }
	}
	
shared->act_img->display_bitmap->Unlock();

mainWindow->PostMessage(UPDATE_TITLE);
mainWindow->PostMessage(UPDATE_WIN_MENU);
mainWindow->PostMessage(ZOOM_CHANGED);
}

