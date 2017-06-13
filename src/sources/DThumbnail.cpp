#include "DThumbnail.h"
#include "DAutoBuffer.h"
#include <interface/Bitmap.h>
#include <interface/View.h>
#include <kernel/fs_attr.h>
#include <storage/Node.h>
#include <support/Autolock.h>


// public constants

_EXPORT const char * DThumbnail::cThumbnailAttributeName = "GRAFX:Thumbnail";
_EXPORT const char * DThumbnail::cIconAttributeName 	= "BEOS:L:STD_ICON";
_EXPORT const char * DThumbnail::cMiniIconAttributeName = "BEOS:M:STD_ICON";
_EXPORT const char * DThumbnail::cWidthAttributeName 	= "GRAFX:Width";
_EXPORT const char * DThumbnail::cHeightAttributeName 	= "GRAFX:Height";


// public functions

_EXPORT
DThumbnail::DThumbnail(void) :

	fThumbnail(), 
	fIcon(), 
	fMiniIcon(),
	fOriginalWidth(0), fOriginalHeight(0), fInit(B_NO_INIT)
{
	// do nothing
}


_EXPORT
DThumbnail::DThumbnail(
	const BBitmap * original,
	bool dither) :

	fThumbnail(), 
	fIcon(), 
	fMiniIcon(),
	fOriginalWidth(0), fOriginalHeight(0), fInit(B_NO_INIT)
{
	SetTo(original, dither);
}


_EXPORT
DThumbnail::~DThumbnail(void)
{
	BAutolock lock(fLock);
	// do nothing
}


_EXPORT
const BBitmap *
DThumbnail::Bitmap(void) const
{
	BAutolock lock(fLock);

	return fThumbnail.get();
}


_EXPORT
const BBitmap *
DThumbnail::Icon(void) const
{
	BAutolock lock(fLock);

	return fIcon.get();
}


_EXPORT
const BBitmap *
DThumbnail::MiniIcon(void) const
{
	BAutolock lock(fLock);

	return fMiniIcon.get();
}


_EXPORT
auto_ptr<BBitmap>
DThumbnail::GetBitmap(void)
{
	BAutolock lock(fLock);

	auto_ptr<BBitmap> bitmap(CopyBitmap(Bitmap()));
	return bitmap;
}


_EXPORT
auto_ptr<BBitmap>
DThumbnail::GetIcon(void)
{
	BAutolock lock(fLock);

	auto_ptr<BBitmap> icon(CopyBitmap(Icon()));
	return icon;
}


_EXPORT
auto_ptr<BBitmap>
DThumbnail::GetMiniIcon(void)
{
	BAutolock lock(fLock);

	auto_ptr<BBitmap> mini_icon(CopyBitmap(MiniIcon()));
	return mini_icon;
}


_EXPORT
status_t
DThumbnail::WriteThumbnailAttribute(
	BNode * node)
{
	BAutolock lock(fLock);

	if (!fThumbnail.get()) return B_NO_INIT;
	if (!node || B_OK != node->InitCheck()) return B_BAD_VALUE;
	
	BMessage archive;
	if (B_OK == fThumbnail->Archive(&archive, false))
	{
		ssize_t length = archive.FlattenedSize();
		DAutoBuffer<char> buffer(length);
		
		if (B_OK == archive.Flatten(buffer.Get(), length))
		{
			node->WriteAttr(
				cThumbnailAttributeName, 
				B_MESSAGE_TYPE, 
				0, 
				buffer.GetVoid(), 
				length);
				
			return B_OK;
		}
	}
	
	return B_ERROR;
}


_EXPORT
status_t
DThumbnail::WriteIconAttribute(
	BNode * node)
{
	BAutolock lock(fLock);

	return WriteSingleIconAttribute(
			node, cIconAttributeName, fIcon.get(), 31.0, 'ICON');
}


