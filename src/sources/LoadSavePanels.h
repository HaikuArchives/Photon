#ifndef LOAD_SAVE_PANELS_H
#define LOAD_SAVE_PANELS_H

#include "PhotonView.h"
#include "share.h"

#define SHOW_THUMB_CHANGED 'sthc'
#define CREATE_THUMBS 'crth'
#define DONE_SELECTING_THUMB_IMG 'dsti'
#define DEACTIVATE_THUMB_IMG 'dthi'
#define CHANGED_BY_THUMB_TRUE 'cgtu'

#define MAX_THUMBS 2048

class ThumbView : public BView 
{
public:
	share *shared;

ThumbView(BRect r,share *sh);
virtual void Draw(BRect);
void UpdateText();

BView *info_container;
BTextView *info;
BCheckBox *thumb_check, *only_img_check;

};

class OneThumbView : public BView 
{
public:
	OneThumbView(const char *n, BRect r, BBitmap *pic, share *sh, int32 le_id, entry_ref the_ref);
	virtual ~OneThumbView();

	share *shared;
	BBitmap *img;
	virtual void Draw(BRect);
	virtual void MouseDown(BPoint point);
	int32 id;
	bool active;
	char the_name[256];
	entry_ref ref;
	bool previous_active;
};


class PhotonOpenPanel : public BFilePanel
{
public:
PhotonOpenPanel(share *sh);
virtual void SelectionChanged();
share *shared;
ThumbView* fond_thumb;
DThumbnail *thumb;

OneThumbView *tab_th_views[MAX_THUMBS];
int32 thumb_amount;
BView *fond_all_thumb;

void UpdateThumbs();
void CreateThumbs();

entry_ref old_dir_ref,the_active_ref;
BCheckBox *check_thumb,*check_only_images;
void ClearThumbSelection();
void SelectFromThumb(int32 number);
void SelectionDone();
bool changed_by_thumb;
BScrollView *scv; 
};

class PhotonSavePanel : public BFilePanel
{
public:
PhotonSavePanel(share *sh);
//virtual void SelectionChanged();
share *shared;

//OneThumbView *tab_th_views[MAX_THUMBS];
//int32 thumb_amount;
//void UpdateThumbs();
//entry_ref old_dir_ref,the_active_ref;

ThumbView* fond_thumb;

};


class ImgRefFilter : public BRefFilter
{
public:
ImgRefFilter(share *sh);
virtual bool Filter(const entry_ref *ref, 
         BNode *node, 
         struct stat *st, 
         const char *filetype);
         
         share *shared;
};

#endif //Photon_WINDOW_H
