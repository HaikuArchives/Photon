#include "GimpStub.h"

#include <stdio.h>
#include <stdlib.h>
#include "gtk/gtk.h"
#include "libgimp/gimp.h"
#include "libgimp/gimpui.h"
#include "libgimp/gimpprotocol.h"
#include <Path.h>
#include <Application.h>
#include "IEResourceHandler.h"

TDlogProgressView	*gProgressView = NULL;
FILTER_NAME	*gCurFilter = NULL;
BApplication *gApp = NULL;

#define DEFAULT_TILE_SIZE	64	/*	To do as in GIMP */
//----------------------------------------------------------------------------------------
//
//	My own stuff....
//
//----------------------------------------------------------------------------------------
extern GPlugInInfo PLUG_IN_INFO;
extern BMessage *ApplyFilter (TFilterParams *parms);

BPath	*filterFile = NULL;
IEResourceHandler *resourcehandler;
static InitInfo	*gInfo = NULL;

void InstallGimp (BPath *path, InitInfo *info) {
	if (info->fVersion < VERSION_NUMBER) return;
	gInfo = info;
	gApp = info->app;
	filterFile = new BPath (*path);
	entry_ref	ref;
	VERIFY (B_OK == get_ref_for_path (path->Path (), &ref));
	
	resourcehandler= new IEResourceHandler (ref, gApp);

	if(resourcehandler->Error() == B_NO_ERROR){
		//	Let's check to see if we have the two main procs!!!
		ASSERT (PLUG_IN_INFO.query_proc != NULL && PLUG_IN_INFO.run_proc != NULL);
		if (PLUG_IN_INFO.init_proc) (*PLUG_IN_INFO.init_proc)();	//	Call init if he wants it!!!
		(*PLUG_IN_INFO.query_proc) ();
	}
}

rgb_color ColInt32ToRGB (int32 col) {
	rgb_color	rgb;

	rgb.blue = col;
	col >>= 8;
	rgb.green = col;
	col >>= 8;
	rgb.red = col;
	return rgb;
}

//----------------------------------------------------------------------------------------
//
//	GIMP stuff....
//
//----------------------------------------------------------------------------------------
#define GET_FILTER_PARMS(xx) ((TFilterParams *)xx)

GDrawable *gimp_drawable_get (gint32 drawable_ID) {
	GDrawable		*result = (GDrawable *) new char[sizeof(GDrawable)];

	if (result) {
		TFilterParams	*parms = (TFilterParams *) drawable_ID;
		result->width = parms->fBounds.IntegerWidth () + 1;
		result->height = parms->fBounds.IntegerHeight () + 1;
		result->bpp = 3;
		result->ntile_rows = result->ntile_cols = 1;
		result->tiles = result->shadow_tiles = NULL;
		result->id = drawable_ID;
	}
	return result;
}

void gimp_drawable_detach (GDrawable *drawable) {
	delete [] (char *) drawable;
}

gint gimp_drawable_color (gint32 drawable_ID) {
	return 1;
}

gint gimp_drawable_gray (gint32 drawable_ID) {
	return 0;
}

gint gimp_drawable_has_alpha (gint32 drawable_ID) {
	return 0;
}

gint gimp_drawable_indexed (gint32 drawable_ID) {
	return 0;
}

gint gimp_drawable_mask_bounds (gint32 drawable_ID, gint *x1, gint *y1, gint *x2, gint *y2) {
	TFilterParams	*parms = GET_FILTER_PARMS (drawable_ID);

	*x1 = gint(parms->fBounds.left); *y1 = gint(parms->fBounds.top);
	*x2 = 1 + gint(parms->fBounds.right); *y2 = 1 + gint(parms->fBounds.bottom);
	return 0;
}

guint gimp_drawable_width (gint32 drawable_ID) {
	TFilterParams	*parms = (TFilterParams *) drawable_ID;
	return (parms->fSrc->Bounds ().IntegerWidth () + 1);
}

guint gimp_drawable_height (gint32 drawable_ID) {
	TFilterParams	*parms = (TFilterParams *) drawable_ID;
	return (parms->fSrc->Bounds ().IntegerHeight () + 1);
}

guint gimp_drawable_bpp (gint32 drawable_ID) { return 3; }

void gimp_drawable_flush (GDrawable *drawable) {
}

void gimp_drawable_merge_shadow (gint32 drawable_ID, gint undoable) {
	TFilterParams	*parms = (TFilterParams *) drawable_ID;
	((FILTER_NAME *)parms->fFilter)->Tiles2Bitmap ();
}

GDrawableType gimp_drawable_type (gint32 drawable_ID) {
	return RGB_IMAGE;
}

void gimp_drawable_update (gint32 drawable_ID, gint x, gint y, guint width, guint height) {
	TFilterParams	*parms = (TFilterParams *) drawable_ID;
	if (parms->fDocument) {	//	Else, this is a min view
		FilterUpdate (parms);
	}
}



guint gimp_layer_bpp (gint32 layer_ID) { return 3; }

char *gProgressMessage = NULL;
void gimp_progress_init (char *message) {
	gProgressMessage = message;
	if (gProgressView) gProgressView->Start (message);
}

void gimp_progress_update (gdouble percentage) {
	if (gProgressView) gProgressView->Update (percentage);
	else {
		BMessage	msg('PrgU');
		VERIFY (B_OK == msg.AddString ("Title", gProgressMessage));
		VERIFY (B_OK == msg.AddDouble ("be:value", percentage));
		ASSERT (gApp);
		gApp->PostMessage (&msg);
	}
}

void gimp_tile_ref (GTile *tile) {
	
}

void gimp_tile_ref_zero (GTile *tile) {
}

void gimp_tile_unref (GTile *tile, int dirty) {
	if (dirty) ((GIMPTile *)tile)->SetDirty (true);
}

void gimp_tile_cache_size (gulong  kilobytes) { }
void  gimp_tile_cache_ntiles (gulong  ntiles) { }
guint gimp_tile_width (void) { return DEFAULT_TILE_SIZE; }	//	Is it such a good idea????
guint gimp_tile_height (void) { return DEFAULT_TILE_SIZE; }

void gimp_displays_flush (void) {
}

