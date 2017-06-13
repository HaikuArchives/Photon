/*
 *  BBitmapAccessor.h
 *  Release 0.6.0 (Aug 23th, 1998)
 *
 *  A class for accessing BBitmaps
 *  May be used by applications (NOT for add-ons)
 *  Written by Edmund Vermeulen <edmundv@xs4all.nl>
 *  Public domain
 */


/* Note: Define _IMPEXP_IMAGEMANIP to nothing before including this header
   file when using weak linkage, i.e. with 'ImportImageManip.cpp' */


#ifndef _BBITMAP_ACCESSOR_H_
#define _BBITMAP_ACCESSOR_H_


#include "BitmapAccessor.h"


class BBitmap;
class BInvoker;


class _IMPEXP_IMAGEMANIP BBitmapAccessor : public BitmapAccessor
{
public:
	BBitmapAccessor(BBitmap *bitmap = NULL, const BRect *section = NULL);
	virtual ~BBitmapAccessor();
	virtual bool IsValid();
	virtual bool CreateBitmap(BRect bounds, color_space space);
	virtual BRect Bounds();
	virtual color_space ColorSpace();
	virtual void *AccessBits(BRect area, int32 *rowBytes);
	virtual void BitsNotChanged();

	// Get pointer to the BBitmap
	virtual BBitmap *Bitmap() const;

	// Get/set if the BBitmap should be deleted at destruction time
	virtual bool Dispose() const;
	virtual void SetDispose(bool flag);

	// Set invokers for automatically sending messages when the bitmap
	// is created and/or when it is updated
	virtual void SetInvokers(BInvoker *bitmapCreated, BInvoker *bitmapUpdated);

private:
	virtual void _ReservedBBitmapAccessor1();
	virtual void _ReservedBBitmapAccessor2();
	virtual void _ReservedBBitmapAccessor3();

	BBitmap *mBitmap;
	BRect mBounds;
	bool mDispose;
	BRect mLastArea;
	bool mLastAreaChanged;
	BInvoker *mCreateInvoker;
	BInvoker *mUpdateInvoker;

	int32 _reserved[3];
};



/* Create a new BBitmapAccessor object; mimics the contructor */
extern "C" _IMPEXP_IMAGEMANIP BBitmapAccessor *
Image_CreateBBitmapAccessor(
	BBitmap *bitmap = NULL,
	const BRect *section = NULL);


#endif  // _BBITMAP_ACCESSOR_H_
