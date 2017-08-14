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
#include "CCLicenseWizSummeryPage.h"

#include "globals.h"

/**
	@return TRUE
*/
BOOL CCLicenseWizSummeryPage::OnSetActive()
{
	// Allow moving backwards, or finishing the wizard
	SetWizardButtons(PSWIZB_FINISH|PSWIZB_BACK);

	// We want to display the proper icons on top, so calculate the here:
	std::tstring sSummery;
	int iIconNum = 0;
	UINT uIcon[3];
	memset(uIcon, 0, sizeof(UINT) * 3);
	switch (m_pLicenseInfo->m_eLicense)
	{
		case LicenseInfo::LTCC:
			{
				// Creative Commons license:
				// Always have a by icon:
				uIcon[iIconNum++] = IDI_ATTRIBUTION;
				// Add non-commercial use icon if so selected
				if (!m_pLicenseInfo->m_bCommercialUse)
					uIcon[iIconNum++] = IDI_COMMERCIAL_NO;
				// Add modification limitations if so selected
				switch (m_pLicenseInfo->m_eModification)
				{
					case LicenseInfo::MTNo:
						uIcon[iIconNum++] = IDI_DERIVATIVE_NO;
						break;
					case LicenseInfo::MTShareAlike:
						uIcon[iIconNum++] = IDI_SHAREALIKE;
						break;
				}
				// Choose license description text:
				int nText = (int)m_pLicenseInfo->m_eModification;
				if (m_pLicenseInfo->m_bCommercialUse)
					nText += 3;
				if (nText < 6)
					sSummery = LoadResourceString(IDS_DESCRIPTION_CC_BY_NC + nText);
				if (m_pLicenseInfo->HasJurisdiction())
					// Add the jurisdiction
					sSummery += LoadResourceString(IDS_DESCRIPTION_JURISDICTION_ADD) + m_pLicenseInfo->m_cJurisdiction;
			}
			break;
		case LicenseInfo::LTSampling:
			// Sampling license:
			// Always have a by icon:
			uIcon[iIconNum++] = IDI_ATTRIBUTION;
			// Add the proper sampling icon
			switch(m_pLicenseInfo->m_eSampling)
			{
				case LicenseInfo::STSampling:
					uIcon[iIconNum++] = IDI_SAMPLING;
					break;
				case LicenseInfo::STSamplingNC:
					// Add the non-commercial icon too
					uIcon[iIconNum++] = IDI_COMMERCIAL_NO;
					// Fall through!
				case LicenseInfo::STSamplingPlus:
					uIcon[iIconNum++] = IDI_SAMPLING_PLUS;
					break;
			}
			// Get the summery for the license
			sSummery = LoadResourceString(IDS_DESCRIPTION_SAMPLING_REGULAR + (int)m_pLicenseInfo->m_eSampling);
			if (m_pLicenseInfo->HasJurisdiction())
				// And the jurisdiction
				sSummery += LoadResourceString(IDS_DESCRIPTION_JURISDICTION_ADD) + m_pLicenseInfo->m_cJurisdiction;
			break;
		case LicenseInfo::LTPublicDomain:
			// Public domain license description and icon
			sSummery = LoadResourceString(IDS_DESCRIPTION_PUBLIC_FINAL);
			uIcon[1] = IDI_PUBLICDOMAIN;
			break;
		case LicenseInfo::LTDevelopingNations:
			// Developing nations license description and icon
			sSummery = LoadResourceString(IDS_DESCRIPTION_DEVNATIONS_FINAL);
			uIcon[1] = IDI_DEVELOPINGNATIONS;
			break;
		case LicenseInfo::LTNone:
			// No license:
			sSummery = LoadResourceString(IDS_DESCRIPTION_NONE_FINAL);
			break;
	}

	// Set the description
	SetDlgItemText(IDC_DESCRIPTION, sSummery.c_str());

	// And the icons
	for (int i=0;i<3;i++)
	{
		if (uIcon[i] == 0)
			ShowWindow(GetDlgItem(IDC_ICON1 + i), SW_HIDE);
		else
		{
			HICON hIcon = ::LoadIcon(ghInstance, MAKEINTRESOURCE(uIcon[i]));
			if (hIcon != NULL)
			{
				::SendMessage(GetDlgItem(IDC_ICON1 + i), STM_SETICON, (WPARAM)hIcon, 0);
				ShowWindow(GetDlgItem(IDC_ICON1 + i), SW_SHOW);
			}
			else
				ShowWindow(GetDlgItem(IDC_ICON1 + i), SW_HIDE);
		}
	}

	return TRUE;
}

/**
	@return ID of jurisdication page for CC and Sampling licenses, license type page ID otherwise
*/
int CCLicenseWizSummeryPage::OnWizBack()
{
	// What kind of license are we talking about?
	switch(m_pLicenseInfo->m_eLicense)
	{
		case LicenseInfo::LTCC:
		case LicenseInfo::LTSampling:
			// Creative Common and Sampling passed through the jurisdiction page to get here:
			return IDD_LICENSEPAGE_JURISDICTION;
		default:
			break;
	}
	// All others just go to the first page
	return IDD_LICENSEPAGE_TYPE;
}

/**
	@param page Reference to the property page structure to fill
	@return true if the structure was filled succesfully, false otherwise
*/
bool CCLicenseWizSummeryPage::PreparePage(PROPSHEETPAGE& page)
{
	// Call base class
	if (!CCLicenseWizBasePage::PreparePage(page))
		return false;

	// Set the header (only used in Wizard pages)
	page.dwFlags |= PSP_USEHEADERTITLE;
	page.pszHeaderTitle = MAKEINTRESOURCE(IDS_TITLE_LICENSE_SUMMERY);
	return true;
}
