
#ifndef NAT_VIEW_H
#define NAT_VIEW_H

#include "share.h"
#include "ProgressWin.h"

#define PAINT_RECT 'ptrt'
class Painting 
{
public:
	Painting(share *sh);
	share *shared;
//-------
/*
long _PasteLine(void *arg);
thread_id painting_thread;
*/

void PasteBrush(BPoint le_point);
void PasteLine(BPoint start, BPoint end);

BPoint brush_pos[2048];
BPoint last_pasted;

uint32 offset, offset_st;
uint8 tmp;
uint16 pos_x, pos_y;
uint8 transp,mask_transp;
uint8 mask_col;
float brush_val;
uint8 *l_bits,*l_paper,*l_bits_st,*l_wbits,*l_brush_bits,*l_mask_ptr,*l_ori_ptr,*l_wori_ptr,*l_ori_ptr_st;
uint16 line_length, line_length_24;

int16 to_cut_left, to_cut_right, to_cut_top, to_cut_bottom;

};

//------------------------------------

class PicView : public BView 
{
public:
	share *shared;

BView *virtualView;
PicView(BRect r,share *sh, long flags);
virtual	~PicView();

//pour draw circle brush pointer
BPoint old_pos_moved;
uint16 old_x_size,old_y_size;

BPoint last_picked;

BPoint last_updated;
uint8 was_circle;
virtual void MouseDown(BPoint pt);
virtual void MouseUp(BPoint pt);
virtual void MouseMoved(BPoint where, uint32 transit, const BMessage* dragDropMsg);
virtual void Draw(BRect);

	unsigned char show_progress;
	
virtual void MessageReceived(BMessage *msg);
virtual void Pulse();


//TOOLS
void WithBucket(BPoint point,uint8 is_wand);

//IMAGE EDIT
void ResizeCanvas(int16 bor_left, int16 bor_top, int16 bor_right, int16 bot_bottom);
void  CropSelected();

void  Filter_Invert();
void  Filter_SlideHSV();
void Filter_Rotate180();
void Filter_FlipHori();
void Filter_LimitLevels(uint8 levels);

void  InvertSelection(); 

void UpdateDisplay(BPoint point);
void FloodFill(BPoint pt);
uint8 TestVal(BPoint pt);


uint8 ZoneOK(BPoint pt);
void BeforeDraw();

//PrepareUndo et les valeurs utilisées
void PrepareForUndo();
BRect modif_plus_brush;  
BRect modified;
BRect real_time_modified;

BRect to_draw; //tlbr
BRect zone_to_update;
uint8 has_scrolled;
BRect mov_rect;
BPoint delta;
BPoint t;

uint8 pasted_once;
rgb_color fill_ori_color;
rgb_color fill_color;

uint8 updated_after_fill;
uint8 fill_done;

//pour filter
void PrepareFilter();
void FilteringDone();
void CopyUnfiltered();
void EndProgress();
void InitProgress();


BRect selected_zone;

Layer **draw_img_ptr;
status_t _PasteLine(void *arg);

Painting *painting;

void smooth_line(BPoint start, BPoint end);
void DrawGuides();

rgb_color stamp_color; 

bool filtering;
uint32 percent_val; //update tous les deux pourcent
uint32 percent_ctr;
void UpdateProgress(uint32 bytes_updated);
uint32 ln_count;
uint32 ln_width;
float percent;	
};



#endif //Photon_VIEW_H
