#ifndef PREFS_WINDOW_H
#define PREFS_WINDOW_H

#include "share.h"

#define PREFS_CHANGED 'pfch'

class PrefView : public BView 
{

public:
PrefView(BRect r, share *sh);
//virtual	~AllBrushesView();

share *shared;
//-----------------------------------------------------
BMenuField *units,*res_type;
};


class LangView : public BView 
{

public:
LangView(BRect r, share *sh);
//virtual	~AllBrushesView();
share *shared;
//-----------------------------------------------------
BListView* lang_list;

void AddLanguages();
	
};





class PrefWindow : public BWindow 
{
public:

	share *shared;

	void LoadPrefs();
	void SavePrefs();
		
	PrefView *pref_view;	
	LangView *lang_view;	
	PrefWindow(BRect frame, char *title, share *sh); 
	virtual ~PrefWindow();
	virtual void MessageReceived(BMessage *msg);
   virtual void FrameMoved(BPoint screenPoint);
   
};
#endif
