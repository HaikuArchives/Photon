#if !defined (__GIMP_STUB__)
#define __GIMP_STUB__

#include <InterfaceKit.h>
#include <Debug.h>
#include <vector.h>
#include <stdio.h>
#include <stdlib.h>
#include "gtk/gtk.h"
#include "libgimp/gimp.h"
#include "Benaphore.h"

class TFilter;

#define VERSION_NUMBER	0x100

extern "C" {
typedef void (*GMenuInstaller) (const char *menu, const char *item, BMessage *msg);
};

struct InitInfo {
	int32			fVersion;
	BApplication	*app;
	GMenuInstaller	fInstaller;
};

class TBitmap {
public:
					TBitmap (void) { fBitmap = NULL; }
					TBitmap (const BRect &bounds, int32 space, bool acceptsViews = false) { fBitmap = new BBitmap (bounds, color_space(space), acceptsViews); }
virtual				~TBitmap () { delete fBitmap; }
operator BBitmap*	() { return fBitmap; }
		BRect		Bounds (void) const { return fBitmap->Bounds (); }
		int32		BytesPerRow (void) const { return fBitmap->BytesPerRow (); }
		void		*Bits (void) const { return fBitmap->Bits (); }
		int32		BitsLength() const { return fBitmap->BitsLength (); }
		BBitmap		*Bitmap (void) { return fBitmap; }
		const BBitmap *Bitmap (void) const { return fBitmap; }
		void		ReadWriteLock (void) { }
		void		ReadWriteUnlock (void) { }
		void		Copy (const TBitmap &src);
protected:
	BBitmap					*fBitmap;
};

class FakeTBitmap : public TBitmap {
public:
					FakeTBitmap (BBitmap *bm) { fBitmap = bm; }
					~FakeTBitmap () { fBitmap = NULL; }
};
/*
typedef struct TFilterParams {
	void			*fDocument;
	const TBitmap	*fSrc;
	TBitmap			*fDst;
	int32			fCurLayer;
	void			*fCurSel;
	BRect			fBounds;
	TFilter			*fFilter;
	int32			fForeColor, fBackColor;
}TFilterParams;
*/
#include "BaseFilter.h"
#include "FilterViews.h"

#if defined(DEBUG)
#define VERIFY(expression) if (!(expression)) ASSERT(false)
#else
#define VERIFY(expression) expression
#endif

#define FILTER_NAME_QUOTED(name) #name
class BPath;

extern BApplication *gApp;

//	Exported function for the conatainer. the container must call InstallGimp, passing in path the path to this
//	plug-in file
extern "C" void InstallGimp (BPath *path, InitInfo *info);
extern BPath	*filterFile;		//	The path is kept in global to have the resources work.

class Benaphore;

#define SQR(x) (x*x)

/*
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
*/

//--------------------------------------------------------------------
//	The following set of codes is used for emulating GIMP tiles.
//	We create a TileManager, it will be destroyed when the filter is
//	done. If the filter does not call any tile functions, this
//	manager will not be created.
//--------------------------------------------------------------------
struct GIMPTileData {
	int32	fRefs;             // reference count
	int32	fAllocLength;       // length of allocation
	// char data[nAllocLength]

	void	*Data() { return (char*)(this+1); }
	void	Dispose () { ASSERT (fRefs >= 0); if (atomic_add (&fRefs, -1) == 0) delete [] this; }
};

class GIMPTile {
public:
				GIMPTile (void);
				GIMPTile (const GIMPTile &src);
				~GIMPTile ();
	void		Clear (bool forced);
	void		FromBitmap (void *src, int32 rowBytes, int32 width, int32 height);
	void		ToBitmap (void *src, int32 rowBytes, int32 width, int32 height) const;
	bool		Empty (void) { return fData == NULL; }
	int32		BytesPerRow (void) { return fRowBytes; }
	void		*DataAndLock (void) { AddUser (); return fData; }
	void		UnlockData (void) { RemoveUser (); }
	int32		Width (void) { return fWidth; }
	int32		Height (void) { return fHeight;	}
	int32		BitsLength (void);
	void		SetDirty (bool dirty) { fDirty = dirty; }
	bool		Dirty (void) { return fDirty; }
protected:
	void		AddUser (void) { if (fData) atomic_add(&(((GIMPTileData *)fData) - 1)->fRefs, 1); }
	void		RemoveUser (void) { if (fData) (((GIMPTileData *)fData) - 1)->Dispose (); }
	void		Allocate (void);

