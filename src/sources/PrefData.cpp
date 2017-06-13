#include "PrefData.h"
#include "defs.h"

PrefData ThePrefs;
CalcTables TheTables;

CalcTables::CalcTables()
{
//----------------------------------------
//CREATION DES TABLES DE CONVERSION tranparence etc

	//TABLEAU 100*256
	int32 aa,bb,tmp;
	aa = 0;
	bb = 0;
	while (aa!=100+1) //Non pas 100 +1!!!! je comprends pas tout... mais bon hein... ça marche..
	{
			while (bb!=255+1)
			{
			//tableau_transp[aa][bb] = valeur;
			tmp = int32 ( ( ((float(bb)) / 256) + (1-((float(bb)) / 256))* (float(aa)/100))*100  );
			if (tmp > 255) tmp=255; if (tmp < 1) tmp = 1; //non pas zéro sinon reboucle... je sais pas pourquoi mais bon ça marche non?
			tab_transp[aa][bb] =  uint8(tmp);
			
			tmp = int32 (((aa*bb)/100));	 if (tmp > 255) tmp=255;  if (tmp <0) tmp=0;  tab_pourcent_x_val[aa][bb]= uint8(tmp);
			tmp = int32(((float(255-bb)/256)*100)+aa); 
			if (tmp <0) tmp=0;   if (tmp >100) tmp=100; tab_mask_transp[aa][bb] = uint8(tmp);
			
			//MODES DESSIN		
			tmp= uint8( (float(100-aa)/100)*bb); if (tmp > 255) tmp=255; if (tmp < 0) tmp=0;
			tab_normal[aa][bb] = uint8(tmp); 
			tmp = int32(bb*1.3); if (tmp > 255) tmp=255;	tab_lighten[aa][bb] = uint8 ((float(100-aa)/100)*tmp);
			tmp = int32(bb/1.3); if (tmp < 0)   tmp=0;		tab_darken[aa][bb]  = uint8 ((float(100-aa)/100)*tmp);
			
			bb++;
			}
	bb=0;
	aa++;
	} 


//-----------------------------------------------------------------------
				
	//TABLEAU 256x256	
	aa = 0;
	bb = 0;
	 while (aa!=255+1)
	 {
			while (bb!=255+1)
			{
			
			tmp = aa+bb; if (tmp>255) tmp= 255; if (tmp<0) tmp= 0;  tab_addition[aa][bb] = uint8(tmp);
			
			tab_multiply[aa][bb] = uint8 (bb*(float(aa)/256));
			tab_difference[aa][bb] = bb-aa;
			if (aa < bb) tmp = bb;  else tmp=aa; tab_combine[aa][bb] = tmp;
			bb++;
			}
	bb=0;
	aa++;
	}


//tableau one dimension

	aa = 0;

			while (aa!=255+1)
			{
			tmp =uint32((float(aa) / 256) *100); if (tmp>100) tmp=100;	tab_char_to_percent[aa]  = 	100-uint8(tmp) ; 
			tmp =uint32((float(aa) / 256) *100); tmp= 50+tmp/2; if (tmp>100) tmp=100;	 //comme en haut mais shadé à 50% pour mask_img
			tab_char_to_shade[aa]  = 	uint8(tmp); 

			//transparence pixel du brush en float (0 à 1) fois 100
			
			aa++;
			}

}


PrefData::PrefData()
{
	
	//ca c'est pas vraiment des prefs... c'est pourquoi qu'elles sont pas dans Reset.
	//comme ça que c'est qu'on me comprends quand j'ai la commentaire que c'est clair dans la tête
	mask_mode=OFF;
	mask_activated=ON;

	no_pictures_left=ON;


}


