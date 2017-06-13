//------------------------------------------------------------------------------
// TestGIMP.cpp
//
//	This program serves two purposes:
//		It can be used as a test platform to port a GIMP filter to the BeOS
//		platform.
//		It can be used to help you understand how you can call GIMP filters
//		with regular BBitmaps.
//------------------------------------------------------------------------------
// A very simple program to demonstrate how to use the Be modified GIMP filters.
//
// Copyright (C) 1999 Human Touch, Inc.
// This program is free software; you can redistribute it and/or modify
// it as you wish.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
//
//------------------------------------------------------------------------------
#include <InterfaceKit.h>
#include <Application.h>
#include <dirent.h>
#include <iostream.h>
#include <fstream.h>
#include <SerialPort.h>
#include <string.h>
#include <StorageKit.h>
#include <sys/stat.h>
#include <Debug.h>
#include <vector>
#include <algo.h>
#include <TranslationKit.h>
#include "libgimp/gimp.h"
#include "gtk/gtk.h"
#include "GimpStub.h"
#include "PhotonWindow.h"

#include "Utils.h"
//-------------------------------------------------------------------------------------------------
//	Class definitions
//-------------------------------------------------------------------------------------------------
/*

class TTestGIMPView : public BView {
public:
				TTestGIMPView (const BRect &bounds, BBitmap *bm = NULL);
				~TTestGIMPView ();
virtual	void	Draw (BRect updateRect);
		void	SetBitmap (BBitmap *bm) { fBitmap = bm; }
protected:
	BBitmap	*fBitmap;
};

class PhotonWindow : public BWindow {
public:
				PhotonWindow (void);
				~PhotonWindow ();
virtual bool	QuitRequested (void) { return true; }
virtual	void	MessageReceived (BMessage *msg);

		void	AddFilter (const char *menu, const char *item, BMessage *msg);
		void	UseBitmap (BBitmap *bm);
		void	SwitchBuffers (bool showMain);
		void	Update (void);
		BBitmap	*Src (void) { return fSrcBitmap; }
		BBitmap	*Dst (void) { return fDstBitmap; }
protected:
		void	BuildMenus (void);
	BMenuBar	*shared->menubar;
	int32		shared->menubarHeight;
	BMenu		*fFilterMenu;
	TTestGIMPView *fView;
	BBitmap		*fSrcBitmap, *fDstBitmap, *fRealSrc, *fRealDst;
};
*/




/*
class TTestGIMPApp : public BApplication {
public:
						TTestGIMPApp();
virtual	void			ReadyToRun();
virtual	void			MessageReceived (BMessage *msg);
virtual	void			RefsReceived(BMessage *message);
protected:
};

//-------------------------------------------------------------------------------------------------
//	View class. This view just draws the bitmap
//-------------------------------------------------------------------------------------------------
TTestGIMPView::TTestGIMPView (const BRect &bounds, BBitmap *bm) : BView (bounds, "", B_FOLLOW_ALL_SIDES, B_WILL_DRAW) {
	fBitmap = bm;
}

TTestGIMPView::~TTestGIMPView () {
}

void TTestGIMPView::Draw (BRect updateRect) {
	if (fBitmap) DrawBitmap (fBitmap);
}

//-------------------------------------------------------------------------------------------------
//	Window class.
//-------------------------------------------------------------------------------------------------
PhotonWindow::PhotonWindow (void) : BWindow (BRect (20, 50, 620, 460), "Test Filters", B_DOCUMENT_WINDOW, B_ASYNCHRONOUS_CONTROLS) {
	fView = NULL; fSrcBitmap = fRealSrc = fDstBitmap = fRealDst = NULL;
	((PhotonWindow*) util.mainWin) = this;
	BRect r;
	r.Set(0, 0, 32767, 15); shared->menubar = new BMenuBar(r, "MenuBar");
	BuildMenus ();
	Lock();
	AddChild(shared->menubar);
	shared->menubarHeight = int32(shared->menubar->Bounds().bottom) + 1;
	Unlock();
	Show ();
}

PhotonWindow::~PhotonWindow () {
	delete fSrcBitmap; delete fDstBitmap;
}

*/


