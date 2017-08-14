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

#ifndef _PNGIMAGE_H_
#define _PNGIMAGE_H_

#include <png.h> 

/**
    @brief Wrapper class for loading and displaying PNG images
*/
class PngImage
{
public:
	// Ctors/Dtor
	/// Default constructor
	PngImage();
	/// File loading constructor
	PngImage(LPCTSTR lpFilename, bool bForce8Bit = true);
	/// Resource loading constructor
	PngImage(UINT uResourceID, bool bForce8Bit = true, HMODULE hModule = NULL, LPCTSTR lpType = _T("PNG"));
	/**
		@brief Destructor: cleans up
	*/
	~PngImage() {clear();};

protected:
	// Members
	/// Width of the image (pixels)
	int 	    m_nWidth;
	/// Height of the image (lines)
	int 	    m_nHeight;
	/// Pointer to the image data
	BYTE* 		m_pData;
	/// Number of color bits per pixel
	int			m_nBitsPerPixel;
	/// Number of full bytes per image line
	int			m_nBytesPerRow;

public:
	// Data Access
	/**
		@brief Returns the image data array
		@return Pointer to the image data
	*/
	const BYTE*	GetBits() const {return m_pData;};
	/**
		@brief Returns the image width
		@return The image width
	*/
	int			GetWidth() const {return m_nWidth;};
	/**
		@brief Returns the image height
		@return The image height
	*/
	int			GetHeight() const {return m_nHeight;};
	/**
		@brief Returns the number of color bits per pixel
		@return The number of color bits per pixel
	*/
	int			GetBitsPerPixel() const {return m_nBitsPerPixel;};
	/**
		@brief Returns the number of full bytes in each image line
		@return The number of full bytes in each image line
	*/
	int			GetWidthInBytes() const {return m_nBytesPerRow;};

public:
	/// Loads a PNG image from a file
	bool		LoadFromFile(LPCTSTR lpFilename, bool bForce8Bit = true, bool bLeaveGray = false);
	/// Loads a PNG image from a resource
	bool		LoadFromResource(UINT uResourceID, bool bForce8Bit = true, HMODULE hModule = NULL, LPCTSTR lpType = _T("PNG"), bool bLeaveGray = false);
	/// Loads a PNG image from a buffer
	bool		LoadFromBuffer(const char* pBuffer, DWORD dwLen, bool bForce8Bit = true, bool bLeaveGray = false);

	/// cleans the loaded image
	void		clear();

	/// Returns a GDI bitmap containing the same image
	HBITMAP		ToBitmap(HDC hDC);

protected:
	/// PNG reader callback function
	static void PNGAPI StaticRead(png_structp pPng, png_bytep pBuffer, png_size_t nSize);
};

#endif   //#define _PNGIMAGE_H_
