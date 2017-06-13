#include "PhotonView.h"


Painting::Painting(share *sh)
{
	shared = sh;
/*
    painting_thread = spawn_thread(_PasteLine,"painter",B_NORMAL_PRIORITY,this);
    if(painting_thread >= 0)   resume_thread(painting_thread);
*/
}

/*
long Painting::_PasteLine(void *arg)
{
    ((Painting*)arg)->PasteLine(shared->pos_old, shared->pos_actuelle);
    return 0;
}
*/

void Painting::PasteLine(BPoint pt1,BPoint pt2)
{
if (pt1==pt2) PasteBrush(pt1);
else
{


//--------------------------------------------------------------------------------------

int16 delta_x = (int16) (pt1.x - pt2.x);
int16 delta_y = (int16) (pt1.y - pt2.y);

//eviter négatif
if (delta_x < 0) delta_x *= -1;
if (delta_y < 0) delta_y *= -1;

int16 delta = int16 (sqrt((delta_x*delta_x) + (delta_y*delta_y)));
int16 nbpts;

int16 step=(shared->brush_x + shared->brush_y)/ (2*4); //div par 2 X valeur arbitraire d'espacement
if (step <=0) step=1;
if (shared->brush_x <= shared->brush_y-1) nbpts = delta / step; //le plus petit...
   else  nbpts = delta / step ;
   
BPoint pos;
uint16 i;

for (i = 0; i <= nbpts-1; i++) //-1 sinon on double le point premier+dernier...
	{	
		//très important de faire un floor sinon cata
		//un BPoint, bien qu'en float, doit toujours être un entier pour être aligné!
		pos.x = floor(pt1.x + i*(pt2.x-pt1.x)/nbpts);
		pos.y = floor(pt1.y + i*(pt2.y-pt1.y)/nbpts);

		PasteBrush(pos);
	}

}
}


