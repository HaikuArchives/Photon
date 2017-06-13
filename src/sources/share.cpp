#include "share.h"

BubbleHelper bb_help; //en extern

//CONSTRUCTEUR share
share::share()
{
//SETTINGS
act_img=NULL;
active_image=-1;
image_amount=-1; //a la création on met la première à zéro
   
stamp_offset = BPoint (64,64);
stamp_offset_changed = OFF;
paint_mode=NORMAL;
paint_transparency=0; //valeurs de 0 à 1 (1=100%)

sprintf(nom_de_brush_perso,"no name for this brush...");


//Ces valeurs doivent être initialisées avant la création de la pic même si elles 
//sont remplacées lors de celle-ci, b-coz l'affichage en a besoin avant...
 
pattern st = { 0xc7, 0x8f, 0x1f, 0x3e, 0x7c, 0xf8, 0xf1, 0xe3 };
stripes = st;

needs_update = false;
first_zone = true;
   
   
stroke=OFF;

current_brush=16;
current_perso_brush=0;
current_paper=0;
max_brush=2;
max_paper=2;


active_tool=PAINTBRUSH;


rgb_color a = {0,0,0,0};
rgb_color b = {255,255,0,0};

fore_color = a;
back_color = b;


	
//cursor_ptr = &simple_cross[0];

active_translator=0;

hardness =0;


brush_is_perso=false;
}





void share::EmptyPaper()
{
//crée un papier de 24 x 24...
BBitmap *temp = new BBitmap(BRect(0,0,24-1,24-1),B_RGB32);
UseAsPaper(temp);

//SURTOUT PAS delete temp; il est utilisé!

}			


void share::CreateNewImage(const char *le_nom, int16 w, int16 h) //crée ou charge une nouvelle pic
{
/*
int8 stop_it = OFF;	
int16 was_active = active_image; //pour restaurer en cas de besoin
if (was_active <0) was_active= 0; if (was_active > image_amount) was_active = 0; //si jamais la première fois..
*/
	ThePrefs.no_pictures_left=ON; //pour arrêter l'affichage pendant le chargement

	image_amount++;
	active_image=image_amount;
	les_images[active_image]= new PhotonImage(le_nom,w,h); 
	act_img = les_images[active_image];
	act_img->CreateNewLayer(NULL);
	act_lay = act_img->the_layers[act_img->layer_amount];
	
	ImageAdded();
	
}	

void share::LoadNewImage(const char *fichier) //crée ou charge une nouvelle pic
{
ThePrefs.no_pictures_left=ON; //pour arrêter l'affichage pendant le chargement
					 //c'est réactivé dans initpic
/*
int8 stop_it = OFF;	
int16 was_active = active_image; //pour restaurer en cas de besoin
if (was_active <0) was_active= 0; if (was_active > image_amount) was_active = 0; //si jamais la première fois..
*/
	image_amount++;  active_image=image_amount;
	les_images[active_image]= new PhotonImage("",32,32); 
	act_img = les_images[active_image];
	if (act_img->LoadNewLayer(fichier) != B_ERROR)
	{
		act_lay = act_img->the_layers[act_img->layer_amount];
		ImageAdded();
	}
	else DeleteImage(image_amount);

}	

void share::NewImageFromBmp(const char *nm, BBitmap *pic) //crée ou charge une nouvelle pic
{

/*
int8 stop_it = OFF;	
int16 was_active = active_image; //pour restaurer en cas de besoin
if (was_active <0) was_active= 0; if (was_active > image_amount) was_active = 0; //si jamais la première fois..
*/

ThePrefs.no_pictures_left=ON; //pour arrêter l'affichage pendant le chargement

	image_amount++;
	active_image=image_amount;
	les_images[active_image]= new PhotonImage(nm,32,32); 
	act_img = les_images[active_image];
	act_img->NewLayerFromBmp(pic);
	act_lay = act_img->the_layers[act_img->layer_amount];
	
	ImageAdded();
	
	
	
}	

