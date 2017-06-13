#include "PhotonImage.h"

//----------------------------------------------------------------------------------------


Layer::Layer(const char *nm, BBitmap *pic, int32 le_id, int32 the_type)
{
	sprintf(name,nm);
	img = pic;
    id = le_id;
	
	draw_mode= NORMAL;
	is_visible = true;
	alpha_activated = false;
	opacity = 100;
	
	if (id==0) active =true;
	else active = false;
	
	x_guides_amount=0;
	y_guides_amount=0;

	guides_color.red = 10;
	guides_color.green = 230;
	guides_color.blue = 255;

	layer_type = the_type;	
	
	AddHoriGuide(32);
	AddHoriGuide(64);
	AddHoriGuide(96);
	AddVertiGuide(96);
	AddVertiGuide(21);
	AddVertiGuide(100);

}

Layer::~Layer()
{
delete img;
//delete name;

}

void Layer::AddHoriGuide(int16 pos)
{
	x_guides[x_guides_amount]=pos;
	x_guides_amount++;
}

void Layer::AddVertiGuide(int16 pos)
{
	y_guides[y_guides_amount]=pos;
	y_guides_amount++;
}
	
	
PhotonImage::PhotonImage(const char *nm, int16 width, int16 height)
{


	undo_data = new UndoData; //c'est une bête struct
	undo_data->undo_amount = -1;
	sprintf(full_path,"");

	layer_amount=-1; //0 c'est 1
	id_counter =-1;
	
	if (nm!=NULL) sprintf(name, nm); //layer minimum donc.
	else sprintf(name,Language.get("UNTITLED")); 
	
	
	active_layer = layer_amount;

	zoom_level = 1;
	is_redo=OFF;
	
	pix_per_line = width+1; 
	pix_per_row  = height+1;
	
	updating=false; //si 

units = UNIT_PIXELS;
res_units = DPI;
resolution = 72;

	
}

PhotonImage::~PhotonImage()
{
	PurgeUndo();
	delete undo_data;

	
	int32 x=0;
	while (x!= layer_amount+1) { delete the_layers[x]; x++; }
    
    delete undo_bitmap;
    delete mask_bitmap;
    delete mask_undo_bitmap;
	
}

void PhotonImage::SetUnitsResType(int32 un, float res, int32 res_type)
{

 units = un;
 res_units = res_type;
 resolution = res;


}

status_t PhotonImage::NewLayerFromBmp(BBitmap *pic)
{
	layer_amount++;
 	the_layers[layer_amount]  = new Layer(ComputeLayerName(NULL, LAYER_TYPE_BITMAP),
 								new BBitmap(pic),layer_amount,LAYER_TYPE_BITMAP);

	FinishLayer();

 	
return B_OK;
}


status_t PhotonImage::CreateNewLayer(const char *nm)
{
	layer_amount++;
 	the_layers[layer_amount]  = new Layer(ComputeLayerName(&nm,LAYER_TYPE_BITMAP),
 									new BBitmap(BRect(0,0,pix_per_line-1,pix_per_row-1),
 									B_RGB32), layer_amount,LAYER_TYPE_BITMAP);
	FinishLayer();

return B_OK;

}

status_t PhotonImage::CreateNewGuideLayer(const char *nm)
{
	layer_amount++;
 	the_layers[layer_amount]  = new Layer(ComputeLayerName(&nm,LAYER_TYPE_GUIDE),
 									NULL, layer_amount,LAYER_TYPE_GUIDE);
 	FinishLayer();
	
return B_OK;

}

status_t PhotonImage::LoadNewLayer(const char* nam_file)
{
	layer_amount++;
	return LoadLayer(ComputeLayerName(&nam_file, LAYER_TYPE_BITMAP));

return B_ERROR; //si loadlayer a pas marché

}	
	
