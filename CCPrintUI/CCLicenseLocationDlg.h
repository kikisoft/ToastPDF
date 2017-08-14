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

#ifndef _CCLICENSELOCATIONDLG_H_
#define _CCLICENSELOCATIONDLG_H_

#include "CCPrintDlg.h"
#include "CCLicenseLocationWnd.h"
#include "resource.h"

/**
    @brief License stamp location dialog
*/
class CCLicenseLocationDlg : public CCPrintDlg
{
public:
	// Ctor
	/**
		@brief Constructor
		@param szPageSize Size of the page (in pixels)
		@param rectLocation Current location of stamp on the page (also specifies the size of the stamp)
	*/
	CCLicenseLocationDlg(SIZE szPageSize, RECT& rectLocation) : CCPrintDlg(IDD_LICENSE_LOCATION), m_wndLocation(szPageSize, rectLocation), m_bSettingEdit(false) {};

protected:
	// Members
	/// The aspect ratio of the page
	double			m_dRatio;
	/// The current location of the stamp
	POINT			m_ptLocation;
	/// Stamp location windows (enables user to drag the stamp around)
	CCLicenseLocationWnd m_wndLocation;
	/// true when the edit boxes are being set by the application (and not the user)
	bool			m_bSettingEdit;

public:
	// Methods
	/**
		@brief Retrieves the current location of the stamp
		@return The current location of the stamp
	*/
	POINT			GetLocation() const {return m_ptLocation;};

protected:
	// CCPrintDlg overrides
	/// Called when a windows message is sent to the page
	virtual BOOL	PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Helpers
	/// Sets the current stamp location in the edit boxes
	void			SetEditLocation();
};

#endif   //#define _CCLICENSELOCATIONDLG_H_