void share::DeleteImage(int32 which_one)
{
ThePrefs.no_pictures_left=ON; //désactive_l'affichage
bool reactivate =false;

			util.mainWin->Lock();
			if (win_menu->ItemAt(which_one)!=NULL)	win_menu->RemoveItem(which_one);

		PhotonImage *old_active =les_images[which_one]; //pour deleter après
								
		if (which_one!=image_amount) //si c'est pas la dernière
			{
			reactivate=true;
			//décalage des pics
			int16 i=which_one; //si on commence à zero et que le num de la pic est zéro la boucle se termine jamais alors on fait deux tours dans le vide
			
			do {
				    if (les_images[i+1]!=NULL)	les_images[i] = les_images[i+1];   
				    i++;
			    } while (i!= image_amount);
				
			if (win_menu->ItemAt(which_one)!=NULL) win_menu->ItemAt(which_one)->SetMarked(true);
			}
		else 
				which_one=image_amount-1; 	//donc si c'est la dernière	
			
			image_amount--;
			
			if (image_amount <0) {ThePrefs.no_pictures_left=ON; util.mainWin->PostMessage(DISABLE_ALL);}
			else win_menu->ItemAt(which_one)->SetMarked(true);
			
			delete old_active;		

			if (reactivate==true) ThePrefs.no_pictures_left=OFF; //réactive l'affichage

			util.mainWin->PostMessage(IMAGE_CHOSEN); //pour redessiner l'actuelle
			util.mainWin->Unlock();


}
  				

void share::ImageAdded() //crée ou charge une nouvelle pic
{


	
 	act_img->undo_bitmap = new BBitmap(act_lay->img->Bounds(),B_RGB32,true,false); //accept views
	
	act_img->mask_bitmap 	   = new BBitmap(act_lay->img->Bounds(),B_GRAY8); //pic en niveau de gris
	act_img->mask_undo_bitmap  = new BBitmap(act_lay->img->Bounds(),B_GRAY8,true,false); //accept views 
	act_img->display_bitmap 	   = new BBitmap(act_lay->img->Bounds(),B_RGB32); //en rgb pour display (avec vraie pic desous en transp) 
		
	initPic();

	act_img->DeleteMask(OFF);
	act_img->UpdateDisplayImg(act_img->undo_bitmap->Bounds());
	
	win_menu->AddItem(new BMenuItem(act_img->name,   new BMessage(IMAGE_CHOSEN)));
	win_menu->ItemAt(active_image)->SetMarked(true);
	
	 
}	


//------------------------

void share:: CopyBitmaptoWork() 
{
  			//ON COPIE L'image chargée normale dans la work, c'est plus speed (selon...) que de charger 2x.
  			uint8 *dest  = (uint8 *) act_img->undo_bitmap->Bits();
  			uint8 *src = (uint8 *)   act_lay->img->Bits();
			uint32 taille = act_img->undo_bitmap->BitsLength();
  			
  			memcpy(dest,src,taille);
}

//------------------------

void share::initPic() //effectué à création, chargement et changement d'image active
{

	act_img = les_images[active_image];
	act_lay = act_img->the_layers[act_img->active_layer];

	
	//si on fait plus un là il y une merde de décalage mais à mon avis
	//le problème est ailleurs vu que là il devrait pas y avoir autrement de +1
	act_img->pix_per_line = int16 (act_lay->img->Bounds().Width()+1);
	act_img->pix_per_row  = int16 (act_lay->img->Bounds().Height()+1);


	initLayer();

	SetManipMenus(); //faut le refaire car c'est lui qui détermine si le act_lay support les filtres X ou y
        
	ThePrefs.no_pictures_left=OFF; 
	util.mainWin->PostMessage(ENABLE_ALL);
	
	util.mainWin->PostMessage(new BMessage(PIC_CREATED));
	util.mainWin->PostMessage(new BMessage(ZOOM_CHANGED));
	 

	//C'est beaucoup plus simple de le refemer et de l'ouvrir...
	//sinon c trop la merde.
	if (ThePrefs.layer_selector_open==true)
	{
	util.layerWin->Lock();
	util.layerWin->Close();
	util.mainWin->PostMessage(SHOW_LAYER_WIN);
	
	}
	

	/*
		BMessage x(INIT_PERSO);
		x.AddInt32("active",0);
		util.layerWin->PostMessage(&x);
	*/	

	
}

