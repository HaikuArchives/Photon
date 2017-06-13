#ifndef LAYER_WINDOW_H
#define LAYER_WINDOW_H

#include "share.h"

#define LAYER_VIEW_HEIGHT 24
#define LAYER_TOP_HEIGHT 48
#define MAKE_VISIBLE 'mkvs'

#define ADD_LAYER				'adly'
#define ADD_GUIDE_LAYER			'adgl'
#define DELETE_LAYER 			'dlly'
#define DUPLICATE_LAYER			'duly'
#define MERGE_LAYERS 			'mgly'
#define MERGE_VISIBLE_LAYERS 	'mgvl'
#define FLATTEN_IMAGE 			'flim'
#define DISPLAY_OPTIONS			'dpop'

#define ICON_EMPTY 0
#define ICON_PAINT 1
#define ICON_MASK  2

class TriangleMenuView  : public BView 
{
public:
	TriangleMenuView (BRect r, share *sh);
	virtual ~TriangleMenuView();
	share *shared;
	virtual void Draw(BRect);
	virtual void MouseDown(BPoint point);

	BBitmap *img,*on,*off;
	BPopUpMenu* pm;
	
};


class LayerOptionsView  : public BView 
{
public:
	LayerOptionsView (BRect r, share *sh);
	share *shared;

	BSlider *opacity;
	BMenuField *draw_mode; 
	TriangleMenuView *t_menu_view;
};


class PaintModeView  : public BView 
{
public:
	PaintModeView (const char *name, BRect r, share *sh,BView* the_p);
	virtual ~PaintModeView();

	share *shared; 
	
	virtual void Draw(BRect);
	virtual void MouseDown(BPoint point);
	int32 active_pic;
		
	BBitmap *icon_paint,*icon_mask,*icon_empty;
	BView *the_parent;
};


class LayerNameView : public BStringView
{
	public:
	LayerNameView(const char *name,const char *layer_name, BRect r, BView* the_p);
	virtual void MouseDown(BPoint point);
	
	BView *the_parent;
};

class OneLayerView : public BView 
{
public:
	OneLayerView(const char *name, BRect r,Layer *ly, share *sh);
	
	share *shared;
	virtual void Draw(BRect);
	virtual void MouseDown(BPoint point);
	
	Layer *the_layer;
	LayerNameView *the_nom;
	BRect mini_view_rect, mini_alpha_rect;
	
	PaintModeView *p_mod;
 	void CreateButton(char [255], char nm2[255]);
	BPictureButton *visible_button;
	BPicture *on,*off;

};

class AllLayersView : public BView 
{

public:
AllLayersView(BRect r, share *sh);
//virtual	~AllLayersView();

share *shared;

	void AddLayers();
	OneLayerView *tab_layer_views[MAX_LAYER];

};


class LayerWindow : public BWindow 
{
public:

	share *shared;
	
	LayerOptionsView *options_view;
	
	AllLayersView *pr_br_view;		
	LayerWindow(BRect frame, char *title, share *sh); 
	virtual ~LayerWindow();

	BScrollView *perso_scroll_view;

	virtual void FrameMoved(BPoint screenPoint);
	virtual void FrameResized(float x, float y);
	virtual void MessageReceived(BMessage *msg);
    bool is_disabled;
};

#endif