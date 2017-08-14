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
#include "CCPrintPropPage.h"

#include "globals.h"
#include "debug.h"

/**
	@param uResourceID ID of property page resource
	@param pHelper Pointer to the Print UI Core object
*/
CCPrintPropPage::CCPrintPropPage(UINT uResourceID, IPrintOemDriverUI* pHelper) : CCPrintDlg(uResourceID), m_pHelper(pHelper)
{
}

/**
	@param page Reference to the property page structure to fill
	@return true
*/
bool CCPrintPropPage::PreparePage(PROPSHEETPAGE& page)
{
	// Clean up the structure
    memset(&page, 0, sizeof(PROPSHEETPAGE));

	// Fill with information
    page.dwSize = sizeof(PROPSHEETPAGE);
    page.dwFlags = PSP_DEFAULT;
    page.hInstance = ghInstance;
    page.pszTemplate = MAKEINTRESOURCE(m_uResourceID);
    page.pfnDlgProc = (DLGPROC) StaticPageProc;
	page.lParam = (LPARAM)this;

	return true;
}

/**
	@param hDlg Handle of the page
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue sending the message to the parent
*/
BOOL APIENTRY CCPrintPropPage::StaticPageProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Is this a dialog initialization message?
    if (uMsg == WM_INITDIALOG)
    {
		// Yes, set up the pointer in the window structure and remember the handle
		PROPSHEETPAGE* pPage = (PROPSHEETPAGE*)lParam;
		CCPrintPropPage* pThis = (CCPrintPropPage*)pPage->lParam;
		ASSERT(pThis->m_hDlg == NULL);
		pThis->m_hDlg = hDlg;
#ifdef _WIN64
		pThis->SetWindowLong(DWLP_USER, (LPARAM)pThis);
#else
		pThis->SetWindowLong(DWL_USER, (LPARAM)pThis);
#endif
		
	}

	// Call the base class
	return StaticDlgProc(hDlg, uMsg, wParam, lParam);
}