_EXPORT
status_t
DThumbnail::WriteMiniIconAttribute(
	BNode * node)
{
	BAutolock lock(fLock);

	return WriteSingleIconAttribute(
			node, cMiniIconAttributeName, fMiniIcon.get(), 15.0, 'MICN');
}


_EXPORT
status_t
DThumbnail::WriteResolutionAttributes(
	BNode * node)
{
	BAutolock lock(fLock);

	if (fOriginalWidth == 0 || fOriginalHeight == 0) return B_NO_INIT;
	if (!node || B_OK != node->InitCheck()) return B_BAD_VALUE;

	node->WriteAttr(
		cWidthAttributeName, 
		B_INT32_TYPE, 
		0, 
		&fOriginalWidth, 
		sizeof(fOriginalWidth));

	node->WriteAttr(
		cHeightAttributeName, 
		B_INT32_TYPE, 
		0, 
		&fOriginalHeight, 
		sizeof(fOriginalHeight));
		
	return B_OK;
}


_EXPORT
status_t
DThumbnail::ReadFromAttributes(
	BNode * node)
{
	BAutolock lock(fLock);

	fInit = DoReadFromAttributes(node);
	
	return fInit;
}


_EXPORT
status_t
DThumbnail::SetTo(
	const BBitmap * original,
	bool dither)
{
	BAutolock lock(fLock);

	fInit = DoSetTo(original, dither);
	
	return fInit;
}
	
	
_EXPORT
auto_ptr<BBitmap>
DThumbnail::MakeThumbnail(
	const BBitmap * original,
	int32 size,
	bool dither)
{
	auto_ptr<BBitmap> thumbnail;
	float fSize, width, height, tWidth, tHeight;
	int32 uncropped_width, uncropped_height;
	
	fSize = static_cast<float>(size);
	
	width = original->Bounds().Width() + 1.0;
	height = original->Bounds().Height() + 1.0;
	
	uncropped_width = static_cast<int32>(width);
	uncropped_height = static_cast<int32>(height);
	
	bool horizontally_cropped = false;
	bool vertically_cropped = false;
	
	if (width > height && width <= 3.0 * height)
	{
		tWidth = min(fSize, width);
		tHeight = max((float)1.0, height * tWidth / width);
	}
	else if (height >= width && height <= 3.0 * width)
	{
		tHeight = min(fSize, height);
		tWidth = max((float)1.0, width * tHeight / height);
	}
	else // aspect more extreme than 1:3 or 3:1 = scale & crop
	{
		tWidth = min(fSize, width);
		tHeight = min(fSize, height);
		if (width < height)
		{
			vertically_cropped = true;
			if (width > fSize)
				height = width;
			else if (height > fSize)
				height = fSize;
			else 
				vertically_cropped = false;
		}
		else {
			horizontally_cropped = true;
			if (height > fSize)
				width = height;
			else if (width > fSize)
				width = fSize;
			else
				horizontally_cropped = false;
		}
	}
	
	// color space has to be B_RGB32
	
	if (original->ColorSpace() != B_RGB32)
	{
		auto_ptr<BBitmap> new_map(new BBitmap(original->Bounds(), B_RGB32, true));

		BView *view = new BView(original->Bounds(), NULL, B_FOLLOW_NONE, 0);		
		new_map->AddChild(view);

		BAutolock lock(view->Looper());
		view->DrawBitmap(original);
		
		original = new_map.release();
	}
	
	// create thumbnail
	
	int32 w = static_cast<int32>(width);
	int32 h = static_cast<int32>(height);
	
	int32 tw = static_cast<int32>(tWidth);
	int32 th = static_cast<int32>(tHeight);
	
	ASSERT(w > 0 && h > 0);
	ASSERT(tw > 0 && th > 0);
	
	thumbnail = auto_ptr<BBitmap>(new BBitmap(
					BRect(0, 0, tw - 1.0, th - 1.0),
					dither ? B_CMAP8 : B_RGB32));
	
	// get color map (in case of dithering)
	
	uint8 colorIndex;
	uint32 rgb15;
	int32 r_error, g_error, b_error, rd, gd, bd;
	int32 new_r_error, new_g_error, new_b_error;
	const color_map* colorMap = NULL;
	
	if (dither)
		colorMap = system_colors();

	DAutoBuffer<int32> be(tw);
	DAutoBuffer<int32> ge(tw);
	DAutoBuffer<int32> re(tw);
		
	// fill thumbnail (very simple algorithm)
	
	// calculate offset
	
	uint32 xoffset = 0, yoffset = 0;

	if (horizontally_cropped)
		xoffset = (static_cast<int32>(uncropped_width) - w) / 2;
	if (vertically_cropped)
		yoffset = (static_cast<int32>(uncropped_height) - h) / 2;
	
	ASSERT(xoffset >= 0 && yoffset >= 0);
	ASSERT(xoffset <= uncropped_width - w && yoffset <= uncropped_height - h);
	
	// setup

	DAutoBuffer<uint32> blue(tw);
	DAutoBuffer<uint32> green(tw);
	DAutoBuffer<uint32> red(tw);
	DAutoBuffer<uint32> alpha(tw);
	DAutoBuffer<uint32> xcount(tw);
	union {
		struct {
			uint8 blue;
			uint8 green;
			uint8 red;
			uint8 alpha;
		} channel;
		uint32 word;
	} bgra;
	
	uint32 *bits = static_cast<uint32*>(original->Bits());
	uint32 *tBits = static_cast<uint32*>(thumbnail->Bits());
	uint8  *tChars = static_cast<uint8*>(thumbnail->Bits());
	
	uint32 intsPerRow = original->BytesPerRow() / 4;
	uint32 tBytesPerRow = thumbnail->BytesPerRow();
	uint32 *line = bits + yoffset * intsPerRow;
	uint8  *tCharLine = tChars;
	
	uint32 x, y, xc, yc, itx, tx, ty, ycount, area, b, g, r, a;
	
	// calculate width of cells
	
	for (x = 0; x < tw; x++)
	{
		blue[x] = green[x] = red[x] = alpha[x] = xcount[x] = 0;
		be[x] = ge[x] = re[x] = 0;
	}
		
	tx = 0;
	xc = tw/2;
	
	do
	{
		xcount[tx]++;
		
		xc += tw;
		if (xc >= w)
		{
			tx++;
			xc -= w;
		}
		
	} while (tx < tw);
	
	// calculate colors of cells

	y = ty = ycount = 0;	
	yc = th/2;
	
	do
	{
		bits = line + xoffset;

		ASSERT(y < h);

		for (tx = 0; tx < tw; tx++)
		{
			for (x = 0; x < xcount[tx]; x++)
			{
				bgra.word = *bits++;
			
				blue[tx] 	+= bgra.channel.blue;
				green[tx] 	+= bgra.channel.green;
				red[tx] 	+= bgra.channel.red;
				alpha[tx] 	+= bgra.channel.alpha;
			}
		}		
		
		line += intsPerRow;
		
		y++;
		ycount++;
		yc += th;
		if (yc >= h)
		{
			// write colors
			
			tBits = static_cast<uint32*>(static_cast<void*>(tCharLine));
			tChars = tCharLine;

			r_error = g_error = b_error = 0;
		
			for (itx = 0; itx < tw; itx++)
			{
				if (dither && !(ty & 1))
					tx = tw - itx - 1;
				else tx = itx;
				
				area = xcount[tx] * ycount;
				b = min((uint32)255, (blue[tx] / area));
				g = min((uint32)255, (green[tx] / area));
				r = min((uint32)255, (red[tx] / area));
				a = min((uint32)255, (alpha[tx] / area));
				
				if (dither)
				{
					// dithering (error diffusion)
					
					rd = r + r_error + re[tx];
					gd = g + g_error + ge[tx];
					bd = b + b_error + be[tx];
					
					rd = min((uint32)255, (uint32)max((int32)0, rd));
					gd = min((uint32)255, (uint32)max((int32)0, gd));
					bd = min((uint32)255, (uint32)max((int32)0, bd));
					
					rgb15 = ((rd & 0xf8) << 7)
							| ((gd & 0xf8) << 2)
							| ((bd & 0xf8) >> 3);
							
					colorIndex = colorMap->index_map[rgb15];
					
					tCharLine[tx] = colorIndex;
					
					rgb_color qCol = colorMap->color_list[colorIndex];
					
					new_r_error = ((int32)r - (int32)qCol.red);
					new_g_error = ((int32)g - (int32)qCol.green);
					new_b_error = ((int32)b - (int32)qCol.blue);
					
					r_error += new_r_error * 5 / 16;
					g_error += new_g_error * 5 / 16;
					b_error += new_b_error * 5 / 16;

					if (tx > 0 && !(ty & 1) || tx+1 < tw && (ty & 1))
					{					
						re[tx + ((ty & 1) ? 1 : -1)] += new_r_error * 3 / 16;
						ge[tx + ((ty & 1) ? 1 : -1)] += new_g_error * 3 / 16;
						be[tx + ((ty & 1) ? 1 : -1)] += new_b_error * 3 / 16;					
					}
					
					re[tx] += new_r_error * 7 / 16;
					ge[tx] += new_g_error * 7 / 16;
					be[tx] += new_b_error * 7 / 16;					

					if (tx > 0 && (ty & 1) || tx+1 < tw && !(ty & 1))
					{					
						re[tx + ((ty & 1) ? -1 : 1)] += 
							new_r_error - new_r_error * 7 / 16
							- new_r_error * 3 / 16 - new_r_error * 5 / 16;
						ge[tx + ((ty & 1) ? -1 : 1)] += 
							new_g_error - new_g_error * 7 / 16
							- new_g_error * 3 / 16 - new_g_error * 5 / 16;
						be[tx + ((ty & 1) ? -1 : 1)] += 
							new_b_error - new_b_error * 7 / 16
							- new_b_error * 3 / 16 - new_b_error * 5 / 16;			
					}
				}
				else
				{
					// true color
					
					bgra.channel.blue 	= b;
					bgra.channel.green 	= g;
					bgra.channel.red 	= r;
					bgra.channel.alpha 	= a;
				
					*tBits++ = bgra.word;
				}
				blue[tx] = green[tx] = red[tx] = alpha[tx] = 0;
			}
		
			tCharLine += tBytesPerRow;
			
			ty++;
			ycount = 0;
			yc -= h;
		}
	} while (ty < th);
	
	return thumbnail;	
}


