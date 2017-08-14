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
#include "CCLicenseWizBasePage.h"

#include "resource.h"

/**
	@param eType The type of the license
	@param lpName Name of the XML field
*/
void CCLicenseWizBasePage::SetDescriptionFromField(LicenseInfo::LoadedLicenseDataType eType, LPCTSTR lpName)
{
	// Get the field node in the license description
	std::tstring sDescription;
	const LPXNode pNode = LicenseInfoLoader::GetLicenseXML(eType).Find(_T("field"), lpName);
	if (pNode != NULL)
	{
		// Get the description node
		const LPXNode pDescription = pNode->GetChild(_T("description"));
		if (pDescription != NULL)
		{
			// Found it, get the text from it
			DISP_OPT opt;
			opt.reference_value = false;
			sDescription = pDescription->GetText(&opt);
		}
	}
	
	SetDlgItemText(IDC_DESCRIPTION, sDescription.c_str());
}

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCLicenseWizBasePage::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Only handle notification message here
	if (uMsg == WM_NOTIFY)
	{
		NMHDR* pNMHdr = (NMHDR*)lParam;
		// Which notification is this?
		switch (pNMHdr->code)
		{
			case PSN_SETACTIVE:
				// Called when the page is activated, call virtual function:
				if (OnSetActive())
					// OK, we can be displayed
#ifdef _WIN64
					SetWindowLong(DWLP_MSGRESULT, 0);
#else
					SetWindowLong(DWL_MSGRESULT, 0);
#endif
				else
					// Don't allow!
#ifdef _WIN64
					SetWindowLong(DWLP_MSGRESULT, -1);
#else
					SetWindowLong(DWL_MSGRESULT, -1);
#endif
					
				return TRUE;
			case PSN_WIZNEXT:
				// Called when the user clicks the next button, call virtual function:
#ifdef _WIN64
				SetWindowLong(DWLP_MSGRESULT, OnWizNext());
#else
				SetWindowLong(DWL_MSGRESULT, OnWizNext());
#endif
				return TRUE;
			case PSN_WIZBACK:
				// Called when the user clicks the back button, call virtual function:
#ifdef _WIN64
				SetWindowLong(DWLP_MSGRESULT, OnWizNext());
#else
				SetWindowLong(DWL_MSGRESULT, OnWizBack());
#endif
				return TRUE;
			case PSN_WIZFINISH:
				// Called when the user clicks the finish button, call virtual function:
#ifdef _WIN64
				SetWindowLong(DWLP_MSGRESULT, !OnWizFinish());
#else
				SetWindowLong(DWL_MSGRESULT, !OnWizFinish());
#endif
				return TRUE;
			case PSN_KILLACTIVE:
				// Called when the page is deactivated, call virtual function:
				if (OnKillActive())
					// Allow hidding the page
#ifdef _WIN64
					SetWindowLong(DWLP_MSGRESULT, FALSE);
#else
					SetWindowLong(DWL_MSGRESULT, FALSE);
#endif
				else
					// Don't allow!
#ifdef _WIN64
					SetWindowLong(DWLP_MSGRESULT, TRUE);
#else
					SetWindowLong(DWL_MSGRESULT, TRUE);
#endif
				return TRUE;
		}
	}

	// Everything else will be handled by the parent class...
	return FALSE;
}

/**
	@param dwFlags Wizard button flags, see PSM_SETWIZBUTTONS message in MSDN
*/
void CCLicenseWizBasePage::SetWizardButtons(DWORD dwFlags)
{
	PropSheet_SetWizButtons(GetParent(m_hDlg), dwFlags);
}