void share::initLayer() //effectué à création, chargement et changement d'image active
{
	ThePrefs.no_pictures_left=ON; //pour arrêter l'affichage pendant le chargement
	
	//ON REGLE LES POINTEURS SUR CELLES-CI UNE FOIS POUR TOUTES
    the_bitmap_bits    = (uint8*) act_lay->img->Bits();
	undo_bitmap_bits   = (uint8*) act_img->undo_bitmap->Bits();
	mask_bits 		   = (uint8*) act_img->mask_bitmap->Bits();
	mask_work_bits     = (uint8*) act_img->mask_undo_bitmap->Bits();


	CopyBitmaptoWork();
	act_img->UpdateDisplayImg(act_img->the_layers[0]->img->Bounds());
	
	ThePrefs.no_pictures_left=OFF; //pour réactiver le display


	//update palette des layers si nécessaire	
	if (ThePrefs.layer_selector_open==true)
	{
		BMessage msg(DRAW_LAYERS);
		msg.AddInt32("active",act_lay->id);
		util.layerWin->PostMessage(&msg);		
	}		

/*
	BMessage tmp(UPDATE_ME);
	tmp.AddRect("zone",BRect(0,0,act_img->pix_per_line-1,act_img->pix_per_line-1));
 	util.mainWin->PostMessage(&tmp);
 	*/
 	
}
    
void share::loadBrush(int16 numbrush) //load la brush num_x
{ 

	char name[NAME_SIZE];
	char tmp[255];
	
	sprintf(name,util.dossier_app);
	strcat(name,"/brushes/");
	
	//trouver nb de brushes dans le dossier
	BDirectory the_dir; 
	the_dir.SetTo(name);	
	max_brush = the_dir.CountEntries();
//	printf("\nnb brushes: %d",max_brush);

	//pas dépasser les bornes!
	if (numbrush <= 0) 	numbrush = 0;
	if (numbrush >= max_brush) numbrush = max_brush;
	current_perso_brush = numbrush;
	
	BEntry the_entry;	

	uint16 i = 0;
	do { the_dir.GetNextEntry(&the_entry,false);   i++;  } while (i!=current_perso_brush+1);
	the_entry.GetName(tmp);
	strcat (name,tmp);
	//---------------------------
	
	BBitmap *temp_32;
    if ( (temp_32 = util.load_bmp(name)) != NULL) 
    { UseAsBrush(temp_32);
 	  sprintf(nom_de_brush_perso,tmp);
    }  //loade la pic en 32 //convertit en 8 bits... 
    
   	brush_is_perso=true;

  }
  //------------------------------------

    
  void share::loadPaper(int16 numpaper) //load le paper num_x
{ 
	char name[NAME_SIZE];
	char tmp[255];
	
	sprintf(name,util.dossier_app);
	strcat(name,"/papers/");
	
	//trouver nb de brushes dans le dossier
	BDirectory the_dir; 
	the_dir.SetTo(name);	
	max_paper = the_dir.CountEntries();

	//pas dépasser les bornes!
	if (numpaper <= 0) 	numpaper = 0;
	if (numpaper >= max_paper) numpaper = max_paper;
	current_paper = numpaper;
	
	
	BEntry the_entry;	

	uint16 i = 0;
	do { the_dir.GetNextEntry(&the_entry,false);   i++;  } while (i!=current_paper+1);
	the_entry.GetName(tmp);
	strcat (name,tmp);
	//---------------------------
	
	BBitmap *temp_32;
    if ( (temp_32 = util.load_bmp(name)) !=NULL) 
    { UseAsPaper(temp_32);
 	  sprintf(nom_paper,tmp);
    }  //loade la pic en 32 //convertit en 8 bits... 
	    
}
  //------------------------------------
  