void PrefData::ResetPrefs()
{
max_undo=32; //ATTENTION PAS PLUS DE 256.. le tableau ne doit pas etre en int32 sinon ça merde.. et bon 256 c'est assez non? non parce-que là une question m'interpelle mon vieux, tu vas lire ce putain de commentaire jusqu'où?

save_with_thumb = B_CONTROL_ON; //par défaut on sauve le thumb
tolerance=64;

rgb_color c = {255,0,0,0}; 
rgb_color d = {200,0,0};
rgb_color e = {0,150,255};
rgb_color f = {0,200,0};
rgb_color g = {255,150,0};
rgb_color hh = {255,50,0};
  
masking_color = c;
color_filter = d;
color_memorize = e;
color_undo = f;
color_load = g;
color_save = hh;

show_only_images = B_CONTROL_ON;
show_thumbs = B_CONTROL_ON;


fore_selector_size=2; //nb de fois à diviser les coordonnées, faudrait éviter autre que 1 ou 2...;-)
back_selector_size=2; //nb de fois à diviser les coordonnées, faudrait éviter autre que 1 ou 2...;-)

//positions par défaut
brushes_frame.Set(20,120,20+3*64+B_V_SCROLL_BAR_WIDTH,120+3*64); 
paper_frame = brushes_frame;
layer_frame = brushes_frame;
layer_frame.right += 56+20; 

paper_frame.OffsetBy(100,20); //évitons le chevauchement
fore_color_frame.Set( 200,200,
				 200+((256+32)/fore_selector_size),
				 200+((256+25)/fore_selector_size)-13);
	
back_color_frame = fore_color_frame; 
back_color_frame.OffsetBy(fore_color_frame.Width()+16,32); 
				 

pref_frame.Set(200,200,200+320-1,200+240-1);

info_frame.Set(0,0,192,128); info_frame.OffsetBy(300,60); 
option_frame.Set(0,0,192,192); option_frame.OffsetBy(300,200); 
navigation_frame.Set(0,0,192,128); navigation_frame.OffsetBy(300,300); 

tool_frame.Set(8,80,8+(24*2)-1,80+(9*23)-1+48+48-2+14);


//par défaut toutes les fenêtres sont fermées
create_win_open=false;
info_win_open=false;
option_win_open = false;
tool_win_open=false;
navigation_win_open=false;
fore_selector_open=false;
back_selector_open=false;
brush_selector_open=false;
paper_selector_open=false;
layer_selector_open = false;
pref_win_open = false;
save_panel_open=false;
open_panel_open=false;

limit_levels_open = false ;
range_selection_open = false ;
	
paper_size=64;
brush_size=32;
perso_brush_size=64;

pref_units = UNIT_CM;
pref_res_type = DPI;

sprintf(lang_name,"English");

}

void PrefData::Load()
{

	//------------------------------- READ PREFS MSG----------------------------------//	

	//effectué à l'ouverture de la window
	TextMessage *my_prefs  =  new TextMessage();
	
	char full_path[NAME_SIZE];
	sprintf(full_path,PREFS_DIR"/"); //le slash est pas dedans
	strcat(full_path,"Photon_Settings");
	
	BAlert *alert;
	BFile x;
	
	if (x.SetTo(full_path,B_READ_ONLY)!=B_OK)
	{
		//LANGUAGE EST PAS ENCORE ACCESSIBLE, Tant pis...
		//alert = new BAlert(NULL,Language.get("COULD_NOT_LOAD_PREFS"), 
		
		alert = new BAlert(NULL,"Error: could not load preferences file. Resetting to defaults.", 
                    "OK",NULL,NULL,B_WIDTH_FROM_WIDEST,B_WARNING_ALERT); 
   		alert->Go(NULL); //null pour pas qu'il arrête le reste

	ThePrefs.ResetPrefs();
	ThePrefs.Save();
	
	return; //don't go any further.
	}

	my_prefs->ReadFromTextFile(full_path, my_prefs);
	
	
	
	//----------------------- ANALYZE AND DISPATCH PREFS MSG----------------------//	


	//units
	 my_prefs->FindInt32("units",&pref_units);
		
	//res_type
	my_prefs->FindInt32("res_type",&pref_res_type);



	//language
	const char *ptr = (char*) malloc (256);
	if (my_prefs->FindString("language", &ptr)!=B_OK) 
			 printf("\Error... language not found. Using English.");
	else sprintf(lang_name,ptr);
	
	//si je fais ça ça plante.... bon tant pis pour 256 bytes..
	//free(ptr); 

	//le menage derrière soi...
	delete my_prefs; 
}

void PrefData::Save()
{

	TextMessage my_prefs('nprf');
	
	
	//------------------------------- Prepare message ----------------------------------//	
		
	//UNITS
	my_prefs.AddInt32("units",ThePrefs.pref_units);

	//RES_type
	my_prefs.AddInt32("res_type",ThePrefs.pref_res_type);

	//language (attention on sauve une string avec le nom et non pas le numéro de l'index!
	//comme ça quand on relit les prefs il y a pas de prob s'il y a de nouveaux filez..
	my_prefs.AddString("language",ThePrefs.lang_name); 
	
	//------------------------------- WRITE PREFS MSG----------------------------------//	

	//MAKE SURE DIRECTORY EXISTS
	BDirectory d,t;
	
	char full_path[NAME_SIZE];
			  
	if (d.SetTo(PREFS_DIR)!=B_OK)  t.CreateDirectory(PREFS_DIR, &d);
				//if (d.SetTo(PREFS_DIR"/")!=B_OK)  system("mkdir "PREFS_DIR);
	sprintf(full_path,PREFS_DIR"/Photon_Settings"); //le slash est pas dedans
	
	BAlert *alert;
	//Write the preferences to the file
	if (my_prefs.WriteToTextFile(full_path) != B_OK)
	{
		//no languageclass here...
		alert = new BAlert(NULL,"Error, could not save preferences!", 
                    "OK",NULL,NULL,B_WIDTH_FROM_WIDEST,B_WARNING_ALERT); 
   		alert->Go(NULL); //null pour pas qu'il arrête le reste
	}

	//----------------------------------------------------------------------------//	

}
