/*
 *  ProgressiveBitmapStream.cpp
 *
 *  Release 3.1.0 (Apr. 27th, 1998)
 *  Written by Edmund Vermeulen <edmundv@xs4all.nl>
 */


// Includes
#include <string.h>
#include <stdio.h>
#include <Bitmap.h>
#include <Invoker.h>
#include <Message.h>
#include <Window.h>
#include <StatusBar.h>
#include <Button.h>
#include <Rect.h>
#include <Errors.h>
#include "PBitmapModif.h"


// Defines
#define UPDATE_BAR    'updb'
#define STOP_PRESSED  'stop'


// BarWindow class
class BarWindow : public BWindow
{
public:
	BarWindow(BRect frame, char *window_title, float max);
	virtual void MessageReceived(BMessage *msg);
	bool StopPressed() const;

private:
	BStatusBar *the_bar;
	bool stop;
};


// Progress bar window constructor
BarWindow::BarWindow(BRect frame, char *window_title, float max)
	: BWindow(
		frame,
		window_title,
		B_FLOATING_WINDOW,
		B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_AVOID_FOCUS
		)
	, stop(false)
				 			
{
	// Background view
	BView *main_view = new BView(Bounds(), "", B_FOLLOW_NONE, B_WILL_DRAW);
	AddChild(main_view);
	main_view->SetViewColor(216,216,216);

	// Progress bar
	BRect r = Bounds();
	r.bottom = 48;
	r.InsetBy(10,10);
	//BRect(10.0, 10.0, 240.0, 50.0)
	the_bar = new BStatusBar(r, "");
	the_bar->SetMaxValue(max);
	main_view->AddChild(the_bar);

	// Stop button
	r = Bounds();
	r.InsetBy(4,4);
	r.top = r.bottom -24;
	r.InsetBy((Bounds().Width()-80)/2,0);
	main_view->AddChild(new BButton(r,
		"",
		"Stop",
		new BMessage(STOP_PRESSED)));

	Show();
}


// Update status bar
void
BarWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case UPDATE_BAR:
			the_bar->Update(msg->FindFloat("delta"));
			break;

		case STOP_PRESSED:
			stop = true;
			break;

		default:
			BWindow::MessageReceived(msg);
			break;
	}
}


// Check if stop button was pressed
bool
BarWindow::StopPressed() const
{
	return stop;
}


// Constructor for when using the bitmap stream as source
ProgressiveBitmapStream::ProgressiveBitmapStream(BBitmap *bitmap)
	: BPositionIO()
{
	

	fWriteOnly = false;  // Read-only
	fBitmap = bitmap;

	fHeader.magic = B_TRANSLATOR_BITMAP;
	fHeader.bounds = bitmap->Bounds();
	fHeader.rowBytes = bitmap->BytesPerRow();
	fHeader.colors = bitmap->ColorSpace();
	fHeader.dataSize = bitmap->BitsLength();

	fDataSize = fHeader.dataSize;
	SetExtras();
	fDisposePrim = false;

	// Convert the header to the correct endianess
	swap_data(B_UINT32_TYPE, &(fHeader.magic), sizeof(uint32), B_SWAP_HOST_TO_BENDIAN);
	swap_data(B_RECT_TYPE, &(fHeader.bounds), sizeof(BRect), B_SWAP_HOST_TO_BENDIAN);
	swap_data(B_UINT32_TYPE, &(fHeader.rowBytes), sizeof(uint32), B_SWAP_HOST_TO_BENDIAN);
	swap_data(B_UINT32_TYPE, &(fHeader.colors), sizeof(color_space), B_SWAP_HOST_TO_BENDIAN);
	swap_data(B_UINT32_TYPE, &(fHeader.dataSize), sizeof(uint32), B_SWAP_HOST_TO_BENDIAN);

	fPosition = 0;
	fForceErrors = false;
	fBar = false;
	fBarWin = 0;

	// Unused by us
	fBitmapInvoker = 0;
	fRectInvoker = 0;
	fDeltaBuffer = 0;
	fOrgBitmap = 0;
	fDisposeOrg = false;
	fKeepOrg = false;
	fDither = false;
	fSeeked = false;
	fHeaderWritten = 0;
}


// Constructor for when using the bitmap stream as destination
ProgressiveBitmapStream::ProgressiveBitmapStream(
	BInvoker *bitmap_created,
	BInvoker *rect_updated,
	bool dither,
	bool keep_org)
	: BPositionIO()
{
	fWriteOnly = true;

	// Init
	fBitmapInvoker = bitmap_created;
	fRectInvoker = rect_updated;
	fDither = dither;
	fKeepOrg = keep_org;

	fBitmap = 0;
	fOrgBitmap = 0;
	fPosition = 0;
	fSize = 0;
	fDisposePrim = true;
	fDisposeOrg = true;
	fForceErrors = false;
	fBar = false;
	fBarWin = 0;
	fSeeked = false;
	fDeltaBuffer = 0;
	fHeaderWritten = 0;

	// Get colour palette for 8-bit screens
	palette = system_colors()->color_list;
	index_list = system_colors()->index_map;
}


