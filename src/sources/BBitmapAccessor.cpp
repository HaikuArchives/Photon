/*
 *  BBitmapAccessor.cpp
 *  Release 0.6.0 (Aug 23th, 1998)
 *
 *  A class for accessing BBitmaps
 *  May be used by applications (NOT for add-ons)
 *  Written by Edmund Vermeulen <edmundv@xs4all.nl>
 *  Public domain
 */


#define _BUILDING_imagemanip


#include <Bitmap.h>
#include <Invoker.h>
#include "BitmapAccessor.h"
#include "BBitmapAccessor.h"


BBitmapAccessor::BBitmapAccessor(BBitmap *bitmap, const BRect *section)
	: BitmapAccessor()
	, mBitmap(bitmap)
	, mDispose(true)
	, mLastAreaChanged(false)
	, mCreateInvoker(NULL)
	, mUpdateInvoker(NULL)
{
	if (bitmap && section)
	{
		mBounds = *section & bitmap->Bounds();
	}
	else
	{
		if (bitmap)
			mBounds = bitmap->Bounds();
	}
}


BBitmapAccessor::~BBitmapAccessor()
{
	if (mUpdateInvoker && mLastAreaChanged)
	{
		BMessage msg(*mUpdateInvoker->Message());
		msg.AddRect("rect", mLastArea);
		mUpdateInvoker->Invoke(&msg);
	}

	delete mCreateInvoker;
	delete mUpdateInvoker;
	if (mDispose)
		delete mBitmap;
}


bool
BBitmapAccessor::IsValid()
{
	return mBitmap && mBitmap->IsValid() && mBounds.IsValid();
}


bool
BBitmapAccessor::CreateBitmap(BRect bounds, color_space space)
{
	if (mDispose)
		delete mBitmap;
	mBitmap = new BBitmap(bounds, space);
	mBounds = mBitmap->Bounds();
	if (mCreateInvoker)
	{
		BMessage msg(*mCreateInvoker->Message());
		msg.AddPointer("bitmap", mBitmap);
		mCreateInvoker->Invoke(&msg);
	}
	return IsValid();
}


BRect
BBitmapAccessor::Bounds()
{
	return mBounds;
}


color_space
BBitmapAccessor::ColorSpace()
{
	return mBitmap->ColorSpace();
}


void *
BBitmapAccessor::AccessBits(BRect area, int32 *rowBytes)
{
	if (mUpdateInvoker && mLastAreaChanged && mLastArea.IsValid())
	{
		BMessage msg(*mUpdateInvoker->Message());
		msg.AddRect("rect", mLastArea);
		mUpdateInvoker->Invoke(&msg);
	}
	mLastArea = area;
	mLastAreaChanged = true;

	*rowBytes = mBitmap->BytesPerRow();

	return static_cast<char *>(mBitmap->Bits()) +
		int(area.top) * *rowBytes +
		int(floor(area.left) * BytesPerPixel());
}


void
BBitmapAccessor::BitsNotChanged()
{
	mLastAreaChanged = false;
}


BBitmap *
BBitmapAccessor::Bitmap() const
{
	return mBitmap;
}


bool
BBitmapAccessor::Dispose() const
{
	return mDispose;
}


void
BBitmapAccessor::SetDispose(bool flag)
{
	mDispose = flag;
}


void
BBitmapAccessor::SetInvokers(BInvoker *bitmapCreated, BInvoker *bitmapUpdated)
{
	delete mCreateInvoker;
	mCreateInvoker = bitmapCreated;
	delete mUpdateInvoker;
	mUpdateInvoker = bitmapUpdated;
}


// For possible future expansion
void BBitmapAccessor::_ReservedBBitmapAccessor1() {}
void BBitmapAccessor::_ReservedBBitmapAccessor2() {}
void BBitmapAccessor::_ReservedBBitmapAccessor3() {}


/* Create a new BBitmapAccessor object; mimics the contructor */
BBitmapAccessor *
Image_CreateBBitmapAccessor(
	BBitmap *bitmap,
	const BRect *section)
{
	return new BBitmapAccessor(bitmap, section);
}
