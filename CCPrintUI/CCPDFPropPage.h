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

#ifndef _CCPDFPROPPAGE_H_
#define _CCPDFPROPPAGE_H_

#include "CCPrintUIPropPage.h"
#include "resource.h"

/**
    @brief PDF properties page class
*/
class CCPDFPropPage : public CCPrintUIPropPage
{
public:
	// Ctor
	/**
		@brief Constructor
		@param hPrinter Handle to the printer
		@param pHelper Pointer to the Print UI Core object
		@param pDevMode Pointer to the CCPrinter data structure (can be NULL)
	*/
	CCPDFPropPage(HANDLE hPrinter, IPrintOemDriverUI* pHelper, POEMDEV pDevMode = NULL) : CCPrintUIPropPage(IDD_PDF_PAGE, hPrinter, pHelper, pDevMode) {};

protected:
	// Members
	/// Name of PDF file to create
	std::tstring m_sPath;

protected:
	// Helper overrides
	/// Called when a window message is sent to the page
	virtual BOOL PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Helpers
	/// Tests if the filename specified by the user is valid
	BOOL		TestValidFilename();
	/// Initializes the page controls
	void		InitControls();
};


#endif   //#define _CCPDFPROPPAGE_H_