gpointer g_malloc (gulong size) { return gpointer (malloc(size)); }
gpointer g_malloc0 (gulong size) { return gpointer (malloc(size)); }
gpointer g_realloc (gpointer  mem, gulong size) { return gpointer(realloc (mem, size)); }
void g_free (gpointer mem) { free (mem); }

int32	gForeColor = 0, gBackColor = 0xFFFFFF;

void gimp_palette_get_background (guchar *red, guchar *green, guchar *blue) {
	rgb_color	col;
	col = ColInt32ToRGB (gBackColor); *red = col.red; *green = col.green; *blue = col.blue;
}

void gimp_palette_get_foreground (guchar *red, guchar *green, guchar *blue) {
	rgb_color	col;
	col = ColInt32ToRGB (gForeColor); *red = col.red; *green = col.green; *blue = col.blue;
}

void g_print   (const gchar *format, ...) {
}
//------------------------------------------------------------------------------------------------
//
//	gimp.c routines.
//
//------------------------------------------------------------------------------------------------
int gimp_main (int argc, char *argv[]) {
	return 0;
}

FILTER_NAME::FILTER_NAME (void) {
	ASSERT (gCurFilter == NULL); gCurFilter = this;
	fInited = false; fData = NULL; PrepareData ();
	fName = fBlurb = fHelp = fAuthor = fCopyright = fMenuPath = fDate = NULL;
	fParams = fReturnVals = NULL;
	fParamsCount = fReturnCounts = 0;
	fLock = NULL; fSrcTiles = fDstTiles = NULL;
}

FILTER_NAME::FILTER_NAME (BMessage *archive) {
	status_t	status;

	ASSERT (gCurFilter == NULL); gCurFilter = this;
	fSrcTiles = fDstTiles = NULL;
	fData = NULL; PrepareData (); fInited = false;
	do {
		VERIFY (B_OK == (status = archive->FindPointer ("Name", (void **)&fName))); if (status != B_OK) break;
		VERIFY (B_OK == (status = archive->FindPointer ("Blurb", (void **)&fBlurb))); if (status != B_OK) break;
		VERIFY (B_OK == (status = archive->FindPointer ("Help", (void **)&fHelp))); if (status != B_OK) break;
		VERIFY (B_OK == (status = archive->FindPointer ("Author", (void **)&fAuthor))); if (status != B_OK) break;
		VERIFY (B_OK == (status = archive->FindPointer ("Copyright", (void **)&fCopyright))); if (status != B_OK) break;
		VERIFY (B_OK == (status = archive->FindPointer ("Menu", (void **)&fMenuPath))); if (status != B_OK) break;
		VERIFY (B_OK == (status = archive->FindPointer ("Date", (void **)&fDate))); if (status != B_OK) break;
		VERIFY (B_OK == (status = archive->FindInt32 ("ParamsCount", &fParamsCount))); if (status != B_OK) break;
		VERIFY (B_OK == (status = archive->FindInt32 ("ReturnCounts", &fReturnCounts))); if (status != B_OK) break;
		VERIFY (B_OK == (status = archive->FindPointer ("Params", (void **)&fParams))); if (status != B_OK) break;
		VERIFY (B_OK == (status = archive->FindPointer ("ReturnVals", (void **)&fReturnVals))); if (status != B_OK) break;
		if (fData) {
			const void	*msgData;
			int32		msgSize;
			if (B_OK == (status = archive->FindData ("Data", 'Data', &msgData, &msgSize))) {
				if (msgSize <= fDataSize) {
					memcpy (fData, msgData, msgSize); fInited = true;
				} else {
					ASSERT (false);
					status = -1;
				}
			}
		} else {
			status = -1;
		}
	} while (false);
	fInited = status == B_OK;
	fLock = NULL;
}

FILTER_NAME::~FILTER_NAME () {
	ASSERT (gCurFilter == this); gCurFilter = NULL; DisposeData ();
	if (fLock) { delete fLock; fLock = NULL; }
	delete fSrcTiles; delete fDstTiles;
}

status_t FILTER_NAME::Archive (BMessage *into, bool deep) const {
	status_t	res;

	do {
		res = TFilter::Archive (into, deep); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddString ("class", FILTER_NAME_QUOTED(FILTER_NAME)))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddPointer ("Name", fName))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddPointer ("Blurb", fBlurb))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddPointer ("Help", fHelp))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddPointer ("Author", fAuthor))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddPointer ("Copyright", fCopyright))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddPointer ("Menu", fMenuPath))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddPointer ("Date", fDate))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddInt32 ("ParamsCount", fParamsCount))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddInt32 ("ReturnCounts", fReturnCounts))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddPointer ("Params", fParams))); if (res != B_OK) break;
		VERIFY (B_OK == (res = into->AddPointer ("ReturnVals", fReturnVals))); if (res != B_OK) break;
		if (fInited && fData) {
			VERIFY (B_OK == (res = into->AddData ("Data", 'Data', fData, fDataSize))); if (res != B_OK) break;
		}
	} while (false);
	return res;
}

BArchivable	*FILTER_NAME::Instantiate (BMessage *archive) {
	BArchivable	*result;

	if (validate_instantiation (archive, FILTER_NAME_QUOTED(FILTER_NAME))) {
		result = new FILTER_NAME (archive);
	} else {
		result = NULL;
	}
	return result;
}

void FILTER_NAME::Install (char *name, char *blurb, char *help, char *author, char *copyright, char *date, char *menu_path, char *image_types, int type, int nparams, int nreturn_vals, struct GParamDef *params, struct GParamDef *return_vals) {
	fName = name; fBlurb = blurb; fHelp = help; fAuthor = author; fCopyright = copyright; fMenuPath = menu_path; fDate = date;
	fParamsCount = nparams; fReturnCounts = nreturn_vals;
	fParams = params; fReturnVals = return_vals;
}

static void CreateAboutView (BView *bg, const char *title, const char *data, int32 top) {
	BView	*view;

	view = new BStringView (BRect (5, top, 10, top + 1), "", title); view->ResizeToPreferred (); bg->AddChild (view);
	view = new BStringView (BRect (10, top, 15, top + 1), "", data); view->ResizeToPreferred (); bg->AddChild (view);
}

