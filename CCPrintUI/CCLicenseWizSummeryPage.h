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

#ifndef _CCLICENSEWIZSUMMERYPAGE_H_
#define _CCLICENSEWIZSUMMERYPAGE_H_

#include "CCLicenseWizBasePage.h"
#include "resource.h"

/**
    @brief License Wizard: Summery page
*/
class CCLicenseWizSummeryPage : public CCLicenseWizBasePage
{
public:
	// Ctor
	/**
		@brief Constructor
		@param pHelper Pointer to the Print UI Core object
		@param pInfo Pointer to the current license information structure
	*/
	CCLicenseWizSummeryPage(IPrintOemDriverUI* pHelper, LicenseInfo* pInfo) : CCLicenseWizBasePage(IDD_LICENSEPAGE_SUMMERY, pHelper, pInfo) {};

public:
	// CCPrintPropPage public overrides
	/// Called to fill the wizard page structure
	virtual bool PreparePage(PROPSHEETPAGE& page);

protected:
	// CCLicenseWizBasePage overrides
	/// Called when the page is being activated (displayed)
	virtual BOOL	OnSetActive();
	/// Called when the user clicks the wizard's Back button
	virtual int		OnWizBack();
};

#endif   //#define _CCLICENSEWIZSUMMERYPAGE_H_
