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
#include "CCLicenseWizTypePage.h"

#include "globals.h"

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCLicenseWizTypePage::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
			// Handle clicks and such
			if (HIWORD(wParam) == BN_CLICKED)
			{
				UINT uID = LOWORD(wParam);
				if ((uID >= IDC_LICENSE_CC) && (uID <= IDC_LICENSE_NONE))
				{
					// User has selected a license type, update the description:
					int nSelected = GetCheckedRadioBtn(IDC_LICENSE_CC, IDC_LICENSE_NONE);
					SetDescription(nSelected);
					if (nSelected != -1)
					{
						// And allow moving on if it's possible
						SetWizardButtons(PSWIZB_NEXT);
						m_pLicenseInfo->m_eLicense = (LicenseInfo::LicenseType)nSelected;
					}
					return TRUE;
				}
			}
			break;
	}

	// Call base class
	BOOL bRet = CCLicenseWizBasePage::PageProc(uMsg, wParam, lParam);
	
	if (uMsg == WM_INITDIALOG)
	{
		// If we have just displayed the page, set the focus (this won't work before calling the base class...)
		SetFocus(GetDlgItem(IDC_LICENSE_CC));
		return FALSE;
	}

	return bRet;
}

/**
	@return TRUE
*/
BOOL CCLicenseWizTypePage::OnSetActive()
{
	// Did the user selected a license?
	if (m_pLicenseInfo->m_eLicense != LicenseInfo::LTUnknown)
	{
		// Yeah, set the correct button
		CheckRadioButton(m_hDlg, IDC_LICENSE_CC, IDC_LICENSE_NONE, IDC_LICENSE_CC + (int)m_pLicenseInfo->m_eLicense);
		SetFocus(GetDlgItem(IDC_LICENSE_CC + (int)m_pLicenseInfo->m_eLicense));
		// And allow moving on
		SetWizardButtons(PSWIZB_NEXT);
	}
	else
	{
		// We don't allow moving as there's no selection
		SetWizardButtons(0);
	}

	// Update the description of the license
	SetDescription((int)m_pLicenseInfo->m_eLicense);
	return TRUE;
}

/**
	@return TRUE (allow hidding) if a license type is selected, FALSE otherwise
*/
BOOL CCLicenseWizTypePage::OnKillActive()
{
	// Don't allow moving on if there's no selection
	if (m_pLicenseInfo->m_eLicense == LicenseInfo::LTUnknown)
		return FALSE;

	// CC and Sampling licenses data should be retrieve from the internet if possible (they change)
	if ((m_pLicenseInfo->m_eLicense == LicenseInfo::LTCC) || (m_pLicenseInfo->m_eLicense == LicenseInfo::LTSampling))
	{
		// Try getting the data
		bool bCancel;
		if (!LicenseInfoLoader::RequestLicenseType((LicenseInfo::LoadedLicenseDataType)m_pLicenseInfo->m_eLicense, GetParent(m_hDlg), bCancel))
		{
			// Can't, so:
			if (bCancel)
			{
				// User canceled - notify
				std::tstring sMsg = LoadResourceString(IDS_WARNING_USERCANCEL);
				MessageBox(sMsg.c_str(), _T("CC PDF Printer"), MB_ICONWARNING|MB_OK);
			}
			else
				// Didn't work, don't go on
				return FALSE;
		}
	}

	return TRUE;
}

/**
	@param nSelected The selected license
*/
void CCLicenseWizTypePage::SetDescription(int nSelected)
{
	if (nSelected == -1)
		// Set description for no license
		SetDlgItemText(IDC_DESCRIPTION, LoadResourceString(IDS_DESCRIPTION_CC + (int)LicenseInfo::LTUnknown).c_str());
	else
		// Set description for the license
		SetDlgItemText(IDC_DESCRIPTION, LoadResourceString(IDS_DESCRIPTION_CC + nSelected).c_str());
}

/**
	@return ID of the next page: usage page for a CC license, sampling type page for Sampling license, summery for all others
*/
int CCLicenseWizTypePage::OnWizNext()
{
	switch (m_pLicenseInfo->m_eLicense)
	{
		case LicenseInfo::LTCC:
			// Creative Commons: ask for commerical usage
			return IDD_LICENSEPAGE_USAGE;
		case LicenseInfo::LTSampling:
			// Sampling: ask for sampling type
			return IDD_LICENSEPAGE_SAMPLING;
		default:
			break;
	}

	// Any other: just show the summery page
	return IDD_LICENSEPAGE_SUMMERY;
}

/**
	@param page Reference to the property page structure to fill
	@return true if the structure was filled succesfully, false otherwise
*/
bool CCLicenseWizTypePage::PreparePage(PROPSHEETPAGE& page)
{
	// Call base class
	if (!CCLicenseWizBasePage::PreparePage(page))
		return false;

	// Set the header (only used in Wizard pages)
	page.dwFlags |= PSP_USEHEADERTITLE;
	page.pszHeaderTitle = MAKEINTRESOURCE(IDS_TITLE_LICENSE_TYPE);
	return true;
}