	GTile		fGIMPTile;
	void		*fData;
	int16		fWidth, fHeight, fRowBytes, fDirty;
};

#if __PPC
typedef vector<GIMPTile, allocator<GIMPTile> > GIMPTileVector;
#else
typedef vector<GIMPTile> GIMPTileVector;
#endif

class GIMPTileManager {
public:
				GIMPTileManager (TBitmap *bm);
				~GIMPTileManager ();
	void		ToTBitmap (void) const;
	void		ToBitmap (BBitmap *bm) const;
	int32		Width (void) const { return fWidth; }
	int32		Height (void) const { return fHeight; }
	int32		TileSize (void) const { return fTileSize; }
	GIMPTile	*GetTile (int32 x, int32 y) { ASSERT (x < fXTiles && y < fYTiles); return GetTile (y * fXTiles + x); }
	GIMPTile	*GetTile (int32 id);
	void		SetTile (int32 id, GIMPTile *zTile);
	int32		CountTiles (void) { return fYTiles * fXTiles; }
protected:
	void		BuildVector (void);
	void		RemoveTiles (void);
	void		TileInfo (int32 xTile, int32 yTile, const TBitmap *bm, void **bitsPos, int32 *width, int32 *height) const;
	void		TileInfo (int32 xTile, int32 yTile, const BBitmap *bm, void **bitsPos, int32 *width, int32 *height) const;

	int32			fWidth, fHeight, fXTiles, fYTiles, fTileSize;
	GIMPTileVector	fTiles;
	TBitmap			*fBitmap;
};

class FILTER_NAME : public TFilter {
public:
					FILTER_NAME (void);
					FILTER_NAME (BMessage *archive);
virtual				~FILTER_NAME ();
virtual	BMessage	*ApplyFilter (TFilterParams *parms);
virtual	status_t	Archive (BMessage *into, bool deep = true) const;
static	BArchivable	*Instantiate (BMessage *archive);
		void		Install (char *name, char *blurb, char *help, char *author, char *copyright, char *date, char *menu_path, char *image_types, int type, int nparams, int nreturn_vals, struct GParamDef *params, struct GParamDef *return_vals);
		bool		Inited (void) { return fInited; }
virtual	void		GetData (gchar *id, gpointer data) { memcpy (data, fData, fDataSize); }
virtual	void		SetData (gchar *id, gpointer data, guint32 length) { memcpy (fData, data, length); }
		void		PrepareData (void);
		void		DisposeData (void);
		void		SetValue (void *value);
		void		DoIt (TFilterParams *parms, const void *values);
		void		DoAbout (void);
		GTile		*GetTile (TFilterParams *parms, bool shadow, int32 row, int32 col);
		void		Tiles2Bitmap (void);
		bool		Tiled (bool shadow) { return (shadow ? fDstTiles : fSrcTiles) != NULL; }
protected:
	char				*fName, *fBlurb, *fHelp, *fAuthor, *fCopyright, *fDate, *fMenuPath;
	int32				fParamsCount, fReturnCounts;
	struct GParamDef	*fParams, *fReturnVals;
	void				*fData;
	int32				fDataSize;
	Benaphore			*fLock;
	bool				fInited, fChanged;
	GIMPTileManager		*fSrcTiles, *fDstTiles;
};

extern void FilterUpdate (TFilterParams *parms);
class TDlogProgressView;

extern struct GPlugInInfo PLUG_IN_INFO;
extern FILTER_NAME	*gCurFilter;
extern int32		gBackColor, gForeColor;
extern TDlogProgressView	*gProgressView;
rgb_color ColInt32ToRGB (int32 col);
#endif	/*	__GIMP_STUB__	*/