void share::generateBrush(int16 size) //génére brush circulaire de size x size
{ 
	brush_is_perso=false;

	if (size < 1) size=1;
	current_brush = size;
	
	BRect rect; 
	rect.Set(0,0,size-1,size-1);
	
	BBitmap *tmp_brush = new BBitmap(rect,B_RGB32,true);	//en 32 bits, true acceptviews...

	tmp_brush->Lock(); //important!

	BView virtualView( tmp_brush->Bounds(), NULL, B_FOLLOW_NONE, 0 );
	tmp_brush->AddChild( &virtualView );


   //---------------------------------------------------
   //DESSIN DE LA BRUSH!
   float tmp_size = floor(size/2); //taille DU RAYON PAS DU DIAMETRE
   int16 val = 255;
   float center = tmp_size;
   float step = 255/size;
   hardness=100;
   step *= 1 * (4*float(hardness)/100); //si le step est plus grand on devient noir avant d'atteindre le centre...
  
  BPicture *my_pict; 
  virtualView.BeginPicture(new BPicture); 

   do {
  		 virtualView.SetHighColor(val,val,val);
   		 virtualView.FillEllipse(BPoint(center,center),tmp_size,tmp_size,B_SOLID_HIGH);
   		 val -= (int16) step;   if (val < 0) val=0;
   		 tmp_size--;
   	   } while (tmp_size!=-1);
 	
   my_pict = virtualView.EndPicture();
 
   virtualView.DrawPicture(my_pict);
	//fin du dessin de la brush
   //---------------------------------------------------

   tmp_brush->RemoveChild( &virtualView );	// A faire avant la destruction de la bitmap sinonm problemes
   tmp_brush->Unlock();
	
  sprintf(nom_de_brush_perso,"pas de nom de brush");
  UseAsBrush(tmp_brush);   //convertit en la 32 en 8 bits (celle qui est utilisée à la fin) 
	
 // SURTOUT PAS delete tmp_brush;
 //vu que useasbrush utilise la pic générée et la garde en brush_32
  }
  //------------------------------------
  
  
//---------------------------------------------------------

rgb_color share::PickColorAt(BPoint pix) //DANS LE CARRE
{

uint32 pos_bits = (uint32) ((pix.x+(act_img->pix_per_line*pix.y))*4); //seul y compte c'est pareil sur toute la ligne
uint8	*p_bits = undo_bitmap_bits +pos_bits;//avance jusqu'au pixel en question
rgb_color col;
col.blue=*p_bits;  	p_bits++;
col.green=*p_bits;  	p_bits++;
col.red=*p_bits;  
//col.alpha=*bits;  
return col;
}

//---------------------------------------------------------


uint8 share::PickMaskColorAt(BPoint pix) //DANS LE CARRE
{

	uint32 pos_bits;
	pos_bits = uint32((pix.x+((act_img->pix_per_line)*pix.y))); //seul y compte c'est pareil sur toute la ligne
	uint8 *p_bits = mask_work_bits ;
	p_bits += pos_bits;//avance jusqu'au pixel en question

	uint8 val = *p_bits;
	return val;

}

//---------------------------------------------------------



