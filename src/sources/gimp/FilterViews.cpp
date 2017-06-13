#if Easel
#include "Flags.h"
#include "TBitmap.h"
#include "Document.h"
#include "BaseFilter.h"
#include "DialogUtils.h"
#include "FilterViews.h"
#include "PaintMessages.h"
#else
#include "GimpStub.h"
typedef enum Zoom_Factors {
	kFirstZoom = -6,
	kZoom8 = kFirstZoom,
	kZoom16,
	kZoom25,
	kZoom33,
	kZoom50,
	kZoom66,
	kZoom100,
	kZoom200,
	kZoom400,
	kZoom600,
	kZoom800,
	kZoom1200,
	kZoom1600,
	kLastZoom = kZoom1600
};
#endif
#include <algo.h>

//------------------------------------------------------------------------------------------
//	class TBitmapPreview, draws a small preview of a TBitmap. Can be moved around. Linked
//	to the TMiniView class.
//------------------------------------------------------------------------------------------
TBitmapPreview::TBitmapPreview (BRect re, const char *name, uint32 resizingMode, uint32 flags) : BView (re, name, resizingMode, flags) {
	SetViewColor (B_TRANSPARENT_32_BIT);
	fParent = NULL;
	fTracking = false;
}

TBitmapPreview::~TBitmapPreview () {
}

void TBitmapPreview::AttachedToWindow (void) {
	fParent = (TMiniView *)Parent ();
	ASSERT (typeid(*fParent)==typeid(TMiniView));
}

void TBitmapPreview::Draw (BRect updateRect) {
	ASSERT (fParent);
	BRect	re = Bounds ();
	SetHighColor (0, 0, 0); StrokeRect (re);
	re.InsetBy (1, 1);
	TBitmap	*bm = fTracking ? fParent->Src() : fParent->Dst ();
	DrawBitmap (bm->Bitmap (), re);
}

void TBitmapPreview::MouseDown (BPoint pt) {
	SetMouseEventMask (B_POINTER_EVENTS | B_KEYBOARD_EVENTS);
	fTracking = true;
	fStartPoint = pt;
}

void TBitmapPreview::MouseMoved (BPoint pt, uint32 transit, const BMessage *msg) {
	if (fTracking) {
		BPoint	delta = fStartPoint - pt;
		fStartPoint = pt;
		if (delta.x != 0 || delta.y != 0)
			fParent->MoveCornerBy (delta);
	} else {
		switch (transit) {
			case B_ENTERED_VIEW:	break;	//	TBD, change the cursor
			case B_EXITED_VIEW:		break;
		}
	}
}

void TBitmapPreview::MouseUp (BPoint where) {
	if (fTracking) {
		fTracking = false; Window()->PostMessage ('_xPr');
	}
}

