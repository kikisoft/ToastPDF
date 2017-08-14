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

#ifndef _CCPRINTPROPPAGE_H_
#define _CCPRINTPROPPAGE_H_

#include "CCPrintDlg.h"
#include "debug.h"

/**
    @brief Parent class for printer UI property pages
*/
class CCPrintPropPage : public CCPrintDlg
{
public:
	// Ctor
	/// Constructor
	CCPrintPropPage(UINT uResourceID, struct IPrintOemDriverUI* pHelper);

protected:
	// Members
	/// Pointer to the Print UI Core object
	IPrintOemDriverUI* m_pHelper;

public:
	// Overridables
	/// Called to fill up the property sheet structure with information
	virtual bool PreparePage(PROPSHEETPAGE& page);
	// Overrides
	/**
		@brief Called to create the dialog - should NEVER be called for Property Page classes
		@param hParent Handle to parent window
		@return IDCANCEL
	*/
	virtual int DoModal(HWND hParent) {/* Should never get here! */ ASSERTMSG(FALSE, "CCPrintPropPage::DoModal() called!"); return IDCANCEL;};

protected:
	// Helper Overrides
	/**
		@brief Called when the page window is destroyed; deletes the object
	*/
	virtual void PostDestroy() {delete this;};

	// Page Proc Callback
	/// Called to handle page window messages
	static BOOL APIENTRY StaticPageProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif   //#define _CCPRINTPROPPAGE_H_
