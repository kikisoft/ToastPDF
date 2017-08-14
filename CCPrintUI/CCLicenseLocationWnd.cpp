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
#include "CCLicenseLocationWnd.h"

#include "PngImage.h"
#include "globals.h"
#include "debug.h"
#include "resource.h"
#include <windowsx.h>

/// Windows ATOM holding the Windows class registration (valid while there is at least one active window)
ATOM CCLicenseLocationWnd::m_aClass = 0;
/// Count of active windows
int CCLicenseLocationWnd::m_nCount = 0;

/**
	
*/
CCLicenseLocationWnd::~CCLicenseLocationWnd()
{
	// Unregister the class
	UnregisterWindowClassNow();
	// Clean up the bitmap
	if (m_hBmp != NULL)
		::DeleteObject(m_hBmp);
}

/**
	@param lpName Name of new window
	@param dwStyle Style of new window
	@param rect Location of new window (in parent co-ordinates)
	@param hParent Handle to parent window
	@return TRUE if created successfully, false if failed
*/
BOOL CCLicenseLocationWnd::CreateWnd(LPCTSTR lpName, DWORD dwStyle, const RECT& rect, HWND hParent)
{
	// Register the class (if necessary)
	if (!RegisterWindowClass())
		return FALSE;

	// Create the actual window
	HWND hWnd = ::CreateWindow((LPCTSTR)m_aClass, lpName, dwStyle, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hParent, NULL, ghInstance, (void*)this);
	if (hWnd == NULL)
	{
		// Failed, decrease the count (will delete later)
		UnregisterWindowClass();
		return FALSE;
	}

	// We should have already received a WM_CREATE message, so make sure:
	ASSERT (hWnd == m_hWnd);
	return TRUE;
}

/**
	@return true if the window class was registered successfully, false if failed
*/
bool CCLicenseLocationWnd::RegisterWindowClass()
{
	// Did we register it already?
	if (m_aClass != 0)
	{
		// Yeah, just increase the count
		m_nCount++;
		return true;
	}

	// This should NEVER happen:
	if (m_nCount != 0)
		return false;

	// OK, register the class:
	WNDCLASS info;
	info.style = CS_DBLCLKS;
	info.lpfnWndProc = WndProc;
	info.cbClsExtra = 0;
	info.cbWndExtra = 0;
	info.hInstance = ghInstance;
	info.hIcon = NULL;
	info.hCursor = LoadCursor(NULL, IDC_ARROW);
	info.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	info.lpszMenuName = NULL;
	info.lpszClassName = _T("CCLicenseLocationWnd");
	m_aClass = ::RegisterClass(&info);
	if (m_aClass == 0)
		return false;

	// OK, go on
	m_nCount = 1;
	return true;
}

/**
	
*/
void CCLicenseLocationWnd::UnregisterWindowClass()
{
	// Don't go overboard
	if (m_nCount == 0)
		return;

	// Just decrese the count
	m_nCount--;
}

/**
	
*/
void CCLicenseLocationWnd::UnregisterWindowClassNow()
{
	// Don't do anything if we have active windows
	if (m_nCount > 0)
		return;

	// OK, remove the class
	if (!::UnregisterClass((LPCTSTR)m_aClass, ghInstance))
		return;

	m_aClass = 0;
}

