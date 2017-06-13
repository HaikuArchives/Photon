#ifndef UTILS_H
#define UTILS_H


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

//-----------------

#include "defs.h"

#include "LanguageClass.h" //By D. Olbertz
#include "PBitmapModif.h"
#include "ProgressWin.h"


class Utils
{
   public:
   
   Utils();    //constructeur

   BBitmap* FetchBitmap(char *filename, uint8 mode);
   status_t StoreTranslatorBitmap(BBitmap *bitmap, char *filename, uint32 type);
   
   void CopyRect(BRect zone_src,  BBitmap *la_source, BRect zone_dest, BBitmap *la_dest, uint8 nb_of_bytes);
   void CopyRectWithMask(BRect zone_src,  BBitmap *la_source, BRect zone_dest, BBitmap *la_dest, BBitmap *le_mask, uint8 nb_of_bytes);
   BBitmap* GrabRect(BRect zone_src,  BBitmap *la_source); 
   
   float ConvertUnits(float val, int32 src_unit,int32 dest_unit, float resolution, float res_units);

   
   char dossier_app[NAME_SIZE];
   void AppDir();

  void NotImplemented();
   BRect find_win_pos(float width, float height, BWindow *win_to_use_to_choose_screen); 
   BRect find_win_pos_on_main_screen(float width, float height); 

   BRect CheckRectangle(BRect ori_rect);
   BBitmap* load_bmp(const char *le_nom);
   void RescaleBitmap(BBitmap *src, BBitmap *dest,bool center_if_smaller);

	BWindow *progress_win;
	bool progress_exists;	
	
	uint8 show_progress_load;
	uint8 loading_in_progress;
	
	BWindow *mainWin,*toolWin,*brushWin,*paperWin,*layerWin;
	BWindow *navWin, *infoWin,*optionWin;
	BWindow *foreWin,*backWin;
	
    char version_txt[256];
    char version_only_txt[256];
	BTranslatorRoster *trans_roster;
	translator_id translators[1024]; //prévoyons large...
	void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );
	void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v ); 

	//utilisé durant le filtre
	BBitmap* sel_pic;
	uint32 sel_length;
	uint8 *sel_pic_bits;

	};

extern Utils util;

#endif  