static char *MenuItemParse (char *src, char *dst) {
	char	ch;

	do {
		ch = *src++;
		if (ch == 0) { *dst = 0; --src; break; }
		if (ch == '/') { *dst = 0; break; }
		*dst++ = ch;
	} while (true);
	return src;
}

void FILTER_NAME::DoAbout (void) {
	static char	aboutStr[] = "About ";
	char		title[256], *p, *menu_path;
	int32		rightMostTitle, rightMostData, startTop, curTop, nextTop;

	strcpy (title, aboutStr); p = title + strlen (title);
	menu_path = MenuItemParse (fMenuPath, p);	//	<image>
	menu_path = MenuItemParse (menu_path, p);	//	filters | image | 
	menu_path = MenuItemParse (menu_path, p); MenuItemParse (menu_path, p);
	BWindow	*aboutWindow = new BWindow (BRect (50, 50, 100, 100), title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_ZOOMABLE | B_NOT_RESIZABLE);
	BView	*bgView = new BView (BRect (0, 0, 1000, 1000), "BG", 0, 0);
	rgb_color	col; col.red = col.green = col.blue = 216;
	aboutWindow->AddChild (bgView); bgView->SetViewColor (col);
	BView	*view;
	int32	i;

	//	Create the author viewa.
	CreateAboutView (bgView, "Author:", fAuthor, 5); CreateAboutView (bgView, "Description:", fBlurb, 10);
	CreateAboutView (bgView, "Help:", fHelp, 15); CreateAboutView (bgView, "Copyright:", fCopyright, 20);
	CreateAboutView (bgView, "Date:", fDate, 25);

	startTop = curTop = 5; rightMostTitle = rightMostData = 0;
	//	First loop to replace the children in y, and find the x positions.
	for (i = 0; i < bgView->CountChildren (); ++i) {
		view = bgView->ChildAt (i);
		BRect	viewFrame = view->Frame ();
		if (viewFrame.top != curTop) {
			curTop = int32 (viewFrame.top); startTop = nextTop;
		}
		nextTop = int32 (viewFrame.bottom + startTop - viewFrame.top + 5);
		view->MoveTo (viewFrame.left, startTop);
		if (viewFrame.left == 5) {
			if (rightMostTitle < viewFrame.right) rightMostTitle = int32(viewFrame.right);
		} else {
			if (rightMostData < viewFrame.right) rightMostData = int32 (viewFrame.right);
		}
	}
	for (i = bgView->CountChildren (); --i >= 0; ) {
		view = bgView->ChildAt (i);
		BRect	viewFrame = view->Frame ();
		if (viewFrame.left == 10) view->MoveBy (rightMostTitle - 5, 0);
	}
	aboutWindow->ResizeTo (rightMostData + rightMostTitle - 5, nextTop);
	aboutWindow->Show ();
}

GTile *FILTER_NAME::GetTile (TFilterParams *parms, bool shadow, int32 row, int32 col) {
	GIMPTileManager	**tiles;
	TBitmap			*bm;
	if (shadow) {
		bm = parms->fDst; tiles = &fDstTiles;
	} else {
		bm = (TBitmap *)parms->fSrc; tiles = &fSrcTiles;
	}
	if (*tiles == NULL) 
		*tiles = new GIMPTileManager (bm);
	if (*tiles)
		return (GTile *)(*tiles)->GetTile (col, row);
	else
		return NULL;
}

void FILTER_NAME::Tiles2Bitmap (void) {
	if (fDstTiles) fDstTiles->ToTBitmap ();
}

void AddFilterPlugin (const char *menu, const char *item, BMessage *archive);

void gimp_install_procedure (char *name, char *blurb, char *help, char *author, char *copyright, char *date, char *menu_path, char *image_types, int type, int nparams, int nreturn_vals, GParamDef *params, GParamDef *return_vals) {
	char	position[256], menu[256], item[256];
	FILTER_NAME	filter;
	filter.Install (name, blurb, help, author, copyright, date, menu_path, image_types, type, nparams, nreturn_vals, params, return_vals);
	BMessage		msg;
	VERIFY (B_NO_ERROR == filter.Archive (&msg));
	//	Let's parse the menu_path stream
	menu_path = MenuItemParse (menu_path, position);	//	<image>
	menu_path = MenuItemParse (menu_path, position);	//	filters | image | 
	menu_path = MenuItemParse (menu_path, menu); MenuItemParse (menu_path, item);
	(*gInfo->fInstaller) (menu, item, &msg);
}
//------------------------------------------------------------------------------------------------
//
//	Pixel Regions routines.
//
//------------------------------------------------------------------------------------------------
#define BOUNDS(a,x,y)  ((a < x) ? x : ((a > y) ? y : a))


typedef struct _GPixelRgnHolder    GPixelRgnHolder;
typedef struct _GPixelRgnIterator  GPixelRgnIterator;

struct _GPixelRgnHolder {
	GPixelRgn	*pr;
	guchar		*original_data;
	int32		startx, starty, count;
};

struct _GPixelRgnIterator {
	GSList	*pixel_regions;
	int32	region_width, region_height, portion_width, portion_height, process_count;
};

static int32	gimp_get_portion_width		(GPixelRgnIterator *pri);
static int32	gimp_get_portion_height		(GPixelRgnIterator *pri);
static gpointer	gimp_pixel_rgns_configure	(GPixelRgnIterator *pri);
static void		gimp_pixel_rgn_configure	(GPixelRgnHolder   *prh, GPixelRgnIterator *pri);

void gimp_pixel_rgn_init (GPixelRgn *pr, GDrawable *drawable, int x, int y, int width, int height, int dirty, int shadow) {
	pr->data = NULL;
	pr->drawable = drawable;
	pr->bpp = drawable->bpp;
	pr->rowstride = pr->bpp * DEFAULT_TILE_SIZE;
	pr->x = x;
	pr->y = y;
	pr->w = width;
	pr->h = height;
	pr->dirty = dirty;
	pr->shadow = shadow;
}