/**
	@param hWnd Handle to the window
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
LRESULT CALLBACK CCLicenseLocationWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CCLicenseLocationWnd* pThis = NULL;
	if (uMsg == WM_CREATE)
	{
		// First message received: retrieve the object pointer
		CREATESTRUCT* pStruct = (CREATESTRUCT*)lParam;
		pThis = (CCLicenseLocationWnd*)pStruct->lpCreateParams;

		// It should NOT be connected yet:
		ASSERT(pThis->m_hWnd == NULL);

		// Connect it
		pThis->m_hWnd = hWnd;
#ifdef _WIN64
		SetWindowLong(hWnd, GWLP_USERDATA, (LONG)pThis);
#else
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)pThis);
#endif
		
	}
	else
		// OK, just retrieve the object pointer
#ifdef _WIN64
		pThis = (CCLicenseLocationWnd*)GetWindowLong(hWnd, GWLP_USERDATA);
#else
		pThis = (CCLicenseLocationWnd*)GetWindowLong(hWnd, GWL_USERDATA);
#endif

	if (pThis == NULL)
		// Bad pointer, let the default window handling occur
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	// Call the handling function
	LRESULT lRet = pThis->ProcessMessage(uMsg, wParam, lParam);

	if (uMsg == WM_NCDESTROY)
	{
		// Last message, we want to disconnect
		pThis->m_hWnd = NULL;
		pThis->PostDestroy();
	}

	return lRet;
}

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
LRESULT CCLicenseLocationWnd::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
			// Calculate the initial locations:
			CREATESTRUCT* pStruct = (CREATESTRUCT*)lParam;
			double dRatio = ((double)pStruct->cx) / pStruct->cy;
			double dPageRatio = ((double)m_szPage.cx) / m_szPage.cy;
			if (dRatio < dPageRatio)
			{
				// Full horizontal
				m_rectPageView.left = 5;
				m_rectPageView.right = pStruct->cx - 5;
				int nHeight = (m_szPage.cy * (m_rectPageView.right - m_rectPageView.left)) / m_szPage.cx;
				m_rectPageView.top = (pStruct->cy - nHeight) / 2;
				m_rectPageView.bottom = pStruct->cy - m_rectPageView.top;
			}
			else
			{
				// Full vertical
				m_rectPageView.top = 5;
				m_rectPageView.bottom = pStruct->cy - 5;
				int nWidth = (m_szPage.cx * (m_rectPageView.bottom - m_rectPageView.top)) / m_szPage.cy;
				m_rectPageView.left = (pStruct->cx - nWidth) / 2;
				m_rectPageView.right = pStruct->cx - m_rectPageView.left;
			}

			// Load the stamp image
			PngImage png;
			if (png.LoadFromResource(IDPNG_SOMERIGHTS, true, ghInstance))
			{
				HDC hDC = ::GetDC(m_hWnd);
				m_szBitmap.cx = png.GetWidth();
				m_szBitmap.cy = png.GetHeight();
				m_hBmp = png.ToBitmap(hDC);
				::ReleaseDC(m_hWnd, hDC);
			}
			
			// Calculate the license view location:
			CalcLicenseViewLocation();
			break;
		}
		case WM_PAINT:
		{
			// Draw the page borders and the stamp:
			// First we need to know where to draw
			RECT rect;
			if (!GetUpdateRect(m_hWnd, &rect, FALSE))
				break;

			// Start the painting
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(m_hWnd, &ps);
			int nSaveDC = ::SaveDC(hDC);

			// OK, for now, do a rectangle for the page
			::SelectObject(hDC, GetStockObject(BLACK_PEN));
			::SelectObject(hDC, GetStockObject(WHITE_BRUSH));
			::Rectangle(hDC, m_rectPageView.left, m_rectPageView.top, m_rectPageView.right, m_rectPageView.bottom);

			// Draw the license...
			BOOL bDrawn = FALSE;
			if (m_hBmp != NULL)
			{
				HDC hBmpDC = ::CreateCompatibleDC(hDC);
				if (hBmpDC != NULL)
				{
					HGDIOBJ hOldBmp = ::SelectObject(hBmpDC, m_hBmp);
					bDrawn = ::StretchBlt(hDC, m_rectLicenseView.left, m_rectLicenseView.top, m_rectLicenseView.right - m_rectLicenseView.left, m_rectLicenseView.bottom - m_rectLicenseView.top, hBmpDC, 0, 0, m_szBitmap.cx, m_szBitmap.cy, SRCCOPY);
					::SelectObject(hBmpDC, hOldBmp);
					::DeleteDC(hBmpDC);
				}
			}
			if (!bDrawn)
				::Rectangle(hDC, m_rectLicenseView.left, m_rectLicenseView.top, m_rectLicenseView.right, m_rectLicenseView.bottom);

			// And the page
			::SelectObject(hDC, GetStockObject(NULL_PEN));
			::SelectObject(hDC, GetStockObject(GRAY_BRUSH));
			::Rectangle(hDC, m_rectPageView.right, m_rectPageView.top + 3, m_rectPageView.right + 5, m_rectPageView.bottom + 5);
			::Rectangle(hDC, m_rectPageView.left + 3, m_rectPageView.bottom, m_rectPageView.right + 5, m_rectPageView.bottom + 5);

			// That's it
			::RestoreDC(hDC, nSaveDC);
			EndPaint(m_hWnd, &ps);
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			// Start dragging?
			if (m_bDragging)
				// Already dragging
				break;

			// OK, where did the user click?
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			if (!PtInRect(&m_rectLicenseView, pt))
				// Outside of the stamp
				break;

			// Calculate the offset
			m_szOffset.cx = m_rectLicenseView.left - pt.x;
			m_szOffset.cy = m_rectLicenseView.top - pt.y;

			// Start dragging
			SetCapture(m_hWnd);
			m_bDragging = true;
			return 0;
		}

		case WM_MOUSEMOVE:
			// User moved the mouse
			// Are we dragging now?
			if (m_bDragging)
			{
				// Yeah, move the license
				POINT pt;
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				MoveLicense(pt);
				return 0;
			}
			break;

		case WM_LBUTTONUP:
			// User release the mouse button
			// Are we dragging?
			if (m_bDragging)
			{
				// Yes, move the license...
				POINT pt;
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				MoveLicense(pt);

				// ... and stop dragging
				ReleaseCapture();
				m_bDragging = false;
				return 0;
			}
			break;

		case WM_NCDESTROY:
			// Finished, unregister now
			UnregisterWindowClass();
			break;
	}

	// Default window handling
	return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

/**
	
*/
void CCLicenseLocationWnd::CalcLicenseViewLocation()
{
	// Calculate the drawing location of the stamp, relative to the page location, in client co-ordinates
	m_rectLicenseView.left = m_rectPageView.left + (((m_rectPageView.right - m_rectPageView.left) * m_rectLocation.left) / m_szPage.cx);
	m_rectLicenseView.right = m_rectPageView.left + (((m_rectPageView.right - m_rectPageView.left) * m_rectLocation.right) / m_szPage.cx);
	m_rectLicenseView.top = m_rectPageView.top + (((m_rectPageView.bottom - m_rectPageView.top) * m_rectLocation.top) / m_szPage.cy);
	m_rectLicenseView.bottom = m_rectPageView.top + (((m_rectPageView.bottom - m_rectPageView.top) * m_rectLocation.bottom) / m_szPage.cy);
}