// private functions

auto_ptr<BBitmap>
DThumbnail::CopyBitmap(const BBitmap * source)
{
	auto_ptr<BBitmap> copy;

	if (source)
	{
		copy = auto_ptr<BBitmap>(
			new BBitmap(source->Bounds(), source->ColorSpace()));		
		memcpy(copy->Bits(), source->Bits(), copy->BitsLength());
	}
	
	return copy;
}


void
DThumbnail::Clear(void)
{
	fThumbnail = auto_ptr<BBitmap>();
	fIcon = auto_ptr<BBitmap>();
	fMiniIcon = auto_ptr<BBitmap>();
	fOriginalWidth = fOriginalHeight = 0;
}


status_t
DThumbnail::WriteSingleIconAttribute(
	BNode * node,
	const char * attrName,
	const BBitmap * source,
	float size,
	uint32 attrType)
{
	ASSERT(size == 15.0 && attrType == 'MICN' 
		|| size == 31.0 && attrType == 'ICON');
	
	if (!source) return B_NO_INIT;
	if (!node || B_OK != node->InitCheck()) return B_BAD_VALUE;

	// create Bitmap "icon" of correct size
	
	auto_ptr<BBitmap> icon(new BBitmap(BRect(0,0,size,size), B_CMAP8, true));

	BView *view = new BView(icon->Bounds(), NULL, B_FOLLOW_NONE, 0);
	icon->AddChild(view);
	
	// clear icon to transparent color
	
	memset(icon->Bits(), B_TRANSPARENT_8_BIT, icon->BitsLength());
		
	// center "source" within "icon"
	 
	BPoint point((size - source->Bounds().Width()) / 2.0,
				(size - source->Bounds().Height()) / 2.0);
	
	{
		BAutolock lock(view->Looper());
		
		view->DrawBitmap(source, point);
	}
	
	// write data of "icon"
	
	size_t length = icon->BitsLength();
	
	if (length != node->WriteAttr(
		attrName, 
		attrType, 
		0, 
		icon->Bits(), 
		length))
	{	
		return B_ERROR;
	}
	
	return B_OK;
}
	
	
status_t
DThumbnail::ReadSingleIconFromAttribute(
	BNode * node,
	const char * attrName,
	const BBitmap * target,
	float size,
	uint32 attrType)
{
	ASSERT(size == 15.0 && attrType == 'MICN' 
		|| size == 31.0 && attrType == 'ICON');
	
	if (!target) return B_NO_INIT;
	if (!node || B_OK != node->InitCheck()) return B_BAD_VALUE;

	// read data of "target"
	
	return node->ReadAttr(
		attrName, 
		attrType, 
		0, 
		target->Bits(), 
		target->BitsLength());
}
	

