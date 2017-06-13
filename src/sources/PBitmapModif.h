#include "Utils.h"
#include "PrefData.h"

/*
 *  ProgressiveBitmapStream.h
 *
 *  Release 3.1.0 (Apr. 27th, 1998)
 *  Written by Edmund Vermeulen <edmundv@xs4all.nl>
 *
 *
 *  A ProgressiveBitmapStream object can be used both as a source or as
 *  a destination for a B_TRANSLATOR_BITMAP by the translation kit. Its
 *  special features consist of sending 'rect updated' messages for incoming
 *  bitmaps (to allow progressive display), and displaying a progress bar
 *  for both incoming and outgoing bitmaps.
 *
 *
 *  When used as a source, you pass a pointer to the bitmap to save in the
 *  contructor of the object. You still remain responsible for deleting the
 *  bitmap yourself.
 *
 *
 *  When used as a destination, a bitmap will automatically be created
 *  when the B_TRANSLATOR_BITMAP comes streaming in. You can retrieve a
 *  pointer to the bitmap when translation has been completed through the
 *  Bitmap() function. Or, if you supply a 'bitmap_created' BInvoker object
 *  in the constructor, you will be notified by a message when the bitmap
 *  is created. A pointer to the bitmap will be attached to the message.
 *
 *    BBitmap *the_bitmap;
 *    msg->FindPointer("bitmap", &the_bitmap);
 *
 *  If you also supply a 'rect_updated' BInvoker object then you will
 *  also be notified every time some data is written to the bitmap. The
 *  messages will have a BRect attached of the area that was affected.
 *
 *    BRect update_rect;
 *    msg->FindRect("rect", &update_rect);
 *
 *  After you have retrieved the bitmap, you will probably want to call
 *  SetDispose(false) on the object so that the bitmap won't be deleted
 *  when the object is destructed.
 *
 *  If you set the 'dither' flag to true in the contructor, any
 *  incoming 32-bit bitmap will automatically be dithered to 8-bit.
 *  If you also set the 'keep_org' flag to true then the original 32-bit
 *  bitmap will also be kept besides the 8-bit dithered one. You can
 *  retrieve the original bitmap through the OrginalBitmap() function.
 *  It will be 0 if there was no dithering.
 *
 *
 *  The DisplayProgressBar() function can be used to display a progress
 *  window during a datatypes translation. You should call it just before
 *  calling BTranslatorRoster::Translate(). The progress window will appear
 *  as soon as data starts being read or written to or from the stream. You
 *  specify a BPoint for where the progress window should appear, and a
 *  character string to be used as the window title. The window will
 *  disappear when the object is destructed.
 *
 *
 *  The ForceErrors() function can be used to try and abort the
 *  translation process by causing the stream to return (fake) errors.
 *  This doesn't work with all translators.
 *
 *  You can use ErrorsForced() to check if ForceErrors() was active in
 *  the stream. This will be the case when the user has pressed the
 *  'Stop' button in the progress window. This allows you to check
 *  whether BTranslatorRoster::Translate() returned an error because it
 *  was forced or because is was real.
 */


#ifndef	PROGRESSIVE_BITMAP_STREAM_H
#define	PROGRESSIVE_BITMAP_STREAM_H


#include <Point.h>
#include <TranslatorFormats.h>
#include <DataIO.h>


class BBitmap;
class BInvoker;
class BarWindow;


class ProgressiveBitmapStream : public BPositionIO
{
public:
						// Constructor for when used as source
						ProgressiveBitmapStream(BBitmap *bitmap);

						// Constructor for when used as destination
						ProgressiveBitmapStream(
							BInvoker *	bitmap_created = 0,
							BInvoker *	rect_updated = 0,
							bool 		dither = false,
							bool 		keep_org = false);
						~ProgressiveBitmapStream();
	ssize_t				Write(const void *buffer, size_t size);
	ssize_t				ReadAt(off_t pos, void *buffer, size_t size);
	ssize_t				WriteAt(off_t pos, const void *buffer, size_t size);
	off_t				Seek(off_t position, uint32 seek_mode);
	off_t				Position() const;
	status_t			SetSize(off_t numBytes);
  	void				SetDispose(bool primary = true, bool original = true);
	BBitmap *			Bitmap() const;
	BBitmap *			OriginalBitmap();
	void				ForceErrors();
	bool				ErrorsForced() const;
	void				DisplayProgressBar(const char *window_title);

private:
	bool				fWriteOnly;
	bool				fDisposePrim;
	bool				fDisposeOrg;
	TranslatorBitmap	fHeader;
	BBitmap *			fBitmap;
	BBitmap *			fOrgBitmap;
	bool				fBar;
	BInvoker *			fBitmapInvoker;
	BInvoker *			fRectInvoker;
	off_t				fPosition;
	off_t				fSize;
	float				fBytesPerPixel;
	void				SetExtras();
	bool				fForceErrors;
	bool				fDither;
	bool				fKeepOrg;
	void				Dither(const uchar *src, int32 place, int32 n_bytes);
	BPoint				fBarLeftTop;
	char				fBarTitle[81];
	BarWindow *			fBarWin;
	bool				fSeeked;
	float *				fDeltaBuffer;
	const rgb_color *	palette;
	const uchar *		index_list;
	int32				fHeaderWritten;
	uint32				fDataSize;
};


#endif  // PROGRESSIVE_BITMAP_STREAM_H