status_t PhotonImage::LoadLayer(const char* nam_file)
{
	//load a layer_amount (donc revert si on a pas fait ++ avant)

	bool une_erreur = false;
	
	BAlert* alert;
	char str[NAME_SIZE];
   	BFile fich;
  	if (fich.SetTo(nam_file,B_READ_ONLY) != B_NO_ERROR)
  		{ 
  			sprintf(str,Language.get("COULD_NOT_LOAD"));
			strcat(str,nam_file);
		    alert = new BAlert("",str,"Sodomy!"); 
    		alert->Go();
    		une_erreur=true;
		 }
		 
  	else {
  			BBitmap *tmp; 
  			if (( tmp= util.load_bmp(nam_file) ) != NULL)
  			{
  				the_layers[layer_amount]    = new Layer(ComputeLayerName(NULL,LAYER_TYPE_BITMAP),tmp, layer_amount,LAYER_TYPE_BITMAP); 
 				active_layer = layer_amount;
				if (layer_amount==0)
				{
				pix_per_line = (uint16) tmp->Bounds().Width();
				pix_per_row =  (uint16) tmp->Bounds().Height();
				
				sprintf(full_path,nam_file);
				BPath pth; pth.SetTo(nam_file);		
				sprintf(name,pth.Leaf());  //just the name not the path
				}
									
			}
			else				
  				{ 
  					sprintf(str,Language.get("NOT_RECOGNIZED"));
					strcat(str,nam_file);
		    		alert = new BAlert("",str,"Fuck!"); 
    				alert->Go();
		    		une_erreur=true;
		 		 }

		}

FinishLayer();
if (une_erreur==true) return B_ERROR;
else return B_OK;
	
}


void PhotonImage::FinishLayer()
{
 	active_layer = layer_amount;
 	
 	if (layer_amount==0)
				{
				pix_per_line = (uint16) the_layers[active_layer]->img->Bounds().Width();
				pix_per_row =  (uint16) the_layers[active_layer]->img->Bounds().Height();
				}
}

const char* PhotonImage::ComputeLayerName(const char **nm, int32 type)
{
	
	bool use_new_one = false;

	char *layer_name = (char *)malloc(B_FILE_NAME_LENGTH);
	
	id_counter++; //only used for the names of the new layers

	if (nm==NULL) use_new_one = true;
	else
	{
     	if (*nm==NULL) use_new_one = true;
	    else sprintf(layer_name,*nm);
	}
	

	//générer un nom...
	if (use_new_one == true)
	{
		char x[64];
		switch(type)
			{	
				
				case LAYER_TYPE_BITMAP:	sprintf(layer_name,Language.get("LAYER")); break;		
				case LAYER_TYPE_TEXT:	sprintf(layer_name,"text"); break; //en fait ce sera le début du texte	
				case LAYER_TYPE_EFFECT:	sprintf(layer_name,"effect"); break; //et ça le nom de l'effet d'ou pas besoin de traduc	
				case LAYER_TYPE_GUIDE:	sprintf(layer_name,Language.get("LAYER_GUIDE")); break;	
				default: beep(); beep(); printf("Wrong type parameter sent to ComputeLayerName"); break;				
			}
			
		sprintf(x," %ld",id_counter);
		strcat(layer_name,x);
	}	
	
	return (layer_name);

}


status_t PhotonImage::DeleteLayer(int32 which_one)
{
	
	if (which_one!=layer_amount)
	{
	delete	the_layers[which_one];
	
	//décalage a partir de celui qu'on supprime
	int32 t=which_one;
	
	while (t != layer_amount)
	{
		the_layers[t] = the_layers[t+1];
		the_layers[t]->id = t;
		
		t++;
	}
	
	the_layers[t+1]=NULL;
	
 	active_layer = which_one; //non ça peut pas être zéro vu qu'on peut pas deleter le 1er
	}
 	else
 	{
 		active_layer = which_one-1; //non ça peut pas être zéro vu qu'on peut pas deleter le 1er
	
 		delete	the_layers[which_one];
		the_layers[which_one]=NULL;
 	}

	layer_amount--;
 	
return B_OK;
}





status_t PhotonImage::Revert()
{
LoadLayer(full_path);
return B_OK;
}	