void gimp_pixel_rgn_resize (GPixelRgn *pr, int x, int y, int width, int height) {
	if (pr->data != NULL)
		pr->data += ((y - pr->y) * pr->rowstride + (x - pr->x) * pr->bpp);
	
	pr->x = x;
	pr->y = y;
	pr->w = width;
	pr->h = height;
}

//----------------------//
//	The Get routines	//
//----------------------//

void gimp_pixel_rgn_get_pixel (GPixelRgn *pr, guchar *buf, int x, int y) {
	TFilterParams	*parms = (TFilterParams *) pr->drawable->id;
	if (((FILTER_NAME *)parms->fFilter)->Tiled (pr->shadow != 0)) {
		GTile	*tile;
		guchar	*tile_data;
		int		b;

		tile = gimp_drawable_get_tile2 (pr->drawable, pr->shadow, x, y);
		gimp_tile_ref (tile);
		tile_data = tile->data + tile->bpp * (tile->ewidth * (y % DEFAULT_TILE_SIZE) + (x % DEFAULT_TILE_SIZE));
		for (b = 0; b < int(tile->bpp); b++)
			*buf++ = *tile_data++;
		gimp_tile_unref (tile, FALSE);
	} else {
		const TBitmap	*bm = pr->shadow ? parms->fDst : parms->fSrc;
		BRect			bmBounds = bm->Bounds ();
		#if DEBUG
		ASSERT (x >= bmBounds.left && x <= bmBounds.right && y >= bmBounds.top && y <= bmBounds.bottom);
		#endif
		guchar	*src = (guchar *)bm->Bits ();
		src += 4 * (x - int32(bmBounds.left)) + bm->BytesPerRow () * (y - int32 (bmBounds.top));
		*buf++ = src[2]; *buf++ = src[1]; *buf++ = src[0];
	}
}

static void Transfer32To24 (guchar *src32, guchar *dst24, int32 cnt) {
	while (--cnt >= 0) {
		*dst24++ = src32[2]; *dst24++ = src32[1]; *dst24++ = src32[0]; src32 += 4;
	}
}

void gimp_pixel_rgn_get_row (GPixelRgn *pr, guchar *buf, int x, int y, int width) {
	TFilterParams	*parms = (TFilterParams *) pr->drawable->id;
	if (((FILTER_NAME *)parms->fFilter)->Tiled (pr->shadow != 0)) {
		GTile		*tile;
		guchar		*tile_data;
		int			bpp, inc, min, end, boundary;
		
		end = x + width;
		
		while (x < end) {
			tile = gimp_drawable_get_tile2 (pr->drawable, pr->shadow, x, y);
			gimp_tile_ref (tile);
			tile_data = tile->data + (int)tile->bpp * (int)(tile->ewidth * (int)(y % DEFAULT_TILE_SIZE) + (x % DEFAULT_TILE_SIZE));
			boundary = x + (tile->ewidth - (x % DEFAULT_TILE_SIZE));
			bpp = tile->bpp;
			memcpy ((void *)buf, (const void *)tile_data, inc = (bpp * ( (min = MIN(end,boundary)) -x) ) );
			x = min; buf += inc;
			gimp_tile_unref (tile, FALSE);
		}
	} else {
		const TBitmap	*bm = pr->shadow ? parms->fDst : parms->fSrc;
		BRect			bmBounds = bm->Bounds ();
	
		ASSERT (x >= bmBounds.left && x + width - 1 <= bmBounds.right && y >= bmBounds.top && y <= bmBounds.bottom);
		guchar	*src = (guchar *)bm->Bits ();
		src += 4 * (x - int32(bmBounds.left)) + bm->BytesPerRow () * (y - int32 (bmBounds.top));
		Transfer32To24 (src, buf, width);
	}
}

void gimp_pixel_rgn_get_col (GPixelRgn *pr, guchar *buf, int x, int y, int height) {
	TFilterParams	*parms = (TFilterParams *) pr->drawable->id;
	if (((FILTER_NAME *)parms->fFilter)->Tiled (pr->shadow != 0)) {
		GTile		*tile;
		guchar		*tile_data;
		int			inc, end, boundary, b;

		end = y + height;
		while (y < end) {
			tile = gimp_drawable_get_tile2 (pr->drawable, pr->shadow, x, y);
			gimp_tile_ref (tile);

			tile_data = tile->data + tile->bpp * (tile->ewidth * (y % DEFAULT_TILE_SIZE) + (x % DEFAULT_TILE_SIZE));
			boundary = y + (tile->eheight - (y % DEFAULT_TILE_SIZE));
			inc = tile->bpp * tile->ewidth;
			
			for ( ; y < end && y < boundary; y++) {
				for (b = 0; b < int(tile->bpp); b++)
					*buf++ = tile_data[b];
				tile_data += inc;
			}
			gimp_tile_unref (tile, FALSE);
		}
	} else {
		const TBitmap	*bm = pr->shadow ? parms->fDst : parms->fSrc;
		BRect			bmBounds = bm->Bounds ();
	
		ASSERT (x >= bmBounds.left && x <= bmBounds.right && y >= bmBounds.top && y + height - 1 <= bmBounds.bottom);
		guchar	*src = (guchar *)bm->Bits ();
		int32	bytesPerRow = bm->BytesPerRow ();
		src += 4 * (x - int32(bmBounds.left)) + bytesPerRow * (y - int32 (bmBounds.top));
		while (--height >= 0) {
			*buf++ = src[2]; *buf++ = src[1]; *buf++ = src[0]; src += bytesPerRow;
		}
	}
}

