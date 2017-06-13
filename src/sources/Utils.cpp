#include "Utils.h"

#include "PBitmapModif.h" //attention pas inclure dans le .h!!!

Utils util; //extern dans le.h
Utils::Utils()
{ 
	show_progress_load = OFF;
	loading_in_progress =OFF;
	progress_exists = false;

	sel_length = 0 ;

}

// r,g,b values are from 0 to 1 
// h = [0,360], s = [0,1], v = [0,1] 
//              if s == 0, then h = -1 (undefined)

void Utils::RGBtoHSV( float r, float g, float b, float *h, float *s, float *v ) 
{ 
        float min, max, delta;
        
        //trouver le plus petit
        min = MIN( r, g); 
        max = MAX( r, g);
        min = MIN(min,b);
        max = MAX(min,b);
        
        *v = max;                               // v
        delta = max - min;
        if( max != 0 ) 
                *s = delta / max;               // s 
        else { 
                // r = g = b = 0                // s = 0, v is undefined 
                *s = 0; 
                *h = -1; 
                return; 
        }
        if( r == max ) 
                *h = ( g - b ) / delta;         // between yellow & magenta 
        else if( g == max ) 
                *h = 2 + ( b - r ) / delta;     // between cyan & yellow 
        else 
                *h = 4 + ( r - g ) / delta;     // between magenta & cyan
        *h *= 60;                               // degrees 
        if( *h < 0 ) 
                *h += 360;
}

void Utils::HSVtoRGB( float *r, float *g, float *b, float h, float s, float v ) 
{ 
        int i; 
        float f, p, q, t;
        if( s == 0 ) { 
                // achromatic (grey) 
                *r = *g = *b = v; 
                return; 
        }
        h /= 60;                        // sector 0 to 5 
        i = floor( h ); 
        f = h - i;                      // factorial part of h 
        p = v * ( 1 - s ); 
        q = v * ( 1 - s * f ); 
        t = v * ( 1 - s * ( 1 - f ) );
        switch( i ) { 
                case 0: 
                        *r = v; 
                        *g = t; 
                        *b = p; 
                        break; 
                case 1: 
                        *r = q; 
                        *g = v; 
                        *b = p; 
                        break; 
                case 2: 
                        *r = p; 
                        *g = v; 
                        *b = t; 
                        break; 
                case 3: 
                        *r = p; 
                        *g = q; 
                        *b = v; 
                        break; 
                case 4: 
                        *r = t; 
                        *g = p; 
                        *b = v; 
                        break; 
                default:                // case 5: 
                        *r = v; 
                        *g = p; 
                        *b = q; 
                        break; 
        }
}

void Utils::RescaleBitmap(BBitmap *src, BBitmap *dest, bool center_if_smaller)
{
//Rescales src copying it into dest.
//if src is smaller than 

//WARNING DEST BBITMAP MUST ACCEPT BVIEWS!

//efface en gris 216
uint8 *bits = (uint8*) dest->Bits();
uint32 pos =0;   
uint32 lg = dest->BitsLength();
do { *bits=216; bits++; } while(pos++ != lg-1);


		//on réduit l'image à l'aide de drawbitmap		
		
		BPicture *my_pict; 

		dest->Lock(); //important!
		BView *virtualView = new BView(dest->Bounds(), NULL, B_FOLLOW_NONE, 0 );
		dest->AddChild( virtualView );

		//------------		
		virtualView->BeginPicture(new BPicture); 
    	
    	BRect rect = dest->Bounds();
		rect.InsetBy(1,1);
		
		
		if (center_if_smaller==true) //center without rescaling
		{
		//On centre l'image sur le fond
		
		
		//hori
		if ( dest->Bounds().Width() > src->Bounds().Width() )  
			{ 
				rect.right = src->Bounds().right;
				rect.OffsetBy( (dest->Bounds().Width()-rect.right)/2,0); //Centrage
			}
			
		//vert
		if ( dest->Bounds().Height() > src->Bounds().Height() ) 
			{
				rect.bottom = src->Bounds().bottom;
				rect.OffsetBy( 0, (dest->Bounds().Height()-rect.bottom)/2); //Centrage
			}
		}
		
		//et on la colle
		virtualView->DrawBitmap(src, src->Bounds(), rect);
			 
		//--------------
    	
    	my_pict = virtualView->EndPicture();
    
		virtualView->DrawPicture(my_pict);
	  	dest->RemoveChild(virtualView );	// A faire avant la destruction de la bitmap sinonm problemes
	  	dest->Unlock();
 
}



