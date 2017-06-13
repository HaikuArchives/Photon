#ifndef PROGRESS_WIN_H
#define PROGRESS_WIN_H

#include "Utils.h"
#include "PrefData.h"

#define SET_PROGRESS_NAME 'spnm'
#define UPDATE_PROGRESS 'uppg'
#define RESET_PROGRESS  'rspg'
#define MAX_VALUE_PROGRESS  'mxvp'

#define SET_PROGRESS_COLOR_FILTER  	 'spcf'
#define SET_PROGRESS_COLOR_MEMORIZE  'spcm'
#define SET_PROGRESS_COLOR_UNDO		 'spcu'
#define SET_PROGRESS_COLOR_LOAD		 'spcl'
#define SET_PROGRESS_COLOR_SAVE		 'spcs'


class ProgressView : public BView 
{
public:

ProgressView(BRect r);
BStatusBar		*progress;

};

class ProgressWindow : public BWindow 
{
public:

	
	ProgressView *progress_view;		
	ProgressWindow(BRect frame, char *title); 
	virtual ~ProgressWindow();

	virtual void MessageReceived(BMessage *msg);

};


#endif