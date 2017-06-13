#include "ProgressWin.h"


ProgressView::ProgressView(BRect r) :
	BView(r, "ProgressView", B_FOLLOW_ALL, B_WILL_DRAW)
{
BRect re =Bounds();
re.InsetBy(4,4); 
 
progress = new BStatusBar(re,"aa","bb","cc");
//progress->SetBarHeight(7);
rgb_color colx ={200,0,0}; 
progress->SetBarColor(colx);

SetViewColor(216,216,216);
progress->SetViewColor(216,216,216);

progress->SetMaxValue(100);

AddChild(progress);
util.progress_exists=true;


}

ProgressWindow::ProgressWindow(BRect frame, char *title)
				: BWindow(frame, title, B_FLOATING_WINDOW,B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE| 
						B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_AVOID_FOCUS)
{
	progress_view = new ProgressView(Bounds());
	AddChild(progress_view);
	

}


ProgressWindow::~ProgressWindow()
{
	util.progress_exists=false;

}


void ProgressWindow::MessageReceived(BMessage *msg)
{
int32 val=0;
float percent;

char str[255];
const char *name = str;

	switch (msg->what)
	{
		case UPDATE_PROGRESS:
		msg->FindInt32("value",&val);
		//percent = 100 * (progress_view->progress->CurrentValue() / progress_view->progress->CurrentValue());
		//if (floor(val)!=floor (percent))
		//{
			progress_view->progress->Update(val);
			//sprintf(str,"%g %%",percent);
			sprintf(str,"%g %%",progress_view->progress->CurrentValue());
			progress_view->progress->SetTrailingText(str);
		//}
		break;
		
		case SET_PROGRESS_NAME:
		msg->FindString("text",&name);
		progress_view->progress->SetText(str);
		break;
		
		
		case RESET_PROGRESS:
		progress_view->progress->Reset();
		break;
		
		case SET_PROGRESS_COLOR_FILTER:
		progress_view->progress->SetBarColor(ThePrefs.color_filter);
		break;
		
		case SET_PROGRESS_COLOR_MEMORIZE:
		progress_view->progress->SetBarColor(ThePrefs.color_memorize);
		break;
		
		case SET_PROGRESS_COLOR_UNDO:
		progress_view->progress->SetBarColor(ThePrefs.color_undo);
		break;
		
		case SET_PROGRESS_COLOR_LOAD:
		progress_view->progress->SetBarColor(ThePrefs.color_load);
		break;
		
		case SET_PROGRESS_COLOR_SAVE:
		progress_view->progress->SetBarColor(ThePrefs.color_save);
		break;
		

	default:
	BWindow::MessageReceived( msg );
   			   		   	
	 }
   	 	
}
