#ifndef SAVE_AS_PANEL_H
#define SAVE_AS_PANEL_H

#include <Handler.h>

#include "LoadSavePanels.h"

// Messages constants
const int32 M_BITMAP_SAVED = 'SAVD';

class BFilePanel;
class SaveAsPanel : public BHandler
{
public:

	share *shared;

	SaveAsPanel( BLooper *looper,share *sh);
	virtual ~SaveAsPanel();
	
	void SaveAs( BBitmap *bitmap );
	void CancelSave();
	
	void SetTarget( BMessenger *messenger );

	BFilePanel *Panel();
	void SetDefaultFormat( int32 type );

	virtual void MessageReceived(BMessage *message);

	ThumbView* fond_thumb;
	DThumbnail *the_thumbnail;
		BFilePanel *panel;

private:
	static const int32 M_SAVEASPANEL_TYPE = 'SAPT';
	BMenuField *menuField;
	BPopUpMenu *BuildFormatsMenu();

	int32 saveTypeWanted;
	BBitmap *toBeSaved;
	
	BMessenger *target;

	
};

#endif // SAVE_AS_PANEL_H
