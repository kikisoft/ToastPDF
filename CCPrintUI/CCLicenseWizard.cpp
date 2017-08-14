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
#include "CCLicenseWizard.h"

#include "CCLicenseWizTypePage.h"
#include "CCLicenseWizUsagePage.h"
#include "CCLicenseWizModificationPage.h"
#include "CCLicenseWizJurisdictionPage.h"
#include "CCLicenseWizSamplingPage.h"
#include "CCLicenseWizSummeryPage.h"
#include "globals.h"

/**

The following workflow is used in the wizard:

  License Type --------------------------
		|				|				|
		| CC license	| Sampling		| All Others
		|				|				|
		v				v				|
	  Usage		   Sampling Type		|
		|				|				|
		v				|				|
	Modification		|				|
		|				|				|
		v				|				|
	Jurisdiction <-------				|
		|								|
		v								|
	  Summery  <-------------------------

*/


/**
	
*/
CCLicenseWizard::CCLicenseWizard()
{
}

/// Number of wizard pages
#define PAGES_NUMBER		6

/**
	@param hParent Handle to the wizard's parent window
	@param pHelper Pointer to the Print UI Core object
	@param pInfo Pointer to the structure containing the current license information (filled with
			the new license information if the user finishes the wizard)
	@return TRUE if the user has finished the wizard, FALSE if canceled
*/
BOOL CCLicenseWizard::DoModal(HWND hParent, IPrintOemDriverUI* pHelper, LicenseInfo* pInfo)
{
	// Create the pages array
	PROPSHEETPAGE* pPages = new PROPSHEETPAGE[PAGES_NUMBER];
	CCLicenseWizBasePage* pPage;

	// Create the page objects and fill the array
	pPage = new CCLicenseWizTypePage(pHelper, pInfo);
	pPage->PreparePage(pPages[0]);
	pPage = new CCLicenseWizUsagePage(pHelper, pInfo);
	pPage->PreparePage(pPages[1]);
	pPage = new CCLicenseWizModificationPage(pHelper, pInfo);
	pPage->PreparePage(pPages[2]);
	pPage = new CCLicenseWizJurisdictionPage(pHelper, pInfo);
	pPage->PreparePage(pPages[3]);
	pPage = new CCLicenseWizSamplingPage(pHelper, pInfo);
	pPage->PreparePage(pPages[4]);
	pPage = new CCLicenseWizSummeryPage(pHelper, pInfo);
	pPage->PreparePage(pPages[5]);

	// Fill the wizard property sheet header
	PROPSHEETHEADER header;
	memset(&header, 0, sizeof(header));
	header.dwSize = sizeof(header);
	header.dwFlags = PSH_PROPSHEETPAGE|PSH_WIZARD|PSH_WIZARD97|PSH_HEADER;
	header.hwndParent = hParent;
	header.hInstance = ghInstance;
	header.pszCaption = _T("CC License");
	header.nPages = PAGES_NUMBER;
	header.nStartPage = 0;
	header.ppsp = pPages;
	header.pszbmHeader = MAKEINTRESOURCE(IDB_CC);

	// Run it!
	int nRet = PropertySheet(&header);
	delete [] pPages;
	return (nRet == 1);
}
