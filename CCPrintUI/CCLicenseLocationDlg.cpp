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
#include "CCLicenseLocationDlg.h"

#include <windowsx.h>

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCLicenseLocationDlg::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			{
				// Calculate the new position for the page display...
				HWND hPage = GetDlgItem(IDC_PLACEHOLDER);
				RECT rect;
				GetClientRect(hPage, &rect);
				MapWindowPoints(hPage, m_hDlg, (LPPOINT)&rect, 2);
				// Create the page dispaly window
				m_wndLocation.CreateWnd(_T("Location"), WS_VISIBLE|WS_CHILD, rect, m_hDlg);
				// Save the current location
				m_ptLocation = m_wndLocation.GetLicenseLocation();
				// Set the location in the edit boxes
				SetEditLocation();
			}
			return TRUE;

		case UM_LICENSEMOVED:
			// License location changed:
			m_ptLocation.x = GET_X_LPARAM(lParam);
			m_ptLocation.y = GET_Y_LPARAM(lParam);
			SetEditLocation();
			break;

		case WM_COMMAND:
			switch (HIWORD(wParam))
			{
				case EN_CHANGE:
					// Which control?
					switch (LOWORD(wParam))
					{
						case IDC_X:
							// X, so:
							if (!m_bSettingEdit)
							{
								// User changed, so get the number
								BOOL bTranslate;
								UINT uNum = GetDlgItemInt(m_hDlg, IDC_X, &bTranslate, FALSE);
								if (!bTranslate)
									// Not valid, so update it back
									SetEditLocation();
								else
									// Move the stamp
									m_wndLocation.SetXLocation(uNum);
							}
							return 0;
						case IDC_Y:
							// Y, so:
							if (!m_bSettingEdit)
							{
								// User changed, so get the number
								BOOL bTranslate;
								UINT uNum = GetDlgItemInt(m_hDlg, IDC_Y, &bTranslate, FALSE);
								if (!bTranslate)
									// Not valid, so update it back
									SetEditLocation();
								else
									// Move the stamp
									m_wndLocation.SetYLocation(uNum);
							}
							return 0;
						default:
							break;
					}
			}
			break;
	}

	// Call base class for further handling
	return CCPrintDlg::PageProc(uMsg, wParam, lParam);
}

/**
	
*/
void CCLicenseLocationDlg::SetEditLocation()
{
	// Make sure we don't have a loop
	m_bSettingEdit = true;
	TCHAR c[20], cOld[20];

	// Set the X location
	_stprintf(c, _T("%u"), m_ptLocation.x);
	if ((GetDlgItemText(IDC_X, cOld, 20) == 0) || (_tcscmp(c, cOld) != 0))
		SetDlgItemText(IDC_X, c);

	// Set the Y locaiton
	_stprintf(c, _T("%u"), m_ptLocation.y);
	if ((GetDlgItemText(IDC_Y, cOld, 20) == 0) || (_tcscmp(c, cOld) != 0))
		SetDlgItemText(IDC_Y, c);

	// OK, finished here
	m_bSettingEdit = false;
}
