#if !defined(__FILTER_VIEWS__)
#define __FILTER_VIEWS__

#include "BaseFilter.h"

class TBitmap;
class TMiniView;
class TFilterParams;

class TBitmapPreview : public BView {
public:
				TBitmapPreview (BRect re, const char *name, uint32 resizingMode, uint32 flags);
virtual			~TBitmapPreview ();
virtual	void	Draw (BRect updateRect);
virtual	void	MouseDown (BPoint pt);
virtual	void	MouseMoved (BPoint pt, uint32 transit, const BMessage *msg);
virtual	void	MouseUp (BPoint where);
virtual	void	AttachedToWindow ();
protected:
	TMiniView	*fParent;
	BPoint		fStartPoint;
	bool		fTracking;
};

class TMiniView : public BView {
public:
				TMiniView (BView *view, TFilterParams *parms);
virtual			~TMiniView ();
		float	Ratio (void) const { return float(fNumerator) / fDenominator; }
		TBitmap	*Src (void) { return fSrc; }
		TBitmap	*Dst (void) { return fDst; }
		TFilterParams *Parms (void) { return &fMiniParms; }
		BRect	Bounds (void) const { return fBounds; }
		void	MoveCornerBy (const BPoint &pt);
virtual	void	MessageReceived(BMessage *message);
		void	DestChanged (void) { fPreview->Invalidate (); }
		void	Zoom (bool zoomIn);
		void	ComputeZoomFactors (void);
		float	Scale (float val) { return val * fNumerator / fDenominator; }
		int32	Scale (int32 val) { return val * fNumerator / fDenominator; }
		void	Scale (int32 *val) { *val = *val * fNumerator / fDenominator; }
		BPoint	Scale (const BPoint &pt) { return BPoint (Scale(pt.x), Scale(pt.y)); }
		void	Scale (BPoint *pt) { pt->x = Scale(pt->x); pt->y = Scale(pt->y); }
		void	Scale (BRect *re);
		float	InvScale (float val) { return val * fDenominator / fNumerator; }
		int32	InvScale (int32 val) { return val * fDenominator / fNumerator; }
		void	InvScale (int32 *val) { *val = *val * fDenominator / fNumerator; }
		BPoint	InvScale (const BPoint &pt) { return BPoint (InvScale(pt.x), InvScale(pt.y)); }
		void	InvScale (BPoint *pt) { pt->x = InvScale(pt->x); pt->y = InvScale(pt->y); }
		void	InvScale (BRect *re);
protected:
		void	DocToSrc (void);
		void	InsureBounds (void);

	int32			fZoom, fNumerator, fDenominator;
	TBitmap			*fSrc, *fDst;
	BButton			*fPlus, *fMinus;
	BStringView		*fZoomText;
	BPoint			fCorner;
	TBitmapPreview	*fPreview;
	TFilterParams	*fParms, fMiniParms;
	BRect			fBounds;
};

class TAngleControl : public BControl {
public:
				TAngleControl (BRect frame, const char *name, BMessage *msg, uint32 resizingMode, uint32 flags);
virtual			~TAngleControl ();
virtual	void	Draw (BRect updateRect);
virtual	void	MouseDown (BPoint pt);
virtual	void	MouseMoved (BPoint pt, uint32 transit, const BMessage *msg);
virtual	void	MouseUp (BPoint where);
protected:
		void	PtToAngle (BPoint pt);

	int32		fValue;
	bool		fTracking;
};

class TAngleView : public BView {
public:
				TAngleView (BView *view, BMessage *msg);
virtual			~TAngleView ();
		void	SetValue (int32 value);
		int32	Value (void) { return fValue; }
virtual	void	MessageReceived(BMessage *message);
protected:
	BTextControl	*fText;
	TAngleControl	*fAngleView;
	int32			fValue;
	BMessage		*fMsg;
};

class BLabelSliderHelper : public BHandler {
public:
				BLabelSliderHelper (const char *sliderName, BWindow *wd);
				~BLabelSliderHelper ();
		void	SetValue (int32 val);
		int32	Value (void) const { return fSlider->Value (); }
virtual	void	MessageReceived (BMessage *message);
		void	SetEnabled (bool enabled) { fSlider->SetEnabled (enabled); }
		bool	IsEnabled (void) { return fSlider->IsEnabled (); }
		BSlider	*Slider (void) { return fSlider; }
protected:
		void	UpdateLabel (int32 val);

