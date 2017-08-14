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
#include "CCLicenseWizModificationPage.h"

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCLicenseWizModificationPage::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
			// Handle clicks and such
			if (HIWORD(wParam) == BN_CLICKED)
			{
				UINT uID = LOWORD(wParam);
				if ((uID >= IDC_DERIVATIVE_ALL) && (uID <= IDC_DERIVATIVE_NO))
				{
					// The user selected a modification type, update the controls:
					UpdateControls();
					return TRUE;
				}
			}
			break;
		case WM_INITDIALOG:
			// Initial display, set the description in the box:
			SetDescriptionFromField(LicenseInfo::LLDTCC, _T("derivatives"));
			break;
	}

	return CCLicenseWizBasePage::PageProc(uMsg, wParam, lParam);
}

/**
	
*/
void CCLicenseWizModificationPage::UpdateControls()
{
	// What is selected?
	int nSel = GetCheckedRadioBtn(IDC_DERIVATIVE_ALL, IDC_DERIVATIVE_NO);
	if (nSel == -1)
	{
		// Nothing, don't allow moving forward
		SetWizardButtons(PSWIZB_BACK);
	}
	else
	{
		// Something, we allow moving on
		SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}
}

/**
	@return TRUE
*/
BOOL CCLicenseWizModificationPage::OnSetActive()
{
	// Set the selected radio button according to the license information (if set)
	if (m_pLicenseInfo->m_eModification != LicenseInfo::MTUnknown)
		CheckRadioButton(m_hDlg, IDC_DERIVATIVE_ALL, IDC_DERIVATIVE_NO, IDC_DERIVATIVE_ALL + (int)m_pLicenseInfo->m_eModification);

	// Update the description text
	UpdateControls();
	return TRUE;
}

/**
	@return TRUE (allow hidding) if a modification type is selected, FALSE otherwise
*/
BOOL CCLicenseWizModificationPage::OnKillActive()
{
	// Which button is selected?
	int nSel = GetCheckedRadioBtn(IDC_DERIVATIVE_ALL, IDC_DERIVATIVE_NO);
	if (nSel == -1)
		// None selected
		m_pLicenseInfo->m_eModification = LicenseInfo::MTUnknown;
	else
		// Set the modification type to the selection
		m_pLicenseInfo->m_eModification = (LicenseInfo::ModificationType)nSel;

	// Only allow closing if the user selected a sampling type
	return (nSel != -1);
}

/**
	@param page Reference to the property page structure to fill
	@return true if the structure was filled succesfully, false otherwise
*/
bool CCLicenseWizModificationPage::PreparePage(PROPSHEETPAGE& page)
{
	// Call base class
	if (!CCLicenseWizBasePage::PreparePage(page))
		return false;

	// Set the header (only used in Wizard pages)
	page.dwFlags |= PSP_USEHEADERTITLE;
	page.pszHeaderTitle = MAKEINTRESOURCE(IDS_TITLE_LICENSE_MODIFICATION);
	return true;
}