void Painting::PasteBrush(BPoint le_point)
{
/*
le_point.x = floor(le_point.x /shared->act_img->zoom_level);
le_point.y = floor(le_point.y /shared->act_img->zoom_level);
*/
	
if (le_point.x > 0 && le_point.y > 0 && le_point.x < shared->act_img->pix_per_line && le_point.y < shared->act_img->pix_per_row)
{

//TROUVER LES BORDS A COUPER POUR PAS DEBORDER
//attention faire ça avant de mettre point au milieu du brush!
//oui on peut toucher les deux bords si la pic est mini!!! (icon p.ex.)

//GAUCHE-DROITE
if (le_point.x < shared->brush_rayon_x) to_cut_left = int16 (shared->brush_rayon_x - le_point.x -1); //moins parce-que sinon il y a un pix de vide... 
	else to_cut_left   = -1;
if (le_point.x > shared->act_img->pix_per_line-shared->brush_rayon_x-1) to_cut_right   = int16(shared->brush_x-(shared->brush_rayon_x-(shared->act_img->pix_per_line-le_point.x-2)));
	else to_cut_right   = shared->brush_x;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//HAUT-BAS
if (le_point.y < shared->brush_rayon_y) to_cut_top = int16 (shared->brush_rayon_y - le_point.y-1); //moins parce-que sinon il y a un pix de vide... 
else to_cut_top   = -1;
//ATTENTION CA joue tout juste pas!! si on fait int16(shared->brush_y-1-(...) on voit que des fois il y a un peu de shade qui se dessine endessous quand même!!!
//seulement sur ce bord apparemment!!! EN FAIT JE SUIS PAS SUR QUE CE SOIT UN VRAI BUG çA A PAS L'AIR de poser de prob autrement.
if (le_point.y > shared->act_img->pix_per_row-shared->brush_rayon_y-1) to_cut_bottom   = int16(shared->brush_y-(shared->brush_rayon_y-(shared->act_img->pix_per_row-le_point.y-2)));
	else to_cut_bottom   = shared->brush_y;



le_point.x -= 	shared->brush_rayon_x; //pour coller au centre de la brush et pas
le_point.y -= 	shared->brush_rayon_y; //en bas à droite vu qu'on clicke au milieu faut que ça dessine de chaque côté

BRect to_update;
to_update.Set(le_point.x, le_point.y,le_point.x+shared->brush_x, le_point.y+shared->brush_y); 


offset    = uint32 ((le_point.y * shared->act_img->pix_per_line)+le_point.x); //pixel de départ
offset_st = uint32 (((le_point.y+shared->stamp_offset.y) * shared->act_img->pix_per_line)+(le_point.x+shared->stamp_offset.x)); //pixel du stamp
uint32 offset_paper = uint32 (
					  ((le_point.y- (floor(le_point.y/shared->paper_y)*shared->paper_y)) *shared->paper_x ) //les lignes
					+ le_point.x- (floor(le_point.x/shared->paper_x)*shared->paper_x) //le x
							);



//ON SE REMET AU DBBUT Des POINTEUR
l_bits 	  = shared->bits  +(offset    * shared->nb_bytes);
l_wbits   = shared->wbits +(offset    * shared->nb_bytes);

l_paper = shared->paper_bits+offset_paper;
l_bits_st = shared->bits  +(offset_st * shared->nb_bytes);

l_mask_ptr    = shared->mask_bits+offset;
l_brush_bits = shared->brush_bits;

line_length     = shared->act_img->pix_per_line - shared->brush_x;
line_length_24  = line_length*4;


//float t =  shared->paint_transparency / shared->pressure; //c'est la transp, pas l'opacité donc on divise
//uint8 tr = uint8(float(shared->pressure *100));
//uint8 paint_transp =  TheTables.tab_pourcent_x_val[transp][shared->paint_transparency];

float yxy = 100-((100-shared->paint_transparency) * shared->pressure);
uint8 paint_transp = uint8(floor(yxy));
//printf("\nPressure: %.1f x Transp: %ld  = %ld",shared->pressure, shared->paint_transparency,paint_transp);



if (ThePrefs.mask_mode==ON)
{
if (shared->active_tool==STAMP)
{

pos_y = 0;
while (pos_y != shared->brush_y) //changement de ligne
{   
 if ((pos_y > to_cut_top) && (pos_y < to_cut_bottom) ) //si on est pas dans la zone à cutter
   { 
   pos_x=0;
   while (pos_x != shared->brush_x) //Changement de pixel sur la ligne
           { 
          transp = TheTables.tab_transp[paint_transp][*l_brush_bits];
 
  			   if ((pos_x > to_cut_left) && (pos_x < to_cut_right) ) //si on est pas dans la zone à cutter
  		       {*l_bits  =   TheTables.tab_normal[transp][*l_bits_st]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;l_bits_st++;}
	 		   else {l_bits++;l_bits_st++;}
			l_brush_bits++; //avance en 8 bits
      	    pos_x++;      //pixel suivant
           }//fin while pos_x, fin de la ligne
	}
 	else { l_bits+=shared->brush_x*shared->nb_bytes; l_bits_st+=shared->brush_x*shared->nb_bytes; l_brush_bits+=shared->brush_x; };
 	pos_y++;      //ligne suivante
	l_bits	     += line_length; 	
    l_bits_st	     += line_length; 	
    }
  	  	  	
}
else //PAS STAMP MODE DONC DESSIN NORMAL!!!! MAIS TOUJOURS MASK!!!
{

pos_y = 0;
while (pos_y != shared->brush_y) //changement de ligne
{   
 if ((pos_y > to_cut_top) && (pos_y < to_cut_bottom) ) //si on est pas dans la zone à cutter
   { 
   pos_x=0;
   while (pos_x != shared->brush_x) //Changement de pixel sur la ligne
           { 
          transp = TheTables.tab_transp[paint_transp][*l_brush_bits];
 
  			   if ((pos_x > to_cut_left) && (pos_x < to_cut_right) ) //si on est pas dans la zone à cutter
  		       {*l_bits  =   TheTables.tab_addition[TheTables.tab_normal[transp][shared->col_blue]] [TheTables.tab_pourcent_x_val[transp][*l_bits]]; l_bits++;}
	 		   else l_bits++;
			l_brush_bits++; //avance en 8 bits
      	    pos_x++;      //pixel suivant
           }//fin while pos_x, fin de la ligne
	}
 	else { l_bits+=shared->brush_x*shared->nb_bytes; l_brush_bits+=shared->brush_x; };
 	pos_y++;      //ligne suivante
	l_bits	     += line_length; 	
    }
 } 	  	  			
}

//------------------------------

else { //donc pas mask_mode
if (shared->active_tool==STAMP)
{
l_wbits   = l_bits_st;

pos_y = 0;
while (pos_y != shared->brush_y) //changement de ligne
{   

 if ((pos_y > to_cut_top) && (pos_y < to_cut_bottom) ) //si on est pas dans la zone à cutter
 { 
   
   pos_x=0;
   while (pos_x != shared->brush_x) //Changement de pixel sur la ligne
           { 
          transp = TheTables.tab_transp[paint_transp][*l_brush_bits];
  		  if ( ThePrefs.mask_activated == ON ) transp = TheTables.tab_mask_transp[transp][*l_mask_ptr];

  			   if ((pos_x > to_cut_left) && (pos_x < to_cut_right) ) //si on est pas dans la zone à cutter
  		      	 {  
  		      	 	switch(shared->paint_mode)
           				{
           				
							//EN STAMP MODE
           					case NORMAL:
           					*l_bits  =   TheTables.tab_normal[transp][*l_bits_st] +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++; l_bits_st++;
							*l_bits  =   TheTables.tab_normal[transp][*l_bits_st] +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++; l_bits_st++;
							*l_bits  =   TheTables.tab_normal[transp][*l_bits_st] +  TheTables.tab_pourcent_x_val[transp][*l_bits];
							break;
	 						
	 						case LIGHTEN:
	 						*l_bits  =   TheTables.tab_lighten[transp][*l_bits_st]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;l_bits_st++;
							*l_bits  =   TheTables.tab_lighten[transp][*l_bits_st]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;l_bits_st++;
							*l_bits  =   TheTables.tab_lighten[transp][*l_bits_st]  +  TheTables.tab_pourcent_x_val[transp][*l_bits];
	 						break;
	 			
	 						case DARKEN:
	 						*l_bits  =   TheTables.tab_darken[transp][*l_bits_st]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;l_bits_st++;
							*l_bits  =   TheTables.tab_darken[transp][*l_bits_st]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;l_bits_st++;
							*l_bits  =   TheTables.tab_darken[transp][*l_bits_st]  +  TheTables.tab_pourcent_x_val[transp][*l_bits];
	 						break;
	 						
	 						case MULTIPLY:
	 						tmp = TheTables.tab_multiply[shared->col_blue][*l_bits_st];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;l_bits_st++;
							tmp = TheTables.tab_multiply[shared->col_green][*l_bits_st];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;l_bits_st++;
							tmp = TheTables.tab_multiply[shared->col_red][*l_bits_st];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits];
	 						break;

							case COMBINE:
	 						tmp = TheTables.tab_combine[shared->col_blue][*l_bits_st];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;l_bits_st++;
							tmp = TheTables.tab_combine[shared->col_green][*l_bits_st];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;l_bits_st++;
							tmp = TheTables.tab_combine[shared->col_red][*l_bits_st];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits];
	 						break;
	 						
	 						case DIFFERENCE: //attention celui-ci se base sur wbits!
	 						tmp = TheTables.tab_difference[shared->col_blue][*l_wbits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++; l_wbits++; 
							tmp = TheTables.tab_difference[shared->col_green][*l_wbits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++; l_wbits++;
							tmp = TheTables.tab_difference[shared->col_red][*l_wbits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; 
	 						break;

	 					}
	 				
	 				}
	 			else {l_bits+=2; l_wbits+=2; l_bits_st+=2;}
		
    		l_bits    += 2;   l_wbits+=2;  l_bits_st+=2;//on avance et on saute l'alpha
    		l_mask_ptr++;
  			l_brush_bits++; //avance en 8 bits
      	    pos_x++;      //pixel suivant
           }//fin while pos_x, fin de la ligne
	}
 	else { l_bits+=shared->brush_x*shared->nb_bytes; l_bits_st+=shared->brush_x*shared->nb_bytes; l_wbits+=shared->brush_x*shared->nb_bytes; l_mask_ptr+=shared->brush_x; l_brush_bits+=shared->brush_x; };
 	pos_y++;      //ligne suivante
	l_bits	     += line_length_24; 	
	l_bits_st	 += line_length_24; 	
 	l_wbits	     += line_length_24; 
 	l_mask_ptr   += line_length;
 	
  }

}

else //PAS STAMP MODE DONC DESSIN NORMAL!!!!
{
pos_y = 0;
uint16 paper_pos_y = 0; uint16 paper_pos_x = 0;
while (pos_y != shared->brush_y) //changement de ligne
{   

 if ((pos_y > to_cut_top) && (pos_y < to_cut_bottom) ) //si on est pas dans la zone à cutter
 { 
   
   pos_x=0;
   while (pos_x != shared->brush_x) //Changement de pixel sur la ligne
           { 
			
          transp = TheTables.tab_transp[paint_transp][*l_brush_bits];
  		  if ( ThePrefs.mask_activated == ON ) transp = TheTables.tab_mask_transp[transp][*l_mask_ptr];
  	
  		  //Test paper
  		  //POUR LE QUE LE PAPIER MARCHE FAUDRAIT DEJA QUE LE TABLEAU TRANSP MASK FONCTIONNE
  		  //transp = TheTables.tab_mask_transp[transp][*l_paper];
  		  

  			   if ((pos_x > to_cut_left) && (pos_x < to_cut_right) ) //si on est pas dans la zone à cutter
  		      	 {  
  		      	 	switch(shared->paint_mode)
           				{
           				
           					case NORMAL:

           					*l_bits  =   TheTables.tab_addition[TheTables.tab_normal[transp][shared->col_blue]]  [TheTables.tab_pourcent_x_val[transp][*l_bits]]; l_bits++;
							*l_bits  =   TheTables.tab_addition[TheTables.tab_normal[transp][shared->col_green]] [TheTables.tab_pourcent_x_val[transp][*l_bits]]; l_bits++;
							*l_bits  =   TheTables.tab_addition[TheTables.tab_normal[transp][shared->col_red]]   [TheTables.tab_pourcent_x_val[transp][*l_bits]];



/*           					*l_bits  = *l_paper; l_bits++;
							*l_bits  = *l_paper; l_bits++;
							*l_bits  = *l_paper;
*/	 						
	 						break;
	 						
	 						case LIGHTEN:
	 						*l_bits  =   TheTables.tab_lighten[transp][*l_bits]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;
							*l_bits  =   TheTables.tab_lighten[transp][*l_bits]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;
							*l_bits  =   TheTables.tab_lighten[transp][*l_bits]  +  TheTables.tab_pourcent_x_val[transp][*l_bits];
	 						break;
	 			
	 						case DARKEN:
	 						*l_bits  =   TheTables.tab_darken[transp][*l_bits]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;
							*l_bits  =   TheTables.tab_darken[transp][*l_bits]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;
							*l_bits  =   TheTables.tab_darken[transp][*l_bits]  +  TheTables.tab_pourcent_x_val[transp][*l_bits];
	 						break;
	 						
	 						case MULTIPLY:
	 						tmp = TheTables.tab_multiply[shared->col_blue][*l_bits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;
							tmp = TheTables.tab_multiply[shared->col_green][*l_bits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;
							tmp = TheTables.tab_multiply[shared->col_red][*l_bits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits];
	 						break;

							case COMBINE:
	 						tmp = TheTables.tab_combine[shared->col_blue][*l_bits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;
							tmp = TheTables.tab_combine[shared->col_green][*l_bits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++;
							tmp = TheTables.tab_combine[shared->col_red][*l_bits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits];
	 						break;
	 						
	 						case DIFFERENCE: //attention celui-ci se base sur wbits!
	 						tmp = TheTables.tab_difference[shared->col_blue][*l_wbits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++; l_wbits++;
							tmp = TheTables.tab_difference[shared->col_green][*l_wbits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; l_bits++; l_wbits++;
							tmp = TheTables.tab_difference[shared->col_red][*l_wbits];
	 						*l_bits  =   TheTables.tab_normal[transp][tmp]  +  TheTables.tab_pourcent_x_val[transp][*l_bits]; 
	 						break;

	 					}
	 				
	 				}
	 			else {l_bits+=2; l_wbits+=2;}
		

		   if (paper_pos_x==shared->paper_x) { paper_pos_x=0; l_paper -= shared->paper_x;} //back au début horiz}
		   paper_pos_x++; l_paper++;

		
    		l_bits    += 2;   l_wbits+=2;  //on avance et on saute l'alpha
    		l_mask_ptr++;
  			l_brush_bits++; //avance en 8 bits
      	  pos_x++;      //pixel suivant
           }//fin while pos_x, fin de la ligne
	}
 	else { l_bits+=shared->brush_x*shared->nb_bytes; l_bits_st+=shared->brush_x*shared->nb_bytes; l_wbits+=shared->brush_x*shared->nb_bytes; l_mask_ptr+=shared->brush_x; l_brush_bits+=shared->brush_x; };

 	pos_y++;      //ligne suivante
	l_bits	     += line_length_24; 	
 	l_wbits	     += line_length_24; 
 	l_mask_ptr   += line_length;
	
	if (paper_pos_y==shared->paper_y) l_paper -=  shared->paper_y*paper_pos_x; //ligne du début mais décalage hori idem
	paper_pos_y++; 
//	else l_paper += shared->paper_x;
 	
  }

} 	  	  			

}

//shared->AddToUpdateZone(to_update);

//to_update est setté tout en haut
/*
BMessage *mmm = new BMessage(PAINT_RECT); mmm->AddRect("zone",to_update);
util.mainWin->PostMessage(mmm);
*/


//to_update est setté tout en haut
/*
BMessage *mmm = new BMessage(UPDATE_ME); mmm->AddRect("zone",to_update);
util.mainWin->PostMessage(mmm);
*/


}//fin zone ok

				


}//fin PasteBrush()

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


PicView::PicView(BRect r, share *sh, long flags) :
	BView(r, "picview", flags, B_WILL_DRAW |
							   B_SUBPIXEL_PRECISE | B_FULL_UPDATE_ON_RESIZE
							   | B_ASYNCHRONOUS_CONTROLS  | B_PULSE_NEEDED)
{
	//DONNEES PARTAGEES DANS SHARED
	shared = sh ;	
		
	//FAUT FAIRE çA une fois au début... après création de l'application en mémoire
	util.AppDir(); //met dossier du prg dans dossier_app
//---------------------------------------------------------------------------------------

//	shared->CreateNewImage(BRect(0,0,639,479),NULL,NULL,OFF); //revert mode offf

	//shared->loadBrush(shared->current_brush);

	shared->generateBrush(32);
    shared->EmptyPaper();			

	
	has_scrolled=OFF;
	
	painting = new Painting (shared);
	
	SetViewColor(140,140,140);
	
	stamp_color.red = 0; stamp_color.green =  128; stamp_color.blue = 255; 
    stamp_color.alpha = 128; //50%
    filtering =false;
       
}

PicView::~PicView()
{

}


   

void PicView::Pulse()
{

/*
if (shared->needs_update==true && ThePrefs.no_pictures_left ==OFF)
	{
		shared->act_img->UpdateDisplayImg(shared->what_to_update);
		
		SetHighColor(0,200,0); SetPenSize(1);
		StrokeRect(shared->what_to_update, B_SOLID_HIGH);	

		shared->needs_update=false;
		shared->first_zone=true;

		
		printf("\nUpdated: ");
		shared->what_to_update.PrintToStream();
		
	}
*/	
}

//------------------------------------------------------------------------------------

void PicView::Draw(BRect update_rect)
{


if (ThePrefs.no_pictures_left==OFF) //désactivé aussi pendant chargement et création.
	{
		if (filtering ==false)
		{
			if (shared->act_img!=NULL)
			{
		
			//si il y a pas ça le redraw des fenêtres du système délire (ça efface pas derrière)
			update_rect.left	= floor(update_rect.left/shared->act_img->zoom_level);
			update_rect.right 	= floor(update_rect.right /shared->act_img->zoom_level);
			update_rect.bottom 	= floor(update_rect.bottom /shared->act_img->zoom_level);
			update_rect.top 	= floor(update_rect.top/shared->act_img->zoom_level);
			update_rect.InsetBy(-1,-1); //pour arrondi
		
			DrawBitmap(shared->act_img->display_bitmap,update_rect,update_rect);
			//DrawGuides();
			}
		}
		else
		{
			DrawBitmap(util.sel_pic,update_rect,update_rect);
		}
		
		
	}
	
	


/*
SetHighColor(0,255,0);
SetPenSize(1);
StrokeRect(Bounds(),B_SOLID_HIGH);	
*/
}
//*****************************************************************************************************

void PicView::DrawGuides()
{
		//we're drawing the guides on the parent view also
		//so that they can be grabbed from the outside
		//errr... someday
		
		//NON La size interne de la parent view suit pas.. alors pour l'instant désactivé
		
		int32 x_guide_amount =0;
		int32 y_guide_amount =0;
		float pt;
		float ht    = Bounds().Height()-1;
		float wdt   = Bounds().Width()-1;
//		float p_ht  = Parent()->Bounds().Height()-1;
//		float p_wdt = Parent()->Bounds().Width()-1;

		//valeurs à décaler pour que ça colle avec la view incluse quand on dessine dans le parent
//	    float x_to_add = Frame().left;
//		float y_to_add = Frame().top;
	

//		Parent()->SetPenSize(1);
		SetPenSize(1/shared->act_img->zoom_level);
		
		int32 lay_amount = shared->act_img->layer_amount;
		 for (int32 i =0; i != lay_amount+1; i++)
		 {  
			x_guide_amount = shared->act_img->the_layers[i]->x_guides_amount;
			y_guide_amount = shared->act_img->the_layers[i]->y_guides_amount;
//			Parent()->SetHighColor(shared->act_img->the_layers[i]->guides_color);
			SetHighColor(shared->act_img->the_layers[i]->guides_color);

				//HORI Guides
		 		for (int32 j =0; j != x_guide_amount; j++)
		 		{
		 			pt = shared->act_img->the_layers[i]->x_guides[j];
// 					Parent()->StrokeLine(BPoint(pt+ x_to_add,0),BPoint(pt+ x_to_add,p_ht),B_SOLID_HIGH);
 					StrokeLine(BPoint(pt,0),BPoint(pt,ht),B_SOLID_HIGH);
				}
				
				//VERTI Guides
		 		for (int32 h =0; h != y_guide_amount; h++)
		 		{
		 			pt = shared->act_img->the_layers[i]->y_guides[h] ;
// 					Parent()->StrokeLine(BPoint(0,pt + y_to_add),BPoint(p_wdt,pt + y_to_add),B_SOLID_HIGH);
 					StrokeLine(BPoint(0,pt),BPoint(wdt,pt),B_SOLID_HIGH);
				}

								 
		 }

		 
}


//***************************************************************************

status_t PicView::_PasteLine(void *arg)
{

    ((Painting*)arg)->PasteLine(shared->pos_old, shared->pos_actuelle);
    return B_OK;
}

uint8 PicView::ZoneOK(BPoint pt)
{

if (pt.x    >= 0 	&& pt.y >= 0 	&& pt.x < (shared->act_img->pix_per_line-1) 
	&& pt.y >=10 
	&& pt.y < (shared->act_img->pix_per_row-1) 
	) //pas clicker en dehors image...
	{return true;}

	
else return false;
}

//----------------------------------------------------------------------------------

void PicView::BeforeDraw()
{
	uint32 buttons;
    GetMouse(&shared->pos_old, &buttons); 

   
 	 
	modified.Set(shared->pos_old.x,  shared->pos_old.y, 
	 			  shared->pos_old.x,  shared->pos_old.y); //rectangle 1x1 pix de base à agrandir par la suite.

}

void PicView::PrepareForUndo()
{

//---------------------------------------------------------------------------------------------
				//AGRANDIR le rectangle modifié
				if (shared->pos_actuelle.x <= modified.left)    modified.left   = shared->pos_actuelle.x;
				if (shared->pos_actuelle.x >= modified.right)   modified.right  = shared->pos_actuelle.x;
				if (shared->pos_actuelle.y <= modified.top) 	modified.top    = shared->pos_actuelle.y;
				if (shared->pos_actuelle.y >= modified.bottom)  modified.bottom = shared->pos_actuelle.y;

//---------------------------------------------------------------------------------------------

			    //RECTANGLE MODIF + LARGEUR BRUSH (dans la version dézoomée)
				modif_plus_brush=modified;		//attention on peut pas utiliser direct modified!		

				//plus largeur brush
				modif_plus_brush.left   -= shared->brush_x ;
				modif_plus_brush.right  += shared->brush_x ;
				modif_plus_brush.top    -= shared->brush_y ;
				modif_plus_brush.bottom += shared->brush_y ;

				//pas dépasser taille pic
				if (modif_plus_brush.left   <= 0) modif_plus_brush.left=0;				
				if (modif_plus_brush.right  >= shared->act_img->pix_per_line) modif_plus_brush.right  = shared->act_img->pix_per_line;				
				if (modif_plus_brush.top    <= 0) modif_plus_brush.top=0;				
				if (modif_plus_brush.bottom >= shared->act_img->pix_per_row)  modif_plus_brush.bottom = shared->act_img->pix_per_row;				
				
				/*
				Draw(modif_plus_brush);
				SetPenSize(1); SetHighColor(0,255,0); StrokeRect(modif_plus_brush,B_SOLID_HIGH);
				*/
}

//--------------------



//--------

//----------------------------------------------------------------------------------

void PicView::WithBucket(BPoint point,uint8 is_wand){

uint32 buttons;
if (ZoneOK(point)==true)
	{
	GetMouse(&shared->pos_old, &buttons);
	 
	fill_color = shared->fore_color; //par défaut
	if (buttons==B_SECONDARY_MOUSE_BUTTON)   fill_color = shared->back_color;

	fill_ori_color=shared->PickColorAt(point); //couleur sur laquelle on se base pour la tolerance
	
	BeforeDraw();
	updated_after_fill=OFF;
	fill_done=ON;
	FloodFill(shared->pos_old); //shared->pos_old est pris par le getmouse de BeforeDraw()
	}
	
	
	
	
	/*
	if (is_wand==ON){ shared->act_img->MemorizeUndo (modified,FORE_COLOR);  
					  shared->act_img->UpdateMaskedImg(modified);
                      }
					 else shared->act_img->MemorizeUndo (modified,MASK_FORE_COLOR);
	Draw(modified);
	*/				
}

//----------------------------------------------------------------------------------



//----------------------------------------------------------------------------------



//-----------------------------------------------------------------------


void PicView::FloodFill(BPoint pt)
{

//les coord ont étét divisées par With Bucket.

//fill_color et fill_ori_color sont settées dans WithBucket
//elles sont pas passées en paramètres à chaque fois

//ATTENTION AUCUN TEST SI LE PIXEL EST VALIDE ICI!!!! Seulement dans testVAL
     		   //SETPIXEL RECOPIE POUR SPEEDER LE NOEUD   
  	           uint8	*s_bits = shared->undo_bitmap_bits ;
  	           s_bits += uint32((pt.x+(shared->act_img->pix_per_line*pt.y))*4);; //avance jusqu'au pixel en question
			   *s_bits  = fill_color.blue;  	s_bits++;
			   *s_bits  = fill_color.green; 	s_bits++;
			   *s_bits  = fill_color.red;  //	s_bits++;
			   //*s_bits	= color.alpha;  
			   
        		shared->pos_actuelle=pt; //PrepareUndo divise déjà!	
				PrepareForUndo();
				
        //on teste en dézoomé mais on passe le paramètre original (avec zoom, donc pt)
        if(TestVal(BPoint(pt.x-1, pt.y)))     {FloodFill(BPoint(pt.x-1, pt.y));    }
		if(TestVal(BPoint(pt.x+1, pt.y)))     {FloodFill(BPoint(pt.x+1, pt.y));    }
        if(TestVal(BPoint(pt.x,   pt.y-1)))   {FloodFill(BPoint(pt.x,   pt.y-1));  }
        if(TestVal(BPoint(pt.x,   pt.y+1)))   {FloodFill(BPoint(pt.x,   pt.y+1));  }		


if (updated_after_fill==OFF)
	{
	beep();
	shared->act_img->MemorizeUndo (modified,FORE_COLOR);  
	//Draw(modified);				
	Draw(Bounds());
	updated_after_fill=ON;
	}
	

}

//vérifie tolérance
uint8 PicView::TestVal(BPoint pt)
{
//fill_color et fill_ori_color sont settées dans WithBucket
//elles sont pas passées en paramètres à chaque fois
/*
pt.x = floor(pt.x / *shared->zoom_level);
pt.y = floor(pt.y / *shared->zoom_level);
*/
if (  (pt.x > 1) && (pt.y > 1) && (pt.x < shared->act_img->pix_per_line-2)  && (pt.y < shared->act_img->pix_per_row-2) 
	       )
{
int16 delta_tol,tmp1,tmp2;
rgb_color picked=shared->PickColorAt(pt);

delta_tol  = (picked.blue  - fill_ori_color.blue);  if (delta_tol  <0) delta_tol *=-1;
tmp1 	   = (picked.green - fill_ori_color.green); if (tmp1 <0) tmp1*=-1;
tmp2       = (picked.red   - fill_ori_color.red);   if (tmp2 <0) tmp2*=-1;

delta_tol = (delta_tol+tmp1+tmp2)/3;  //on prend la moyenne des 3 valeurs pour la tolérance...


if (delta_tol <= ThePrefs.tolerance)	return true; //si diff pas assez grande on continue
else return false;

}
else return false;
}


void PicView::ResizeCanvas(int16 bor_left, int16 bor_top, int16 bor_right, int16 bot_bottom)
{
		util.NotImplemented();
}



//---------------------------------------------------------------------------------------------

void  PicView::CropSelected()
{
		
		BRect recta = shared->FindSelectedRect();
		
		SetDrawingMode(B_OP_INVERT);
		SetPenSize(1); StrokeRect(recta,B_SOLID_HIGH);	
		//FillRect(recta,B_SOLID_HIGH);
		SetDrawingMode(B_OP_COPY);
		
		uint8 old_mask_status = ThePrefs.mask_mode;
		ThePrefs.mask_mode=OFF; //Sinon ça merde si jamais on est en mask mode... (copyrect croit qu'il reçoit un truc en 8 bits)
		//util.toolWin->PostMessage(new BMessage(SET_MASK_BUTTON_OFF)); //juste le bouton...

		recta.Set(0,0, recta.Width()-1, recta.Height()-1);
		BBitmap* new_size = new BBitmap(recta,B_RGB32);
		util.CopyRect(recta,shared->act_img->undo_bitmap,recta, new_size,4);

		
		char str[NAME_SIZE];
		sprintf(str,shared->act_img->name);
		strcat(str," (");
		strcat(str,Language.get("CROPPED"));
		strcat(str,")");

		shared->NewImageFromBmp(str,new_size);
		delete new_size;
		
	//FAUDRAIT FAIRE  UN CREATE A PARTIR D'une image...
	//	shared->CreateNewImage(str,BRect(0,0,0,0), new_size, NULL, OFF); //crée new pic à partir de bbitmap le reste (file ou rect) = NULL
//		Draw(Bounds());
		
		ThePrefs.mask_mode = old_mask_status;
		
}





void PicView::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case B_KEY_DOWN:
		util.mainWin->PostMessage(msg);
		break;
		
	}
	
}





void PicView::MouseDown(BPoint where)
{
BPoint not_zoomed =where;

where.x = floor(where.x /shared->act_img->zoom_level);
where.y = floor(where.y /shared->act_img->zoom_level);

/*
	//WACOM SUPPORT
  Pressure and tilt data are as follows.

  B_MOUSE_DOWN/B_MOUSE_UP/B_MOUSE_DOWN
  ------------------------------------------------------
   "pressure"	B_FLOAT_TYPE		0.0 - 1.0
   "tiltX"	B_FLOAT_TYPE		-63.0 - 63.0(degree)
   "tiltY"	B_FLOAT_TYPE		-63.0 - 63.0(degree)
  ------------------------------------------------------
*/


if (ThePrefs.no_pictures_left==OFF)//-1 s'il y en a aucune
{

shared->stroke=ON;

//on stocke la taille (dé)zoomée!
shared->ori_pos = not_zoomed;

SetMouseEventMask(B_POINTER_EVENTS,B_LOCK_WINDOW_FOCUS | B_NO_POINTER_HISTORY); //NO POINTER HISTORY EVITE QUE LES MESSAGES s'entassent!
GetMouse(&shared->pos_old, &shared->ori_buttons); //seulement pour les boutons!!!
shared->pos_old = shared->ori_pos; //version zoomée!!

if (shared->ori_buttons == B_PRIMARY_MOUSE_BUTTON)   	
	{
	SetHighColor(shared->fore_color);
	switch(shared->active_tool)
		{
		case STAMP:	if (shared->stamp_offset_changed==ON) { shared->stamp_offset = shared->stamp_old_pos - shared->ori_pos;
										    shared->stamp_offset_changed = OFF; } break;	

		case BUCKET:
		BeforeDraw(); //exceptionnelement ici pour cet outil...
		fill_color = shared->fore_color; 	updated_after_fill=OFF; 
		FloodFill(shared->pos_old);
		return; //pour pas qu'il refasse beforedraw
		break;
		
/*

	///ROUTINE FILLL
	
	
	
uint32 buttons;
if (ZoneOK(point)==true)
	{
	GetMouse(&shared->pos_old, &buttons);
	 
	fill_color = shared->fore_color; //par défaut
	if (buttons==B_SECONDARY_MOUSE_BUTTON)   fill_color = shared->back_color;

	fill_ori_color=shared->PickColorAt(point); //couleur sur laquelle on se base pour la tolerance
	
	BeforeDraw();
	fill_done=ON;
	FloodFill(shared->pos_old); //shared->pos_old est pris par le getmouse de BeforeDraw()
	}
	
		Draw(modif_plus_brush);
				SetPenSize(1);
				StrokeRect(BRect(shared->pos_old,shared->pos_actuelle),shared->stripes);
				break;
	
	*/
		}
		
		if (ThePrefs.mask_mode==OFF)
			{
				shared->col_red = shared->fore_color.red; shared->col_green = shared->fore_color.green; shared->col_blue  = shared->fore_color.blue;
 				shared->bits = shared->undo_bitmap_bits;  shared->wbits = shared->the_bitmap_bits;
 			    shared->nb_bytes=4;	
 			}
 			else
 			{
				shared->bits	= shared->mask_work_bits;  shared->col_blue= (shared->fore_color.blue + shared->fore_color.green +shared->fore_color.red )/3; 
				shared->wbits	= shared->mask_bits;  	   shared->col_blue= (shared->fore_color.blue + shared->fore_color.green +shared->fore_color.red )/3; 
 			    shared->nb_bytes=1;	
 			}
 			
	}

if (shared->ori_buttons == B_SECONDARY_MOUSE_BUTTON) 	
	{ SetHighColor(shared->back_color);  
	
	switch(shared->active_tool)
		{
		case STAMP:	shared->stamp_offset_changed=ON;  shared->stamp_old_pos = shared->ori_pos; break;
		}
		if (ThePrefs.mask_mode==OFF)
			{
				shared->col_red   = shared->back_color.red;
 				shared->col_green = shared->back_color.green;
 				shared->col_blue  = shared->back_color.blue;
			 	shared->bits = shared->undo_bitmap_bits;  shared->wbits = shared->the_bitmap_bits;
			    shared->nb_bytes=4;	
 				 }
		else
 			{
 				shared->bits	= shared->mask_work_bits;  shared->col_blue= (shared->back_color.blue + shared->back_color.green +shared->back_color.red )/3; //moyenne gris des trois composantes
				shared->wbits	= shared->mask_bits;  	   shared->col_blue= (shared->back_color.blue + shared->back_color.green +shared->back_color.red )/3; //moyenne gris des trois composantes
			    shared->nb_bytes=1;	
 	
			}	
	}

shared->brush_bits = shared->the_brush_bits; //attention on utilise un autre pointeur car on va l'incrémenter

BeforeDraw();

MouseMoved(not_zoomed,0,NULL);
mov_rect=Frame(); //pour MOVE
}
}

void PicView::MouseMoved(BPoint where, uint32 transit, const BMessage* dragmsg)
{
BPoint not_zoomed = where;
uint32 buttons;

if (ThePrefs.no_pictures_left==OFF)//-1 s'il y en a aucune
{

where.x = floor(where.x /shared->act_img->zoom_level);
where.y = floor(where.y /shared->act_img->zoom_level);

BRect old_brush_rect;
old_brush_rect.Set(0,0,0,0);


//ACTIVATE_POINTER IF OVER GUIDE

/*
//-------------------------------------------------------------------------------------------------------
SetHighColor(shared->act_lay[i]->guides_color);

				//HORI Guides
		 		for (int32 j =0; j != x_guide_amount; j++)
		 		{
		 			pt = shared->act_img->the_layers[i]->x_guides[j];
// 					Parent()->StrokeLine(BPoint(pt+ x_to_add,0),BPoint(pt+ x_to_add,p_ht),B_SOLID_HIGH);
 					StrokeLine(BPoint(pt,0),BPoint(pt,ht),B_SOLID_HIGH);
				}
				
				//VERTI Guides
		 		for (int32 h =0; h != y_guide_amount; h++)
		 		{
		 			pt = shared->act_img->the_layers[i]->y_guides[h] ;
// 					Parent()->StrokeLine(BPoint(0,pt + y_to_add),BPoint(p_wdt,pt + y_to_add),B_SOLID_HIGH);
 					StrokeLine(BPoint(0,pt),BPoint(wdt,pt),B_SOLID_HIGH);
				}
				
//-------------------------------------------------------------------------------------------------------
*/			


if (transit==B_ENTERED_VIEW)  
{
		BMessage *mx = new BMessage(SET_CURSOR);
		mx->AddInt32("id",CURSOR_OF_ACTIVE_TOOL); //6666 CURSOR_OF_ACTIVE_TOOL
		util.mainWin->PostMessage(mx);
}

if (transit==B_EXITED_VIEW)  
{
		BMessage *mx = new BMessage(SET_CURSOR);
		mx->AddInt32("id",7); //7 = B_HAND_CURSOR
		util.mainWin->PostMessage(mx);
}


//NO POINTER HISTORY EVITE QUE LES MESSAGES s'entassent!
SetMouseEventMask(B_POINTER_EVENTS,B_LOCK_WINDOW_FOCUS | B_NO_POINTER_HISTORY); 


if  (shared->stroke==ON  && last_picked!=where )
{
	
shared->pressure = 1.0; //par défaut si on a une bête mouse au lieu d'une wacom

BMessage *message = Window()->CurrentMessage();
if (message!=NULL)
	{	
		if( message->FindFloat( "pressure", &shared->pressure ) != B_OK ) shared->pressure = 1.0;
		if (shared->pressure == 0) shared->pressure = 1.0; //si c'est la mouse on force à 100.
	} 
	
if (ThePrefs.option_win_open==true) util.optionWin->PostMessage(TRANSP_CHANGED);
	

	BMessage *mx;

	last_picked=where;

	shared->pos_actuelle=where; 
	PrepareForUndo();
	SetPenSize(floor(((shared->brush_rayon_x+shared->brush_rayon_y)*0.75))); //x0.75 because le smooth donne l'air plus piti
	rgb_color col_col;
		//REAL-TIME_PREVIEW
		switch (shared->active_tool)
			{	
				case ZOOM: 
				if (shared->ori_buttons==B_SECONDARY_MOUSE_BUTTON) 
				{
					mx = new BMessage(SET_CURSOR);
					mx->AddInt32("id",4); //4 = cursor_zoom_out
					util.mainWin->PostMessage(mx);
				}

				case LINE:
				Draw(modif_plus_brush);
				//faut dézoomer ori_pos pour l'écran
				StrokeLine(shared->ori_pos,where,B_SOLID_HIGH);
				break;
	
				case ELLIPSE:
				Draw(modif_plus_brush);
				StrokeEllipse(BRect(shared->ori_pos,where),B_SOLID_HIGH);
				break;
			
				case RECTANGLE:
				Draw(modif_plus_brush);
				StrokeRect(BRect(shared->pos_old,shared->pos_actuelle),B_SOLID_HIGH);
				break;
			
				case PICK_BRUSH:
				Draw(modif_plus_brush);
				SetPenSize(1);
				SetHighColor(0,0,0);
				//noir plus blanc pour le traitillé
				StrokeRect(BRect(shared->pos_old,shared->pos_actuelle),shared->stripes);
				break;
				
				case PICKER:
					if  (ZoneOK(where)==true)
						{ 
						col_col = shared->PickColorAt(where);
						if (shared->ori_buttons==B_PRIMARY_MOUSE_BUTTON)   shared->fore_color = col_col;
						if (shared->ori_buttons==B_SECONDARY_MOUSE_BUTTON) shared->back_color = col_col;
						util.toolWin->PostMessage(new BMessage(COL_SELECTED)); //envoie messge à la fenêtre mère
						
						}	
				break;

				case STAMP:
										
				//-----------------------------------------------------------------------------------------------------------
				if (shared->ori_buttons==B_SECONDARY_MOUSE_BUTTON) break; //on continue pas dans paintbrush si on change juste la source
				
				case PAINTBRUSH:		 
				
				to_draw= BRect(shared->pos_old,shared->pos_actuelle);
				//plus largeur brush
				to_draw.left   -= shared->brush_x ;
				to_draw.right  += shared->brush_x ;
				to_draw.top    -= shared->brush_y ;
				to_draw.bottom += shared->brush_y ;


				painting->PasteLine(shared->pos_old, shared->pos_actuelle);		
				
				shared->pos_old = where;
				break;
				
				
				case MOVE:
				if (shared->stroke==ON)
				{
				//ATTENTION si je fais
				//end = point, c'est pas comme le getmouse
				//apparement il y a un délai et çA fait tout merder (ca scrolle trop loin)
				//donc bien que dans un mousemoved on refait un getmouse pour les coord
	       		GetMouse(&shared->pos_actuelle, &buttons);
	       		if (!buttons) MouseUp(shared->pos_actuelle);
	       		
		   		shared->pos_actuelle.x = floor(shared->pos_actuelle.x /shared->act_img->zoom_level);
				shared->pos_actuelle.y = floor(shared->pos_actuelle.y /shared->act_img->zoom_level);

				if (shared->pos_old != shared->pos_actuelle)
	      	 		{
      	 			delta=shared->pos_old-shared->pos_actuelle;      	 	
      	 
      	 			//Pour EVITER BLOP quand on essaie de scroller out of limits...
					//---------------------------------
					/*
					//si nég et barre tout en haut à gauche on scrolle pas...
					if (delta.x < 0 && (jour->scroll_hori->Value() ) == 0)  delta.x=0;
					if (delta.y < 0 && (jour->scroll_verti->Value()) == 0)  delta.y=0;

					//si pos
					if (delta.x > 0 && (jour->scroll_hori->Value() )   >= 
						jour->map_x_size- (v_rect.right )-B_V_SCROLL_BAR_WIDTH)  delta.x=0;
				
					if (delta.y > 0 && (jour->scroll_verti->Value() )  >= 
						jour->map_y_size-(v_rect.bottom ) -B_V_SCROLL_BAR_WIDTH) delta.y=0;
					*/
					
					/*
					BPoint move_it(0,0);
					if (Bounds().Width() < util.mainWin->Bounds().Width()/2) 
						{ move_it.x = delta.x; delta.x = 0; }
					
					if (Bounds().Height() < util.mainWin->Bounds().Height()/2) 
						{ move_it.y = delta.y; delta.y = 0; }
					MoveBy(move_it.x,move_it.y);
       				
					*/	
      		 		
      		 		Parent()->ScrollBy(delta.x,delta.y);
      		 		
      		 		shared->pos_old=shared->pos_actuelle;
					shared->pos_old.x += delta.x; //car quand on scrolle le BPoint change aussi!!!
					shared->pos_old.y += delta.y; //faut le redécaler d'autant
					//v_rect.PrintToStream();
					}
				} //fin if stroke 				
				break;
			}
		
			
	}
		

	if (where != last_updated)
	{
		
		BMessage  amsg(UPDATE_DISPLAY);
		amsg.AddPoint("where",not_zoomed);
		util.mainWin->PostMessage(&amsg);
		
		
		if (ThePrefs.info_win_open==true) 
		 {		
			BMessage *my_msg = new BMessage(MOUSE_MOVEMENT); //message perso
			my_msg->AddFloat("x_pos",where.x );
			my_msg->AddFloat("y_pos",where.y );

			util.infoWin->PostMessage(my_msg);
		 }
		
		last_updated=where;

		
	}
}
}





//--------------------------------------------------------------------------------------------------------------------------

void PicView::MouseUp(BPoint where)
{

shared->stroke=OFF;


if (ThePrefs.no_pictures_left==OFF)//-1 s'il y en a aucune
{
//thread_id painting_thread;
uint8 has_modified=OFF;

SetPenSize((shared->brush_rayon_x+shared->brush_rayon_y)*0.75); //x0.75 because le smooth donne l'air plus piti
BPicture *my_pict; 
BRect rect;
has_scrolled=OFF;
//BMessage *msg_x;

bool uses_virtual_view=false;
	switch (shared->active_tool)
	{	
	//all tools that use virtualView
	case ELLIPSE: 		
		uses_virtual_view=true;
	break;
	
	//otherwise	
	default: 
		uses_virtual_view=false; 
	break;
	}

	if (uses_virtual_view==true)
	{
		shared->act_img->undo_bitmap->Lock(); //important!
		virtualView = new BView(shared->act_img->undo_bitmap->Bounds(), NULL, B_FOLLOW_NONE, 0 );
		shared->act_img->undo_bitmap->AddChild( virtualView );
	}
	
	//DESSIN FINAL
	switch (shared->active_tool)
	{	
	case ZOOM:
	if (shared->ori_buttons==B_PRIMARY_MOUSE_BUTTON)  	Window()->PostMessage(new BMessage(ZOOM_IN));;
	if (shared->ori_buttons==B_SECONDARY_MOUSE_BUTTON)	Window()->PostMessage(new BMessage(ZOOM_OUT));;
	break;

	case STAMP:
	case PAINTBRUSH: 	  
	has_modified=ON;
	break; 
/*	case BUCKET: 		WithBucket(point,OFF); break; 
	case WAND:	 		WithBucket(point,ON); break; //ON dit que c'est une wand donc on draw on the mask...
	case ZOOM: 			WithZoom(point);   break; 
	case PICKER: 		WithPicker(point); break; 
	case ELLIPSE: 		WithEllipse(point);   break; 
	case MOVE: 			WithHand(point);   break; 
*/
	case LINE:
	
	//painting->PasteLine(shared->pos_old, shared->pos_actuelle);
	smooth_line(shared->pos_old, shared->pos_actuelle);
	has_modified=ON;
 	break; 

	case ELLIPSE: 		
	virtualView->BeginPicture(new BPicture); 
    virtualView->StrokeEllipse(BRect(shared->pos_old,shared->pos_actuelle),B_SOLID_HIGH);
    my_pict = virtualView->EndPicture();
	has_modified=ON;
 	break; 
 	
	case PICK_BRUSH:
	
	    rect = BRect(shared->ori_pos,where); 
   		//si la brush est à zéro on la prend pas
   		if ((rect.RightBottom() != rect.LeftTop()))
   		{
   			//faut que le rectangle commence en haut a droite, le cas échéant on inverse les points
   			float tx;
   			if (rect.bottom < rect.top) { tx = rect.bottom; rect.bottom = rect.top;  rect.top = tx;}
   			if (rect.right < rect.left) { tx = rect.right;  rect.right  = rect.left; rect.left = tx;}
	   		shared->the_brush_24 = util.GrabRect(rect,  shared->act_img->undo_bitmap); 
	   		shared->UseAsBrush(shared->the_brush_24);
   		}  else beep();

   		
	    //restaure l'ancien tool
	    if ( shared->previous_tool!=PICK_BRUSH) shared->active_tool=shared->previous_tool;
		else shared->active_tool=PAINTBRUSH;
		util.mainWin->PostMessage(TOOL_CHANGED);
		has_modified=OFF;
		break;
				
	}//FIN SWITCH TOOL


	if (uses_virtual_view==true)
	{

	  virtualView->DrawPicture(my_pict);
	  shared->act_img->undo_bitmap->RemoveChild(virtualView );	// A faire avant la destruction de la bitmap sinonm problemes
	  shared->act_img->undo_bitmap->Unlock();
	} 



if (has_modified==ON) //si c'est un outil de dessin on memorize l'undo
	{
	if (ThePrefs.mask_mode == OFF) 	shared->act_img->MemorizeUndo (modif_plus_brush,FORE_COLOR);
    else shared->act_img->MemorizeUndo(modif_plus_brush,MASK_FORE_COLOR);       //C'est toujours FORE... mais de toute façon on s'en fout c'est juste pour savoir si c'est mask ou pas.,.
			
		shared->act_img->UpdateDisplayImg(modif_plus_brush);
		
		if (ThePrefs.layer_selector_open==true)
		{		
			BMessage msg(UPDATE_ACTIVE_LAYER);
			msg.AddInt32("active",shared->act_lay->id);
			util.layerWin->PostMessage(&msg);		
		}		
	}


		//restore le curseur normal si zoom ou un autre l'a modifie
		BMessage *mx = new BMessage(SET_CURSOR);
		mx->AddInt32("id",CURSOR_OF_ACTIVE_TOOL); //6666 CURSOR_OF_ACTIVE_TOOL
		util.mainWin->PostMessage(mx);
 }


shared->pressure = 0;
if (ThePrefs.option_win_open==true) util.optionWin->PostMessage(TRANSP_CHANGED);


}



void PicView::UpdateDisplay(BPoint point)
{

BPoint not_zoomed = point;

point.x = floor(point.x /shared->act_img->zoom_level);
point.y = floor(point.y /shared->act_img->zoom_level);

uint16 x_size =  (uint16) (shared->brush_rayon_x * shared->act_img->zoom_level);
uint16 y_size  = (uint16) (shared->brush_rayon_y * shared->act_img->zoom_level);

				

if (shared->act_img!=NULL)
{

//--------------------------------------------
//POINTEUR EN CERCLE (+ croix source si nécessaire)
//--------------------------------------------
SetPenSize(1);


//Update zone ancien circle pointer une last time si on changé pour un outil qui montre pas cercle 
if (was_circle==ON)
{
BRect la_zone;




la_zone.Set(old_pos_moved.x-old_x_size -3, old_pos_moved.y-old_y_size-3,
		   old_pos_moved.x+old_x_size+3 ,  old_pos_moved.y+old_y_size+3);
/*
la_zone.InsetBy(-((la_zone.Width()  *shared->act_img->zoom_level) -la_zone.Width())   /2,
				-((la_zone.Height() *shared->act_img->zoom_level) -la_zone.Height())  /2  );
*/
		   
Draw(la_zone);
//pour le stamp le cas échéant..
if (shared->active_tool==STAMP)
		{
		la_zone.OffsetBy(shared->stamp_offset);
		Draw(la_zone);
		}
	
was_circle=OFF;

}



switch (shared->active_tool) //ouais c'est le même switch mais c'est plus clair et pis bon on a pas besoin d'optimiser ici
		{		
				case PAINTBRUSH:	
				case ERASER:		
				case LINE:			
				case SPLINE:		
				case RECTANGLE:		
				case ELLIPSE:		
				case STAMP:		
				//pour tous ceux-ci on affiche la taille brush (ceux qui tirent des traits)

				was_circle=ON;
				//Update zone ancien circle pointer
				Draw(BRect(old_pos_moved.x-old_x_size , old_pos_moved.y-old_y_size,
		   		old_pos_moved.x+old_x_size , old_pos_moved.y+old_y_size));

				
					if (shared->brush_x < 100) //plein en mini, après en outline
							{
						  	    SetDrawingMode(B_OP_INVERT); 
								StrokeEllipse(point,shared->brush_rayon_x,shared->brush_rayon_y);
							}		
					else
					{
					
					//sinon rien... ça saccade trop
					was_circle=OFF;
					}
								
				if (shared->active_tool==STAMP && shared->stamp_offset_changed==OFF)	
					{
					
					BPoint tt =shared->stamp_offset;
					 SetDrawingMode(B_OP_ALPHA); 
       				SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_OVERLAY); 
				    SetHighColor(stamp_color); 					
					if (shared->brush_x < 100) //plein en mini, après en outline
					   FillEllipse(point + tt ,shared->brush_rayon_x,shared->brush_rayon_y);
					   else {
					   		SetPenSize(2);
					   		StrokeEllipse(point + tt ,shared->brush_rayon_x,shared->brush_rayon_y);
					   		}
					   
					}
				SetDrawingMode(B_OP_COPY);

				old_pos_moved = not_zoomed;
			
				old_x_size = x_size;
				old_y_size = y_size;
				
				break;	

			}//end switch
}
}
//*****************************************************************************************************

//*****************************************************************************************************


void PicView::smooth_line(BPoint start, BPoint end)
{

    uchar mp[256];

    for (int32 i = 0;i < 32; i++) {
        mp[i] = i*8;
    }
    
    char str[255];
    //--------------
   
   	int32    x1,y1,x2,y2;
   	int32    dx,dy;
    int32   sy;
    int32    rowbyte;
    int32    error;
    int32    cpt;
    uchar   *base;
    float   k;
	
	uchar *composante;
		
	x1 = int32 (floor(start.x));	
	y1 = int32 (floor(start.y));	
	x2 = int32 (floor(end.x));	
	y2 = int32 (floor(end.y));	

//get bitmap base
    rowbyte = shared->act_img->pix_per_line*4;
//number of byte per row

   dx = x2-x1;
// delta x
    dy = y2-y1;
// delta y

    base = shared->bits + ( ( y1 * rowbyte) + x1*4 );
//compute start point address

    if (dx==0 && dy==0) {
//single pixel case optimisation
        *base = *mp;
        
        beep();
        BMessage *mmm = new BMessage(PAINT_RECT); mmm->AddRect("zone",BRect(start,end));
		util.mainWin->PostMessage(mmm);

        return;
    }



    if (dy<0) {
//if the line goes up
        sy = -1;
//step up
        dy = -dy;
//get dy positive again
        rowbyte = -rowbyte;
//invert rowbyte to go up
    }
    else
        sy = 1;

    if (dx > 0) {
//if going right
        if (dx > dy) {
//select case, is it more horizontal than vertical?
            cpt = x2 - x1;
//how many pixels do we draw
            k = (31*65536)/(dx);
//init the error
            dy = (int)(dy*k);
//scale the dithered variables
            dx = (int)(dx*k);
            error = dx >> 1;
//init the error at half the delta x

            while(cpt>=0) {
            	*base = mp[31 - (error>>16)];
            	*(base+1) = mp[31 - (error>>16)];
            	*(base+2) = mp[31 - (error>>16)];
            	*(base+3) = mp[31 - (error>>16)];

//write a pixel
                cpt--;
                *(base+(rowbyte)) = mp[(error>>16)];
              	*(base+(rowbyte)+1)= mp[(error>>16)];
            	*(base+(rowbyte)+2) = mp[(error>>16)];
            	*(base+(rowbyte)+3)= mp[(error>>16)];
//and its complement one line up or down
                base+=4;
                //go right one pixel
                error += dy;
//update the error dither
                if (error >= dx) {
                    base += rowbyte;
//go up or down one line if the error is greater than dx
                    error -= dx;
                }
            }
        }
        else {
//second case, more horizontal than vertical
            cpt = dy;
                //how many pixels to draw
            k = (31*65536)/(dy);

            dy = (int)(dy*k);
            dx = (int)(dx*k);
            //scale and init the dithered variables
            error = dy>>1;
            while(cpt >= 0) {

				*base = mp[31 - (error>>16)];
            	*(base+1) = mp[31 - (error>>16)];
            	*(base+2) = mp[31 - (error>>16)];
            	*(base+3)=  mp[31 - (error>>16)];
//write a pixel
                cpt--;
                *(base+1) = mp[(error>>16)];
            	*(base+1+1) = mp[ (error>>16)];
            	*(base+1+2) = mp[(error>>16)];
            	*(base+1+3)= mp[ (error>>16)];
    

//and its complement one to the right
                base += rowbyte;
//go one line down (or up).
                error += dx;
//update the error
                if (error >= dy) {
                    base+=4;
//go one right if necessary
                    error -= dy;
                }
            }
        }
    }
    else {
//basically the same code by going left
        dx = -dx;
//instead of right
        if (dx > dy) {
            error = dx >> 1;
//the code could be compacted by using
            cpt = dx;
//a positive or negative value for the
            k = (31*65536)/(dx);
//horizontal direction, but there is a
            dy = (int)(dy*k);
//small performance advantage into writing
            dx = (int)(dx*k);
//the explicit code.

            while(cpt>=0) {
            
                *(base) = mp[31 - (error>>16)];
            	*(base+1) = mp[31 - (error>>16)];
            	*(base+2) = mp[31 - (error>>16)];
            	*(base+3)=  mp[31 - (error>>16)];
                
                cpt--;
           
          	    *(base+(rowbyte)) = mp[(error>>16)];
            	*(base+(rowbyte)+1) = mp[(error>>16)];
            	*(base+(rowbyte)+2) = mp[ (error>>16)];
            	*(base+(rowbyte)+3)=  mp[(error>>16)];
                
                base--;
                error += dy;
                if (error >= dx) {
                    base += rowbyte;
                    error -= dx;
                }
            }
        }
        else {
            error = dy>>1;
            cpt = dy;
            k = (31*65536)/(dy);
            dy = (int)(dy*k);
            dx = (int)(dx*k);

            while(cpt >= 0) {
            
            
                *(base) = mp[31 - (error>>16)];
            	*(base+1) = mp[31 - (error>>16)];
            	*(base+2) = mp[31 - (error>>16)];
            	*(base+3)=  mp[31 - (error>>16)];
             
                cpt--;
                *(base-1) = mp[ (error>>16)];
            	*(base-1+1) = mp[(error>>16)];
            	*(base-1+2) = mp[(error>>16)];
            	*(base-1+3)= mp[(error>>16)];
            	
                base += rowbyte;
                error += dx;
                if (error >= dy) {
                    base--;
                    error -= dy;
                }
            }
        }
    }
BMessage *mmm = new BMessage(PAINT_RECT); 
//mmm->AddRect("zone",BRect(0, 0, shared->act_img->pix_per_line-1, shared->act_img->pix_per_row-1));
mmm->AddRect("zone",BRect(start,end));
util.mainWin->PostMessage(mmm);

}


void PicView::CopyUnfiltered()
{
	if (ThePrefs.mask_mode==OFF)	
	{
	util.CopyRect(selected_zone, shared->act_img->undo_bitmap, BRect(0,0, selected_zone.Width(),
						  selected_zone.Height()-1), util.sel_pic,   4);
	}					  
	else 
	{
	util.CopyRect(selected_zone, shared->act_img->mask_undo_bitmap, BRect(0,0, selected_zone.Width(),
						  selected_zone.Height()-1), util.sel_pic,   1 );
	}
}
//---------------------------------------------------------------------------------------------

void  PicView::PrepareFilter()
{
		ln_count = 0;
			
		if (ThePrefs.mask_mode==OFF)
		{
			selected_zone = shared->FindSelectedRect(); //on agit que sur des rects qui font partie de la sélection
			//1. on copie l'image dans un bitmap
			util.sel_pic = new BBitmap(BRect(0,0, selected_zone.Width(),selected_zone.Height()), B_RGB32);
			ln_width = util.sel_pic->Bounds().Width()*4;

		}
		else
		{
			selected_zone = shared->act_lay->img->Bounds(); //pas de sélection dans la sélection pour l'instant
			util.sel_pic = new BBitmap(BRect(0,0, selected_zone.Width(),selected_zone.Height()), B_GRAY8);
			ln_width = util.sel_pic->Bounds().Width();
		}
		
		CopyUnfiltered();

		util.sel_pic_bits = (uint8 *) util.sel_pic->Bits();
		util.sel_length   = util.sel_pic->BitsLength();
		
		
		
		filtering = true;


}

void  PicView::InitProgress()
{


		if (util.sel_length >= SHOW_PROGRESS_LIMIT) 
		{
		
		show_progress=ON;		
		util.progress_win->Show();

		BMessage *mm = new BMessage(SET_PROGRESS_NAME);
		mm->AddString("text",Language.get("FILTERING"));
		util.progress_win->PostMessage(mm);
		
		util.progress_win->PostMessage(RESET_PROGRESS);
		util.progress_win->PostMessage(SET_PROGRESS_COLOR_FILTER);
		
		percent_val = util.sel_length/100; //update chaque pourcent
		percent_ctr = 0;
	    percent=0;	
	    }
	    else show_progress = OFF;

}

void  PicView::EndProgress()
{
		util.progress_win->Hide();

}

//---------------------------------------------------------------------------------------------

void  PicView::FilteringDone()
{

	if (ThePrefs.mask_mode==OFF)
		{
		//ATTENTION TOUJOURS UTILISER l'undo bitmap!
		util.CopyRectWithMask(util.sel_pic->Bounds(),util.sel_pic, selected_zone, 
					shared->act_img->undo_bitmap, shared->act_img->mask_bitmap,4);
					
		shared->act_img->MemorizeUndo(selected_zone,FORE_COLOR);
		}
		else
		{
		//ATTENTION TOUJOURS UTILISER l'undo bitmap!
		util.CopyRect(util.sel_pic->Bounds(), util.sel_pic, selected_zone, 
					shared->act_img->undo_bitmap, 1);
		shared->act_img->MemorizeUndo(selected_zone,MASK_FORE_COLOR);
		}
		
		filtering = false;

		shared->act_img->UpdateDisplayImg(selected_zone);
	
		//si on l'efface ça crashe au retour de filter_limit_levels par ex.
	//	delete util.sel_pic; //on efface la pic temp...
}


void PicView::UpdateProgress(uint32 bytes_updated)
{ 
if (show_progress==ON)
{
	percent_ctr += bytes_updated;

	if ( percent_ctr  >= percent_val) 
				{

				percent+=1.0;	
				
					BMessage *p = new BMessage(UPDATE_PROGRESS);
					p->AddInt32("value",1); util.progress_win->PostMessage(p); 
					util.progress_win->PostMessage(p); 
			percent_ctr=0; 

				}
	} //fin if show progress == on
	
}

//---------------------------------------------------------------------------------------------
void  PicView::Filter_SlideHSV()
{



		PrepareFilter();
		InitProgress();

		
		float r,g,b,h,s,v;
		
		for (uint32 pos=0; pos!=util.sel_length;  pos+=4)
		{	

			//FILTRE A PROPREMENT PARLER -----------------
			
			r = *util.sel_pic_bits;  util.sel_pic_bits++; 
			g = *util.sel_pic_bits; 	util.sel_pic_bits++; 
			b = *util.sel_pic_bits;  	util.sel_pic_bits++; 
			util.sel_pic_bits++; //skip the alpha
			
			util.RGBtoHSV( r, g, b, &h, &s, &v );
			h+=60;
			util.HSVtoRGB( &r, &g, &b, h, s, v);
			
			util.sel_pic_bits-=4; //on revient en arrière
			*util.sel_pic_bits = r; util.sel_pic_bits++; 
			*util.sel_pic_bits = g; util.sel_pic_bits++; 
			*util.sel_pic_bits = b; util.sel_pic_bits++; 
			util.sel_pic_bits++; //skip the alpha
			
			pos+=4;	
            if ((ln_count+=4) >= ln_width)  { ln_count=0; UpdateProgress(ln_width); } //bytes modifiées

			//--------------------------------------------
		
		}
		
		EndProgress();
		FilteringDone();
		
		
}

void  PicView::InvertSelection()
{
	uint8 *bits = shared->mask_work_bits;
 	
 	uint32 pos=0;
	uint32 length = shared->act_img->mask_undo_bitmap->BitsLength();
	while (pos!=length)   {	*bits = 255-*bits ; 	bits++; pos++; } //tout à 255 (transparent)
	shared->act_img->MemorizeUndo(shared->act_img->mask_undo_bitmap->Bounds(),MASK_FORE_COLOR);
	shared->act_img->UpdateDisplayImg(shared->act_img->mask_undo_bitmap->Bounds());
	
	//Draw(shared->act_img->mask_undo_bitmap->Bounds()); //et non pas Draw(sel_pic->Bounds()); !!!
}

void  PicView::Filter_Invert()
{

if (ThePrefs.mask_mode==OFF)
{
		
		PrepareFilter();
		InitProgress();
		
		for (uint32 pos = 0; pos!=util.sel_length; pos++)
		{	

			//FILTRE A PROPREMENT PARLER -----------------
			*util.sel_pic_bits = 255-*util.sel_pic_bits ;
			 util.sel_pic_bits++;  
			//--------------------------------------------
			
			if (ln_count++ >= ln_width)  { ln_count=0; UpdateProgress(ln_width); } //bytes modifiées
		} 
		
		EndProgress();
		FilteringDone();
}
else InvertSelection();

}



void PicView::Filter_Rotate180()
{

PrepareFilter();
InitProgress();

uint32 percent_val =0; //update tous les deux pourcent
uint32 percent_ctr = 0;
		

//mode 8bits ou pas

uint8 *pic_bits = util.sel_pic_bits;
uint8 *flipped_bits = util.sel_pic_bits;

uint32 taillePic=0;
uint8 tmp0,tmp1,tmp2,tmp3;

			
if (ThePrefs.mask_mode==OFF)
{

//Mode 24 bits
 taillePic=(util.sel_length/4)/2; //div par deux parce qu'on flipe la moitité du haut avec celle du bas

//on va à la fin de la pic
pic_bits +=  util.sel_length; //sur le red du dernier


//et on swappe du début à la fin
for (uint32 i = 0; i != taillePic; i++)
	{
	
		
		//sauve avant d'écraser
		tmp0 = *(flipped_bits+0);//red
   	    tmp1 = *(flipped_bits+1); //green
   	    tmp2 = *(flipped_bits+2); //blue
   	    tmp3 = *(flipped_bits+3); //alpha

		//copy b to a
		*(flipped_bits+0) = *(pic_bits+0); //red
   	    *(flipped_bits+1) = *(pic_bits+1); //green
   	    *(flipped_bits+2) = *(pic_bits+2); //blue
   	    *(flipped_bits+3) = *(pic_bits+3); //alpha
   	    
   	    //copy temp (=a) to b 
		*(pic_bits+0) = tmp0; //red
   	    *(pic_bits+1) = tmp1; //green
   	    *(pic_bits+2) = tmp2; //blue
   	    *(pic_bits+3) = tmp3; //alpha
   	    

 	   pic_bits -= 4;       
       flipped_bits += 4;
       if (ln_count+=4 >= ln_width)  { ln_count=0; UpdateProgress(ln_width); } //bytes modifiées

	}	

}//fin if mask mode = OFF
else
{

//Mode 8 bits
 taillePic=(util.sel_length)/2;

 //on va à la fin de la pic
pic_bits +=  util.sel_length;

//et on swappe du début à la fin
for (uint32 i = 0; i < taillePic; i++)
	{
		
	     tmp0 = *(flipped_bits+0);//red
	     *(flipped_bits+0) = *(pic_bits+0); //red
         *(pic_bits+0) = tmp0; //red
 
   	   pic_bits --;    flipped_bits ++;
   	   
   	   if (ln_count++ >= ln_width)  { ln_count=0; UpdateProgress(ln_width); } //bytes modifiées

	}	

}//fin mode mask (8bits)

EndProgress();
FilteringDone();

}//fin rotate180

void PicView::Filter_FlipHori()
{

PrepareFilter();
InitProgress();
		
uint32 percent_val =0; //update tous les deux pourcent
uint32 percent_ctr = 0;
		
//mode 8bits ou pas
uint8 *pic_bits = util.sel_pic_bits;
uint8 *flipped_bits = util.sel_pic_bits;

uint8 tmp0,tmp1,tmp2,tmp3;
int32 x = selected_zone.Width()-1; 
int32 y = selected_zone.Height()-1;
int32 i,j;

if (ThePrefs.mask_mode==OFF)
{
//Mode 24 bits
//on va à la fin première ligne
pic_bits +=  (x)*4;

//on flippe ligne après ligne
for (i = 0; i != y; i++)
	{
			for (j = 0; j != ceil(x/2); j++)
      {


          tmp0 = *(flipped_bits+0); //red
   	      tmp1 = *(flipped_bits+1); //green
   	      tmp2 = *(flipped_bits+2); //blue
   	      tmp3 = *(flipped_bits+3); //alpha
   	      
          *(flipped_bits+0) = *(pic_bits+0); //red
   	      *(flipped_bits+1) = *(pic_bits+1); //green
   	      *(flipped_bits+2) = *(pic_bits+2); //blue
   	      *(flipped_bits+3) = *(pic_bits+3); //alpha

	      *(pic_bits+0) = tmp0; //red
   	      *(pic_bits+1) = tmp1; //green
   	      *(pic_bits+2) = tmp2; //blue
   	      *(pic_bits+3) = tmp3; //alpha
		  
		
           pic_bits -= 4;       flipped_bits += 4;
      } //fin x 

       UpdateProgress(ln_width);  //bytes modifiées

		pic_bits +=  (x+1)*4; //2 fois parce qu'on est revenu en arrière  pour flipper!	
		flipped_bits += 4;
	}	//fin y


   	  
}//fin if mask mode = OFF
else
{

}//fin if mask mode = OFF

EndProgress();
FilteringDone();

}//fin flip_H



void PicView::Filter_LimitLevels(uint8 levels)
{
		//Isohélie = limite le nombre de niveau ... eg 16 niveaux etc.
		//c'est juste une question d'arrondir les chiffres au multiple plus proche

InitProgress();
CopyUnfiltered();

if (levels <1) levels = 1;
uint8 step = 255/levels;
if (step <1) step = 1;

uint8 *pic_bits = util.sel_pic_bits;

for (uint32 i = 0; i != util.sel_length; i++)
	{

   	 *pic_bits = uint8((*pic_bits / step) * step);
  	  pic_bits++;
  	  
      if (ln_count++ >= ln_width)  { ln_count=0; UpdateProgress(ln_width); } //bytes modifiées
	}	

EndProgress();


}



/*
//A INTEGRER
//Dans chaque undo_data rajouter la taille actuelle de l'image
//comme ça on peut restaurer easy en cas de crop ou autre resize








//FAIRE FLIP_Y quand ça marchera...

void PicView::Filter_FlipX()
{

//mode 8bits ou pas

uint32 *pic_bits = shared->bitmap_work_bits;
uint32 *flipped_bits = dest_bits;

int32 x = shared->act_img->pix_per_line; //on va que jusqu'à la moitié
int32 y = shared->act_img->pix_per_row;
uint32 i,j;

if (shared->mask_mode==OFF)
{
//Mode 24 bits

 //on va à la fin première ligne
pic_bits +=  (pix_per_row-1)*4;

//on flippe ligne après ligne
for (i = 0; i != y; i++)
	{
			for (j = 0; j != x/2; j++)
      {
   	      *(flipped_bits+0) = *(pic_bits+0); //red
   	      *(flipped_bits+1) = *(pic_bits+1); //green
   	      *(flipped_bits+2) = *(pic_bits+2); //blue
   	      *(flipped_bits+3) = *(pic_bits+3); //alpha

 	        pic_bits -= 4;       flipped_bits += 4;
      } //fin x 

		pic_bits +=  x*4;		flipped_bits +=  x*4;
	}	//fin y

}//fin if mask mode = OFF
else
{
//Mode 8 bits (mask)

 //on va à la fin première ligne
pic_bits +=  (pix_per_row-1);

//on flippe ligne après ligne
for (i = 0; i != y; i++)
	{
			for (j = 0; j != x/2; j++)
      {
   	      *flipped_bits = *pic_bits; //red
 	        pic_bits--;
  	       flipped_bits++;
      } //fin x 

		pic_bits ++;   flipped_bits ++;
	}	//fin y

}//fin mode mask (8bits)


}






void HistogramView::BuildHistogramData()
{
		//Histogramme -- graphique à barres montrant la quantité de pixels
		//pour chaque valeur de 0 à 255
		//reste surtout à pondre l'interface;
	
uint32 data[255];
float percent_data[255];
uint32 *pic_bits = shared->bitmap_work_bits;


//tout à zéro pour commencer
for (uint32 x = 0; x != 255; x++) data [x] =0;

//on ajoutes les pixels selon les valeurs
for (uint32 i = 0; i != taillePic; i++)
	{
			data[*pic_bits]+=1;
  	  pic_bits++;
	}	

//et les pourcentages
for (uint32 y = 0; y != 255; y++) percent_data[y] = 100* (data[y] / taillePic);

}

void HistogramView::Draw()
{
		BPoint startPos(0,0);
		BPoint endPos(0,100);
	
		SetHighColor(0,0,0);

		for (uint32 x = 0; x != 255; x++) 
		{
		 startPos.y = 100-percent_data [x];
		 StrokeLine(startPos,endPos, B_SOLID_HIGH);
 	
	   startPos.x+=1; 
   	endPos.x +=1;

		}

}





void PicView::Filter_BrightnessContrast(int8 bright, int8 contrast)
{

//Théorie
//pour brightness on additione juste la valeur en question (idem que photoshop)
//pour contrast on rapproche la couleur la plus sombre de la plus claire...
//on monte le plus sombre en % de 255 

//pour contraste
//d'abord on doit trouver les valeurs extremes de la pic
uint32 *search_bits = shared->bitmap_work_bits;

uint8 darkest =255; //valeurs maximum inverses pour la recherche
uint8 lightest =0;

for (uint32 i = 0; i != taillePic; i++)
	{
   	 if (*search_bits  > lightest) lightest = *search_bits;
   	 if (*search_bits  < darkest)  darkest   = *search_bits;
  	  search_bits++;
	}	

//si plus grand que 100 on modifie le blanc sinon le noir
if (contrast <=100)   darkest = (darkest*contrast)/100);  
												 else lightest = (lightest*contrast)/100);

//-------------------------------------------------------------------

float current;

uint32 *pic_bits = shared->bitmap_work_bits;
uint32 *flipped_bits = dest_bits;

uint8 new_values[255];
int16 temp;

//calcul du tableau des valeurs 
for (uint32 i = 0; i != 255; i++)
	{
			//apply brightness ------------------------------------------
      temp = i +bright;

			//apply contrast ------------------------------------------
			current = temp/255;

      if (contrast <=100)  temp =   temp =  darkest + ((lightest-darkest)*current);
				                        			else 		 temp =  lightest - ((lightest-darkest)*current);

			//vérif validité-----------------------------------
      if (temp < 0 )  new_values[i] =0;
	    else 
			{
				if (temp > 255) new_values[i] =255; 
		    else new_values[i] = uint8(temp);
      }  

  }


}//fin brightnessContrast filter



Dans photoshop:
Généraliser la sélection = sélection plage couleurs (mode addition) 
Etendre la sélection = plusieurs coups de baguette magique ( aux bords de la sélection et en mode addition) donc pix adjacents seul sélectionnés



*/
