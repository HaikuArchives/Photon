#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H
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
#include <TranslatorRoster.h>
#include <List.h>
#include <ListView.h>
#include <TextView.h>
#include <TextControl.h>
#include <ColorControl.h>
#include <CheckBox.h>
#include <Beep.h>
#include <FilePanel.h>
#include <Slider.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <Screen.h>
#include "share.h"


class SlideTextView : public BView 
{
public:

SlideTextView(BRect frame, long flags); 
virtual void Draw(BRect update_rect);
virtual void Pulse();
//virtual	void MouseDown(BPoint point);
int32 max_posi;
int32 posi;

char *text;
};



class AboutView : public BView 
{
public:
	share *shared;

AboutView(BRect frame,share *sh, long flags); 
virtual	~AboutView();
virtual void Draw(BRect update_rect);

virtual	void MouseDown(BPoint point);
	BBitmap *title_pic;

SlideTextView *stview;
};

//---------------

class AboutWindow : public BWindow 
{
public:

	share *shared;
	AboutWindow(BRect frame,share *sh); 
	virtual	~AboutWindow();
	virtual void MessageReceived(BMessage *msg);

	AboutView *ab_view;
};


#endif