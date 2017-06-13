#ifndef NAT_IMAGE_H
#define NAT_IMAGE_H

//---------------------------------

#include "all_includes.h"

//-----------------

#define MAX_GUIDES 512

#define LAYER_TYPE_BITMAP  0
#define LAYER_TYPE_TEXT    1
#define LAYER_TYPE_EFFECT  2
#define LAYER_TYPE_GUIDE   3


#define MODE_REPLACE_SELECTION  0
#define MODE_ADD_TO_SELECTION    1
#define MODE_SUBTRACT_FROM_SELECTION    2


struct UndoData{
					BBitmap* undo[MAX_UNDO]; //des bitmaps pour chaque undo... qu'on recopie...
					int16    undo_amount;
					int32    width[MAX_UNDO]; //pour pouvoir restaurer en cas de resize etc.
					int32    height[MAX_UNDO];
					int8     undo_type[MAX_UNDO];
					int16    layer[MAX_UNDO];
					
				 };	


class Layer
{
   public:
   
	Layer(const char *nm, BBitmap *pic,int32 id, int32 the_type);    //constructeur
	virtual	~Layer(); //destructeur important faut effacer les bitmaps!

	char name[B_FILE_NAME_LENGTH];	
    BBitmap *img;
    int32 id;
	int32 draw_mode;
	bool is_visible;
	bool alpha_activated;
	
	
	float opacity;
	bool active;

	
	//for special types
	int32 layer_type;
	char *text;
	int16 x_guides[MAX_GUIDES];
	int16 y_guides[MAX_GUIDES];
	int16 x_guides_amount;
	int16 y_guides_amount;
	
	rgb_color guides_color;
	
	void AddHoriGuide(int16 pos);  //en pixels
	void AddVertiGuide(int16 pos);	
	
};	



class PhotonImage
{
   public:
   
	PhotonImage(const char *nm, int16 width, int16 height);    //constructeur
	virtual	~PhotonImage(); //destructeur important faut effacer les bitmaps!
	status_t CreateNewLayer(const char *nm);
	status_t DeleteLayer(int32 which_one);

	status_t CreateNewGuideLayer(const char *nm);

	status_t LoadNewLayer(const char* file_name);
	status_t Revert();
	status_t LoadLayer(const char *nm);
	status_t NewLayerFromBmp(BBitmap *pic);
	
	bool updating;
	char name[B_FILE_NAME_LENGTH];	
	char full_path[NAME_SIZE];	

    Layer *the_layers[MAX_LAYER];
    
	BBitmap *display_bitmap, *undo_bitmap, *mask_undo_bitmap, *mask_bitmap;
	
	int32 active_layer;
	int32 layer_amount,id_counter;
	
	UndoData *undo_data;

	float zoom_level; //1 =100%

    void MemorizeUndo(BRect zone_to_keep, uint8 mode);   
    void Undo(); void Redo();
	void PurgeUndo();
	uint8 is_redo;

	void SetPixel(BPoint pix, rgb_color color); 
	void SetMaskPixel(BPoint pix, rgb_color color);  
	void DeleteMask(uint8 mem_old_mask);
	void FillMask();
	void UpdateDisplayImg(BRect a_rect);
	const char* ComputeLayerName(const char**, int32 type);
	void FinishLayer();
	void SetMaskFromColorZone(rgb_color col, uint8 tolerance, uint8 mode);

	void SetUnitsResType(int32 units, float res, int32 res_type);

	int32 pix_per_line,pix_per_row;

 int32 units;
 float res_units;
 float resolution;
   
};	

#endif  