BBitmap *Utils::FetchBitmap(char *filename, uint8 mode)
{

  BFile file;
  
  if (file.SetTo(filename, B_READ_ONLY) == B_OK)
 {
 uint8 dither=false;
 if (mode == 8) dither=true;

   BTranslatorRoster *roster = BTranslatorRoster::Default();

//----------------------------------------------------------   
//POUR PROGRESSIVE

ProgressiveBitmapStream stream(NULL, NULL, dither,false); //dither true

char str[8192];
if (show_progress_load==ON)
{			

				sprintf(str, Language.get("LOADING"));
				strcat(str," ");
				strcat(str,filename);
				stream.DisplayProgressBar(str);

      }


   roster->Translate(&file, 0, 0, &stream, B_TRANSLATOR_BITMAP);
  
   //POUR PROGRESSIVE
   stream.SetDispose(false);
   return stream.Bitmap();

///----------------------------------------------------------   
/*
  	 BBitmapStream stream;
  	 BBitmap *result = NULL;
  	 if (roster->Translate(&file, NULL, NULL, &stream, B_TRANSLATOR_BITMAP) < B_OK) return NULL;
  	 stream.DetachBitmap(&result);
  	 return result; 
  	 */
   }
  else return NULL;
}


   status_t Utils::StoreTranslatorBitmap(BBitmap *bitmap, char *filename, uint32 type) 
   { 
 	  BAlert *alert;
      BTranslatorRoster *roster = BTranslatorRoster::Default(); 
      BBitmapStream stream(bitmap); // init with contents of bitmap 
      BFile file(filename, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);

      switch ( roster->Translate(&stream, NULL, NULL, &file, type) ) 
      			{
      			
   				case B_NO_TRANSLATOR : 
   		   		alert = new BAlert("","No translator.", "Pfff..."); 
   		   		alert->Go(NULL);
   		   		return B_ERROR;
   				break;
   				
   				case B_NOT_INITIALIZED : 
   		   		alert = new BAlert("","Not initalized.", "Pfff..."); 
		  		alert->Go(NULL);
   		   		return B_ERROR;
   				break;
   				
   				case B_BAD_VALUE : 
   		   		alert = new BAlert("","Bad value, in or out source is NULL.", "Pfff..."); 
   		   		alert->Go(NULL);
   		   		return B_ERROR;
   				break;
   				
   				case B_NO_ERROR :
   				 
   				return B_OK;	
   		   //		alert = new BAlert("","Saved. No problem.", "Yep!"); 
   		   	//	alert->Go(NULL);
   				break;
   				
      			}
return B_ERROR; //par défaut

}


void Utils::AppDir() //met dossier apllication dans dossier_app ET LA VERSION AUSSI dans version_txt
{
  
  app_info ai; 
  BAppFileInfo afi; 
  BFile file; 
  
  app_info info;
  BPath path;
  be_app->GetAppInfo(&info);  //infos sur l'app dont le directory
  BEntry tmp_entry(&info.ref);
  BEntry file_entry;
  tmp_entry.GetParent(&file_entry); //on revient en arriere car il y a le nom de l'exec!
  file_entry.GetPath(&path); 
  sprintf(dossier_app,path.Path()); //recupere le nom en string pour pouvoir le modifier après
 
  file.SetTo(&info.ref, B_READ_WRITE); 
  afi.SetTo(&file);
  version_info v_info;
  afi.GetVersionInfo(&v_info, B_APP_VERSION_KIND);
  
  char tmp[255];
  sprintf(version_txt,v_info.short_info);
  sprintf(tmp,"V%ld.%ld%ld",v_info.major,v_info.middle,v_info.minor);
  sprintf(version_only_txt,tmp);
  switch(v_info.variety)
  		{
  		case 1: strcat(tmp," ALPHA"); break;
  		case 2: strcat(tmp," BETA"); break;
  		}
  strcat(version_txt," ");
  strcat(version_txt,tmp);
  }

//------------------------


void Utils::NotImplemented()
{
BAlert *alert = new BAlert("",Language.get("NOT_YET"),"Oompf!");  alert->Go(NULL);
mainWin->Activate();
}