void share::UseAsBrush(BBitmap *temp_32)
{

    the_brush_24 = new BBitmap(temp_32); 
    
    uint8 *tmp_ptr,*dest_ptr;

	BRect rect = (temp_32->Bounds()); //même taille
	the_brush = new BBitmap(rect,B_GRAY8);	//mais en 8 bits
	

    tmp_ptr = (uint8*) temp_32->Bits();
	dest_ptr= (uint8*) the_brush->Bits();

    //-------------------------------
	//ON COPIE LA 32 bits dans du 8 bits...
	uint16 moyenne=0;
	uint32 pos=0;
	uint32 taille=the_brush->BitsLength();
	while (pos!=taille)
		{
		moyenne  = 0;
		moyenne += *tmp_ptr; tmp_ptr++; //bleu
		moyenne += *tmp_ptr; tmp_ptr++;	//vert
		moyenne += *tmp_ptr; tmp_ptr++;	//rouge
		tmp_ptr++;						//alpha sauté
		moyenne/=3; //moyenne des trois composantes
		
		*dest_ptr = (uint8) moyenne;
		dest_ptr++; //8 bits un seul octet
		pos++;
		}
    //-------------------------------

    the_brush_bits = (uint8 *)the_brush->Bits();
	brush_x = uint16 (the_brush->Bounds().Width()  + 1);
	brush_y = uint16 (the_brush->Bounds().Height() + 1);
 	
 	brush_rayon_x = (brush_x/2)-1;
 	brush_rayon_y = (brush_y/2)-1;
 	 
 	if (util.toolWin!=NULL) util.toolWin->PostMessage(new BMessage(BRUSH_CHANGED)); 
	
}

void share::UseAsPaper(BBitmap *temp_32)
{

	//idem que use as brush mais bon..
    paper_24 = temp_32; 
    
	uint8 *tmp_ptr,*dest_ptr;

	BRect rect = (temp_32->Bounds()); //même taille
	paper = new BBitmap(rect,B_GRAY8);	//mais en 8 bits

    tmp_ptr = (uint8*) temp_32->Bits();
	dest_ptr= (uint8*) paper->Bits();

    //-------------------------------
	//ON COPIE LA 32 bits dans du 8 bits...
	uint16 moyenne=0;
	uint32 pos=0;
	uint32 taille=paper->BitsLength();
	while (pos!=taille)
		{
		moyenne  = 0;
		moyenne += *tmp_ptr; tmp_ptr++; //bleu
		moyenne += *tmp_ptr; tmp_ptr++;	//vert
		moyenne += *tmp_ptr; tmp_ptr++;	//rouge
		tmp_ptr++;						//alpha sauté
		moyenne/=3; //moyenne des trois composantes
		
	
		*dest_ptr = (uint8) moyenne;
		dest_ptr++; //8 bits un seul octet
		pos++;
		}
    //-------------------------------

    paper_bits = (uint8 *)paper->Bits();
	paper_x = uint16 (paper->Bounds().Width()  + 1);
	paper_y = uint16 (paper->Bounds().Height() + 1);
 	 
 if (util.toolWin!=NULL) util.toolWin->PostMessage(new BMessage(PAPER_CHANGED)); 
	
}



BRect share::FindSelectedRect()
//trouve la zone ou le mask est actif
{

	BRect limit;
	uint8 val;
	//valeurs inverses par défaut, comme ça on peut revenir en arrière pour trouver bord
	limit.top    = act_img->pix_per_row-1;
	limit.left   = act_img->pix_per_line-1;
	limit.bottom = 0;
	limit.right  = 0;


	BPoint le_point;
	le_point.Set(0,0);
	
	while(le_point.y != act_img->pix_per_row) //pas PIC_SIZE-1 sinon il fait pas le dernier
		{
						
					while(le_point.x!= act_img->pix_per_line) //chaque pixel
					{	
					    val = PickMaskColorAt(le_point);
					    
						//si la valeur est pas à zéro
						 if (val > 0) 
							{	
						    	if (le_point.x < limit.left)  limit.left = le_point.x;
								if (le_point.y < limit.top)   limit.top  = le_point.y;
							
								if (le_point.x > limit.right)	limit.right  = le_point.x;
								if (le_point.y > limit.bottom)	limit.bottom = le_point.y;
							}
					    le_point.x++; //avance dans l'horizontal
					}

		le_point.x=0; //on se remet au début de la ligne		
		le_point.y++; //et on passe à la suivante
		}
//--fin recherche rectangle

//Si les positions sont identiques à celles du début ça veut dire que rien est sélectionné, donc tout!
if (
	   limit.top    == act_img->pix_per_row-1
	&& limit.left   == act_img->pix_per_line-1
	&& limit.bottom == 0
	&& limit.right  == 0   ) 
	
	{
	limit.top    = 0;
	limit.left   = 0;
	limit.bottom = act_img->pix_per_row-1;
	limit.right  = act_img->pix_per_line-1;
	}

return limit;
}