status_t
DThumbnail::DoReadFromAttributes(
	BNode * node)
{
	Clear();
		
	if (!node || B_OK != node->InitCheck()) return B_BAD_VALUE;
	
	// read thumbnail
	
	attr_info info;
	status_t err;
	
	err = node->GetAttrInfo(cThumbnailAttributeName, &info);
	if (B_OK != err) return err;
	
	DAutoBuffer<char> buffer(info.size);
	
	if (info.size != node->ReadAttr(
			cThumbnailAttributeName, 
			B_MESSAGE_TYPE, 
			0,
			buffer.GetVoid(), 
			info.size))
	{
		return B_ERROR;
	}
	
	BMessage archive;
	err = archive.Unflatten(buffer.Get());	
	if (B_OK != err) return err;
	
	fThumbnail = auto_ptr<BBitmap>(
		dynamic_cast<BBitmap*>(instantiate_object(&archive)));
	
	if (!fThumbnail.get()) return B_ERROR;
	
	// read icon
	
	ReadSingleIconFromAttribute(
		node,
		cIconAttributeName,
		fIcon.get(),
		31.0,
		'ICON');
		
	// read mini icon
	
	ReadSingleIconFromAttribute(
		node,
		cMiniIconAttributeName,
		fMiniIcon.get(),
		15.0,
		'MICN');
		
	// read resolution
	
	node->ReadAttr(
		cWidthAttributeName, 
		B_INT32_TYPE, 
		0, 
		&fOriginalWidth, 
		sizeof(fOriginalWidth));
	
	node->ReadAttr(
		cHeightAttributeName, 
		B_INT32_TYPE, 
		0, 
		&fOriginalHeight, 
		sizeof(fOriginalHeight));
		
	return B_OK;
}


status_t
DThumbnail::DoSetTo(
	const BBitmap * original,
	bool dither)
{
	
	Clear();
	
	if (!original || !original->Bounds().IsValid()) return B_BAD_VALUE;
	
	// set size
	
	fOriginalWidth = static_cast<int32>(original->Bounds().Width() + 1.0);
	fOriginalHeight = static_cast<int32>(original->Bounds().Height() + 1.0);
	
	// create mini icon
	
	fMiniIcon = auto_ptr<BBitmap>(MakeThumbnail(original, 16, true));
	
	// create icon
	
	fIcon = auto_ptr<BBitmap>(MakeThumbnail(original, 32, true));
	
	// create thumbnail
	
	fThumbnail = auto_ptr<BBitmap>(MakeThumbnail(original, cThumbnailSize, dither));
	
	return B_OK;
}
