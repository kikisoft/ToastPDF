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
#include "CCLicenseWizSamplingPage.h"

/**
	@return TRUE
*/
BOOL CCLicenseWizSamplingPage::OnSetActive()
{
	// Do we know the kind of sampling license?
	if (m_pLicenseInfo->m_eSampling != LicenseInfo::STUnknown)
		// Yeah, select the proper button
		CheckRadioButton(m_hDlg, IDC_SAMPLING, IDC_SAMPLING_NC, IDC_SAMPLING + (int)m_pLicenseInfo->m_eSampling);

	// Update the description text
	UpdateControls();
	return TRUE;
}

/**
	@return TRUE (allow hidding) if a sampling type is selected, FALSE otherwise
*/
BOOL CCLicenseWizSamplingPage::OnKillActive()
{
	// Which button is selected?
	int nSel = GetCheckedRadioBtn(IDC_SAMPLING, IDC_SAMPLING_NC);
	if (nSel == -1)
		// None - don't know which license
		m_pLicenseInfo->m_eSampling = LicenseInfo::STUnknown;
	else
		// Set the sampling license type
		m_pLicenseInfo->m_eSampling = (LicenseInfo::SamplingType)nSel;

	// Only allow closing if the user selected a sampling type
	return (nSel != -1);
}

/**
	@return ID of jurisdication page
*/
int CCLicenseWizSamplingPage::OnWizNext()
{
	return IDD_LICENSEPAGE_JURISDICTION;
}

/**
	@return ID of license type page
*/
int CCLicenseWizSamplingPage::OnWizBack()
{
	return IDD_LICENSEPAGE_TYPE;
}

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCLicenseWizSamplingPage::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
			// Handle clicks and such
			if (HIWORD(wParam) == BN_CLICKED)
			{
				UINT uID = LOWORD(wParam);
				if ((uID >= IDC_SAMPLING) && (uID <= IDC_SAMPLING_NC))
				{
					// User has selected a sampling type, update the page controls
					UpdateControls();
					return TRUE;
				}
			}
			break;
		case WM_INITDIALOG:
			// Initial display, set the description in the box:
			SetDescriptionFromField(LicenseInfo::LLDTSampling, _T("sampling"));
			break;
	}

	return CCLicenseWizBasePage::PageProc(uMsg, wParam, lParam);
}

/**
	
*/
void CCLicenseWizSamplingPage::UpdateControls()
{
	// What is selected?
	int nSel = GetCheckedRadioBtn(IDC_SAMPLING, IDC_SAMPLING_NC);
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
	@param page Reference to the property page structure to fill
	@return true if the structure was filled succesfully, false otherwise
*/
bool CCLicenseWizSamplingPage::PreparePage(PROPSHEETPAGE& page)
{
	// Call base class
	if (!CCLicenseWizBasePage::PreparePage(page))
		return false;

	// Set the header (only used in Wizard pages)
	page.dwFlags |= PSP_USEHEADERTITLE;
	page.pszHeaderTitle = MAKEINTRESOURCE(IDS_TITLE_LICENSE_SAMPLING);
	return true;
}
