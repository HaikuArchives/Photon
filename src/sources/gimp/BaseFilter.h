#if !defined(__TFILTER__)
#define __TFILTER__

#if Easel
#include "MyStrings.h"
#endif

class TBitmap;
class TDocument;
class TSelectionBase;

#define TFILTER_CHANNEL_RED 0x01L
#define TFILTER_CHANNEL_GREEN 0x02L
#define TFILTER_CHANNEL_BLUE 0x04L
#define TFILTER_CHANNEL_ALPHA 0x08L
#define TFILTER_CHANNEL_RGB 0x07L

class TFilter;
#define FILTER_VERSION_NUMBER	0x100

extern "C" {
	typedef void (*GFilterUpdater) (struct TFilterParams *parms);
	typedef void (*GBufferSwitcher) (struct TFilterParams *parms, bool showMain);
};

typedef struct TFilterParams {
	int32			fVersion;
	TDocument		*fDocument;
	const TBitmap	*fSrc;
	TBitmap			*fDst;
	int32			fCurLayer;
	TSelectionBase	*fCurSel;
	BRect			fBounds;
	TFilter			*fFilter;
	int32			fForeColor, fBackColor;
	GFilterUpdater	fUpdater;
	GBufferSwitcher	fBufferSwitcher;
}TFilterParams;

#define SQR(x) (x*x)

class TFilter : public BArchivable {
public:
					TFilter (void);
					TFilter (BMessage *archive);
virtual				~TFilter ();
virtual	BMessage	*ApplyFilter (TFilterParams *parms) = 0;
virtual	void		SetChannels(int32 ChannelsBits, bool value = true);
virtual	int32		GetChannels(void) {return fChannelsBits;}
double			ShadowsCoefficient(int32 value) 
 { return max_c(0.0, 0.64 - ((SQR((double(value) - 0.0) / 256.0)) / 1.5625)); }
double			MidtonesCoefficient(int32 value) 
 { return max_c(0.0, 0.64 - ((SQR((double(value) - 127.0) / 128.0)) / 1.5625)); }
double			HighlightsCoefficient(int32 value) 
 { return max_c(0.0, 0.64 - ((SQR((double(value) - 255.0) / 256.0)) / 1.5625)); }
virtual	status_t	Archive (BMessage *into, bool deep = true) const;
		void		Abort(void) {fAbortRequest = true; }
		void		AbortClear(void) { fAbortRequest = false; }
protected:
 int32 fChannelsBits;
 bool fAbortRequest;
};

#if Easel
#include "Benaphore.h"

class TProgressFilter : public TFilter {
public:
					TProgressFilter (void);
					TProgressFilter (BMessage *archive);
	void			SetProgressView (class TDlogProgressView *view);
	void			StartProgress (MyString caption);
	void			EndProgress (void);
	void			UpdateProgress (double value);
protected:
	MyString		fCaption;
	bigtime_t 		fStartTime;
	class TDlogProgressView *fProgressView;
	Benaphore		fViewLocker;
};
#endif
#endif