//------------------------------------------------------------------------------------------------------------

void PhotonImage::PurgeUndo()  
{

	//EFFACER LES undos eventuels de la pic précédente...
	if (undo_data->undo[undo_data->undo_amount]!=NULL && undo_data->undo_amount > 0)
	{
		while (undo_data->undo_amount!=0)
		{
		delete undo_data->undo[undo_data->undo_amount]; undo_data->undo[undo_data->undo_amount]=NULL;
		undo_data->undo_amount--;
		}
	}
	
}	

void PhotonImage::FillMask()
{
	uint8 *xx_bits2 = (uint8*) mask_undo_bitmap->Bits();
	uint32 length = mask_undo_bitmap->BitsLength();
	memset(xx_bits2,0,length);
  	MemorizeUndo (BRect(0,0,pix_per_line-1,pix_per_row-1),MASK_FORE_COLOR);  
}


void PhotonImage::SetMaskFromColorZone(rgb_color col, uint8 tolerance, uint8 mode)
{


//JE SAIS PAS SI c'est sélecté à 0 ou 255 à vérifier et corriger...
//UN MASK 0 PAR Défaut (tout transparent)

uint8  *pic_bits  = (uint8*) display_bitmap->Bits();

 
uint8  *mask_bits = (uint8*) mask_undo_bitmap->Bits();

int16 tol_total;
float temp;

uint8 new_val;
uint32 taillePic = mask_undo_bitmap->BitsLength();

for (uint32 i = 0; i != taillePic; i++)
	{
			 tol_total=0;

		 //Red
	     temp =  col.red    -   *pic_bits;     if (temp < 0) temp *= -1; //delta
			 tol_total +=		int16(temp);		pic_bits++;					

		
		//Green
	     temp =  col.green    -   *pic_bits;     if (temp < 0) temp *= -1; //delta
			 tol_total +=		int16(temp);		pic_bits++;		

	   //Blue
	     temp =  col.blue    -   *pic_bits;     if (temp < 0) temp *= -1; //delta
			 tol_total +=		int16(temp);		pic_bits++;				
			 

			//skip alpha
			 pic_bits++; 

			 tol_total /= 3;


		   new_val = 0;
		  if (tol_total <tolerance) new_val = *mask_bits = 255-tol_total;
		   
     		switch(mode)
			  {
						 case MODE_REPLACE_SELECTION:   
					     *mask_bits = new_val;    
					     break;

						 case MODE_ADD_TO_SELECTION:   
   	 					if (  new_val > *mask_bits) *mask_bits = new_val;
	 					break;

						 case MODE_SUBTRACT_FROM_SELECTION:   
						 if (  new_val < *mask_bits) *mask_bits = new_val;
						 break;

			   }
			  mask_bits++;
	
	}


//on copie dans le undo_mask
//uint8 *xx_bits2 = (uint8*) mask_undo_bitmap->Bits();
//memcpy(xx_bits2,mask_bits,mask_bitmap->BitsLength()); 
MemorizeUndo (BRect(0,0,pix_per_line-1,pix_per_row-1),MASK_FORE_COLOR);  


}




void PhotonImage::DeleteMask(uint8 mem_old_mask)  
{

	//UN MASK 0 PAR Défaut (tout transparent)
	uint8 *xx_bits2 = (uint8*) mask_undo_bitmap->Bits();
	uint32 length = mask_undo_bitmap->BitsLength();
	memset(xx_bits2,255,length);

    if (mem_old_mask==ON) MemorizeUndo (BRect(0,0,pix_per_line-1,pix_per_row-1),MASK_FORE_COLOR);  
    else
    {
    //on efface aussi l'ori
    uint8 *xx_bits = (uint8*) mask_bitmap->Bits();
	uint32 lg = mask_bitmap->BitsLength();
	memset(xx_bits,255,lg);
    }
}
    //------------------------------------
 
  