//------------------------------------------------------------------------------------------
//	Implements with included views a preview of the document. This preview can be zoomed,
//	moved around. Has 2 internal bitmaps, one for the "clean" preview, one for the dest on
//	which the filter is applied.
//------------------------------------------------------------------------------------------
TMiniView::TMiniView (BView *view, TFilterParams *parms) :
					BView (view->Frame (), view->Name (), view->ResizingMode (), view->Flags ())
{
	BView	*parent;
	BRect	re, bounds;

	SetViewColor (view->ViewColor()); SetLowColor (view->LowColor ());
	fParms = parms;
	fMiniParms.fDocument = parms->fDocument; fMiniParms.fCurLayer = parms->fCurLayer; fMiniParms.fCurSel = parms->fCurSel;
	parent = view->Parent ();
	if (parent) parent->RemoveChild (view);
	fZoom = kZoom100; fNumerator = fDenominator = 1;
	re = bounds = view->Bounds ();
	re.left += 10; re.top = re.bottom - 18; re.right = re.left + 15;
	fPlus = new BButton (re, "+", "+", new BMessage ('Plus'));
	fPlus->ResizeTo (re.Width(), re.Height());
	re = bounds; re.right -= 10; re.top = re.bottom - 18; re.left = re.right - 15;
	fMinus = new BButton (re, "-", "-", new BMessage ('Mins'));
	fMinus->ResizeTo (re.Width(), re.Height());

	re = bounds; re.top = re.bottom - 18; re.left += 30; re.right -= 30; re.bottom -= 2;
	fZoomText = new BStringView (re, "Zoom", "100%");
	fZoomText->SetAlignment (B_ALIGN_CENTER);

	re = bounds;
	re.bottom -= 25;
	fPreview = new TBitmapPreview (re, "BM", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	re = fPreview->Bounds (); re.right -= 2; re.bottom -= 2;
	fSrc = new TBitmap (re, B_RGBA32, true); fDst = new TBitmap (re, B_RGBA32, false);
	BView	*bmHelper = new BView (re, "Helper", 0, B_WILL_DRAW);
	fSrc->Bitmap ()->AddChild (bmHelper);
	fMiniParms.fSrc = fSrc; fMiniParms.fDst = fDst; fMiniParms.fBounds = fSrc->Bounds ();
	fCorner.x = fCorner.y = 0;
	DocToSrc (); Window()->PostMessage ('_xPr');
	AddChild (fPlus); AddChild (fMinus); AddChild (fZoomText); AddChild (fPreview);
	if (parent) parent->AddChild (this);
	fPlus->SetTarget (this); fMinus->SetTarget (this);
	delete view;
}

TMiniView::~TMiniView () {
	delete fSrc; delete fDst;
}

void TMiniView::DocToSrc (void) {
	BRect	re = fPreview->Bounds ();
	re.InsetBy (1, 1);
	fSrc->Bitmap()->Lock ();
	BView	*view = fSrc->Bitmap ()->ChildAt (0);
	ASSERT (view);
	BRect	reSrc, reDst;
	reDst = view->Bounds ();
	reSrc.top = fCorner.y; reSrc.left = fCorner.x;
	reSrc.bottom = reSrc.top + Scale (reDst.IntegerHeight () + 1);
	reSrc.right = reSrc.left + Scale (reDst.IntegerWidth () + 1);
	view->SetHighColor (255, 255, 255, 255); view->FillRect (view->Bounds ());
	view->DrawBitmap (fParms->fSrc->Bitmap (), reSrc, reDst);
	view->Sync ();
	fSrc->Bitmap()->Unlock ();
}

void TMiniView::MoveCornerBy (const BPoint &pt) {
	BPoint	newCorner = pt;

	if (fDenominator < fNumerator) {
		newCorner.x = Scale (newCorner.x);
		newCorner.y = Scale (newCorner.y);
	}
	newCorner = fCorner + newCorner;
	if (newCorner.x < 0) newCorner.x = 0;
	if (newCorner.y < 0) newCorner.y = 0;
	int32	max;
	max = int32(fParms->fSrc->Bounds().right)- Scale ((fSrc->Bounds().IntegerWidth() + 1));
	if (newCorner.x > max) newCorner.x = max;
	max = int32(fParms->fSrc->Bounds().bottom) - Scale ((fSrc->Bounds().IntegerHeight() + 1) * fDenominator);
	if (newCorner.y > max) newCorner.y = max;
	if (newCorner != fCorner) {
		fCorner = newCorner;
		DocToSrc ();
		fPreview->Invalidate ();
	}
}

void TMiniView::MessageReceived(BMessage *message) {
	switch (message->what) {
		case 'Plus':	Zoom (false); break;
		case 'Mins':	Zoom (true); break;
	}
}

void TMiniView::Zoom (bool zoomIn) {
	if (zoomIn) {
		if (fZoom == kFirstZoom) fPlus->SetEnabled (true);
		if (fZoom != kLastZoom) {
			fZoom = Zoom_Factors(int32(fZoom) + 1);
			ComputeZoomFactors (); InsureBounds ();
			DocToSrc (); Window()->PostMessage ('_xPr');
			if (fZoom == kLastZoom) fMinus->SetEnabled (false);
		}
	} else {
		if (fZoom == kLastZoom) fMinus->SetEnabled (true);
		if (fZoom != kFirstZoom) {
			fZoom = Zoom_Factors(int32(fZoom) - 1);
			ComputeZoomFactors (); InsureBounds ();
			DocToSrc (); Window()->PostMessage ('_xPr');
			if (fZoom == kFirstZoom) fPlus->SetEnabled (false);
		}
	}
	int32	x = 100;
	char	string[32];
	x = 100; x = InvScale (x); sprintf (string, "%i%%", int(x));
	fZoomText->SetText (string);
}

void TMiniView::ComputeZoomFactors (void) {
	switch (fZoom) {
		case kZoom8:	fNumerator = 1; fDenominator = 12; break;
		case kZoom16:	fNumerator = 1; fDenominator = 6; break;
		case kZoom25:	fNumerator = 1; fDenominator = 4; break;
		case kZoom33:	fNumerator = 1; fDenominator = 3; break;
		case kZoom50:	fNumerator = 1; fDenominator = 2; break;
		case kZoom66:	fNumerator = 2; fDenominator = 3; break;
		case kZoom100:	fNumerator = 1; fDenominator = 1; break;
		case kZoom200:	fNumerator = 2; fDenominator = 1; break;
		case kZoom400:	fNumerator = 4; fDenominator = 1; break;
		case kZoom600:	fNumerator = 6; fDenominator = 1; break;
		case kZoom800:	fNumerator = 8; fDenominator = 1; break;
		case kZoom1200:	fNumerator = 12; fDenominator = 1; break;
		case kZoom1600:	fNumerator = 16; fDenominator = 1; break;
	}
}

void TMiniView::Scale (BRect *re) {
	float	width, height;

	width = re->Width () + 1; height = re->Height() + 1;
	re->top = Scale (re->top); re->left = Scale (re->left);
	re->bottom = re->top + Scale (height) - 1; re->right = re->left + Scale (width) - 1;
}

void TMiniView::InvScale (BRect *re) {
	float	width, height;

	width = re->Width () + 1; height = re->Height() + 1;
	re->top = InvScale (re->top); re->left = InvScale (re->left);
	re->bottom = re->top + InvScale (height) - 1; re->right = re->left + InvScale (width) - 1;
}

void TMiniView::InsureBounds (void) {
//	BRect	re = fParms->fSrc->Bounds ();
//	re.right = re.right * fNumerator / fDenominator; re.bottom = re.
}

//------------------------------------------------------------------------------------------
//	Implements with included views an angle control. This control can be used with the circle
//	or with the the text control. When the value is changed, the window is fired with the
//	message msg.
//------------------------------------------------------------------------------------------
TAngleView::TAngleView (BView *view, BMessage *msg) :
					BView (view->Frame (), view->Name (), view->ResizingMode (), view->Flags ())
{
	BView	*parent;
	BRect	re, bounds;

	SetViewColor (view->ViewColor()); SetLowColor (view->LowColor ());
	parent = view->Parent ();
	if (parent) parent->RemoveChild (view);
	re = bounds = view->Bounds ();
	re.right = re.left + 30; re.top = (re.top + re.bottom) / 2 - 8; re.bottom = re.top + 8;
	fText = new BTextControl (re, "TxtAngle", "", "", new BMessage ('AnTx'));
	fText->SetText ("0");
	fText->SetDivider (0);
	re = bounds; re.left = re.left + 32;
	int32	diff = re.IntegerWidth() - re.IntegerHeight();
	if (diff < 0)  {
		re.top -= diff / 2; re.bottom = re.top + re.IntegerWidth ();
	} else if (diff > 0) {
		re.left += diff / 2; re.right = re.left + re.IntegerHeight ();
	}
	fAngleView = new TAngleControl (re, "Angle", new BMessage ('Angl'), B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	if (parent) parent->AddChild (this);
	AddChild (fText); AddChild (fAngleView);
	fAngleView->SetTarget (this); fText->SetTarget (this);
	fMsg = msg;
	fMsg->AddInt32 ("be:value", 0);
}

TAngleView::~TAngleView () {
}

void TAngleView::SetValue (int32 value) {
	if (value != fAngleView->Value()) {
		char	string[64];
		fAngleView->SetValue (value);
		sprintf (string, "%i", int(value)); fText->SetText (string);
	}
}

void TAngleView::MessageReceived (BMessage *message) {
	char	*endText, string[64];
	int32	val;
	switch (message->what) {
		case 'AnTx':
			val = strtol (fText->Text (), &endText, 10) % 360;
			fAngleView->SetValue (val);
			VERIFY (B_OK == fMsg->ReplaceInt32 ("be:value", val));
			Window ()->PostMessage (fMsg);
			break;
		case 'Angl':
			VERIFY (B_OK == message->FindInt32 ("be:value", &val));
			sprintf (string, "%i", int(val)); fText->SetText (string);
			VERIFY (B_OK == fMsg->ReplaceInt32 ("be:value", val));
			Window ()->PostMessage (fMsg);
			break;
	}
}

TAngleControl::TAngleControl (BRect frame, const char *name, BMessage *msg, uint32 resizingMode, uint32 flags) :
			BControl (frame, name, "", msg, resizingMode, flags)
{
	SetViewColor (B_TRANSPARENT_32_BIT); SetLowColor (B_TRANSPARENT_32_BIT);
	fTracking = false;
}

TAngleControl::~TAngleControl () {
}

void TAngleControl::Draw (BRect updateRect) {
	BRect	re = Bounds ();
	SetHighColor (0, 0, 0); StrokeEllipse (re);
	SetHighColor (212, 212, 212); re.InsetBy (1, 1); FillEllipse (re);
	BPoint	center, pt;
	int32	ray = re.IntegerWidth () / 2;
	center.y = (re.top + re.bottom) / 2; center.x = (re.left + re.right) / 2;
	pt.x = center.x + sin (6.283 / 360 * Value ()) * ray;
	pt.y = center.y - cos (6.283 / 360 * Value ()) * ray;
	SetHighColor (0, 0, 0); StrokeLine (center, pt);
}

void TAngleControl::PtToAngle (BPoint pt) {
	BRect	re = Bounds ();
	int32	ray = re.IntegerWidth () / 2, angle;

	angle = int32(360 * atan ((pt.x - ray) / (ray - pt.y)) / 6.283);
	if (pt.y > ray) angle = angle + 180;
	angle = (angle + 360) % 360;
	if (Value () != angle) {
		SetValue (angle);
		Invoke ();
	}
}

void TAngleControl::MouseDown (BPoint pt) {
	BRect	re = Bounds ();

	BPoint	center;
	center.y = (re.top + re.bottom) / 2; center.x = (re.left + re.right) / 2;
	center = pt - center;
	int32	distance = int32(sqrt (center.x * center.x + center.y * center.y));
	if (distance <= re.IntegerWidth () / 2 + 1) {
		SetMouseEventMask (B_POINTER_EVENTS | B_KEYBOARD_EVENTS);
		fTracking = true;
		PtToAngle (pt);
	}
}

void TAngleControl::MouseMoved (BPoint pt, uint32 transit, const BMessage *msg) {
	if (fTracking) PtToAngle (pt);
}

void TAngleControl::MouseUp (BPoint where) {
	if (fTracking) {
		PtToAngle (where); fTracking = false;
	}
	BControl::MouseUp (where);
}

//------------------------------------------------------------------------------------------
//	class BLabelSliderHelper, small helper to add the value of a slider in its label.
//------------------------------------------------------------------------------------------
BLabelSliderHelper::BLabelSliderHelper (const char *sliderName, BWindow *wd) {
	fSlider= (BSlider *) wd->FindView (sliderName); ASSERT (fSlider && typeid(*fSlider)==typeid(BSlider));
	fSlider->Window()->AddHandler (this); fSlider->SetTarget (this);
	fSlider->ResizeToPreferred (); fSlider->ResizeBy (0, -3);	//	I don't like the way they look in "preferred"
	//	Now, we get and store the label.
	const char	*label = fSlider->Label ();
	int32		length = strlen (label);
	fOrgLabel = new char [length + 1];
	if (fOrgLabel) {
		memcpy (fOrgLabel, label, length + 1);
	}
	//	Now, take care of the modification message
	if (fSlider->ModificationMessage() == NULL)
		fSlider->SetModificationMessage (new BMessage ('_Cnt'));
}

BLabelSliderHelper::~BLabelSliderHelper () {
	delete [] fOrgLabel;
}

void BLabelSliderHelper::UpdateLabel (int32 val) {
	if (fOrgLabel) {
		char	label[256], string[12], *p, *q, *st;

		sprintf (string, "%i", int(val));
		for (p = label, q = fOrgLabel; ; ) {
			if (*q == 0) { *p++ = 0; break; }
			if (*q == '^' && q[1] == '0') {
				q += 2;
				//	We copy the string
				for (st = string; *st; ) *p++ = *st++;
			} else {
				*p++ = *q++;
			}
		}
		if (strcmp (label, fSlider->Label ()) != 0) fSlider->SetLabel (label);
	}
}

void BLabelSliderHelper::SetValue (int32 val) {
	UpdateLabel (val); fSlider->SetValue (val);
}

void BLabelSliderHelper::MessageReceived (BMessage *message) {
	BMessage	*ctlMsg;
	int32		val;

	if (NULL != (ctlMsg = fSlider->Message()) && message->what == ctlMsg->what) {
		VERIFY (B_OK == message->FindInt32 ("be:value", &val));
		UpdateLabel (val);
		fSlider->Window()->PostMessage (message);
	} else if (NULL != (ctlMsg = fSlider->ModificationMessage()) && message->what == ctlMsg->what) {
		VERIFY (B_OK == message->FindInt32 ("be:value", &val));
		UpdateLabel (val);
		if (message->what != '_Cnt') fSlider->Window()->PostMessage (message);
	}
}

//------------------------------------------------------------------------------------------
//	class BSliderTextHelper, small helper to link a slider and a text control.
//------------------------------------------------------------------------------------------
#if 0
BSliderTextHelper::BSliderTextHelper (BSlider *slider, BTextControl *text) {
	fSlider = slider; fTextCtl = text;
	fSlider->Window()->AddHandler (this);
	VERIFY (B_OK == fSlider->SetTarget (this)); VERIFY (B_OK == fTextCtl->SetTarget (this));
	fSlider->ResizeToPreferred (); fSlider->ResizeBy (0, -3);	//	I don't like the way they look in "preferred"
	//	Now, take care of the modification messages
	SetModificationMessages ();
}

BSliderTextHelper::BSliderTextHelper (const char *sliderName, const char *textName, BWindow *wd) {
	fSlider= (BSlider *) wd->FindView (sliderName); ASSERT (fSlider && typeid(*fSlider)==typeid(BSlider));
	fTextCtl = (BTextControl *)wd->FindView (textName); ASSERT (fTextCtl && typeid(*fTextCtl)==typeid(BTextControl));
	fSlider->Window()->AddHandler (this);
	VERIFY (B_OK == fSlider->SetTarget (this)); VERIFY (B_OK == fTextCtl->SetTarget (this));
	fSlider->ResizeToPreferred (); fSlider->ResizeBy (0, -3);	//	I don't like the way they look in "preferred"
	//	Now, take care of the modification messages
	SetModificationMessages ();
}

BSliderTextHelper::~BSliderTextHelper () {
}

void BSliderTextHelper::SetModificationMessages (void) {
	if (fSlider->ModificationMessage() == NULL)
		fSlider->SetModificationMessage (new BMessage ('_Cnt'));
	if (fTextCtl->ModificationMessage() == NULL)
		fTextCtl->SetModificationMessage (new BMessage ('_Cnt'));
	if (fTextCtl->Message () == NULL)
		fTextCtl->SetMessage (new BMessage ('TEXT'));
}

void BSliderTextHelper::SetValue (int32 val, int32 fromWhom) {
	if (fromWhom <= 0 && val != fSlider->Value ()) fSlider->SetValue (val);
	if (fromWhom >= 0) {
		char	string[64];
		sprintf (string, "%i", int(val));
		if (!fTextCtl->TextView ()->IsFocus () && strcmp (string, fTextCtl->Text ()) != 0)
			fTextCtl->SetText (string);
	}
}

void BSliderTextHelper::MessageReceived (BMessage *message) {
	BMessage	*ctlMsg;
	int32		val;
	char		*endText;

	if (NULL != (ctlMsg = fSlider->Message()) && message->what == ctlMsg->what) {
		VERIFY (B_OK == message->FindInt32 ("be:value", &val));
		SetValue (val, 1);
		fSlider->Window()->PostMessage (message);
	} else if (NULL != (ctlMsg = fSlider->ModificationMessage()) && message->what == ctlMsg->what) {
		VERIFY (B_OK == message->FindInt32 ("be:value", &val));
		if (val == fSlider->Value()) {
			SetValue (val, 1);
			if (message->what != '_Cnt') fSlider->Window()->PostMessage (message);
		}
	} else {
		int32	min, max;

		fSlider->GetLimits (&min, &max);
		val = strtol (fTextCtl->Text (), &endText, 10);
		if (val < min) val = min;
		if (val > max) val = max;
		SetValue (val, -1);
		if (message->what != '_Cnt') {
			BMessage	msg2Send (fSlider->Message());
			msg2Send.RemoveName ("be:value");
			VERIFY (B_OK == msg2Send.AddInt32 ("be:value", val));
			fSlider->Window()->PostMessage (&msg2Send);
		}
	}
}

void BSliderTextHelper::SetEnabled (bool enabled) {
	fSlider->SetEnabled (enabled); fTextCtl->SetEnabled (enabled);
}

BSliderTextHelperFloat::BSliderTextHelperFloat (const char *sliderName, const char *textName, BWindow *wd, int32 denominator) {
	fSlider= (BSlider *) wd->FindView (sliderName); ASSERT (fSlider && typeid(*fSlider)==typeid(BSlider));
	fTextCtl = (BTextControl *)wd->FindView (textName); ASSERT (fTextCtl && typeid(*fTextCtl)==typeid(BTextControl));
	fSlider->Window()->AddHandler (this);
	fSlider->SetTarget (this); fTextCtl->SetTarget (this);
	fDenominator = denominator;
	fSlider->ResizeToPreferred (); fSlider->ResizeBy (0, -3);	//	I don't like the way they look in "preferred"
	//	Now, take care of the modification message
	if (fSlider->ModificationMessage() == NULL)
		fSlider->SetModificationMessage (new BMessage ('_Cnt'));
	if (fTextCtl->ModificationMessage() == NULL)
		fTextCtl->SetModificationMessage (new BMessage ('_Cnt'));
	if (fTextCtl->Message () == NULL)
		fTextCtl->SetMessage (new BMessage ('TEXT'));
}

BSliderTextHelperFloat::~BSliderTextHelperFloat () {
}

void BSliderTextHelperFloat::ToText (int32 value, char *text) {
	if (fDenominator <= 10)
		sprintf (text, "%.1f", double(value) / fDenominator);
	else if (fDenominator <= 100)
		sprintf (text, "%.2f", double(value) / fDenominator);
	else
		sprintf (text, "%.3f", double(value) / fDenominator);
}

int32 BSliderTextHelperFloat::FromText (const char *text) {
	char	*endText;

	return int32 (fDenominator * strtod (text, &endText));
}

void BSliderTextHelperFloat::SetValue (int32 val, int32 fromWhom) {
	//	DEBUGGER ("Show Me");
	if (fromWhom <= 0 && val != fSlider->Value ()) fSlider->SetValue (val);
	if (fromWhom >= 0) {
		char	string[64];
		ToText (val, string);
		if (!fTextCtl->TextView ()->IsFocus () && strcmp (string, fTextCtl->Text ()) != 0)
			fTextCtl->SetText (string);
	}
}

void BSliderTextHelperFloat::MessageReceived (BMessage *message) {
	BMessage	*ctlMsg;
	int32		val;

	if (NULL != (ctlMsg = fSlider->Message()) && message->what == ctlMsg->what) {
		VERIFY (B_OK == message->FindInt32 ("be:value", &val));
		SetValue (val, 1);
		fSlider->Window()->PostMessage (message);
	} else if (NULL != (ctlMsg = fSlider->ModificationMessage()) && message->what == ctlMsg->what) {
		VERIFY (B_OK == message->FindInt32 ("be:value", &val));
		if (val == fSlider->Value()) {
			SetValue (val, 1);
			if (message->what != '_Cnt') fSlider->Window()->PostMessage (message);
		}
	} else {
		int32	min, max;

		fSlider->GetLimits (&min, &max);
		val = FromText (fTextCtl->Text ());
		if (val < min) val = min;
		if (val > max) val = max;
		SetValue (val, -1);
		if (message->what != '_Cnt') {
			BMessage	msg2Send (fSlider->Message());
			msg2Send.RemoveName ("be:value");
			VERIFY (B_OK == msg2Send.AddInt32 ("be:value", val));
			fSlider->Window()->PostMessage (&msg2Send);
		}
	}
}

void BSliderTextHelperFloat::SetEnabled (bool enabled) {
	fSlider->SetEnabled (enabled); fTextCtl->SetEnabled (enabled);
}
#else

BSliderTextHelper::BSliderTextHelper (BSlider *slider, BTextControl *text) {
	fSlider = slider; fTextCtl = text;
	fSlider->Window()->AddHandler (this);
	VERIFY (B_OK == fSlider->SetTarget (this)); VERIFY (B_OK == fTextCtl->SetTarget (this));
	fSlider->ResizeToPreferred (); fSlider->ResizeBy (0, -3);	//	I don't like the way they look in "preferred"
	//	Now, take care of the modification messages
	SetModificationMessages ();
	fValue = fSlider->Value ();
}

BSliderTextHelper::BSliderTextHelper (const char *sliderName, const char *textName, BWindow *wd) {
	fSlider= (BSlider *) wd->FindView (sliderName); ASSERT (fSlider && typeid(*fSlider)==typeid(BSlider));
	fTextCtl = (BTextControl *)wd->FindView (textName); ASSERT (fTextCtl && typeid(*fTextCtl)==typeid(BTextControl));
	fSlider->Window()->AddHandler (this);
	VERIFY (B_OK == fSlider->SetTarget (this)); VERIFY (B_OK == fTextCtl->SetTarget (this));
	fSlider->ResizeToPreferred (); fSlider->ResizeBy (0, -3);	//	I don't like the way they look in "preferred"
	//	Now, take care of the modification messages
	SetModificationMessages ();
	fValue = fSlider->Value ();
}

BSliderTextHelper::~BSliderTextHelper () {
}

void BSliderTextHelper::SetModificationMessages (void) {
	if (fSlider->ModificationMessage() == NULL)
		fSlider->SetModificationMessage (new BMessage ('_Cnt'));
	if (fTextCtl->ModificationMessage() == NULL)
		fTextCtl->SetModificationMessage (new BMessage ('_Cnu'));
	if (fTextCtl->Message () == NULL)
		fTextCtl->SetMessage (new BMessage ('TEXT'));
}

void BSliderTextHelper::SetValue (int32 val, int32 fromWhom) {
	fValue = val;
	if (fromWhom <= 0 && val != fSlider->Value ())
		fSlider->SetValue (val);
	if (fromWhom >= 0) {
		char	string[64];
		ToText (val, string);
		if (strcmp (string, fTextCtl->Text ()) != 0)
			fTextCtl->SetText (string);
	}
}

void BSliderTextHelper::SendMessage (int32 val) {
	BMessage	msg2Send (fSlider->Message());
	msg2Send.RemoveName ("be:value");
	VERIFY (B_OK == msg2Send.AddInt32 ("be:value", val));
	fSlider->Window()->PostMessage (&msg2Send);
}

void BSliderTextHelper::MessageReceived (BMessage *message) {
	BMessage	*ctlMsg;
	int32		val;

	if (NULL != (ctlMsg = fSlider->Message()) && message->what == ctlMsg->what) {
		//	Slider message
		VERIFY (B_OK == message->FindInt32 ("be:value", &val));
		if (val != fValue && val == fSlider->Value ()) {
			if (val != fValue) SetValue (val, 1);
			SendMessage (fValue);
		}
	} else if (NULL != (ctlMsg = fSlider->ModificationMessage()) && message->what == ctlMsg->what) {
		//	Slider modification message
		VERIFY (B_OK == message->FindInt32 ("be:value", &val));
		if (val != fValue && val == fSlider->Value ()) {
			SetValue (val, 1);
			//if (message->what != '_Cnt')
			SendMessage (fValue);
		}
	} else if (NULL != (ctlMsg = fTextCtl->ModificationMessage()) && message->what == ctlMsg->what) {
		//	Text modification message
		val = ValidateValue (FromText (fTextCtl->Text ()));
		if (val != fValue) {
			SetValue (val, -1);
			//if (fSlider->ModificationMessage() && fSlider->ModificationMessage()->what != '_Cnt') {
				SendMessage (fValue);
			//}
		}
	} else {
		val = ValidateValue (FromText (fTextCtl->Text ()));
		//bool	sendMsg = val != fValue;
		SetValue (val, 1);	//	To force the text if needeed.
		#if 0
		if (sendMsg && fTextCtl->ModificationMessage()->what == '_Cnt') {
			//	Then it was not sent before, so send it now...
			BMessage	msg2Send (fSlider->Message());
			msg2Send.RemoveName ("be:value");
			VERIFY (B_OK == msg2Send.AddInt32 ("be:value", val));
			fSlider->Window()->PostMessage (&msg2Send);
		}
		#endif
	}
}

int32 BSliderTextHelper::ValidateValue (int32 value) {
	int32	min, max;

	fSlider->GetLimits (&min, &max);
	if (value < min) value = min;
	if (value > max) value = max;
	return value;
}

void BSliderTextHelper::ToText (int32 value, char *text) {
	sprintf (text, "%i", int(value));
}

int32 BSliderTextHelper::FromText (const char *text) {
	char	*endText;

	return int32(strtol (text, &endText, 10));
}

void BSliderTextHelper::SetEnabled (bool enabled) {
	fSlider->SetEnabled (enabled); fTextCtl->SetEnabled (enabled);
}

BSliderTextHelperFloat::BSliderTextHelperFloat (const char *sliderName, const char *textName, BWindow *wd, int32 denominator) :
	BSliderTextHelper (sliderName, textName, wd) {
	fDenominator = denominator;
}

BSliderTextHelperFloat::~BSliderTextHelperFloat () {
}

void BSliderTextHelperFloat::ToText (int32 value, char *text) {
	if (fDenominator <= 10)
		sprintf (text, "%.1f", double(value) / fDenominator);
	else if (fDenominator <= 100)
		sprintf (text, "%.2f", double(value) / fDenominator);
	else
		sprintf (text, "%.3f", double(value) / fDenominator);
}

int32 BSliderTextHelperFloat::FromText (const char *text) {
	char	*endText;

	return int32 (fDenominator * strtod (text, &endText));
}

#endif

//------------------------------------------------------------------------------------------
//	class THistogramView, implements a view of an histogram.
//------------------------------------------------------------------------------------------
THistogramView::THistogramView (BView *view) :
					BView (view->Frame (), view->Name (), view->ResizingMode (), view->Flags ())
{
	BView	*parent;
	BRect	bounds;

	parent = view->Parent (); bounds = view->Bounds ();
	fBitmap = new BBitmap (BRect (0, 0, 256, bounds.bottom - 5), B_GRAYSCALE_8_BIT, true);
	BView	*bmView = new BView (fBitmap->Bounds (), "BM", 0, B_WILL_DRAW);
	fBitmap->AddChild (bmView);
	if (parent) parent->RemoveChild (view);
	if (parent) parent->AddChild (this);
	delete view;
}

THistogramView::~THistogramView () {
	delete fBitmap;
}

void THistogramView::Draw (BRect updateRect) {
	updateRect = Bounds ();
	DrawSunkenFrame (this, updateRect);
	DrawBitmap (fBitmap, BPoint (2, 2));
}

void THistogramView::FillBitmap (float *values) {
	BRect		re = fBitmap->Bounds ();
	fBitmap->Lock ();
	BView		*view = fBitmap->ChildAt (0);
	rgb_color	black;
	int32		i;

	black.red = black.green = black.blue = 0;
	view->SetHighColor (216, 216, 216); view->FillRect (re);
	view->BeginLineArray (256);
	for (i = 256; --i >= 0; ) {
		if (values[i] != 0)
			view->AddLine (BPoint (i, re.bottom), BPoint (i, re.bottom - values[i]), black);
	}
	view->EndLineArray ();
	view->Sync ();
	fBitmap->Unlock ();
}

void THistogramView::SetValues (float *values) {
	int32	height = fBitmap->Bounds().IntegerHeight () + 1;
	float	tmpVals[256];
	float	biggest = *max_element (values, values + 256);
	float	ratio = height / biggest;
	float	scaledValues[256];
	int32	i;

	memcpy (tmpVals, values, 256 * sizeof (float));
	sort (tmpVals, tmpVals + 256);
	//	biggest = (tmpVals[127] + tmpVals[128]) * 2;
	biggest = (tmpVals[255] + tmpVals[254]) * .475; ratio = height / biggest;
	for (i = 256; --i >= 0; ) {
		scaledValues[i] = values[i] * ratio;
	}
	FillBitmap (scaledValues); Invalidate ();
}

void THistogramView::SetValues (int32 *values) {
}

TSunkenView::TSunkenView (BView *view) :
	BView (view->Frame (), view->Name (), view->ResizingMode (), view->Flags ())
{
	BView	*parent;

	parent = view->Parent ();
	ASSERT (parent);
	if (parent) parent->RemoveChild (view);
	if (parent) parent->AddChild (this);
	delete view;
}

void TSunkenView::Draw (BRect updateRect) {
	DrawSunkenFrame (this, Bounds ());
}

void DrawSunkenFrame (BView *view, const BRect &re) {
	view->BeginLineArray (12);
	rgb_color	col;
	col.red = col.green = col.blue = 155;
	view->AddLine (BPoint (re.left, re.bottom - 2), BPoint (re.left, re.top), col);
	view->AddLine (BPoint (re.left, re.top), BPoint (re.right - 2, re.top), col);
	view->AddLine (BPoint (re.left + 2, re.bottom), BPoint (re.right, re.bottom), col);
	view->AddLine (BPoint (re.right, re.bottom), BPoint (re.right, re.top + 2), col);
	col.red = col.green = col.blue = 244;
	view->AddLine (BPoint (re.left + 1, re.bottom - 2), BPoint (re.left + 1, re.top + 1), col);
	view->AddLine (BPoint (re.left + 1, re.top + 1), BPoint (re.right - 3, re.top + 1), col);
	col.red = col.green = col.blue = 88;
	view->AddLine (BPoint (re.right - 1, re.top), BPoint (re.right - 1, re.bottom - 1), col);
	view->AddLine (BPoint (re.right - 1, re.bottom - 1), BPoint (re.left, re.bottom - 1), col);
	col.red = col.green = col.blue = 199;
	view->AddLine (BPoint (re.right - 2, re.top + 1), BPoint (re.right - 2, re.bottom - 2), col);
	view->AddLine (BPoint (re.right - 2, re.bottom - 2), BPoint (re.left + 1, re.bottom - 2), col);
	view->EndLineArray ();
}
//------------------------------------------------------------------------------------------
//	class TDlogProgressView, implements a view of a progress indicator.
//------------------------------------------------------------------------------------------
TDlogProgressView::TDlogProgressView (BView *view) :
					BView (view->Frame (), view->Name (), view->ResizingMode (), view->Flags ())
{
	BView	*parent;

	parent = view->Parent ();
	if (parent) parent->RemoveChild (view);
	if (parent) parent->AddChild (this);
	delete view;
	fInited = false;
}

TDlogProgressView::~TDlogProgressView () {
}

void TDlogProgressView::Draw (BRect updateRect) {
	if (fInited) {
		BRect	re = Bounds ();
		rgb_color	col;
		col.red = col.green = col.blue = 189;
		BeginLineArray (12);
		AddLine (BPoint (re.left, re.top), BPoint (re.right, re.top), col);
		AddLine (BPoint (re.left, re.top), BPoint (re.left, re.bottom), col);
		col.red = col.green = col.blue = 239;
		AddLine (BPoint (re.right, re.top + 1), BPoint (re.right, re.bottom), col);
		AddLine (BPoint (re.left + 1, re.bottom), BPoint (re.right, re.bottom), col);
		re.InsetBy (1, 1); col.red = col.green = col.blue = 99;
		AddLine (BPoint (re.left, re.top), BPoint (re.right, re.top), col);
		AddLine (BPoint (re.left, re.top), BPoint (re.left, re.bottom), col);
		col.red = col.green = col.blue = 222;
		AddLine (BPoint (re.right, re.top + 1), BPoint (re.right, re.bottom), col);
		AddLine (BPoint (re.left + 1, re.bottom), BPoint (re.right, re.bottom), col);
		re.InsetBy (1, 1); col.red = 132; col.green = 198; col.blue = 255;
		BRect	re1 = re;
		re.right = re.left + (re.right - re.left) * fPercent; re1.left = re.right + 1;
		AddLine (BPoint (re.left, re.top), BPoint (re.right, re.top), col);
		AddLine (BPoint (re.left, re.top), BPoint (re.left, re.bottom), col);
		col.red = 24; col.green = 90; col.blue = 148;
		AddLine (BPoint (re.right, re.top + 1), BPoint (re.right, re.bottom), col);
		AddLine (BPoint (re.left + 1, re.bottom), BPoint (re.right, re.bottom), col);
		EndLineArray ();
		re.InsetBy (1, 1); SetHighColor (49, 148, 255); FillRect (re);
		SetHighColor (255, 255, 255); FillRect (re1);
	} else {
		SetHighColor (Parent()->ViewColor ()); FillRect (updateRect);
	}
}

void TDlogProgressView::Start (char *msg) {
	fInited = true; fPercent = 0; TryUpdate ();
}

void TDlogProgressView::Update (double percentage) {
	if (percentage > 0.95) {
		fInited = false;
	} else {
		fPercent = percentage; fInited = true;
	}
	TryUpdate ();
}

void TDlogProgressView::MessageReceived (BMessage *message) {
	if (message->what == '_Dr_') {
		BMessageQueue	*msgQueue = Window()->MessageQueue ();
		if (msgQueue->FindMessage ('_Dr_', 0)) return;	//	Wait for the better one!
		Draw (Bounds ());
	}
}

void TDlogProgressView::TryUpdate (void) {
	if (B_OK == Window ()->LockWithTimeout (1000)) {
		Draw (Bounds ()); Window()->Unlock ();
	} else {
		Window ()->PostMessage ('_Dr_', this);
		//Invalidate ();
	}
}