void gimp_pixel_rgn_get_rect (GPixelRgn *pr, guchar *buf, int x, int y, int width, int height) {
	TFilterParams	*parms = (TFilterParams *) pr->drawable->id;
	if (((FILTER_NAME *)parms->fFilter)->Tiled (pr->shadow != 0)) {
		GTile		*tile;
		guchar		*src, *dest;
		gulong		bufstride;
		int			xstart, ystart;
		int			xend, yend;
		int			xboundary;
		int			yboundary;
		int			xstep, ystep;
		int			ty, bpp;
		
		bpp = pr->bpp;
		bufstride = bpp * width;

		xstart = x; ystart = y; xend = x + width; yend = y + height; ystep = 0;
		
		while (y < yend) {
			x = xstart;
			while (x < xend) {
				tile = gimp_drawable_get_tile2 (pr->drawable, pr->shadow, x, y);
				gimp_tile_ref (tile);

				xstep = tile->ewidth - (x % DEFAULT_TILE_SIZE);
				ystep = tile->eheight - (y % DEFAULT_TILE_SIZE);
				xboundary = x + xstep;
				yboundary = y + ystep;
				xboundary = MIN (xboundary, xend);
				yboundary = MIN (yboundary, yend);

				for (ty = y; ty < yboundary; ty++) {
					src = tile->data + tile->bpp * (tile->ewidth * (ty % DEFAULT_TILE_SIZE) + (x % DEFAULT_TILE_SIZE));
					dest = buf + bufstride * (ty - ystart) + bpp * (x - xstart);
					
					memcpy ((void *)dest, (const void *)src, (xboundary-x)*bpp);
				}
				gimp_tile_unref (tile, FALSE);
				x += xstep;
			}
			y += ystep;
		}
	} else {
		const TBitmap	*bm = pr->shadow ? parms->fDst : parms->fSrc;
		BRect			bmBounds = bm->Bounds ();
		int32			x1, y1;
	
		x1 = x + width - 1; y1 = y + height - 1;
		ASSERT (x >= bmBounds.left && x1 <= bmBounds.right && y >= bmBounds.top && y1 <= bmBounds.bottom);
		guchar	*src = (guchar *)bm->Bits ();
		int32	bytesPerRow = bm->BytesPerRow ();
		src += 4 * (x - int32(bmBounds.left)) + bytesPerRow * (y - int32 (bmBounds.top));
		while (--height >= 0) {
			Transfer32To24 (src, buf, width);
			buf += 3 * width; src += bytesPerRow;
		}
	}
}

//----------------------//
//	The Set routines	//
//----------------------//
static void Transfer24To32 (guchar *src24, guchar *dst32, int32 cnt) {
	while (--cnt >= 0) {
		dst32[2] = *src24++; dst32[1] = *src24++; dst32[0] = *src24++; dst32 += 4;
	}
}

void gimp_pixel_rgn_set_pixel (GPixelRgn *pr, guchar *buf, int x, int y) {
	TFilterParams	*parms = (TFilterParams *) pr->drawable->id;
	if (((FILTER_NAME *)parms->fFilter)->Tiled (pr->shadow != 0)) {
		GTile		*tile;
		guchar		*tile_data;
		int			b;

		tile = gimp_drawable_get_tile2 (pr->drawable, pr->shadow, x, y);
		gimp_tile_ref (tile);
		tile_data = tile->data + tile->bpp * (tile->ewidth * (y % DEFAULT_TILE_SIZE) + (x % DEFAULT_TILE_SIZE));
		for (b = 0; b < int(tile->bpp); b++) *tile_data++ = *buf++;
		gimp_tile_unref (tile, TRUE);
	} else {
		const TBitmap	*bm = pr->shadow ? parms->fDst : parms->fSrc;
		BRect			bmBounds = bm->Bounds ();
		#if DEBUG
		ASSERT (x >= bmBounds.left && x <= bmBounds.right && y >= bmBounds.top && y <= bmBounds.bottom);
		#endif
		guchar	*src = (guchar *)bm->Bits ();
		src += 4 * (x - int32(bmBounds.left)) + bm->BytesPerRow () * (y - int32 (bmBounds.top));
		src[2] = *buf++; src[1] = *buf++; src[0] = *buf++;
	}
}

void gimp_pixel_rgn_set_row (GPixelRgn *pr, guchar *buf, int x, int y, int width) {
	TFilterParams	*parms = (TFilterParams *) pr->drawable->id;
	if (((FILTER_NAME *)parms->fFilter)->Tiled (pr->shadow != 0)) {
		GTile		*tile;
		guchar		*tile_data;
		int			inc, min, end, boundary;
		
		end = x + width;
		while (x < end) {
			tile = gimp_drawable_get_tile2 (pr->drawable, pr->shadow, x, y);
			gimp_tile_ref (tile);
			tile_data = tile->data + tile->bpp * (tile->ewidth * (y % DEFAULT_TILE_SIZE) + (x % DEFAULT_TILE_SIZE));
			boundary = x + (tile->ewidth - (x % DEFAULT_TILE_SIZE));
			memcpy ((void *)tile_data, (const void *)buf, inc = (tile->bpp * ( (min = MIN(end,boundary)) -x) ) );
			x = min; buf += inc;
			gimp_tile_unref (tile, TRUE);
		}
	} else {
		const TBitmap	*bm = pr->shadow ? parms->fDst : parms->fSrc;
		BRect			bmBounds = bm->Bounds ();
	
		ASSERT (x >= bmBounds.left && x + width - 1 <= bmBounds.right && y >= bmBounds.top && y <= bmBounds.bottom);
		guchar	*dst = (guchar *)bm->Bits ();
		dst += 4 * (x - int32(bmBounds.left)) + bm->BytesPerRow () * (y - int32 (bmBounds.top));
		Transfer24To32 (buf, dst, width);
	}
}

void gimp_pixel_rgn_set_col (GPixelRgn *pr, guchar *buf, int x, int y, int height) {
	TFilterParams	*parms = (TFilterParams *) pr->drawable->id;
	if (((FILTER_NAME *)parms->fFilter)->Tiled (pr->shadow != 0)) {
		GTile	*tile;
		guchar	*tile_data;
		int		inc, end, boundary, b;

		end = y + height;
		while (y < end) {
			tile = gimp_drawable_get_tile2 (pr->drawable, pr->shadow, x, y);
			gimp_tile_ref (tile);
			tile_data = tile->data + tile->bpp * (tile->ewidth * (y % DEFAULT_TILE_SIZE) + (x % DEFAULT_TILE_SIZE));
			boundary = y + (tile->eheight - (y % DEFAULT_TILE_SIZE));
			inc = tile->bpp * tile->ewidth;
			for ( ; y < end && y < boundary; y++) {
				for (b = 0; b < int(tile->bpp); b++) tile_data[b] = *buf++;
				tile_data += inc;
			}
			gimp_tile_unref (tile, TRUE);
		}
	} else {
		const TBitmap	*bm = pr->shadow ? parms->fDst : parms->fSrc;
		BRect			bmBounds = bm->Bounds ();
	
		ASSERT (x >= bmBounds.left && x <= bmBounds.right && y >= bmBounds.top && y + height - 1 <= bmBounds.bottom);
		guchar	*dst = (guchar *)bm->Bits ();
		int32	bytesPerRow = bm->BytesPerRow ();
		dst += 4 * (x - int32(bmBounds.left)) + bytesPerRow * (y - int32 (bmBounds.top));
		while (--height >= 0) {
			dst[2] = *buf++; dst[1] = *buf++; dst[0] = *buf++; dst += bytesPerRow;
		}
	}
}

