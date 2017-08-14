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
#include "CCLicenseWizJurisdictionPage.h"

#include <tchar.h>

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCLicenseWizJurisdictionPage::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
			// Handle clicks and such
			if (HIWORD(wParam) == BN_CLICKED)
			{
				UINT uID = LOWORD(wParam);
				if ((uID >= IDC_GENERAL) && (uID <= IDC_SPECIFIC))
				{
					// The user selected a jurisdiction type, update the controls
					UpdateControls();
					return TRUE;
				}
			}
			break;
	}

	return CCLicenseWizBasePage::PageProc(uMsg, wParam, lParam);
}

/**
	@return TRUE to allow displaying the page, FALSE not to
*/
BOOL CCLicenseWizJurisdictionPage::OnSetActive()
{
	// Set up the jurisdiction box
	CleanDlgCombo(IDC_JURISDICTION);
	
	LicenseInfo::LoadedLicenseDataType eType = (m_pLicenseInfo->m_eLicense == LicenseInfo::LTCC) ? LicenseInfo::LLDTCC : LicenseInfo::LLDTSampling;

	// Retrieve the jurisdiction node from the XML
	const LPXNode pJuri = LicenseInfoLoader::GetLicenseXML(eType).Find(_T("field"), _T("jurisdiction"));
	if (pJuri != NULL)
	{
		DISP_OPT opt;
		opt.reference_value = false;

		// First, set the description
		const LPXNode pDescription = pJuri->GetChild(_T("description"));
		if (pDescription != NULL)
			SetDlgItemText(IDC_DESCRIPTION, pDescription->GetText(&opt).c_str());

		// Now go over all the child (<enum>) nodes and get all the non-general values:
		for (XNodes::const_iterator i = pJuri->childs.begin(); i != pJuri->childs.end(); i++)
		{
			const LPXAttr pID = (*i)->GetAttr(_T("id"));
			if ((pID == NULL) || pID->value.empty())
				// No id or empty id means it's the general option, we don't want it in the list
				continue;

			// Get the label
			const LPXNode pLabel = (*i)->GetChild(_T("label"));
			if ((pLabel != NULL) && !pLabel->GetText().empty())
			{
				// Add an entry to the combo's list with this name
				AddDlgComboString(IDC_JURISDICTION, pLabel->GetText(&opt).c_str(), 0);
			}	
		}
	}

	// Select the radio button according to the user's choice:
	if (m_pLicenseInfo->HasJurisdiction())
	{
		// Specific: set the button and find and select the proper entry
		CheckRadioButton(m_hDlg, IDC_GENERAL, IDC_SPECIFIC, IDC_SPECIFIC);
		SetDlgComboSel(IDC_JURISDICTION, m_pLicenseInfo->m_cJurisdiction);
	}
	else
		// Empty jurisdiction means general
		CheckRadioButton(m_hDlg, IDC_GENERAL, IDC_SPECIFIC, IDC_GENERAL);

	// Update the <next> button and enable the specific jurisdiction box if necessary
	UpdateControls();

	return TRUE;
}

/**
	
*/
void CCLicenseWizJurisdictionPage::UpdateControls()
{
	// Get the user-selected radio button
	int nSel = GetCheckedRadioBtn(IDC_GENERAL, IDC_SPECIFIC);

	// Only allow moving onwards if one of them is selected
	SetWizardButtons(PSWIZB_BACK|((nSel == -1) ? 0 : PSWIZB_NEXT));

	// Enable the specific jurisdiction box only if the specific button is selected
	EnableDlgItem(IDC_JURISDICTION, (nSel == 1));
	if (nSel == 1)
	{
		// If there's no selection in the jurisdiction box, select the first entry:
		if ((GetDlgComboSel(IDC_JURISDICTION) == CB_ERR) && (GetDlgComboCount(IDC_JURISDICTION) > 0))
			SetDlgComboItem(IDC_JURISDICTION, 0);
	}
}

/**
	@return The ID of the summery (last) page
*/
int CCLicenseWizJurisdictionPage::OnWizNext()
{
	return IDD_LICENSEPAGE_SUMMERY;
}

/**
	@return The ID of the modification page (CC license) or the sampling license page (Sampling license)
*/
int CCLicenseWizJurisdictionPage::OnWizBack()
{
	// The previous page depends on the license type: derivatives for Creative Commons or sampling selection for Sampling
	return (m_pLicenseInfo->m_eLicense == LicenseInfo::LTCC) ? IDD_LICENSEPAGE_MODIFICATION : IDD_LICENSEPAGE_SAMPLING;
}

/**
	@return TRUE if the user selected a jursidiction, FALSE if not
*/
BOOL CCLicenseWizJurisdictionPage::OnKillActive()
{
	// Which button is selected?
	int nSel = GetCheckedRadioBtn(IDC_GENERAL, IDC_SPECIFIC);
	if (nSel == -1)
		// None - don't allow to close
		return FALSE;

	if (nSel == 0)
		// General jurisdiction
		m_pLicenseInfo->SetJurisdiction(_T(""));
	else
	{
		// Retrieve jurisdication area:
		m_pLicenseInfo->SetJurisdiction(GetDlgComboText(IDC_JURISDICTION).c_str());
	}
	return true;
}

/**
	@param page Reference to the property page structure to fill
	@return true if the structure was filled succesfully, false otherwise
*/
bool CCLicenseWizJurisdictionPage::PreparePage(PROPSHEETPAGE& page)
{
	// Call base class
	if (!CCLicenseWizBasePage::PreparePage(page))
		return false;

	// Set the header (only used in Wizard pages)
	page.dwFlags |= PSP_USEHEADERTITLE;
	page.pszHeaderTitle = MAKEINTRESOURCE(IDS_TITLE_LICENSE_JURISDICTION);
	return true;
}
