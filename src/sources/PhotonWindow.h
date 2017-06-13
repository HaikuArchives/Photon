#ifndef NAT_WINDOW_H
#define NAT_WINDOW_H

#include "PrefWindow.h"
#include "CreateWindow.h"
#include "CurveWindow.h"
#include "LimitLevelsWindow.h"

#include "SaveAsPanel.h"

#include "PhotonView.h"
#include "ProgressWin.h"
#include "share.h"
#include "LoadSavePanels.h"
#include "PaperWindow.h"
#include "BrushWindow.h"
#include "LayerWindow.h"
#include "InfoWindow.h"
#include "RangeWindow.h"

#include "OptionWindow.h"
#include "NavigationWindow.h"
#include "ToolWindow.h"
#include "cursors.h"

#define MENU_PREFS 		'mnpr'
#define HIDE_ALL_WIN	'hdal'

#define FULL_SCREEN_NONE 0
#define FULL_SCREEN_HALF 1
#define FULL_SCREEN_TOTAL 2


class PhotonWindow : public BWindow 
{
public:
	
	share *shared;

				PhotonWindow(BRect frame,share *sh); 
				~PhotonWindow();
virtual	bool	QuitRequested();
void UpdateToolInfo();
BView		*back_view;
PicView		*inside_view; 
BScrollView	*scroll_view;
//OutputFormatWindow *format_win;

BrushWindow		*brWindow;
PaperWindow 	*ppWindow;
LayerWindow 	*lyWindow;
InfoWindow		*ifWindow;
OptionWindow	*opWindow;
NavigationWindow *nvWindow;
ToolWindow 		 *tlWindow;


RangeWindow 		 *rgWindow;
LimitLevelsWindow    *llWindow;
bool rgWindow_visible;

BBitmap *back_bmp;	

uint8 full_screen;

void FrameResized(float width, float height);
void DisableAll();
void EnableAll();

CurveWindow *curv_win;
CreateWindow *crt_win;

PrefWindow *prefWindow;

void MessageReceived(BMessage *msg);
void ShortCuts(BMessage *msg);

void OpenPanel();
void SavePanel();
 
PhotonOpenPanel *panneau_load;
SaveAsPanel     *panneau_save;

	BMenu		*gimpFilterMenu;

void UpdateTitle();
void ZoomChanged();		
void SetPointerForActiveTool();

bool menu_bar_shown;
BMenuItem *mask_item;
void SetFullScreen();
BRect windowed_frame;

bool all_win_hidden;


//----------------------- FOR SUPPORT GIMP FILTERS -------------------------------------
		//Pointeurs sur lesquels doivent pointer les sources et dest pour filter gimp ok
		BBitmap *fSrcBitmap; 
		BBitmap	*fDstBitmap;
		
		void	AddFilter (const char *menu, const char *item, BMessage *msg);
		void	UseBitmap (BBitmap *bm);
		void	SwitchBuffers (bool showMain);
		void	Update (void);
		BBitmap	*Src (void) { return fSrcBitmap; }
		BBitmap	*Dst (void) { return fDstBitmap; }
		
void RegisterGimpFilters (void) ;
int32 AsyncFilter (void *arg);

protected:
	void	BuildMenus (void);
	BMenu		*fFilterMenu;
};

#endif //Photon_WINDOW_H