#define VERSION_NUMBER	0x100
extern "C" {
typedef void (*GMenuInstaller) (const char *menu, const char *item, BMessage *msg);
	void	GimpMenuInstaller (const char *menu, const char *item, BMessage *msg);
};


extern "C" {
typedef void (*GInstallGimp) (BPath *path, InitInfo *info);
};

void GimpMenuInstaller (const char *menu, const char *item, BMessage *msg) {
	BMessage	*menuMsg = new BMessage (msg);		//	WARNING, the message beolongs to the GIMP filter.
	menuMsg->what = 'Fltr';
	((PhotonWindow*) util.mainWin)->AddFilter (menu, item, menuMsg);
}

static void RecursInstallFilters (BDirectory *dir) {
	InitInfo	info;
	info.fVersion = VERSION_NUMBER; info.app = be_app; info.fInstaller = &GimpMenuInstaller;

	do {
		BEntry	entry;
		if (B_OK != dir->GetNextEntry (&entry, true)) break;
		struct stat	entryStat;		//	entryStat.st_mode == S_ISREG S_ISDIR
		ASSERT (entry.Exists ());
		if (B_OK == entry.GetStat (&entryStat)) {
			if (S_ISREG(entryStat.st_mode)) {
				BPath		entryPath;
				if (B_OK == entry.GetPath (&entryPath)) {
					image_id	addon_image;
					addon_image = load_add_on (entryPath.Path());
					if (long(addon_image) > 0) {
						status_t		status;
						GInstallGimp	funcPtr;
						status = get_image_symbol (addon_image, "InstallGimp", B_SYMBOL_TYPE_TEXT, (void **)&funcPtr);
						ASSERT (status == B_OK);
						(*funcPtr) (&entryPath, &info);
					}
				} else {
					ASSERT (false);
				}
			} else if (S_ISDIR(entryStat.st_mode)) {
				BDirectory	dir2Parse (&entry);
				RecursInstallFilters (&dir2Parse);
			} else {
				ASSERT (false);
			}
		} else {
			ASSERT (false);
		}
				
	} while (true);
}

void PhotonWindow::RegisterGimpFilters (void) {
	app_info	appInfo;
	be_app->GetAppInfo (&appInfo);
	entry_ref	filtersRef = appInfo.ref;
	filtersRef.set_name ("GIMP");
	BDirectory	filtersDir (&filtersRef);

	RecursInstallFilters (&filtersDir);
}