BRect Utils::CheckRectangle(BRect ori_rect)
{
//corrige un rect pour qu'il soit utilisable par les fonctions

BRect modified = ori_rect;

//PAS QUE LA POS SOIT NEGATIVE...
ori_rect.left=0;
ori_rect.top=0;
if (ori_rect.left > -1) modified.left = floor(ori_rect.left);
if (ori_rect.top  > -1) modified.top  = floor(ori_rect.top);
modified.right =  int32(floor(ori_rect.right)); 
modified.bottom = int32(floor(ori_rect.bottom)); 

//TRES important il faut toujours que la droite soit plus grande que la gauche , idem haut bas..
//le cas échéant on inverse les coins...
float tmp;
if (modified.top  > modified.bottom) 
	{ tmp = modified.top;  modified.top = modified.bottom; modified.bottom = tmp; }
if (modified.left > modified.right)  
	{ tmp = modified.left; modified.left = modified.right; modified.right = tmp; }

return modified;

}


void Utils::CopyRect(BRect zone_src,  BBitmap *la_source, 
					 BRect zone_dest, BBitmap *la_dest,
					 uint8 nb_of_bytes)
{



	uint8 *src_bits  = (uint8*) la_source->Bits();	
	uint8 *dest_bits = (uint8*) la_dest->Bits();	

	uint32 src_x  = uint32 (la_source->Bounds().Width() +1);
	uint32 dest_x = uint32 (la_dest->Bounds().Width()   +1);

    uint32 src_pos_bits  = uint32 (((zone_src.top  * src_x)  + zone_src.left)  * nb_of_bytes); //pixel de départ
    uint32 dest_pos_bits = uint32 (((zone_dest.top * dest_x) + zone_dest.left) * nb_of_bytes); //pixel de départ

	src_bits      += src_pos_bits;	
	dest_bits     += dest_pos_bits;	

	//------------------------------------------------------------------------------------------------

	uint32 src_length  = uint32 ((src_x -  (zone_src.right - zone_src.left+1))   * nb_of_bytes);
	uint32 dest_length = uint32 ((dest_x - (zone_dest.right - zone_dest.left+1)) * nb_of_bytes); 

	uint32 line_size = uint32 ((zone_dest.right - zone_dest.left+1)*nb_of_bytes);
	uint32 lines = uint32 (zone_dest.bottom-zone_dest.top+1+1);

	for (uint32 pos_y = 0; pos_y!=lines; pos_y++)
	{ 
		memcpy(dest_bits,src_bits,line_size);
   		src_bits += src_length+line_size;  dest_bits += dest_length+line_size;
	}

}//fin if zone est sup à zero



void Utils::CopyRectWithMask(BRect zone_src,  BBitmap *la_source, 
					 BRect zone_dest, BBitmap *la_dest, BBitmap *le_mask,
					 uint8 nb_of_bytes)
{



	uint8 *src_bits  = (uint8*) la_source->Bits();	
	uint8 *dest_bits = (uint8*) la_dest->Bits();	
	uint8 *t_msk_bits  = (uint8*) le_mask->Bits();	
 
	uint32 src_x  = uint32 (la_source->Bounds().Width()  +1);
	uint32 dest_x = uint32 (la_dest->Bounds().Width()  +1);

    uint32 src_pos_bits  = uint32 (((zone_src.top  * src_x)  + zone_src.left)  * nb_of_bytes); //pixel de départ
    uint32 dest_pos_bits = uint32 (((zone_dest.top * dest_x) + zone_dest.left) * nb_of_bytes); //pixel de départ
    uint32 mask_pos_bits = uint32 (((zone_dest.top * dest_x) + zone_dest.left)); //pixel mask comme la dest

	src_bits      += src_pos_bits;	
	dest_bits     += dest_pos_bits;	
	t_msk_bits      += mask_pos_bits;	

	//------------------------------------------------------------------------------------------------

	uint32 src_length  = uint32 ((src_x -  (zone_src.right - zone_src.left+1))   * nb_of_bytes);
	uint32 dest_length = uint32 ((dest_x - (zone_dest.right - zone_dest.left+1)) * nb_of_bytes); 
	uint32 mask_length = uint32 ((dest_x - (zone_dest.right - zone_dest.left+1))); 

	uint32 pos_x, pos_y;
	pos_x = 0;
	pos_y = 0;

	uint32 line_size = uint32 ((zone_dest.right - zone_dest.left+1));
	uint32 lines     = uint32 (zone_dest.bottom-zone_dest.top+1);
	uint8 transp;
		
	while (pos_y != lines) 
	{ 
		while (pos_x!=line_size)
		{
		   transp = TheTables.tab_char_to_percent[*t_msk_bits];
	       
	       *dest_bits  =  TheTables.tab_addition[TheTables.tab_normal[transp][*src_bits]] [TheTables.tab_pourcent_x_val[transp][*dest_bits]];  dest_bits++; src_bits++; 
		   *dest_bits  =  TheTables.tab_addition[TheTables.tab_normal[transp][*src_bits]] [TheTables.tab_pourcent_x_val[transp][*dest_bits]];  dest_bits++; src_bits++; 
           *dest_bits  =  TheTables.tab_addition[TheTables.tab_normal[transp][*src_bits]] [TheTables.tab_pourcent_x_val[transp][*dest_bits]];

		   pos_x++;  t_msk_bits++; dest_bits+=2; src_bits+=2; 
		}
		
   		pos_y++;     
   		src_bits  += src_length;  
   		dest_bits += dest_length;
		t_msk_bits += mask_length;
		pos_x = 0;
	}

}//fin if zone est sup à zero