void gimp_pixel_rgn_set_rect (GPixelRgn *pr, guchar *buf, int x, int y, int width, int height) {
	TFilterParams	*parms = (TFilterParams *) pr->drawable->id;
	if (((FILTER_NAME *)parms->fFilter)->Tiled (pr->shadow != 0)) {
		GTile		*tile;
		guchar		*src, *dest;
		gulong		bufstride;
		int			xstart, ystart, xend, yend, xboundary, yboundary, xstep, ystep, ty, bpp;
		
		bpp = pr->bpp;
		bufstride = bpp * width;
		
		xstart = x;
		ystart = y;
		xend = x + width;
		yend = y + height;
		ystep = 0;
		
		while (y < yend) {
			x = xstart;
			while (x < xend) {
				tile = gimp_drawable_get_tile2 (pr->drawable, pr->shadow, x, y);
				gimp_tile_ref (tile);

				xstep = tile->ewidth - (x % DEFAULT_TILE_SIZE);
				ystep = tile->eheight - (y % DEFAULT_TILE_SIZE);
				xboundary = x + xstep;
				yboundary = y + ystep;
				xboundary = MIN (xboundary, xend);
				yboundary = MIN (yboundary, yend);

				for (ty = y; ty < yboundary; ty++) {
					src = buf + bufstride * (ty - ystart) + bpp * (x - xstart);
					dest = tile->data + tile->bpp * (tile->ewidth * (ty % DEFAULT_TILE_SIZE) + (x % DEFAULT_TILE_SIZE));
					
					memcpy ((void *)dest, (const void *)src, (xboundary-x)*bpp); 
				}
				gimp_tile_unref (tile, TRUE);
				x += xstep;
			}
			
			y += ystep;
		}
	} else {
		const TBitmap	*bm = pr->shadow ? parms->fDst : parms->fSrc;
		BRect			bmBounds = bm->Bounds ();
		#if DEBUG
		int32			x1, y1;
	
		x1 = x + width - 1; y1 = y + height - 1;
		ASSERT (x >= bmBounds.left && x1 <= bmBounds.right && y >= bmBounds.top && y1 <= bmBounds.bottom);
		#endif
		guchar	*dst = (guchar *)bm->Bits ();
		int32	bytesPerRow = bm->BytesPerRow ();
		dst += 4 * (x - int32(bmBounds.left)) + bytesPerRow * (y - int32 (bmBounds.top));
		while (--height >= 0) {
			Transfer24To32 (buf, dst, width);
			buf += 3 * width; dst += bytesPerRow;
		}
	}
}

//------------------------------//
//	The Tile iterators routines	//
//------------------------------//
gpointer gimp_pixel_rgns_register (int nrgns, ...) {
	GPixelRgn			*pr;
	GPixelRgnHolder		*prh;
	GPixelRgnIterator	*pri;
	va_list				ap;
	int					found;

	if (nrgns < 1) return NULL;

	pri = g_new (GPixelRgnIterator, 1);
	pri->pixel_regions = NULL;
	pri->process_count = 0;

	va_start (ap, nrgns);
	
	found = FALSE;
	while (nrgns --) {
		pr = va_arg (ap, GPixelRgn *);
		prh = g_new (GPixelRgnHolder, 1);
		prh->pr = pr;
	
		if (pr != NULL) {
			/*  If there is a defined value for data, make sure tiles is NULL  */
			if (pr->data)
				pr->drawable = NULL;
			prh->original_data = pr->data;
			prh->startx = pr->x;
			prh->starty = pr->y;
			prh->pr->process_count = 0;
	
			if (!found) {
				found = TRUE;
				pri->region_width = pr->w;
				pri->region_height = pr->h;
			}
		}
		/*  Add the pixel Rgn holder to the list  */
		pri->pixel_regions = g_slist_prepend (pri->pixel_regions, prh);
	}
	
	va_end (ap);
	
	return gimp_pixel_rgns_configure (pri);
}

gpointer gimp_pixel_rgns_process (gpointer pri_ptr) {
	GSList				*list;
	GPixelRgnHolder		*prh;
	GPixelRgnIterator	*pri;
	
	pri = (GPixelRgnIterator*) pri_ptr;
	pri->process_count++;

	/*  Unref all referenced tiles and increment the offsets  */
	list = pri->pixel_regions;
	while (list) {
		prh = (GPixelRgnHolder*) list->data;
		list = list->next;
		if ((prh->pr != NULL) && (int(prh->pr->process_count) != int(pri->process_count))) {
			/*  This eliminates the possibility of incrementing the
			*  same region twice
			*/
			prh->pr->process_count++;
			/*  Unref the last referenced tile if the underlying region is a tile manager  */
			if (prh->pr->drawable) {
				GTile *tile = gimp_drawable_get_tile2 (prh->pr->drawable, prh->pr->shadow, prh->pr->x, prh->pr->y);
				gimp_tile_unref (tile, prh->pr->dirty);
			}
			
			prh->pr->x += pri->portion_width;
			
			if (int(prh->pr->x - prh->startx) >= int(pri->region_width)) {
				prh->pr->x = prh->startx;
				prh->pr->y += pri->portion_height;
			}
		}
	}
	return gimp_pixel_rgns_configure (pri);
}


