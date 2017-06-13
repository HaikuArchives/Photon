// class DThumbnail
//
// Copyright 1997,1998 Thorsten Seitz
//
// License:
// 		This class may be freely used in your programs.
//
// Purpose:
//		create Thumbnails/Icons from BBitmaps, 
//		read Thumbnails/Icons from attributes,
//		write Thumbnails/Icons to attributes

#ifndef D_THUMBNAILX_H
#define D_THUMBNAILX_H

#include <memory>
#include <storage/Node.h>
#include <support/Debug.h>
#include <support/Locker.h>

class BBitmap;


// class Thumbnail

class
_EXPORT
DThumbnail
{
public:
	static const char * cThumbnailAttributeName;
	static const char * cIconAttributeName;
	static const char * cMiniIconAttributeName;
	static const char * cWidthAttributeName;
	static const char * cHeightAttributeName;
	static const int32 	cThumbnailSize = 96;;
	
								// default ctor
								DThumbnail(void);
								
								// ctor
								DThumbnail(
									const BBitmap * original,
									bool dither = true);
							
								// dtor
								~DThumbnail(void);
						
								// check if there were errors
			status_t			InitCheck(void) const { return fInit; }
			
								// get a pointer (BBitmaps are owned by the
								// DThumbnail)
								// these are NULL in case of an error

	//CONST after Bitmap(void) -- Icon(void  --- MiniIcon(void) removed
	//by SLEMA to compile under PPC
	
	#ifdef __POWERPC__
	
	const 	BBitmap *			Bitmap(void);
	const 	BBitmap *			Icon(void);
	const 	BBitmap *			MiniIcon(void);
	
	#else
	
	const 	BBitmap *			Bitmap(void) const;
	const 	BBitmap *			Icon(void) const ;
	const 	BBitmap *			MiniIcon(void) const;
	
	#endif
	
	
								// get a copy
								// the auto_ptr's contain NULL pointers
								// in case of an error
	auto_ptr<BBitmap>			GetBitmap(void);
	auto_ptr<BBitmap>			GetIcon(void);
	auto_ptr<BBitmap>			GetMiniIcon(void);
	
								// write to disk
			status_t			WriteThumbnailAttribute(BNode * node);
			status_t			WriteIconAttribute(BNode * node);
			status_t			WriteMiniIconAttribute(BNode * node);
			status_t			WriteResolutionAttributes(BNode * node);
			
								// read from disk
			status_t			ReadFromAttributes(
									BNode * node);
								
								// set from BBitmap
			status_t			SetTo(
									const BBitmap * original, 
									bool dither = true);
	
								// create a thumbnail of a certain size
								// from an original BBitmap
	static	auto_ptr<BBitmap>	MakeThumbnail(
									const BBitmap * original,
									int32 size,
									bool dither = true);

private:
	auto_ptr<BBitmap>			CopyBitmap(const BBitmap * source);
	
			void				Clear(void);
			
			status_t			WriteSingleIconAttribute(
									BNode * node,
									const char * attrName,
									const BBitmap * source,
									float size,
									uint32 attrType);
			
			status_t			ReadSingleIconFromAttribute(
									BNode * node,
									const char * attrName,
									const BBitmap * target,
									float size,
									uint32 attrType);
			
								// read from disk
			status_t			DoReadFromAttributes(
									BNode * node);
								
								// set from BBitmap
			status_t			DoSetTo(
									const BBitmap * original, 
									bool dither = true);
	
	auto_ptr<BBitmap>			fThumbnail;
	auto_ptr<BBitmap>			fIcon;
	auto_ptr<BBitmap>			fMiniIcon;
			
			int32				fOriginalWidth;
			int32				fOriginalHeight;
			
			status_t			fInit;
			
	mutable	BLocker				fLock;
			
private:	// not implemented on purpose
								DThumbnail(const DThumbnail &);
	const DThumbnail &			operator = (const DThumbnail &);
};

 


#endif