void PhotonImage::MemorizeUndo(BRect zone, uint8 mode)
{
if (zone.bottom > 0 && zone.right > 0)
{
//VERIF SIZE VALABLE POUR MASK ET PIC
if (zone.top  < 0) zone.top=0; 
if (zone.left < 0) zone.left=0;
if (zone.bottom > pix_per_row-1)  zone.bottom = pix_per_row-1;
if (zone.right  > pix_per_line-1) zone.right  = pix_per_line-1;


uint8 nb_of_bytes;
uint8 *m_bits,*m_work_bits,*m_undo_bits; //COMMUN AUX DEUX
uint16 undo_x,undo_y;

//LE PRINCIPE: 
//ON SAUVEGARDE D'ABORD ZONE MODIFIEE (depuis original non modifiée: the_bitmap) DANS UNDO PUIS
//COPIE LE undo_bitmap DANS LE BITMAP FINAL the_bitmap

//---------------------------------------------------------------------------------------

	//CA MARCHE PAS DU TOUT ... a la fin de l'undo il recommence à zéro au lieu de décaler... à revoir.
	if (undo_data->undo_amount <= ThePrefs.max_undo-1) undo_data->undo_amount++;
	else { //si maxundo est atteint on décale tout vers le bas
		uint16 pos=0;
			do{
			
			  undo_data->undo[pos]		= undo_data->undo[pos+1];  			//undo bitmap
			  undo_data->undo_type[pos] = undo_data->undo_type[pos+1];  			//undo type
			  undo_data->layer[pos] 	= undo_data->layer[pos+1];
			  undo_data->width[pos] 	= undo_data->width[pos+1];
			  undo_data->height[pos] 	= undo_data->height[pos+1];
			
			  pos++;

			} while (pos != undo_data->undo_amount);

		}

//---------------------------------------------------------------------------------------

if (mode != MASK_FORE_COLOR)   { 
							   undo_data->undo[undo_data->undo_amount] = new BBitmap(zone,B_RGB32); 	
							   undo_data->undo_type[undo_data->undo_amount] = mode;
							   undo_data->layer[undo_data->undo_amount] = active_layer;
							   undo_data->width[undo_data->undo_amount] = pix_per_line; //pour pouvoir restaurer en cas de resize etc.
							   undo_data->height[undo_data->undo_amount]  = pix_per_row;
							   nb_of_bytes = 4; 
							   m_bits        = (uint8*) the_layers[active_layer]->img->Bits();
							   m_work_bits   = (uint8*) undo_bitmap->Bits();
							   }
							   
                        else { undo_data->undo[undo_data->undo_amount] = new BBitmap(zone,B_GRAY8); 
                        	   undo_data->undo_type[undo_data->undo_amount] = mode; 
                        	   undo_data->layer[undo_data->undo_amount] = -1;  //pas de layer c'est le mask!
                    		   undo_data->width[undo_data->undo_amount] = pix_per_line; //pour pouvoir restaurer en cas de resize etc.
							   undo_data->height[undo_data->undo_amount]  = pix_per_row;
   							   nb_of_bytes = 1; 
							   m_bits        =  (uint8*) mask_bitmap->Bits();
							   m_work_bits   =  (uint8*) mask_undo_bitmap->Bits();
							   }


//-----------------------------------------------------------------
    
	m_undo_bits = (uint8*) undo_data->undo[undo_data->undo_amount]->Bits(); //pointeur sur bitmap dans tableau de bitmaps

	undo_x = uint16 (undo_data->undo[undo_data->undo_amount]->Bounds().Width()  +1);
	undo_y = uint16 (undo_data->undo[undo_data->undo_amount]->Bounds().Height() +1);

    uint32 pos_bits = uint32((zone.top *	pix_per_line)+zone.left) * nb_of_bytes; //pixel de départ

	m_bits      += pos_bits;	//on avance jusqu'au point en question
	m_work_bits += pos_bits;	//on avance jusqu'au point en question

	uint16 line_length=(pix_per_line - undo_x)* nb_of_bytes;
	uint16 undo_length= undo_x * nb_of_bytes; //ARGB

	uint16 pos_x,pos_y;
	pos_x = 0;
	pos_y = 0;
	
	uint32 line_length_et_undo_length =line_length + undo_length;

	//-----------------------------
	
	
	
		//progress_bar->SetBarColor(ThePrefs.color_memorize);
		//progress_bar->Reset();  

//	unsigned char show_progress;
//	if (undo_length*undo_y >= SHOW_PROGRESS_LIMIT) show_progress=ON;		
		
	do {  
//			if (show_progress==ON) progress_bar->Update(percent_val);	
			
			memcpy(m_undo_bits,m_bits,undo_length); //save ancienne zone
			memcpy(m_bits,m_work_bits,undo_length); //save ancienne zone
	
			pos_y++;      //ligne suivante
    		m_bits	  += line_length_et_undo_length;
			m_work_bits += line_length_et_undo_length;
    		m_undo_bits += undo_length;
    	
	}	while (pos_y != undo_y); //changement de ligne
		//progress_bar->Reset();  


		
	undo_data->undo[undo_data->undo_amount+1]=NULL; //pour pas qu'on puisse faire redo avec des anciennes datas...

}//fin if zone est sup à zero

}//fin fonction MemorizeUndo



