/**
	@file
	@brief 
*/

/*
 * CC PDF Converter: Windows PDF Printer with Creative Commons license support
 * Copyright (C) 2007 Guy Hachlili <hguy@cogniview.com>, Cogniview LTD.
 * 
 * This file is part of CC PDF Converter.
 * 
 * CC PDF Converter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CC PDF Converter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. * 
 */

#include "precomp.h"
#include "PngImage.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>

/**
	
*/
PngImage::PngImage() : m_pData(NULL), m_nWidth(0), m_nHeight(0), m_nBitsPerPixel(0), m_nBytesPerRow(0)
{
}

/**
	@param lpFilename Name of file to load
	@param bForce8Bit true to force the image to be loaded as 8-bit color image
*/
PngImage::PngImage(LPCTSTR lpFilename, bool bForce8Bit /* = true */) : m_pData(NULL), m_nWidth(0), m_nHeight(0), m_nBitsPerPixel(0), m_nBytesPerRow(0)
{
	// Call file-loading function
	LoadFromFile(lpFilename, bForce8Bit);
}

/**
	@param uResourceID ID of PNG image resource
	@param bForce8Bit true to force the image to be loaded as 8-bit color image
	@param hModule Handle of module that the resource belongs to
	@param lpType Type of resource
*/
PngImage::PngImage(UINT uResourceID, bool bForce8Bit /* = true */, HMODULE hModule /* = NULL */, LPCTSTR lpType /* = _T("PNG") */) : m_pData(NULL), m_nWidth(0), m_nHeight(0), m_nBitsPerPixel(0), m_nBytesPerRow(0)
{
	// Call resource-loading function
	LoadFromResource(uResourceID, bForce8Bit, hModule, lpType);
}

/**
	@param lpFilename Name of file to load
	@param bForce8Bit true to force the image to be loaded as 8-bit color image
	@param bLeaveGray false to force loading the image in color, true to leave as grayscale if was so
	@return true if loaded successfully, false if failed
*/
bool PngImage::LoadFromFile(LPCTSTR lpFilename, bool bForce8Bit /* = true */, bool bLeaveGray /* = false */)
{
	// Load file to memory, and call LoadFromBuffer:

	// Get file size
	struct _stat st;
	if (_tstat(lpFilename, &st) != 0)
		// Isn't there!
		return false;

	if (st.st_mode & _S_IFDIR)
		// Not a file
		return false;

	DWORD dwSize = st.st_size;

	// Open the file itself
	int nFile = _topen(lpFilename, _O_BINARY | _O_RDONLY);
	if (nFile == -1)
		// Failed!
		return false;

	// Create a buffer
	char* pBuffer = new char[dwSize];
	// Load the data
	if (_read(nFile, pBuffer, dwSize) != (int)dwSize)
	{
		// Failed!
		delete [] pBuffer;
		_close(nFile);
		return false;
	}

	// OK, finish up
	_close(nFile);

	// And create the image
	bool bRet = LoadFromBuffer(pBuffer, dwSize, bForce8Bit, bLeaveGray);

	// Clean up
	delete [] pBuffer;
	return bRet;
}

/**
	@param uResourceID ID of PNG image resource
	@param bForce8Bit true to force the image to be loaded as 8-bit color image
	@param hModule Handle of module that the resource belongs to
	@param lpType Type of resource
	@param bLeaveGray false to force loading the image in color, true to leave as grayscale if was so
	@return true if loaded successfully, false if failed
*/
bool PngImage::LoadFromResource(UINT uResourceID, bool bForce8Bit /* = true */, HMODULE hModule /* = NULL */, LPCTSTR lpType /* = _T("PNG") */, bool bLeaveGray /* = false */)
{
	// Find the resource
	HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(uResourceID), lpType);
	if (hResource == NULL)
		// Failed
		return false;

	// Get size
	DWORD dwSize = SizeofResource(hModule, hResource);
	if (dwSize == 0)
		// Failed
		return false;

	// Load and lock
	HGLOBAL hData = LoadResource(hModule, hResource);
	if (hData == NULL)
		return false;
	LPVOID lpData = LockResource(hData);
	if (lpData == NULL)
		return false;

	// OK, just create an image from the data:
	return LoadFromBuffer((const char*)lpData, dwSize, bForce8Bit, bLeaveGray);
}

/**
    @brief Structure containing buffer data (used in the callback
*/
struct PngReadInfo
{
	// Ctor
	/**
		@param p Pointer to the data buffer
		@param dw Size of data buffer
	*/
	PngReadInfo(const char* p, DWORD dw) : pBuffer(p), dwLen(dw) {};

	// Members
	/// Pointer to the buffer
	const char* pBuffer;
	/// Size of remaining buffer
	DWORD dwLen;
};

/**
	@param pPng Pointer to PNG data buffer
	@param pBuffer Pointer to the buffer to fill
	@param nSize Size of the buffer to fill
*/
void PNGAPI PngImage::StaticRead(png_structp pPng, png_bytep pBuffer, png_size_t nSize)
{
	// Get the read info structure pointer:
	PngReadInfo* pInfo = (PngReadInfo*)png_get_io_ptr(pPng);
	if (pInfo == NULL)
		png_error(pPng, "StaticRead error 1");
	if (nSize > pInfo->dwLen)
		png_error(pPng, "Not enough input");

	// OK, copy the data
	memcpy(pBuffer, pInfo->pBuffer, nSize);

	// Move along
	pInfo->pBuffer += nSize;
	pInfo->dwLen -= nSize; 

}