// Destructor
ProgressiveBitmapStream::~ProgressiveBitmapStream()
{
	delete fBitmapInvoker;
	delete fRectInvoker;
	delete[] fDeltaBuffer;

	if (fDisposePrim)
		delete fBitmap;
	if (fDisposeOrg)
		delete fOrgBitmap;

	if (fBarWin)
		fBarWin->PostMessage(B_QUIT_REQUESTED);
}


// ReadAt() - used when saving a bitmap
ssize_t
ProgressiveBitmapStream::ReadAt(off_t pos, void *buffer, size_t size)
{
	if (fWriteOnly || !fBitmap || pos >= fSize)
		return B_ERROR;

	// Truncate if someone tries to read too much
	if (size > fSize - pos)
		size = fSize - pos;

	// Read data from the header
	size_t from_header = 0;
	if (pos < sizeof(TranslatorBitmap))
	{
		from_header = sizeof(TranslatorBitmap) - pos;
		if (from_header > size)
			from_header = size;
		memcpy(buffer, &fHeader + pos, from_header);
	}

	// Read data from the bitmap
	size_t from_bitmap = size - from_header;
	if (from_bitmap > 0)
	{
		memcpy(
			static_cast<char *>(buffer) + from_header,
			(const char *)(fBitmap->Bits()) + pos + from_header - sizeof(TranslatorBitmap),
			from_bitmap);

		// Update the progress bar if there is one
		if (fBar)
		{
			if (!fBarWin)
			{
				fBarWin = new BarWindow(util.find_win_pos(350,75, util.mainWin),
					fBarTitle,
					fDataSize);
			}
			BMessage msg(UPDATE_BAR);	
			msg.AddFloat("delta", from_bitmap);
			fBarWin->PostMessage(&msg);
			if (fBarWin->StopPressed())
				fForceErrors = true;
		}
	}

	if (fForceErrors)
		return B_ERROR;

	// Return real size read
	return size;
}


// Write() implemented without using Seek()
ssize_t
ProgressiveBitmapStream::Write(const void *buffer, size_t size)
{
	ssize_t ret_val = WriteAt(fPosition, buffer, size);
	if (ret_val > 0)
		fPosition += ret_val;
	return ret_val;
}


