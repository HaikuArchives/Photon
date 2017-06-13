/*
 *  ImageManip.h
 *  Release 0.6.1 (Jan 3rd, 1999)
 *
 *  Image manipulation library
 *  Written by Edmund Vermeulen <edmundv@xs4all.nl>
 *  Public domain
 */


/* Note: Define _IMPEXP_IMAGEMANIP to nothing before including this header
   file when using weak linkage, i.e. with 'ImportImageManip.cpp' */


#ifndef _IMAGE_MANIP_H_
#define _IMAGE_MANIP_H_


#include <SupportDefs.h>


#ifndef _IMPEXP_IMAGEMANIP
#ifdef _BUILDING_imagemanip
#define _IMPEXP_IMAGEMANIP __declspec(dllexport)
#else
#define _IMPEXP_IMAGEMANIP __declspec(dllimport)
#endif  // _BUILDING_imagemanip
#endif  // _IMPEXP_IMAGEMANIP


class BMessage;
class BView;
class BitmapAccessor;


typedef int32 image_addon_id;


#define IMAGE_LIB_CUR_VERSION 61
#define IMAGE_LIB_MIN_VERSION 60


/* The functions in the library */
extern "C"
{
	/* Get the current version of the library and the minimum
	   version with which it is still compatible */
	_IMPEXP_IMAGEMANIP const char *
	Image_Version(
		int32 *curVersion,             /* will receive the current version */
		int32 *minVersion);            /* will receive the minimum version */

	/* Initialize the library before usage */
	_IMPEXP_IMAGEMANIP status_t
	Image_Init(
		const char *loadPath);         /* NULL for the default */

	/* Shutdown the library after usage */
	_IMPEXP_IMAGEMANIP status_t
	Image_Shutdown();

	/* Get all image manipulators that support a given source bitmap
	   and io extension */
	_IMPEXP_IMAGEMANIP status_t
	Image_GetManipulators(
		BitmapAccessor *sourceBitmap,  /* NULL to get all manipulators */
		BMessage *ioExtension,         /* can be NULL */
		image_addon_id **outList,      /* call delete[] on it when done */
		int32 *outCount);              /* will receive amount in list */

	/* Get all image converters that support a given source bitmap
	   and io extension */
	_IMPEXP_IMAGEMANIP status_t
	Image_GetConverters(
		BitmapAccessor *sourceBitmap,  /* NULL to get all converters */
		BMessage *ioExtension,         /* can be NULL */
		image_addon_id **outList,      /* call delete[] on it when done */
		int32 *outCount);              /* will receive amount in list */

	/* Get info on an image manipulator or image converter add-on */
	_IMPEXP_IMAGEMANIP status_t
	Image_GetAddonInfo(
		image_addon_id imageAddon,
		const char **addonName,        /* will receive pointer to the name */
		const char **addonInfo,        /* will receive pointer to the info */
		const char **addonCategory,    /* will receive pointer to the category */
		int32 *addonVersion);          /* will receive the version */

	/* Let an add-on manipulate a bitmap */
	_IMPEXP_IMAGEMANIP status_t
	Image_Manipulate(
		image_addon_id imageManipulator,
		BitmapAccessor *sourceBitmap,
		BMessage *ioExtension,         /* can be NULL */
		bool checkOnly = false);

	/* Let an add-on convert a bitmap to another bitmap */
	_IMPEXP_IMAGEMANIP status_t
	Image_Convert(
		image_addon_id imageConverter,
		BitmapAccessor *sourceBitmap,
		BitmapAccessor *destBitmap,    /* will be called CreateBitmap() on */
		BMessage *ioExtension,         /* can be NULL */
		bool checkOnly = false);

	/* Let an add-on make a BView that allows the user to configure it */
	_IMPEXP_IMAGEMANIP status_t
	Image_MakeConfigurationView(
		image_addon_id imageAddon,
		BMessage *ioExtension,         /* can be NULL */
		BView **configView);           /* will receive pointer to the new BView */

	/* Get configuration and capabilities from an add-on */
	_IMPEXP_IMAGEMANIP status_t
	Image_GetConfigurationMessage(
		image_addon_id imageAddon,
		BMessage *ioExtension,         /* message to add config info to */
		BMessage *ioCapability);       /* message to add capability info to */
}


#endif  // _IMAGE_MANIP_H_
