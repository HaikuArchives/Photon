#ifndef NAT_MAIN_H
#define NAT_MAIN_H

#include "share.h"
#include "PhotonWindow.h"
#include "ToolWindow.h"
#include "ProgressWin.h"
#include "AboutWindow.h"

class PhotonApplication : public BApplication 
{
public:
	PhotonApplication();
	PhotonWindow		*mainWindow;
	ToolWindow		*toolWindow;
	ProgressWindow	*progWindow;
share *shared;

virtual void RefsReceived(BMessage *message);
virtual void ArgvReceived(int32 argc, char **argv);

void MessageReceived(BMessage *msg);
void SaveImg(BMessage *msg);

AboutWindow *ab_win;

};

#endif //Photon_WORLD_H
