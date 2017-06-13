/*
 *  BitmapAccessor.cpp
 *  Release 0.6.0 (Aug 23th, 1998)
 *
 *  Proxy class for accessing bitmaps through libimagemanip.so
 *  Written by Edmund Vermeulen <edmundv@xs4all.nl>
 *  Public domain
 */


#define _BUILDING_imagemanip


#include "BitmapAccessor.h"


BitmapAccessor::~BitmapAccessor() {}


float
BitmapAccessor::BytesPerPixel()
{
	switch (ColorSpace())
	{
		case B_RGB32:
		case B_RGB32_BIG:
		case B_RGBA32:
		case B_RGBA32_BIG:
			return 4.0;

		case B_RGB16:
		case B_RGB16_BIG:
		case B_RGB15:
		case B_RGB15_BIG:
		case B_RGBA15:
		case B_RGBA15_BIG:
			return 2.0;

		case B_GRAY8:
		case B_CMAP8:
			return 1.0;

		case B_GRAY1:
			return 1.0 / 8.0;

		default:  // unknown
			return 0.0;
	}
}

void BitmapAccessor::_ReservedBitmapAccessor1() {}
void BitmapAccessor::_ReservedBitmapAccessor2() {}
void BitmapAccessor::_ReservedBitmapAccessor3() {}
