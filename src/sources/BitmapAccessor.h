/*
 *  BitmapAccessor.h
 *  Release 0.6.0 (Aug 23th, 1998)
 *
 *  Proxy class for accessing bitmaps through libimagemanip.so
 *  Written by Edmund Vermeulen <edmundv@xs4all.nl>
 *  Public domain
 */


/* Note: Define _IMPEXP_IMAGEMANIP to nothing before including this header
   file when using weak linkage, i.e. with 'ImportImageManip.cpp' */


#ifndef _BITMAP_ACCESSOR_H_
#define _BITMAP_ACCESSOR_H_


#include <Rect.h>
#include <GraphicsDefs.h>


#ifndef _IMPEXP_IMAGEMANIP
#ifdef _BUILDING_imagemanip
#define _IMPEXP_IMAGEMANIP __declspec(dllexport)
#else
#define _IMPEXP_IMAGEMANIP __declspec(dllimport)
#endif  // _BUILDING_imagemanip
#endif  // _IMPEXP_IMAGEMANIP


// This is the abstract base class that is used for the interface
class _IMPEXP_IMAGEMANIP BitmapAccessor
{
public:
	virtual ~BitmapAccessor();
	virtual bool IsValid() = 0;
	virtual bool CreateBitmap(BRect bounds, color_space space) = 0;
	virtual BRect Bounds() = 0;
	virtual color_space ColorSpace() = 0;
	virtual float BytesPerPixel();

	// With the AccessBits() function you gain access to the raw bytes of a
	// certain area of the bitmap. The number of bytes to skip to get to the
	// next row of pixels is returned in rowBytes. The pointer returned by
	// AccessBits() is only valid as long as no other calls to AccessBits()
	// (or to the destructor) have been made. For efficiency reasons you
	// should keep the requested area small.
	virtual void *AccessBits(BRect area, int32 *rowBytes) = 0;

	// If you haven't changed the bits accessed through AccessBits() then
	// you should call BitsNotChanged() before accessing a new area.
	// Unchanged areas don't need to be updated, which is more efficient.
	virtual void BitsNotChanged() = 0;

private:
	// For possible future expansion
	virtual void _ReservedBitmapAccessor1();
	virtual void _ReservedBitmapAccessor2();
	virtual void _ReservedBitmapAccessor3();
	int32 _reserved[3];
};


#endif  // _BITMAP_ACCESSOR_H_
