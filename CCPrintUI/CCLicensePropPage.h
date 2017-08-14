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

#ifndef _CCLICENSEPROPPAGE_H_
#define _CCLICENSEPROPPAGE_H_

#include "CCPrintUIPropPage.h"
#include "resource.h"

#include "LicenseInfoLoader.h"
#include "devmode.h"

/**
    @brief Class handling the license selection page of the Printer UI
*/
class CCLicensePropPage : public CCPrintUIPropPage
{
public:
	// Ctor/Dtor
	/**
		@brief Constructor
		@param hPrinter Handle to the printer
		@param pHelper Pointer to the Print UI Core object
		@param pDevMode Pointer to the CCPrinter data structure (can be NULL)
		@param pPublicDM Pointer to the General printer data structure (can be NULL)
	*/
	CCLicensePropPage(HANDLE hPrinter, IPrintOemDriverUI* pHelper, POEMDEV pDevMode = NULL, PDEVMODE pPublicDM = NULL) : CCPrintUIPropPage(IDD_LICENSE_PAGE, hPrinter, pHelper, pDevMode), m_hBmp(NULL), m_pPublicDM(pPublicDM) {};
	/**
		@brief Destructor: deletes the license image if available
	*/
	~CCLicensePropPage() {if (m_hBmp != NULL) DeleteObject(m_hBmp);};

protected:
	// Members
	/// Handle to the license image
	HBITMAP				m_hBmp;
	/// License data
	LicenseInfoLoader	m_license;
	/// License location data
	LicenseLocationInfo	m_location;
	/// Flag incidating if the license properties should be added to the PDF's XMP
	BOOL				m_bSetProperties;
	/// Pointer to the general printer data structure
	PDEVMODE			m_pPublicDM;

protected:
	// Helper Overrides
	/// Called when a window message is sent to the page
	virtual BOOL PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Helpers
	/**
		@brief Updates the page controls (enabling/showing according to the current settings)
	*/
	void		UpdateControls() {UpdateLicenseControls(); UpdateLicenseLocationControls();};
	/// Updates the license selection controls
	void		UpdateLicenseControls();
	/// Updates the license location controls
	void		UpdateLicenseLocationControls();
	/// Resets the license and location data and updates the page controls
	void		InitControls();

	/// Returns the size of the currently selected print page
	SIZE		GetPageSize();
	/// Returns the size of the currently selected license stamp
	SIZE		GetLicenseSize();
};

#endif   //#define _CCLICENSEPROPPAGE_H_