void PhotonImage::Undo()
{
if (undo_data->undo_amount >= 1) //sinon on undo trop...
{

uint8 nb_of_bytes;
uint8 *u_bits,*u_work_bits,*u_undo_bits;
BPoint le_point = undo_data->undo[undo_data->undo_amount]->Bounds().LeftTop();

uint16 undo_x = uint16 (undo_data->undo[undo_data->undo_amount]->Bounds().Width()  + 1);
uint16 undo_y = uint16 (undo_data->undo[undo_data->undo_amount]->Bounds().Height() + 1);

//changer de layer au besoin
if (active_layer != undo_data->layer[undo_data->undo_amount]) {
	active_layer = undo_data->layer[undo_data->undo_amount];
	BMessage msg_x(ACTIVATE_LAYER);
	msg_x.AddInt32("num",active_layer);
	util.mainWin->PostMessage(&msg_x);
}	
	
	if (undo_data->undo_type[undo_data->undo_amount] != MASK_FORE_COLOR)	 
								{  nb_of_bytes = 4;
								u_bits       = (uint8*) the_layers[undo_data->layer[undo_data->undo_amount]]->img->Bits();
								u_work_bits  = (uint8*) undo_bitmap->Bits(); 
								}
													
						else {  nb_of_bytes=1; 
								u_bits       = (uint8*) mask_bitmap->Bits();
								u_work_bits  = (uint8*) mask_undo_bitmap->Bits(); 
								}
    
u_undo_bits = (uint8*) undo_data->undo[undo_data->undo_amount]->Bits(); //pointeur sur bitmap dans tableau de bitmaps


    uint32 pos_bits = uint32 (((le_point.y *	pix_per_line)+le_point.x)*nb_of_bytes); //pixel de départ
	u_bits           += pos_bits;	//on avance jusqu'au point en question//RGB+A!!!
	u_work_bits      += pos_bits;	//on avance jusqu'au point en question//RGB+A!!!

uint16 line_length=(pix_per_line - undo_x)*nb_of_bytes;
uint16 undo_length= undo_x*nb_of_bytes; //ARGB

uint16 pos_x,pos_y;
pos_x = pos_y = 0;



//progress_bar->SetBarColor(ThePrefs.color_undo);
//progress_bar->Reset();  

	
//unsigned char show_progress;
//if (undo_length*undo_y >= SHOW_PROGRESS_LIMIT) show_progress=ON;		

uint8 *temp = (unsigned char*) malloc(undo_length);
while (pos_y != undo_y) //changement de ligne
{  
//	if (show_progress==ON) progress_bar->Update(percent_val);

	memcpy(temp,u_undo_bits,undo_length); 		//stock undo dans temp
	memcpy(u_undo_bits,u_work_bits,undo_length);	//copy work dans undo
	memcpy(u_work_bits,temp,undo_length);			//copy temp dans work
	memcpy(u_bits,u_work_bits,undo_length); 		//recopie juste work à l'écran...

	u_undo_bits += undo_length; 
   	u_bits      += line_length+undo_length; 
   	u_work_bits += line_length+undo_length; 
   		
    pos_y++;      //ligne suivante
   
}

//progress_bar->Reset();  


if (undo_data->undo_amount  >= 0  && is_redo == OFF) //minimum un undo (on commence à un pas zéro)
	{
	//delete undo[undo_amount]; //efface le bmp
	undo_data->undo_amount--;
	}

}//fin undo_amount!=0


	if (ThePrefs.layer_selector_open==true)
		{		
			BMessage msg(UPDATE_ACTIVE_LAYER);
			msg.AddInt32("active",active_layer);
			util.layerWin->PostMessage(&msg);		
		}	
	
	
	
}