void PhotonWindow::BuildMenus (void) {
	BMenu		*menu;
	BMenuItem	*item;

	menu = new BMenu ("File");
	menu->AddItem(item = new BMenuItem("Open", new BMessage('Open'), 'O')); item->SetTarget(be_app);
	menu->AddItem(item = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q')); item->SetTarget(be_app);
	shared->menubar->AddItem(menu);
	fFilterMenu = new BMenu ("Filters");
	RegisterGimpFilters ();
	shared->menubar->AddItem(fFilterMenu); fFilterMenu->SetEnabled (false);
}

void PhotonWindow::AddFilter (const char *menu, const char *item, BMessage *msg) {
	fFilterMenu->AddItem (new BMenuItem (item, msg));
}
/*
void PhotonWindow::UseBitmap (BBitmap *bm) {
	Lock ();
	fFilterMenu->SetEnabled (true);
	if (fView) { RemoveChild (fView); delete fView; }
	ResizeTo (bm->Bounds().IntegerWidth (), bm->Bounds().IntegerHeight () + shared->menubarHeight);
	BRect	r = Bounds (); r.top += shared->menubarHeight;
	fView = new TTestGIMPView (r, bm); AddChild (fView);
	delete fSrcBitmap; delete fDstBitmap;
	fSrcBitmap = fRealSrc = new BBitmap (bm); fDstBitmap = fRealDst = bm;
	Unlock ();
}
*/

void PhotonWindow::SwitchBuffers (bool showMain) {
/*
	bool	doUpdate;
	if (showMain) {
		doUpdate = fSrcBitmap != fRealSrc;
		fSrcBitmap = fRealSrc; fDstBitmap = fRealDst;
	} else {
		doUpdate = fSrcBitmap == fRealSrc;
		fSrcBitmap = fRealDst; fDstBitmap = fRealSrc;
	}
	if (doUpdate) { fView->SetBitmap (fDstBitmap); Update (); }
*/
}

void FilterUpdateProc (TFilterParams *parms) {
	((PhotonWindow*) util.mainWin)->Update (); 
}

void BufferSwitcherProc (struct TFilterParams *parms, bool showMain) {
	((PhotonWindow*) util.mainWin)->SwitchBuffers (showMain);
}

void PhotonWindow::Update (void) {
//	Lock (); fView->Invalidate (); Unlock ();
}


int32 PhotonWindow::AsyncFilter (void *arg) {
	BMessage	*msg = (BMessage *)arg;
	BArchivable	*unarchived = instantiate_object (msg);
	if (unarchived) {
		TFilter	*filter = cast_as(unarchived, TFilter);
		ASSERT (filter);
		TFilterParams	parms;
		parms.fVersion = FILTER_VERSION_NUMBER; parms.fForeColor = 0xFFFFFFFF; parms.fBackColor = 0;
		parms.fDocument = NULL; parms.fCurLayer = 0; parms.fUpdater = FilterUpdateProc; parms.fBufferSwitcher = BufferSwitcherProc;
		parms.fCurSel = NULL;
		FakeTBitmap		srcBits(((PhotonWindow*) util.mainWin)->Src ()), dstBits(((PhotonWindow*) util.mainWin)->Dst ());
		parms.fBounds = srcBits.Bounds ();
		parms.fSrc = &srcBits; parms.fDst = &dstBits; parms.fFilter = filter;
		memcpy (dstBits.Bits (), srcBits.Bits (), srcBits.BitsLength ());
		BMessage	*cmd = filter->ApplyFilter (&parms);
		((PhotonWindow*) util.mainWin)->SwitchBuffers (true);
		delete cmd;
		delete filter;	//	We are gone with the filter, let's dispose of it.
	}
	delete msg;
	return 0;
}

/*
void PhotonWindow::MessageReceived (BMessage *msg) {
	switch (msg->what) {
		case 'Fltr':	resume_thread (spawn_thread (AsyncFilter, "AsyncFilter", B_NORMAL_PRIORITY, new BMessage (msg))); break;
		default:		ASSERT (false); BWindow::MessageReceived (msg); break;
	}
}

//-------------------------------------------------------------------------------------------------
//	Opening of a file
//-------------------------------------------------------------------------------------------------
class TransInfo {
public:
				TransInfo (void) { *fMime = 0; };
				~TransInfo () { }
				TransInfo (const TransInfo &info) { fType = info.fType; fTranslator = info.fTranslator; strcpy (fMime, info.fMime); }
				TransInfo (uint32 trans, uint32 type, const char *mime) : fTranslator (trans), fType(type) { strcpy (fMime, mime); }
	bool		operator == (const TransInfo &info) { return strcmp (info.fMime, fMime) == 0; }
	bool		operator == (const char *mime) { return strcmp (fMime, mime) == 0; }
	int32		Translator (void) { return fTranslator; }
	int32		Type (void) { return fType; }
protected:
	int32		fTranslator, fType;
	char		fMime[512];
};

class PaintRefFilter : public BRefFilter {
public:
	virtual			~PaintRefFilter () { }
	virtual	bool	Filter (const entry_ref *ref, BNode *node, struct stat *st, const char *fileType);
			void	InitFilter (void);
			void	FindTranslator (const char *mime, int32 *translator, int32 *type);
protected:
#if __PPC
	typedef vector<TransInfo, allocator<TransInfo> > TTransInfoArray;
#else
	typedef vector<TransInfo> TTransInfoArray;
#endif
	TTransInfoArray	fMimeTypes;
};

bool PaintRefFilter::Filter (const entry_ref *ref, BNode *node, struct stat *st, const char *fileType) {
	bool	res = false;

	if (S_ISDIR(st->st_mode)) {
		res = true;
	} else if (S_ISREG (st->st_mode)) {
		res = find (fMimeTypes.begin(), fMimeTypes.end(), fileType) != fMimeTypes.end();
	//	} else if (S_ISLINK (st->st_mode)) {
	}
	return res;
}

void PaintRefFilter::InitFilter (void) {
	BTranslatorRoster	*r = BTranslatorRoster::Default();

	translator_id * list = NULL;
	int32 count = 0;

	fMimeTypes.erase (fMimeTypes.begin(), fMimeTypes.end());
	status_t err = r->GetAllTranslators(&list, &count);
	if (err == B_OK) do {
		for (int tix=0; tix < count; tix++) {
			const char * name;
			const char * info;
			int32 version;

			if ((err = r->GetTranslatorInfo(list[tix], &name, &info, &version)) < B_OK)
				break;

			int32 num_input = 0;
			const translation_format * input_formats = NULL;
			if ((err = r->GetInputFormats(list[tix], &input_formats, &num_input)) < B_OK)
				break;

			for (int iix=0; iix < num_input; iix++) {
				// only print formats that aren't the "base" format for the group 
				if (input_formats[iix].type != input_formats[iix].group && input_formats[iix].group == B_TRANSLATOR_BITMAP) {
					fMimeTypes.push_back (TransInfo (list[tix], input_formats[iix].type, input_formats[iix].MIME));
				}
			}
		}
	} while (false);

	delete[] list;
}

void PaintRefFilter::FindTranslator (const char *mime, int32 *translator, int32 *type) {
	TransInfo	*info;
	info = find (fMimeTypes.begin(), fMimeTypes.end(), mime);
	*translator = *type = 0;
	if (info && info != fMimeTypes.end()) {
		*translator = info->Translator (); *type = info->Type ();
	}
}

static PaintRefFilter	myFilter;
static BFilePanel		*gOpenPanel;

void ShowOpenDialog (void) {
	myFilter.InitFilter();
	if (!gOpenPanel) {
		gOpenPanel = new BFilePanel (B_OPEN_PANEL, &be_app_messenger, NULL, 0, true, NULL, &myFilter);
		//	gOpenPanel->SetTarget (be_app_messenger);
	}
	if (gOpenPanel) {
		gOpenPanel->Show ();
		//	gOpenPanel->SetRefFilter (&myFilter);
	}
}

void FindTranslator (const char *mime, int32 *translator, int32 *type) {
	myFilter.InitFilter();
	myFilter.FindTranslator (mime, translator, type);
}

//-------------------------------------------------------------------------------------------------
//	Application class.
//-------------------------------------------------------------------------------------------------
TTestGIMPApp::TTestGIMPApp()
	 	   :BApplication("application/x-vnd.Human_Touch-Wacom")
{
	app_info info;
	GetAppInfo(&info);
}

void TTestGIMPApp::ReadyToRun() {
	((PhotonWindow*) util.mainWin) = new PhotonWindow;
}

void TTestGIMPApp::MessageReceived (BMessage *msg) {
	switch (msg->what) {
		case 'Open':	ShowOpenDialog (); break;
		default:		ASSERT (false); BApplication::MessageReceived (msg); break;
	}
}

void TTestGIMPApp::RefsReceived(BMessage *message) {
	entry_ref	ref;
	int32		translator, fileType;

	if (B_NO_ERROR != message->FindRef ("refs", 0, &ref)) return;
	BFile				file (&ref, B_READ_ONLY);
	//	Let's get the file type
	char				mime[512];
	int32				cntRead;
	cntRead = file.ReadAttr ("BEOS:TYPE", B_STRING_TYPE, 0, mime, 256);

	FindTranslator (mime, &translator, &fileType);
	if (translator && fileType) {
		BBitmap *bm = BTranslationUtils::GetBitmap (&file);
		if (bm) {
			((PhotonWindow*) util.mainWin)->UseBitmap (bm);
		}
	}
}

int main(long argc, char* argv[]) {
	TTestGIMPApp*		app;

	app = new TTestGIMPApp();
	app->Run();
	delete(app);
	return(0);
}
*/