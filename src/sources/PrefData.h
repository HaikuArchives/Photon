#ifndef PREF_DATA_H
#define PREF_DATA_H

//#include "all_includes.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Application.h>
#include <Window.h>
#include <View.h>


#include <Alert.h>
#include <Path.h>
#include <Bitmap.h>
#include <StringView.h>
#include <ScrollView.h>
#include <ScrollBar.h>
#include <BitmapStream.h>
#include <StorageKit.h>
#include <TranslatorRoster.h>
#include <List.h>
#include <ListView.h>
#include <TextView.h>
#include <TextControl.h>
#include <ColorControl.h>
#include <CheckBox.h>
#include <Beep.h> 
#include <File.h>
#include <FilePanel.h>
#include <Slider.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <Screen.h>
#include <Roster.h>
#include <TranslationUtils.h>
#include <PictureButton.h>
#include <Control.h>
#include <StatusBar.h>
#include <NodeInfo.h>
#include <AppFileInfo.h>
#include <String.h>
#include <TabView.h>



#include "TextMessage.hh" // By Bjorn Tornqvist



class PrefData
{
public:
PrefData();

void  ResetPrefs();
void Load();
void Save();


int32 pref_units;
int32 pref_res_type;

char lang_name[256];

//----------


 	bool pref_win_open;
 	bool create_win_open;
  	bool info_win_open;
  	bool option_win_open;
 	bool tool_win_open;
 	bool navigation_win_open;
 	bool fore_selector_open;
 	bool layer_selector_open;
 	bool back_selector_open;
 	bool brush_selector_open;
 	bool paper_selector_open;
	bool save_panel_open;
	bool open_panel_open;
	
	bool limit_levels_open;
	bool range_selection_open;
	
	

 	int8 back_selector_size; //nb de fois à diviser les coordonnées
 	int8 fore_selector_size; //nb de fois à diviser les coordonnées

 	int32 save_with_thumb;
 	int32 show_only_images;
 	int32 show_thumbs;
 	
  int16 max_undo;
    
 	uint8 tolerance; //pour fill
	
 	rgb_color color_filter;
 	rgb_color color_memorize;
 	rgb_color color_undo;
 	rgb_color color_load;
 	rgb_color color_save;

    rgb_color masking_color;

 	BRect brushes_frame, back_color_frame,fore_color_frame;
 	BRect paper_frame, pref_frame,layer_frame;
 	BRect info_frame, option_frame, navigation_frame,tool_frame;
	
	uint8	no_pictures_left;

 	int32 paper_size;
 	int32 perso_brush_size;
 	int32 brush_size;
	
	//Ca c'est pas vraiment des prefs mais... on peut pas les mettre dans share
	uint8 mask_mode, mask_activated; 
   

};



class CalcTables
{

	public:
	CalcTables();
	 
	//JE NE comprends pas pourquoi il faut 256 au lieu de 255 mais c'est NECESSAIRE
	//nes surtout pas changer sino ça fout une merde noire!
	uint8 tab_transp[101][256];
	uint8 tab_pourcent_x_val[101][256];
	uint8 tab_normal[101][256];
	uint8 tab_lighten[101][256];
	uint8 tab_darken[101][256];
	uint8 tab_multiply[256][256];
	uint8 tab_colorize[256][256];
	uint8 tab_difference[256][256];
	uint8 tab_combine[256][256];

	uint8 tab_addition[256][256];
	
	uint8 tab_mask_transp[101][256];
	uint8 tab_char_to_percent[256];	
	uint8 tab_char_to_shade[256];	
};

extern CalcTables TheTables;
extern PrefData ThePrefs;

#endif