/**
	@param pt The location to put the stamp at (client co-ordinates)
*/
void CCLicenseLocationWnd::MoveLicense(POINT pt)
{
	// Calculate the new location
	RECT rect;
	::CopyRect(&rect, &m_rectLicenseView);
	::OffsetRect(&rect, pt.x + m_szOffset.cx - m_rectLicenseView.left, pt.y + m_szOffset.cy - m_rectLicenseView.top);
	
	// Now, move it back in the page if it's outside
	if (rect.top < m_rectPageView.top)
		::OffsetRect(&rect, 0, m_rectPageView.top - rect.top);
	else if (rect.bottom > m_rectPageView.bottom)
		::OffsetRect(&rect, 0, m_rectPageView.bottom - rect.bottom);
	if (rect.left < m_rectPageView.left)
		::OffsetRect(&rect, m_rectPageView.left - rect.left, 0);
	else if (rect.right > m_rectPageView.right)
		::OffsetRect(&rect, m_rectPageView.right - rect.right, 0);

	// Is this the same place?
	if (::EqualRect(&rect, &m_rectLicenseView))
		// Yeah, do nothing
		return;

	// Moved, so invalidate the regions
	RECT rectInvalid;
	UnionRect(&rectInvalid, &rect, &m_rectLicenseView);
	InvalidateRect(m_hWnd, &rectInvalid, FALSE);

	::CopyRect(&m_rectLicenseView, &rect);

	// Set the ACTUAL license location
	::OffsetRect(&m_rectLocation, (((m_rectLicenseView.left - m_rectPageView.left) * m_szPage.cx) / (m_rectPageView.right - m_rectPageView.left)) - m_rectLocation.left, (((m_rectLicenseView.top - m_rectPageView.top) * m_szPage.cy) / (m_rectPageView.bottom - m_rectPageView.top)) - m_rectLocation.top);
	// Send message to parent
	SendLicenseLocationMsg();
}

/**
	
*/
void CCLicenseLocationWnd::SendLicenseLocationMsg()
{
	DWORD dw = MAKEWPARAM(m_rectLocation.left, m_rectLocation.top);
	::PostMessage(GetParent(m_hWnd), UM_LICENSEMOVED, 0, dw);
}

/**
	@param uX The new X locaiton of the stamp (in page co-ordinates)
*/
void CCLicenseLocationWnd::SetXLocation(UINT uX)
{
	// Make sure the stamp stays in the page
	if (uX > m_szPage.cx - (m_rectLocation.right - m_rectLocation.left))
		uX = m_szPage.cx - (m_rectLocation.right - m_rectLocation.left);
	// OK, move the stamp
	::OffsetRect(&m_rectLocation, uX - m_rectLocation.left, 0);
	// And update the image
	UpdateLocation();
}

/**
	
*/
void CCLicenseLocationWnd::UpdateLocation()
{
	// Save the location
	RECT rect;
	::CopyRect(&rect, &m_rectLicenseView);
	// Calculate the new location
	CalcLicenseViewLocation();

	// Is this in the same place?
	if (!::EqualRect(&rect, &m_rectLicenseView))
	{
		// No, so invalidate the view
		RECT rectInvalid;
		UnionRect(&rectInvalid, &rect, &m_rectLicenseView);
		InvalidateRect(m_hWnd, &rectInvalid, FALSE);
	}

	// Notify parent about the move
	SendLicenseLocationMsg();
}

/**
	@param uY The new Y locaiton of the stamp (in page co-ordinates)
*/
void CCLicenseLocationWnd::SetYLocation(UINT uY)
{
	// Make sure the stamp stays in the page
	if (uY > m_szPage.cy - (m_rectLocation.bottom - m_rectLocation.top))
		uY = m_szPage.cy - (m_rectLocation.bottom - m_rectLocation.top);
	// OK, move the stamp
	::OffsetRect(&m_rectLocation, 0, uY - m_rectLocation.top);
	// And update the image
	UpdateLocation();
}