void share::ImageManip(BBitmap *bit)
{
	mBitmapInUse = true;
	
	// Create bitmap accessor object
	BBitmapAccessor src_accessor(bit);
	src_accessor.SetDispose(false);

	// Flippin' do it!
	Image_Manipulate(mAddonId, &src_accessor, 0);
	mBitmapInUse = false;
}


void share::ImageConvert(BBitmap *bit)
{
	mBitmapInUse = true;
	// Create bitmap accessor object
	BBitmapAccessor src_accessor(bit);
	src_accessor.SetDispose(false);

	// Create empty bitmap accessor object for the destination
	BBitmapAccessor dest_accessor;

	// Do image conversion
	Image_Convert(mAddonId, &src_accessor, &dest_accessor, NULL);

	// Got valid new bitmap?
	if (dest_accessor.IsValid())
	{
		if (dest_accessor.Bitmap()->ColorSpace()==B_RGB32)
		{
		char str[NAME_SIZE];
		sprintf(str,"(");
		strcat(str,Language.get("FILTERED"));
		strcat(str,")");
		NewImageFromBmp(str,dest_accessor.Bitmap()); //crée new pic à partir de bbitmap le reste (file ou rect) = NULL
		// Don't dispose it
		dest_accessor.SetDispose(false);
		}
		else
		{
		
		BAlert *alert = new BAlert("",Language.get("DEST_NOT_B_RGB32"), Language.get("YES"),NULL,NULL,
						   B_WIDTH_FROM_WIDEST,B_WARNING_ALERT); 
   		alert->Go(NULL);
		}
	} 

	mBitmapInUse = false;
}


void share::SetManipMenus()
{
util.mainWin->Lock();	

	manip_menu->RemoveItems(0,manip_menu->CountItems());
	convert_menu->RemoveItems(0,convert_menu->CountItems());

	// Create bitmap accessor object to pass along, so that we
	// only get the add-ons that support this bitmap in the menu
	BBitmapAccessor accessor(act_lay->img);
	accessor.SetDispose(false);

	
	const char *addonName;
	const char *addonInfo;
	const char *addonCategory;
	int32 addonVersion;
	image_addon_id *outList;
	int32 outCount = 0;
	if (Image_GetManipulators(&accessor, NULL, &outList, &outCount) == B_OK)
	{
		for (int i = 0; i < outCount; ++i)
		{
			if (Image_GetAddonInfo(outList[i], &addonName, &addonInfo,
					&addonCategory, &addonVersion) == B_OK)
			{
				BMessage *msg = new BMessage(IMAGE_MANIP);
				msg->AddInt32("addon_id", outList[i]);
				manip_menu->AddItem(new BMenuItem(addonName, msg));
			}
		}
		delete[] outList;
	}
	if (outCount == 0)
		manip_menu->SetEnabled(false);

	// Create sub menu with image converters
	outCount = 0;
	if (Image_GetConverters(&accessor, NULL, &outList, &outCount) == B_OK)
	{
		for (int i = 0; i < outCount; ++i)
		{
			if (Image_GetAddonInfo(outList[i], &addonName, &addonInfo,
					&addonCategory, &addonVersion) == B_OK)
			{
				BMessage *msg = new BMessage(IMAGE_CONV);
				msg->AddInt32("addon_id", outList[i]);
				convert_menu->AddItem(new BMenuItem(addonName, msg));
			}
		}
		delete[] outList;
	}
	if (outCount == 0)
		convert_menu->SetEnabled(false);


util.mainWin->Unlock();	

		
}

void share::AddToUpdateZone(BRect r)
{
   needs_update = true;
   
   //si c'est le premier on prend le rectangle direct
   if (first_zone==true) 
    {
	   what_to_update = r;
   	   first_zone = false;
   	}
   else
    {
   		what_to_update = what_to_update | r ; //union des deux rectangles
   	}	

}