static int32 gimp_get_portion_width (GPixelRgnIterator *pri) {
	GSList			*list;
	GPixelRgnHolder *prh;
	int				min_width = G_MAXINT;
	int				width;

	/* Find the minimum width to the next vertical tile (in the case of a tile manager)
	* or to the end of the pixel region (in the case of no tile manager)
	*/
	
	list = pri->pixel_regions;
	while (list) {
		prh = (GPixelRgnHolder *) list->data;
		
		if (prh->pr) {
			/* Check if we're past the point of no return  */
			if (int(prh->pr->x - prh->startx) >= int(pri->region_width))
				return 0;
			
			if (prh->pr->drawable) {
				width = DEFAULT_TILE_SIZE - (prh->pr->x % DEFAULT_TILE_SIZE);
				width = BOUNDS (width, 0, int(pri->region_width - (prh->pr->x - prh->startx)));
			} else
				width = (pri->region_width - (prh->pr->x - prh->startx));
			
			if (width < min_width)
				min_width = width;
		}
		list = list->next;
	}
	
	return min_width;
}

static int32 gimp_get_portion_height (GPixelRgnIterator *pri) {
	GSList			*list;
	GPixelRgnHolder *prh;
	int				min_height = G_MAXINT;
	int				height;
	
	/* Find the minimum height to the next vertical tile (in the case of a tile manager)
	* or to the end of the pixel region (in the case of no tile manager) */
	 
	list = pri->pixel_regions;
	while (list) {
		prh = (GPixelRgnHolder *) list->data;
		
		if (prh->pr) {
			/* Check if we're past the point of no return  */
			if (int(prh->pr->y - prh->starty) >= int(pri->region_height))
				return 0;
			
			if (prh->pr->drawable) {
				height = DEFAULT_TILE_SIZE - (prh->pr->y % DEFAULT_TILE_SIZE);
				height = BOUNDS (height, 0, int(pri->region_height - (prh->pr->y - prh->starty)));
			} else {
				height = (pri->region_height - (prh->pr->y - prh->starty));
			}
			
			if (height < min_height)
				min_height = height;
		}
		
		list = list->next;
	}

	return min_height;
}

static gpointer gimp_pixel_rgns_configure (GPixelRgnIterator *pri) {
	GPixelRgnHolder	*prh;
	GSList			*list;

	/*  Determine the portion width and height  */
	pri->portion_width = gimp_get_portion_width (pri);
	pri->portion_height = gimp_get_portion_height (pri);

	if ((pri->portion_width == 0) || (pri->portion_height == 0)) {
		/*  free the pixel regions list  */
		if (pri->pixel_regions) {
			list = pri->pixel_regions;
			while (list) {
				g_free (list->data);
				list = list->next;
			}
			g_slist_free (pri->pixel_regions);
			g_free (pri);
		}
		return NULL;
	}

	pri->process_count++;
	
	list = pri->pixel_regions;
	while (list) {
		prh = (GPixelRgnHolder *) list->data;
	
		if ((prh->pr != NULL) && (int(prh->pr->process_count) != int(pri->process_count))) {
			prh->pr->process_count++;
			gimp_pixel_rgn_configure (prh, pri);
		}
	
		list = list->next;
	}
	return pri;
}

static void gimp_pixel_rgn_configure (GPixelRgnHolder *prh, GPixelRgnIterator *pri) {
	/* Configure the rowstride and data pointer for the pixel region
	* based on the current offsets into the region and whether the
	* region is represented by a tile manager or not
	*/
	if (prh->pr->drawable) {
		GTile	*tile;
		int		offx, offy;

		tile = gimp_drawable_get_tile2 (prh->pr->drawable, prh->pr->shadow, prh->pr->x, prh->pr->y);
		gimp_tile_ref (tile);

		offx = prh->pr->x % DEFAULT_TILE_SIZE;
		offy = prh->pr->y % DEFAULT_TILE_SIZE;

		prh->pr->rowstride = tile->ewidth * prh->pr->bpp;
		prh->pr->data = tile->data + offy * prh->pr->rowstride + offx * prh->pr->bpp;
	} else {
		prh->pr->data = (prh->original_data +
		(prh->pr->y - prh->starty) * prh->pr->rowstride +
		(prh->pr->x - prh->startx) * prh->pr->bpp);
	}
	prh->pr->w = pri->portion_width;
	prh->pr->h = pri->portion_height;
}

GTile *gimp_drawable_get_tile (GDrawable *drawable, gint shadow, gint row, gint col) {
	TFilterParams	*parms = (TFilterParams *) drawable->id;

	return (GTile *) ((FILTER_NAME *)parms->fFilter)->GetTile (parms, shadow, row, col);
}

GTile *gimp_drawable_get_tile2 (GDrawable *drawable, gint shadow, gint x, gint y) {
	return gimp_drawable_get_tile (drawable, shadow, y / DEFAULT_TILE_SIZE, x / DEFAULT_TILE_SIZE);
}
//------------------------------------------------------------------------------------------------
//
//	gtk routines.
//
//------------------------------------------------------------------------------------------------
GSList *g_slist_alloc (void) {
  GSList *new_list;

  new_list = g_new (GSList, 1);
  new_list->data = NULL;
  new_list->next = NULL;

  return new_list;
}

GSList* g_slist_prepend (GSList *list, gpointer  data) {
  GSList *new_list;

  new_list = g_slist_alloc ();
  new_list->data = data;
  new_list->next = list;

  return new_list;
}

void g_slist_free (GSList *list) {
	while (list) {
		GSList	*next = list->next;
		g_free (list); list = next;
	}
}

//------------------------------------------------------------------------------------------------
//
//	My Tile manager classes.
//
//------------------------------------------------------------------------------------------------
GIMPTileManager::GIMPTileManager (TBitmap *bm) {
	fBitmap = bm;
	BRect	re = fBitmap->Bounds ();
	fWidth = re.IntegerWidth () + 1; fHeight = re.IntegerHeight() + 1; fTileSize = DEFAULT_TILE_SIZE;
	BuildVector ();
}

GIMPTileManager::~GIMPTileManager () {
	RemoveTiles ();
}

void GIMPTileManager::RemoveTiles (void) {
	GIMPTile	*tile;

	for (tile = fTiles.begin (); tile < fTiles.end (); ++tile) {
		tile->Clear (true);
	}
	fTiles.erase (fTiles.begin (), fTiles.end());
}