BBitmap* Utils::GrabRect(BRect zone_src,  BBitmap *la_source) 
{
	//zone_src.right-=1;
	//zone_src.bottom-=1;
	BBitmap* la_dest = new BBitmap(zone_src,B_RGB32);
	
	CopyRect(zone_src,la_source,
			 BRect(0,0, zone_src.right-zone_src.left, zone_src.bottom-zone_src.top),la_dest, 4);
	
return la_dest;
}


BRect Utils::find_win_pos(float width, float height, BWindow *win_to_use_to_choose_screen)
{
//trouve le point idéal pour poser la win
//on pourrait passer la mainWin tout le temps le problème c'est avec différents workspaces à différentes résolutions
//et aussi l'about au démarrage alors que la mainwin est pas encore créée

BRect sr = BScreen(win_to_use_to_choose_screen).Frame();

BRect r;

r.left = ((sr.Width()-width)/2);
r.top = ((sr.Height()-height)/2);
r.right = r.left + width-1;
r.bottom = r.top + height-1;


return r;
}

BRect Utils::find_win_pos_on_main_screen(float width, float height)
{
//version spéciale sans win en paramètres... vu qu'elle est pas accessible

BRect sr = BScreen(B_MAIN_SCREEN_ID).Frame();

BRect r;

r.left = ((sr.Width()-width)/2);
r.top = ((sr.Height()-height)/2);
r.right = r.left + width-1;
r.bottom = r.top + height-1;


return r;
}



BBitmap* Utils::load_bmp(const char* le_nom)
{ 
   BAlert* alert;	 
	char str[512];
 	
  	BFile fich;
  	if (fich.SetTo(le_nom,B_READ_ONLY)!=B_NO_ERROR)
  		{ 
  			sprintf(str,Language.get("COULD_NOT_LOAD"));
  			strcat(str,"\n");
			strcat(str,le_nom);
		    alert = new BAlert("",str,"oops..."); 
    		alert->Go(NULL);
    		return NULL;

		 }
  	else {  
  			return FetchBitmap((char*)le_nom,32);

		  }
  	
} 

float Utils::ConvertUnits(float val, int32 src_unit,int32 dest_unit, float resolution, float res_units)
{

		float divider =0;
		if (res_units==DPI) divider=2.54; 		//dots per inch (dpi)
		else divider = 1;								//dots per centimeterd
		
		float inch_divider;
		if (divider ==1) inch_divider=2.54;
		else inch_divider=1;
		
		
	switch(src_unit)
				{
				
					case UNIT_CM:
					switch(dest_unit)
						{
							case UNIT_CM:			 				break; //que dalle
							case UNIT_MM:   	val *= 10;	 		break;
							case UNIT_INCH: 	val /= 2.54;		break;
							case UNIT_PIXELS: 	val  *= (resolution/divider); break;
						}
					break;
					
					
					case UNIT_MM:
					switch(dest_unit)
						{
							case UNIT_CM:		val /= 10;			break;
							case UNIT_MM:   						break; //que dalle
							case UNIT_INCH: 	val /= (2.54*10);	break;
							case UNIT_PIXELS: 	val  *= (resolution/divider)/10;	break;
						}
					break;
					
					
					case UNIT_INCH:
					switch(dest_unit)
						{
							case UNIT_CM:		val *= 2.54;			break; 
							case UNIT_MM:   	val *= (2.54*10);		break;
							case UNIT_INCH: 	break; //que dalle
							case UNIT_PIXELS: 	val  *= (resolution*inch_divider);	break;						}
					break;
					
					
					case UNIT_PIXELS:
					switch(dest_unit)
						{
							case UNIT_CM: 		val  /= (resolution/divider);		break;
												
							case UNIT_MM:   	val  /= (resolution/divider)/10;	break; 
												
							case UNIT_INCH: 	val  /= (resolution*inch_divider);	break; 
											
							case UNIT_PIXELS: 	break; //que dalle
						}
					break;
												
										
				}
		
return val;

}	