// WriteAt() - used when loading a bitmap
ssize_t
ProgressiveBitmapStream::WriteAt(off_t pos, const void *buffer, size_t size)
{
	if (!fWriteOnly)
		return B_ERROR;

	if (pos != fPosition)
		fSeeked = true;

	// Write data to the header
	size_t to_header = 0;
	if (pos < sizeof(TranslatorBitmap))
	{
		to_header = sizeof(TranslatorBitmap) - pos;
		if (to_header > size)
			to_header = size;
		// It is not allowed to write to the header
		// after a bitmap has been allocated
		if (fBitmap && to_header > 0)
			return B_ERROR;
		memcpy(&fHeader + pos, buffer, to_header);

		// We assume that when 32 bytes have been written to the header,
		// the header is complete and the bitmap may be allocated
		fHeaderWritten += to_header;
		if (fHeaderWritten >= sizeof(TranslatorBitmap))
		{
			// First convert the header to the correct endianess
			swap_data(B_UINT32_TYPE, &(fHeader.magic), sizeof(uint32), B_SWAP_BENDIAN_TO_HOST);
			swap_data(B_RECT_TYPE, &(fHeader.bounds), sizeof(BRect), B_SWAP_BENDIAN_TO_HOST);
			swap_data(B_UINT32_TYPE, &(fHeader.rowBytes), sizeof(uint32), B_SWAP_BENDIAN_TO_HOST);
			swap_data(B_UINT32_TYPE, &(fHeader.colors), sizeof(color_space), B_SWAP_BENDIAN_TO_HOST);
			swap_data(B_UINT32_TYPE, &(fHeader.dataSize), sizeof(uint32), B_SWAP_BENDIAN_TO_HOST);

			// Some rigorous checks
			if (fHeader.magic != B_TRANSLATOR_BITMAP ||
				!fHeader.bounds.IsValid() ||
			    fHeader.dataSize != (fHeader.bounds.bottom -
			    	fHeader.bounds.top + 1) * fHeader.rowBytes)
			{
				return B_ERROR;
			}		

			if (fHeader.colors != B_RGB32)
				fDither = false;

			fHeader.bounds.OffsetTo(0.0, 0.0);

			// Allocate the bitmap(s)
			if (fDither)
			{
				fBitmap = new BBitmap(fHeader.bounds, B_GRAY8);
				if (fKeepOrg)
					fOrgBitmap = new BBitmap(fHeader.bounds, fHeader.colors);
			}
			else
			{
				fBitmap = new BBitmap(fHeader.bounds, fHeader.colors);
			}

			SetExtras();

			// Send 'bitmap created' message
			if (fBitmapInvoker)
			{
				BMessage msg(*fBitmapInvoker->Message());
				msg.AddPointer("bitmap", fBitmap);
				if (fOrgBitmap)
					msg.AddPointer("org_bitmap", fOrgBitmap);
				fBitmapInvoker->Invoke(&msg);
			}
		}
	}

	// Write data to the bitmap
	size_t to_bitmap = size - to_header;
	if (to_bitmap > 0)
	{
		if (!fBitmap)
			return B_ERROR;

		// Truncate if someone tries to write too much
		if (size > fSize - pos)
		{
			to_bitmap -= size - fSize + pos;
			size = fSize - pos;
		}

		off_t in_bitmap = pos + to_header - sizeof(TranslatorBitmap);
		if (fDither)
		{
			if (fKeepOrg)
			{
				memcpy(
					static_cast<char *>(fOrgBitmap->Bits()) + in_bitmap,
					static_cast<const char *>(buffer) + to_header,
					to_bitmap);
			}
			Dither(
				static_cast<const uchar *>(buffer) + to_header,
				in_bitmap,
				to_bitmap);
		}
		else
		{
			memcpy(
				static_cast<char *>(fBitmap->Bits()) + in_bitmap,
				static_cast<const char *>(buffer) + to_header,
				to_bitmap);
		}

		// Send 'rect updated' message
		if (fRectInvoker)
		{
			// First calculate the rect that needs updating
			int32 top = in_bitmap / fHeader.rowBytes;
			int32 left = in_bitmap % fHeader.rowBytes;
			int32 right = left + to_bitmap - 1;
			int32 bottom = top + (right / fHeader.rowBytes);

			if (bottom == top)
			{
				left /= int32(fBytesPerPixel);
				right /= int32(fBytesPerPixel);
			}
			else
			{
				left = 0;
				right = int32(fHeader.bounds.right);
			}

			BMessage msg(*fRectInvoker->Message());
			msg.AddRect("rect", BRect(left, top, right, bottom));
			fRectInvoker->Invoke(&msg);
		}

		// Update the progress bar if there is one
		if (fBar)
		{
			if (!fBarWin)
			{
			
			fBarWin = new BarWindow(util.find_win_pos(350,75, util.mainWin),
					fBarTitle,
					fHeader.dataSize);
			}
			BMessage msg(UPDATE_BAR);
			msg.AddFloat("delta", to_bitmap); //beep();
			fBarWin->PostMessage(&msg);
			
			
			if (fBarWin->StopPressed())
				fForceErrors = true;
		}
		
	}

	if (fForceErrors)
		return B_ERROR;

	// Return real size written
	return size;
}


// Seek()
off_t
ProgressiveBitmapStream::Seek(off_t position, uint32 seek_mode)
{
	switch(seek_mode)
	{
		case SEEK_SET:
			break;

		case SEEK_CUR:
			position += fPosition;
			break;

		case SEEK_END:
			position += fSize;
			break;

		default:
			return B_ERROR;
			break;
	}

	if (position < 0 || position > fSize)
		return B_ERROR;

	if (fPosition != position)
		fSeeked = true;

	fPosition = position;
	return fPosition;
}


// Position()
off_t
ProgressiveBitmapStream::Position() const
{
	return fPosition;
}


// SetSize()
status_t
ProgressiveBitmapStream::SetSize(off_t numBytes)
{
	if (fBitmap && numBytes != fSize)
		return B_ERROR;

	return B_OK;
}


// Delete the bitmap at destruction time?
void
ProgressiveBitmapStream::SetDispose(bool primary, bool original)
{
	fDisposePrim = primary;
	fDisposeOrg = original;
}


// A simple way to retrieve the bitmap.
// You don't really need it if you use the bitmap_created invoker.
BBitmap *
ProgressiveBitmapStream::Bitmap() const
{
	return fBitmap;
}


// If you set the keep_org flag and there was dithering,
// there might be an original 32-bit bitmap to retrieve.
BBitmap *
ProgressiveBitmapStream::OriginalBitmap()
{
	fDisposeOrg = false;
	return fOrgBitmap;
}