void GIMPTileManager::BuildVector (void) {
	RemoveTiles ();
	fXTiles = (fWidth + fTileSize - 1) / fTileSize; fYTiles = (fHeight + fTileSize - 1) / fTileSize;
	fTiles.reserve (fXTiles * fYTiles);
	int32	i;
	for (i = fXTiles * fYTiles; --i >= 0; ) {
		GIMPTile	tile;
		fTiles.push_back (tile);
	}
}

void GIMPTileManager::TileInfo (int32 xTile, int32 yTile, const TBitmap *bm, void **bitsPos, int32 *width, int32 *height) const {
	BRect	bounds = bm->Bounds ();

	*bitsPos = (uint8 *)bm->Bits () + bm->BytesPerRow () * (int32(bm->Bounds().top) + yTile * fTileSize) + (int32(bm->Bounds().left) + xTile * fTileSize) * 4;
	*width = int32(bounds.right) - int32(bounds.left) - xTile * fTileSize + 1; if (*width > fTileSize) *width = fTileSize;
	*height = int32(bounds.bottom) - int32(bounds.top) - yTile * fTileSize + 1; if (*height > fTileSize) *height = fTileSize;
}

void GIMPTileManager::TileInfo (int32 xTile, int32 yTile, const BBitmap *bm, void **bitsPos, int32 *width, int32 *height) const {
	BRect	bounds = bm->Bounds ();

	*bitsPos = (uint8 *)bm->Bits () + bm->BytesPerRow () * yTile * fTileSize + xTile * fTileSize * 4;
	*width = int32(bounds.right) - xTile * fTileSize + 1; if (*width > fTileSize) *width = fTileSize;
	*height = int32(bounds.bottom) - yTile * fTileSize + 1; if (*height > fTileSize) *height = fTileSize;
}

void GIMPTileManager::ToTBitmap (void) const {
	fBitmap->ReadWriteLock ();
	ToBitmap (fBitmap->Bitmap ());
	fBitmap->ReadWriteUnlock ();
}

void GIMPTileManager::ToBitmap (BBitmap *bm) const {
	void	*curBits;

	ASSERT (bm->Bounds ().IntegerWidth () + 1 == fWidth && bm->Bounds ().IntegerHeight () + 1 == fHeight);
	int32	x, y, width, height, rowBytes;

	rowBytes = bm->BytesPerRow ();
	for (y = fYTiles; --y >= 0; ) {
		for (x = fXTiles; --x >= 0; ) {
			TileInfo (x, y, bm, &curBits, &width, &height);
			fTiles[y*fXTiles + x].ToBitmap (curBits, rowBytes, width, height);
		}
	}
}

GIMPTile *GIMPTileManager::GetTile (int32 id) {
	ASSERT (id < fXTiles * fYTiles);
	if (fTiles[id].Empty ()) {
		int32	width, height;
		void	*curBits;

		TileInfo (id % fXTiles, id / fXTiles, fBitmap, &curBits, &width, &height);
		fTiles[id].FromBitmap (curBits, fBitmap->BytesPerRow (), width, height);
	}
	return (GIMPTile *)&(fTiles[id]);
}

GIMPTile::GIMPTile (void) {
	fWidth = fHeight = 0; fData = NULL; fDirty = false;
}

GIMPTile::GIMPTile (const GIMPTile &src) {
	fGIMPTile = src.fGIMPTile; fData = src.fData; AddUser ();
	fWidth = src.fWidth; fHeight = src.fHeight; fDirty = src.fDirty;
}

GIMPTile::~GIMPTile () {
	Clear (false);
}

void GIMPTile::Clear (bool forced) {
	if (fData) {
		GIMPTileData	*tData = ((GIMPTileData *)fData) - 1;
		if (forced) {
			ASSERT (tData->fRefs == 0); tData->fRefs = 0;
		}
		tData->Dispose ();
		fData = NULL;
	}
}

void GIMPTile::Allocate (void) {
	ASSERT (fData == NULL);
	GIMPTileData	*data;
	int32			dataSize;
	ASSERT (fWidth <= DEFAULT_TILE_SIZE && fHeight <= DEFAULT_TILE_SIZE);
	dataSize = 3 * DEFAULT_TILE_SIZE * DEFAULT_TILE_SIZE;
	data = (GIMPTileData *) (new char[dataSize + sizeof(GIMPTileData)]);
	if (data) {
		data->fRefs = 0; data->fAllocLength = dataSize;
		fData = data->Data ();
	}
	fGIMPTile.ewidth = fWidth; fGIMPTile.eheight = fHeight; fGIMPTile.bpp = 3;
	fGIMPTile.tile_num = 0; fGIMPTile.ref_count = 0; fGIMPTile.dirty = 0;
	fGIMPTile.shadow = 0; fGIMPTile.data = (guchar *)fData; fGIMPTile.drawable = NULL;
}

void GIMPTile::FromBitmap (void *src, int32 rowBytes, int32 width, int32 height) {
	int32	y;
	guchar	*q, *p;

	fWidth = width; fHeight = height; fRowBytes = /*DEFAULT_TILE_SIZE*/fWidth * 3;
	Allocate ();
	if (!fData) return;
	for (p = (guchar *)src, q = (guchar *)fData, y = height; --y >= 0; q += fRowBytes, p += rowBytes) {
		Transfer32To24 (p, q, width);
	}
}

void GIMPTile::ToBitmap (void *src, int32 rowBytes, int32 width, int32 height) const {
	int32	y;
	guchar	*q, *p;

	if (fData != NULL) { // && fDirty) {
		//	The tile is some data
		for (p = (guchar *)src, q = (guchar *)fData, y = height; --y >= 0; q += fRowBytes, p += rowBytes) {
			Transfer24To32 (q, p, width);
		}
	}
}

//------------------------------------------------------------------------------------------------
//	TBitmap class
//------------------------------------------------------------------------------------------------
void TBitmap::Copy (const TBitmap &src) {
	ASSERT (&src != this);
	ASSERT (src.BitsLength () == BitsLength ());
	memcpy (Bits (), src.Bits (), src.BitsLength ());
}

void FilterUpdate (TFilterParams *parms) {
	(*parms->fUpdater) (parms);
}
