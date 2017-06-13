#ifndef SHARE_TRUC_H
#define SHARE_TRUC_H

//---------------------------------
#include "all_includes.h"
//---------------------------------f

#include "PhotonImage.h"

extern BubbleHelper bb_help;
	
class share
{
   public:
   
   share();    //constructeur

	//pour Thumbnails (load et save)
    int32 current_select_thumb;	
	DThumbnail *thumbnail;

//---------------------------------------------------------------------------------
	
	uint8 *the_bitmap_bits,*undo_bitmap_bits;
	uint8 *mask_bits,*mask_work_bits;
    uint8 *the_brush_bits;

//---------------------------------------------------------------------------------
   pattern stripes;
  
   PhotonImage *les_images[2048];
   PhotonImage *act_img;
   Layer *act_lay;
   
   int16 active_image;
   int16 image_amount; //surtout pas unsigned!!!

//---------------------------------------------------------------------------------

	
   bool brush_is_perso;
   uint16 brush_x,brush_y,brush_rayon_x,brush_rayon_y;
   uint16 paper_x, paper_y;   
   
   bool needs_update;
   bool first_zone;
   
   BRect what_to_update;
   void AddToUpdateZone(BRect zone);

   uint8 active_tool,previous_tool;
   uint8 paint_mode;
   uint8 paint_transparency; //0 à 100
   uint8 hardness ;	// 0 à 100
   float pressure;
	//couleurs
	rgb_color fore_color;
	rgb_color back_color;
	rgb_color picker_color; //seulement utilisé par picker pendant affichage

	BBitmap *the_brush,*the_brush_24,*paper,*paper_24;
	 
	void CreateNewImage(const char *name,int16 w, int16 h);
	void LoadNewImage(const char *nam_file);
	void NewImageFromBmp(const char *nm,BBitmap *pic);
	void DeleteImage(int32 which_one);

	void ImageAdded();
	//void CreateNewImage(const char *name, BRect size, BBitmap* image, char *nam_file, uint8 is_revert);
	void initPic();
	void initLayer();

	
	void loadPaper(int16 nb);
	void EmptyPaper();
	void loadBrush(int16 nb);
	void generateBrush(int16 nb);
	float mb_height;//hauteur barre menu
	void UseAsBrush(BBitmap *temp_32);
	void UseAsPaper(BBitmap *temp_32);
	
	BRect FindSelectedRect();
	void   CopyBitmaptoWork();

	int32 current_perso_brush,current_paper;	
	int32 current_brush;

	int32 max_brush,max_paper;

	uint8 *cursor_ptr; //pointeur sur les bits du pointeur actuel
	int32 active_cursor_id;

	BScrollView *scroll_view_ptr;
	BScrollBar *scroll_hori,*scroll_verti;
	BView		*back_view_ptr;
	
	//coordonnées des points dans les selecteurs de couleur
	BPoint ori_back_slide;	BPoint ori_front_slide;
	BPoint ori_back_square;	BPoint ori_front_square;

	rgb_color PickColorAt(BPoint x);	 //dans le carré
	uint8 PickMaskColorAt(BPoint x);	 //dans le carré

	translator_id active_translator;

	uint8 stamp_offset_changed;
	BPoint stamp_offset; //distance entre le point source du stamp et sa copie
    BPoint stamp_old_offset; //distance entre le point source du stamp et sa copie
    BPoint stamp_old_pos;

	BPoint pos_old,pos_actuelle,ori_pos;
	BMenu *win_menu,*manip_menu,*convert_menu,*display_menu;
	BMenuBar	*menubar;
	char nom_de_brush_perso[255],nom_paper[255];
	uint8 stroke;
	uint32 ori_buttons;

	//pour imagemaniplib
	void SetManipMenus();
	bool mBitmapInUse;
	image_addon_id mAddonId;
	void ImageManip(BBitmap *bit);
	void ImageConvert(BBitmap *bit);
//	void PrintAddonInfo();

	BStringView* tool_name_ptr; 
	char tool_name_backup[255];


	
	BMenuItem *close_item;
	BMenuItem *revert_item;
	BMenuItem *save_item;
	BMenuItem *save_as_item;

    uint8 col_blue,col_red,col_green;
	//pointeurs quand on paste la brush
	uint8 *bits,*bits_st,*wbits,*brush_bits,*paper_bits,*mask_ptr,*ori_ptr,*wori_ptr,*ori_ptr_st;
	uint8 nb_bytes;

	
	
	};

#endif  