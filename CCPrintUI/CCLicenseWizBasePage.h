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


#ifndef _CCLICENSEWIZBASEPAGE_H_
#define _CCLICENSEWIZBASEPAGE_H_

#include "CCPrintPropPage.h"
#include "LicenseInfoLoader.h"

/**
    @brief Base class for the license wizard pages
*/
class CCLicenseWizBasePage : public CCPrintPropPage
{
public:
	// Ctor
	/**
		@brief Constructor
		@param uResourceID ID of the page resource
		@param pHelper Pointer to the Print UI Core object
		@param pInfo Pointer to the current license information structure
	*/
	CCLicenseWizBasePage(UINT uResourceID, IPrintOemDriverUI* pHelper, LicenseInfo* pInfo) : CCPrintPropPage(uResourceID, pHelper), m_pLicenseInfo(pInfo) {};

protected:
	// Members
	/// Pointer to the license information structure
	LicenseInfo*	m_pLicenseInfo;

protected:
	// Overrides
	/// Called when a windows message is sent to the page
	virtual BOOL	PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Overridables
	/**
		@brief Called when the page is being activated (displayed)
		@return TRUE to allow displaying the page, FALSE not to
	*/
	virtual BOOL	OnSetActive() {return TRUE;};
	/**
		@brief Called when the page is being deactivated (hidden)
		@return TRUE to allow hidding the page (also closing the wizard!), FALSE to keep it
	*/
	virtual BOOL	OnKillActive() {return TRUE;};
	/**
		@brief Called when the user clicks the wizard's Next button
		@return The ID of the resource of the page to display, or -1 to stay on the page
	*/
	virtual int		OnWizNext() {return 0;};
	/**
		@brief Called when the user clicks the wizard's Back button
		@return The ID of the resource of the page to display, or -1 to stay on this page
	*/
	virtual int		OnWizBack() {return 0;};
	/**
		@brief Called when the user clicks the wizard's Finish button
		@return TRUE to allow closing the wizard, FALSE not to
	*/
	virtual BOOL	OnWizFinish() {return TRUE;};

	// Helpers
	/// Called to write the description of an XML license field in a control named IDC_DESCRIPTION
	void			SetDescriptionFromField(LicenseInfo::LoadedLicenseDataType eType, LPCTSTR lpName);
	/// Called to set the wizard's buttons (disabling/enabling and changing the captions)
	void			SetWizardButtons(DWORD dwFlags);
};

#endif   //#define _CCLICENSEWIZBASEPAGE_H_
