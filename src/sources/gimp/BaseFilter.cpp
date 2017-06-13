//--------------------------------------------------------------------------------------------
//
//			Generic TFilter class. Does nothing!
//
//--------------------------------------------------------------------------------------------

#if 0
#include "Flags.h"
#if __x86
#include <stl.h>
#else
#include <vector.h>
#endif
#include "TBitmap.h"
#include "Document.h"
#include "Exceptions.h"
#include "GraphicsTypes.h"
#endif
#include <InterfaceKit.h>
#include <Debug.h>
#include "BaseFilter.h"
#include <Application.h>

#if DEBUG
#define VERIFY(expression) if (!(expression)) ASSERT(false)
#else
#define VERIFY(expression) expression
#endif
#include "FilterViews.h"

TFilter::TFilter (void) {
 fChannelsBits = TFILTER_CHANNEL_RGB;
 fAbortRequest = false;
}

TFilter::TFilter (BMessage *archive) {
 VERIFY (B_OK == archive->FindInt32 ("Channels", &fChannelsBits));
 fAbortRequest = false;
}

TFilter::~TFilter () {
}

void TFilter::SetChannels(int32 ChannelsBits, bool value) {
 if (value)
 	fChannelsBits = ChannelsBits;
 else
 	fChannelsBits &= ~ChannelsBits;
}

status_t TFilter::Archive (BMessage *into, bool deep = true) const {
	status_t	res = BArchivable::Archive (into, deep);
	if (res == B_OK) {
		VERIFY (B_OK == (res =into->AddInt32 ("Channels", fChannelsBits)));
	}
	return res;
}

#if Easel
TProgressFilter::TProgressFilter (void) {
	fProgressView = NULL;
}

TProgressFilter::TProgressFilter (BMessage *archive) {
	fProgressView = NULL;
}

void TProgressFilter::SetProgressView (class TDlogProgressView *view) {
	AutoLockBenaphore	autoLock(&fViewLocker);
	fProgressView = view;
}

void TProgressFilter::StartProgress (MyString caption) {
	AutoLockBenaphore	autoLock(&fViewLocker);
	fStartTime = system_time() + 200000; // wait for 2/10 sc
	fCaption = caption; 
}

void TProgressFilter::EndProgress (void) { 
	UpdateProgress (1.0); 
}

void TProgressFilter::UpdateProgress (double value) {
	AutoLockBenaphore	autoLock(&fViewLocker);
	if (fStartTime != -1 && system_time() > fStartTime) { // Is the process long enough to justify a progress bar to be displayed ?
	 	if (fProgressView) 
		 fProgressView->Start(fCaption); 
		fStartTime = -1;
	}
	if (fStartTime == -1) {
		if (fProgressView) 
	 		fProgressView->Update (value);
		else {
			BMessage	msg('PrgU');
			VERIFY (B_OK == msg.AddString ("Title", fCaption));
			VERIFY (B_OK == msg.AddDouble ("be:value", value));
			be_app->PostMessage (&msg);
		}
	}
}
#endif