//---------------------------------------------------------------------------------------------------------


void PhotonImage::Redo()
{ 
if (undo_data->undo[undo_data->undo_amount+1]!=NULL)
	{
	undo_data->undo_amount++;
	is_redo = ON;
	Undo();
	is_redo = OFF;
	}
}


//---------------------------------------------------------------------------------------------------------

void PhotonImage::SetPixel(BPoint pix, rgb_color color)  
{

uint32 pos_bits;
pos_bits= uint32 ((pix.x+(pix_per_line*pix.y))*4); //seul y compte c'est pareil sur toute la ligne
uint8	*s_bits;
s_bits  = (uint8*) undo_bitmap->Bits(); //pour accèder aux pixels directement
s_bits += pos_bits;//avance jusqu'au pixel en question

*s_bits     = color.blue;  	s_bits++;
*s_bits	  = color.green; 	s_bits++;
*s_bits	  = color.red;  	s_bits++;
*s_bits	  = color.alpha;  
}

void PhotonImage::SetMaskPixel(BPoint pix, rgb_color color)  
{

uint32 pos_bits;
pos_bits=uint32((pix.x+(pix_per_line*pix.y))*4); //seul y compte c'est pareil sur toute la ligne
uint8	*s_bits;
s_bits = (uint8*) mask_undo_bitmap->Bits(); //pour accèder aux pixels directement
s_bits+=pos_bits;//avance jusqu'au pixel en question

*s_bits     = (color.blue+color.red+color.green)/3;
}

//++++++++++++++++++++




///--------------------------------------------------------

