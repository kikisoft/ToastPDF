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

#ifndef _CCLICENSELOCATIONWND_H_
#define _CCLICENSELOCATIONWND_H_

/// Message sent when the license stamp is moved
#define UM_LICENSEMOVED		(WM_USER + 0x3F1)

/**
    @brief License stamp location window class: displays the stamp and allows the user to move it
*/
class CCLicenseLocationWnd
{
public:
	// Ctor/Dtor
	/**
		@brief Contstructor
		@param szPage Size of the page
		@param rectLocation Initial location (and size) of the license stamp
	*/
	CCLicenseLocationWnd(SIZE szPage, RECT rectLocation) : m_hWnd(NULL), m_szPage(szPage), m_rectLocation(rectLocation), m_bDragging(false), m_hBmp(NULL) {};
	/// Destructor
	~CCLicenseLocationWnd();

protected:
	// Members
	/// Handle to the window
	HWND		m_hWnd;
	/// Actual size of the page (relative to the stamp)
	SIZE		m_szPage;
	/// Current location of the license stamp
	RECT		m_rectLocation;
	/// The page display location (in client co-ordinates)
	RECT		m_rectPageView;
	/// The license stamp display location (in client co-ordinates)
	RECT		m_rectLicenseView;
	/// Handle to license stamp image
	HBITMAP		m_hBmp;
	/// Size of the stamp image (in client units)
	SIZE		m_szBitmap;

	// Dragging members
	/// true will the user is dragging the stamp, false otherwise
	bool		m_bDragging;
	/// Offset of the mouse cursor from the top-left corner of the stamp (while dragging)
	SIZE		m_szOffset;

	// Static window registration members
	/// Windows ATOM holding the Windows class registration (valid while there is at least one active window)
	static ATOM	m_aClass;
	/// Count of active windows
	static int	m_nCount;


public:
	// Methods
	/// This function creates a license stamp location window
	BOOL		CreateWnd(LPCTSTR lpName, DWORD dwStyle, const RECT& rect, HWND hParent);
	/**
		@brief Retrieves the current license stamp location (in page co-ordinates)
		@return The location of the license stamp
	*/
	POINT		GetLicenseLocation() const {POINT pt; pt.x = m_rectLocation.left; pt.y = m_rectLocation.top; return pt;};

	/// Sets the X co-ordinates of the license stamp
	void		SetXLocation(UINT uX);
	/// Sets the Y co-ordinates of the license stamp
	void		SetYLocation(UINT uY);

protected:
	// Window Class registeration helpers
	/// Registers the license stamp location window class
	static bool	RegisterWindowClass();
	/// Unregisters the license stamp location window class (holds until all such windows are destroyed)
	static void	UnregisterWindowClass();
	/// Actually unregisters the license stamp location window class
	static void UnregisterWindowClassNow();

	// Helpers
	/// Calculates the stamp location in client co-ordinates
	void		CalcLicenseViewLocation();
	/// Moves the license stamp
	void		MoveLicense(POINT pt);
	/// Sends a license stamp location change message to the parent
	void		SendLicenseLocationMsg();
	/// Updates the view and the parent about changed location of the stamp
	void		UpdateLocation();

	// Overridables
	/// Called to handle windows messages
	virtual LRESULT ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	/**
		@brief Called when the window is destroyed
	*/
	virtual void PostDestroy() {};

	// Static callbacks
	/// Window message handling callback
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif   //#define _CCLICENSELOCATIONWND_H_
