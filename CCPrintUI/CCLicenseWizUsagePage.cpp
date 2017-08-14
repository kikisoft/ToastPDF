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
#include "CCLicenseWizUsagePage.h"

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCLicenseWizUsagePage::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
			// Handle clicks and such
			if (HIWORD(wParam) == BN_CLICKED)
			{
				UINT uID = LOWORD(wParam);
				if ((uID >= IDC_COMMERICAL_YES) && (uID <= IDC_COMMERICAL_NO))
				{
					// User has selected a usage type, update the page controls
					UpdateControls();
					return TRUE;
				}
			}
			break;
		case WM_INITDIALOG:
			// Initial display, set the description in the box:
			SetDescriptionFromField(LicenseInfo::LLDTCC, _T("commercial"));
			break;
	}

	// Call base class for default handling
	return CCLicenseWizBasePage::PageProc(uMsg, wParam, lParam);
}

/**
	@return TRUE
*/
BOOL CCLicenseWizUsagePage::OnSetActive()
{
	// Set the selected commerical use radio button
	CheckRadioButton(m_hDlg, IDC_COMMERICAL_YES, IDC_COMMERICAL_NO, IDC_COMMERICAL_YES + (m_pLicenseInfo->m_bCommercialUse ? 0 : 1));

	// Update the controls
	UpdateControls();

	return TRUE;
}

/**
	@return TRUE if the user has selected a usage type, FALSE if not
*/
BOOL CCLicenseWizUsagePage::OnKillActive()
{
	// Which button is selected?
	int nSel = GetCheckedRadioBtn(IDC_COMMERICAL_YES, IDC_COMMERICAL_NO);
	if (nSel == -1)
		// None - don't allow to close
		return FALSE;

	// Set the commercial usage flag
	m_pLicenseInfo->m_bCommercialUse = (nSel == 0);
	return TRUE;
}

/**
	
*/
void CCLicenseWizUsagePage::UpdateControls()
{
	// Get the currently selected radio button
	int nSel = GetCheckedRadioBtn(IDC_COMMERICAL_YES, IDC_COMMERICAL_NO);

	// Allow moving onwards only if one of them is selected
	SetWizardButtons(PSWIZB_BACK|((nSel == -1) ? 0 : PSWIZB_NEXT));
}

/**
	@param page Reference to the property page structure to fill
	@return true if the structure was filled succesfully, false otherwise
*/
bool CCLicenseWizUsagePage::PreparePage(PROPSHEETPAGE& page)
{
	// Call base class
	if (!CCLicenseWizBasePage::PreparePage(page))
		return false;

	// Set the header (only used in Wizard pages)
	page.dwFlags |= PSP_USEHEADERTITLE;
	page.pszHeaderTitle = MAKEINTRESOURCE(IDS_TITLE_LICENSE_USAGE);
	return true;
}