void PhotonImage::UpdateDisplayImg(BRect a_rect)
{ 

//FOR TEST:... on se fait la totale à chaque fois...
//a_rect.Set(0,0,pix_per_line,pix_per_row);
  
if (updating==false && ThePrefs.no_pictures_left ==OFF)
{
updating=true; //si 

	
uint8 *bits_src,*bits_dest,*bits_msk;
int16 pos_x,pos_y,largeur,hauteur;   
uint8 tr;   

BRect rect = util.CheckRectangle(a_rect);


largeur =  (int32) (rect.right  - rect.left+1); //+1 pour boucler la boucle
hauteur =  (int32) (rect.bottom - rect.top+1);	

if (largeur > pix_per_line) largeur = pix_per_line; //Pas moins un vu que la boucle doit faire un tour de plus
if (hauteur > pix_per_row)  hauteur = pix_per_row;

int16 line_size_8_bits = pix_per_line-largeur;
int16 line_size_32 	   = line_size_8_bits * 4;

uint8 tmp;


uint8 mask_blue  = ThePrefs.masking_color.blue;
uint8 mask_green = ThePrefs.masking_color.green;
uint8 mask_red   = ThePrefs.masking_color.red;


	//ON AVANCE JUSqu'à la pos en question dans toutes les pics...
    uint32 pos_bits = uint32 (rect.left+(pix_per_line*rect.top)); //pixel de départ (haut gauche du rect)
	uint32 pos_bits_32 = pos_bits*4; 

int32 quantity_drawn=1;

int32 i =0;
while(i!=layer_amount+1)
{


if (the_layers[i]->is_visible==true)
{

switch (the_layers[i]->layer_type)
{

case LAYER_TYPE_BITMAP:

	quantity_drawn++;
	
	//pointeurs au débuts des pics
	bits_src   = (uint8*) the_layers[i]->img->Bits();
	bits_dest  = (uint8*) display_bitmap->Bits(); //faut le remettre à zéro chaque fois

	bits_src   += pos_bits_32; 
	bits_dest  += pos_bits_32; //BGR+A = x4
	
	//surtout pas avancer dans bits_under!!! vu que c'est forcéement le même que src ou dest!

	tr = uint8(the_layers[i]->opacity);
	
pos_y=0;
while (pos_y <  hauteur) //changement de ligne
{   
  
   pos_x=0;
   while (pos_x < largeur) //Changement de pixel sur la ligne
           { 
			switch(the_layers[i]->draw_mode)
				{
					case NORMAL:
				    *bits_dest  = TheTables.tab_normal[tr][*bits_dest]  + TheTables.tab_pourcent_x_val[tr][*bits_src];	bits_src++;  bits_dest++;
				    *bits_dest  = TheTables.tab_normal[tr][*bits_dest]  + TheTables.tab_pourcent_x_val[tr][*bits_src];	bits_src++;  bits_dest++;
					*bits_dest  = TheTables.tab_normal[tr][*bits_dest]  + TheTables.tab_pourcent_x_val[tr][*bits_src];	bits_src+=2; bits_dest+=2;
					break;

					//MULTIPLY OK NICKEL NO PROBLEMO
					case MULTIPLY:
					tmp = TheTables.tab_multiply[*bits_dest][*bits_src];
					*bits_dest  =   TheTables.tab_normal[tr][*bits_dest]  +  TheTables.tab_pourcent_x_val[tr][tmp]; 
	 				bits_src++;  bits_dest++; 

					tmp = TheTables.tab_multiply[*bits_dest][*bits_src];
					*bits_dest  =   TheTables.tab_normal[tr][*bits_dest]  +  TheTables.tab_pourcent_x_val[tr][tmp];
	 				bits_src++;  bits_dest++; 

					tmp = TheTables.tab_multiply[*bits_dest][*bits_src];
					*bits_dest  =   TheTables.tab_normal[tr][*bits_dest]  +  TheTables.tab_pourcent_x_val[tr][tmp];
					bits_src+=2; bits_dest+=2; 
					break;
					
					
					/*
					case LIGHTEN:
					
					tmp = TheTables.tab_lighten[tr][*bits_under];
					*bits_dest  =   TheTables.tab_normal[tr][*bits_under]  +  TheTables.tab_pourcent_x_val[tr][tmp]; 
					bits_src++;  bits_dest++; bits_under++;

					tmp = TheTables.tab_lighten[tr][*bits_under];
					*bits_dest  =   TheTables.tab_normal[tr][*bits_under]  +  TheTables.tab_pourcent_x_val[tr][tmp]; 
	 				bits_src++;  bits_dest++; bits_under++;

					tmp = TheTables.tab_lighten[tr][*bits_under];
					*bits_dest  =   TheTables.tab_normal[tr][*bits_under]  +  TheTables.tab_pourcent_x_val[tr][tmp]; 
					bits_src+=2; bits_dest+=2; bits_under+=2;
	 				break;
	 			
	 						case DARKEN:
	 						*l_bits  =   TheTables.tab_darken[tr][*l_bits]  +  TheTables.tab_pourcent_x_val[tr][*l_bits]; l_bits++;
							*l_bits  =   TheTables.tab_darken[tr][*l_bits]  +  TheTables.tab_pourcent_x_val[tr][*l_bits]; l_bits++;
							*l_bits  =   TheTables.tab_darken[tr][*l_bits]  +  TheTables.tab_pourcent_x_val[tr][*l_bits];
	 						break;
*/
					
					case COMBINE:
	 				tmp = TheTables.tab_combine[*bits_dest][*bits_src];
	 				*bits_dest  =   TheTables.tab_normal[tr][*bits_dest]  +  TheTables.tab_pourcent_x_val[tr][tmp];
					bits_src++;  bits_dest++; 

	 				tmp = TheTables.tab_combine[*bits_dest][*bits_src];
	 				*bits_dest  =   TheTables.tab_normal[tr][*bits_dest]  +  TheTables.tab_pourcent_x_val[tr][tmp];
					bits_src++;  bits_dest++; 

	 				tmp = TheTables.tab_combine[*bits_dest][*bits_src];
	 				*bits_dest  =   TheTables.tab_normal[tr][*bits_dest]  +  TheTables.tab_pourcent_x_val[tr][tmp];
					bits_src+=2; bits_dest+=2; 
	 				break;

	 						
					case DIFFERENCE: //attention celui-ci se base sur wbits!
					
					tmp = TheTables.tab_difference[*bits_dest][*bits_src];
					*bits_dest  =   TheTables.tab_normal[tr][*bits_dest]  +  TheTables.tab_pourcent_x_val[tr][tmp];
					bits_src++;  bits_dest++; 
					
					tmp = TheTables.tab_difference[*bits_dest][*bits_src];
					*bits_dest  =   TheTables.tab_normal[tr][*bits_dest]  +  TheTables.tab_pourcent_x_val[tr][tmp];
					bits_src++;  bits_dest++; 
	
					tmp = TheTables.tab_difference[*bits_dest][*bits_src];
					*bits_dest  =   TheTables.tab_normal[tr][*bits_dest]  +  TheTables.tab_pourcent_x_val[tr][tmp];
					bits_src+=2; bits_dest+=2; 
					break;


					
			}//fin switch draw_mode
			
			
		pos_x++;
		}//fin ligne horizontale	
	
pos_y++;      //ligne suivante

bits_dest	+= line_size_32;
bits_src	+= line_size_32;
		
	}//fin verticale
	
i++; //layer suivant

break; //fin case: TYPE_BITMAP:

case LAYER_TYPE_TEXT: break;
case LAYER_TYPE_EFFECT: break;
case LAYER_TYPE_GUIDE: break;

}//fin switch type de layer

} else i++; //si layer pas visible on passe direct au suivant


//----------------------------------------------- M A S K ------------------------------------------------------------------


	if (ThePrefs.mask_mode==ON && quantity_drawn >= 0)
	{

	 
	bits_dest =  (uint8*) display_bitmap->Bits(); //on rajoute sur ce qu'on a déjà dessiné (des autres calques)
	bits_msk   = (uint8*) mask_bitmap->Bits();
	bits_dest += pos_bits_32; 
	bits_msk   += pos_bits;
		
		pos_y=0;
		while (pos_y <  hauteur) //changement de ligne
		{   
  
  		 pos_x=0;
   			while (pos_x <  largeur) //Changement de pixel sur la ligne
            { 

			tr = TheTables.tab_char_to_shade[*bits_msk];
		
            *bits_dest = TheTables.tab_normal[tr][mask_blue]  + TheTables.tab_pourcent_x_val[tr][*bits_dest];
			bits_dest++; 

			*bits_dest  = TheTables.tab_normal[tr][mask_green] + TheTables.tab_pourcent_x_val[tr][*bits_dest];
			bits_dest++; 

			*bits_dest  = TheTables.tab_normal[tr][mask_red]   + TheTables.tab_pourcent_x_val[tr][*bits_dest];
			bits_dest+=2;
		
			bits_msk++;

			pos_x++;
			}//fin ligne horizontale	
	
		pos_y++;      //ligne suivante

		bits_dest	+= line_size_32;
		bits_msk	+= line_size_8_bits;
		
		}//fin verticale
	}//fin if mask_mode


}

//printf("\nUpdated zone:");
//a_rect.PrintToStream();

//Screen refresh
BMessage *msg_a = new BMessage(DRAW_ME);

msg_a->AddRect("zone",a_rect);
util.mainWin->PostMessage(msg_a);

updating = false;
}//fin if not updating





}