	BSlider			*fSlider;
	char			*fOrgLabel;
};

#if 0
class BSliderTextHelper : public BHandler {
public:
				BSliderTextHelper (BSlider *slider, BTextControl *text);
				BSliderTextHelper (const char *sliderName, const char *textName, BWindow *wd);
				~BSliderTextHelper ();
		void	SetValue (int32 val, int32 fromWhom = 0);
		int32	Value (void) const { return fSlider->Value (); }
virtual	void	MessageReceived (BMessage *message);
		void	SetEnabled (bool enabled);
		bool	IsEnabled (void) { return fSlider->IsEnabled (); }
		BSlider	*Slider (void) { return fSlider; }
protected:
		void	SetModificationMessages (void);

	BSlider			*fSlider;
	BTextControl	*fTextCtl;
};

class BSliderTextHelperFloat : public BHandler {
public:
				BSliderTextHelperFloat (const char *sliderName, const char *textName, BWindow *wd, int32 denominator);
				~BSliderTextHelperFloat ();
		void	SetValue (int32 val, int32 fromWhom = 0);
		int32	Value (void) const { return fSlider->Value (); }
virtual	void	MessageReceived (BMessage *message);
		void	SetEnabled (bool enabled);
		bool	IsEnabled (void) { return fSlider->IsEnabled (); }
virtual int32	FromText (const char *text);
virtual void	ToText (int32 value, char *text);
protected:
	BSlider			*fSlider;
	BTextControl	*fTextCtl;
	int32			fDenominator;
};
#else
class BSliderTextHelper : public BHandler {
public:
				BSliderTextHelper (BSlider *slider, BTextControl *text);
				BSliderTextHelper (const char *sliderName, const char *textName, BWindow *wd);
				~BSliderTextHelper ();
		void	SetValue (int32 val, int32 fromWhom = 0);
		int32	Value (void) const { return fValue; }
virtual	void	MessageReceived (BMessage *message);
		void	SetEnabled (bool enabled);
		bool	IsEnabled (void) { return fSlider->IsEnabled (); }
		BSlider	*Slider (void) { return fSlider; }
		BTextControl *TextControl (void) { return fTextCtl; }
		bool	TextFocused (void) { return fTextCtl->TextView ()->IsFocus (); }
virtual int32	FromText (const char *text);
virtual void	ToText (int32 value, char *text);
virtual	int32	ValidateValue (int32 value);
protected:
		void	SetModificationMessages (void);
		void	SendMessage (int32 val);

	BSlider			*fSlider;
	BTextControl	*fTextCtl;
	int32			fValue;
};

class BSliderTextHelperFloat : public BSliderTextHelper {
public:
				BSliderTextHelperFloat (const char *sliderName, const char *textName, BWindow *wd, int32 denominator);
				~BSliderTextHelperFloat ();
virtual int32	FromText (const char *text);
virtual void	ToText (int32 value, char *text);
protected:
	int32			fDenominator;
};
#endif

class THistogramView : public BView {
public:
				THistogramView (BView *view);
virtual			~THistogramView ();
virtual	void	Draw (BRect updateRect);
		void	SetValues (float *values);
		void	SetValues (int32 *values);
protected:
		void	FillBitmap (float *values);

	BBitmap			*fBitmap;
};

class TSunkenView : public BView {
public:
				TSunkenView (BView *view);
virtual	void	Draw (BRect updateRect);
protected:
};

class TDlogProgressView : public BView {
public:
				TDlogProgressView (BView *view);
virtual			~TDlogProgressView ();
virtual	void	Draw (BRect updateRect);
virtual	void	MessageReceived (BMessage *message);
		void	Start (char *msg);
		void	Update (double percentage);
protected:
		void	TryUpdate (void);
	char		*fString;
	double		fPercent;
	bool		fInited;
};

void DrawSunkenFrame (BView *view, const BRect &re);
#endif