// Causes each read/write call to return errors.
// Ideal for aborting a data translation in progress.
// If the translator chooses to ignore the errors,
// everything will still work fine.
void
ProgressiveBitmapStream::ForceErrors()
{
	fForceErrors = true;
}


// Check if the errors were forced. It can be switched on
// from within the class if the user presses the stop
// button in the progress window.
bool
ProgressiveBitmapStream::ErrorsForced() const
{
	return fForceErrors;
}


// Display a progress bar as data is read/written to/from the stream
void
ProgressiveBitmapStream::DisplayProgressBar( const char *window_title)
{
	fBar = true;
	//fBarLeftTop = left_top;
	fBarTitle[80] = '\0';
	strncpy(fBarTitle, window_title, 80);
}


// Private function to set some bitmap specific stuff
void
ProgressiveBitmapStream::SetExtras()
{
	fSize = sizeof(TranslatorBitmap) + fHeader.dataSize;

	switch (fHeader.colors)
	{
		case B_RGB32:
		case B_RGBA32:
			fBytesPerPixel = 4.0;
			break;

		case B_RGB16:
		case B_RGB15:
		case B_RGBA15:
			fBytesPerPixel = 2.0;
			break;

		case B_GRAY1:
			fBytesPerPixel = 1.0 / 8.0;
			break;

		default:  // 8-bit
			fBytesPerPixel = 1.0;
			break;
	}

	if (fDither)
	{
		int32 size = int32(3 * (fHeader.bounds.Width() + 3));
		fDeltaBuffer = new float[size];
		for (int32 i = 0; i < size; ++i)
			fDeltaBuffer[i] = 0.0;
	}
}


// Private function to dither 32-bit data to an 8-bit bitmap
void
ProgressiveBitmapStream::Dither(const uchar *src, int32 place, int32 n_bytes)
{
	// Where are we?
	int y_pos = place / fHeader.rowBytes;
	int x_pos = place % fHeader.rowBytes;

	int skip = 4 - x_pos % 4;
	if (skip < 4)
	{
		src += skip;
		n_bytes -= skip;
	}

	x_pos /= 4;
	int x_len = fHeader.rowBytes / 4;
	int row_bytes = fBitmap->BytesPerRow();

	uchar *in_bitmap = static_cast<uchar *>(fBitmap->Bits()) + y_pos * row_bytes + x_pos;
	float *deltas = &fDeltaBuffer[x_pos * 3];

	// Work our way through the data row by row
	while (n_bytes > 2)
	{
		float r_delta = 0.0;
		float g_delta = 0.0;
		float b_delta = 0.0;

		// Do all pixels in one row
		for (int x = x_pos; x < x_len && n_bytes > 2; ++x)
		{
			int r;
			int g;
			int b;

			// Get a pixel from the input buffer and
			// add corrections from the pixels around it
			if (fSeeked)
			{
				// Can't use the delta buffer,
				// so we'll only use horizontal error diffusion
				r = (int32) (src[2] + r_delta);
				g = (int32) src[1] + (int32) g_delta;
				b = (int32) src[0] + (int32) b_delta;
			}
			else
			{
				// Floyd-Steinberg diffusion
				r = int (src[2] + r_delta * 0.4375 + deltas[0] * 0.0625 + deltas[3] * 0.3125 + deltas[6] * 0.1875);
				g = int (src[1] + g_delta * 0.4375 + deltas[1] * 0.0625 + deltas[4] * 0.3125 + deltas[7] * 0.1875);
				b = int (src[0] + b_delta * 0.4375 + deltas[2] * 0.0625 + deltas[5] * 0.3125 + deltas[8] * 0.1875);
			}

			// Store deltas for the current pixel
			deltas[0] = r_delta;
			deltas[1] = g_delta;
			deltas[2] = b_delta;
			deltas += 3;

			// Fix high and low values
			r = r > 255 ? 255 : r;
			r = r <   0 ?   0 : r;
			g = g > 255 ? 255 : g;
			g = g <   0 ?   0 : g;
			b = b > 255 ? 255 : b;
			b = b <   0 ?   0 : b;

			// Lets see which colour we get from the BeOS palette
			uchar colour = index_list[((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3)];

			// Output the pixel
			*in_bitmap = colour;
			++in_bitmap;

			// Remember the differences with the requested color
			r_delta = r - palette[colour].red;
			g_delta = g - palette[colour].green;
			b_delta = b - palette[colour].blue;

			src += 4;
			n_bytes -= 4;
		}
		in_bitmap += row_bytes - x_len;  // Skip bytes at row end
		x_pos = 0;
		deltas = fDeltaBuffer;
		fSeeked = false;
	}
}