/**
	@param pBuffer Pointer to the image data
	@param dwLen Size of the image data
	@param bForce8Bit true to force the image to be loaded as 8-bit color image
	@param bLeaveGray false to force loading the image in color, true to leave as grayscale if was so
	@return true if loaded successfully, false if failed
*/
bool PngImage::LoadFromBuffer(const char* pBuffer, DWORD dwLen, bool bForce8Bit /* = true */, bool bLeaveGray /* = false */)
{
	// Clean up
	clear();
	if (dwLen < 8)
		return false;

	// Make sure this is a PNG in the buffer
	if (!png_check_sig((png_bytep)pBuffer, 8))
		return false;

	pBuffer += 8;
	dwLen -= 8;

	//create pPng and info_png
	png_structp pPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (pPng == NULL)
		return false;

	// Initialize the info_png buffers
	png_infop pPngInfo = png_create_info_struct(pPng);
	if (pPngInfo == NULL)
	{
		png_destroy_read_struct(&pPng, NULL, NULL); 
		return false;
	}

    png_infop pPngEndInfo = png_create_info_struct(pPng);
    if (pPngEndInfo == NULL)
    {
        png_destroy_read_struct(&pPng, &pPngInfo, NULL);
        return false;
    }

	if (setjmp(png_jmpbuf(pPng))) 
	{
		// problem:
		png_destroy_read_struct(&pPng, &pPngInfo, &pPngEndInfo);
		return false;
	}

	try
	{
		// Read the buffer and create an image:
		PngReadInfo info(pBuffer, dwLen);
		png_set_read_fn(pPng, &info, StaticRead);

		png_set_sig_bytes(pPng, 8);  // we already read the 8 signature bytes 
		png_read_info(pPng, pPngInfo);  // read all PNG info up to image data 

		int bitDepth, colorType;
		ULONG width, height;
		png_get_IHDR(pPng, pPngInfo, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);

		m_nWidth = width;
		m_nHeight = height;  

		// apply filters to image so we can get proper image data format
		if (colorType == PNG_COLOR_TYPE_PALETTE)
			png_set_palette_to_rgb(pPng);
		if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
			png_set_gray_1_2_4_to_8(pPng);
		if (png_get_valid(pPng, pPngInfo, PNG_INFO_tRNS))
			png_set_tRNS_to_alpha(pPng);

		if (bitDepth == 16)
		{
			if(bForce8Bit)
				png_set_strip_16(pPng); //make 8-bit
			else
				png_set_swap(pPng); //swap endian order (PNG is Big Endian)
		}

		if (bForce8Bit) // force8bit depth per channel
			bitDepth = 8;

		int byteDepth = bitDepth/8;

	    if (colorType & PNG_COLOR_MASK_ALPHA)
		    png_set_strip_alpha(pPng);

		if (((colorType == PNG_COLOR_TYPE_GRAY) || (colorType == PNG_COLOR_TYPE_GRAY_ALPHA)) && !bLeaveGray)
			png_set_gray_to_rgb(pPng);

		png_read_update_info(pPng, pPngInfo);

		// Now, create the actual image:
		// Calculate data
		m_nBytesPerRow = png_get_rowbytes(pPng, pPngInfo);
		m_nBitsPerPixel = (m_nBytesPerRow / m_nWidth) * bitDepth;
		// Create buffers and set them up
		m_pData = new BYTE[m_nBytesPerRow * m_nHeight];
		BYTE** pRowPointers = new BYTE*[m_nHeight];
		for (int i = 0;  i < m_nHeight;  ++i)
			pRowPointers[i] = m_pData + i*m_nBytesPerRow;

		// Read the data
		png_read_image(pPng, pRowPointers);
		png_read_end(pPng, pPngEndInfo);
		png_destroy_read_struct(&pPng, &pPngInfo, &pPngEndInfo);

		delete [] pRowPointers;
	}
	catch(...)
	{
		// Failed, clean up
		clear();
		return false;
	}

	// All ends well
	return true;
}

/**
	
*/
void PngImage::clear()
{
	if (m_pData != NULL)
	{
		delete [] m_pData;
		m_pData = NULL;
	}
}

/**
	@param hDC Handle to DC to use when creating the compatible bitmap
	@return Handle to the created bitmap, or NULL if failed
*/
HBITMAP PngImage::ToBitmap(HDC hDC)
{
	// Do we have any data?
	if (m_pData == NULL)
		// Nope, nothing to create
		return NULL;

	// OK, set up the header
	BITMAPINFO info;
	info.bmiHeader.biSize = sizeof(info);
	info.bmiHeader.biWidth = m_nWidth;
	info.bmiHeader.biHeight = -m_nHeight;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = m_nBitsPerPixel;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = 0;
	info.bmiHeader.biXPelsPerMeter = (long)(72 * 100 / 2.56);
	info.bmiHeader.biYPelsPerMeter = (long)(72 * 100 / 2.56);
	info.bmiHeader.biClrUsed = 0;
	info.bmiHeader.biClrImportant = 0;

	// Create the bitmap
	HBITMAP hBmp = CreateCompatibleBitmap(hDC, m_nWidth, m_nHeight);
	// And fill it
	SetDIBits(hDC, hBmp, 0, m_nHeight, m_pData, &info, DIB_RGB_COLORS);

	// That's it:
	return hBmp;
}
