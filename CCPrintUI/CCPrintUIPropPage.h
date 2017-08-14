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

#ifndef _CCPRINTUIPROPPAGE_H_
#define _CCPRINTUIPROPPAGE_H_

#include "devmode.h"
#include "CCPrintPropPage.h"
#include "CCPrintRegistry.h"

/**
    @brief Parent class for Print UI main property pages (non-wizard)
*/
class CCPrintUIPropPage : public CCPrintPropPage
{
public:
	// Ctor
	/// Constructor
	CCPrintUIPropPage(UINT uResourceID, HANDLE hPrinter, IPrintOemDriverUI* pHelper, POEMDEV pDevMode = NULL);

protected:
	// Members
	/// Pointer to the plugin data structure (can be NULL for device property pages)
	POEMDEV		m_pDevMode;
	/// Handle to the printer
	HANDLE		m_hPrinter;
	/// Pointer to the Common Property Sheet handler function
	PFNCOMPROPSHEET m_pfnComPropSheet;
	/// Handle of the common property sheet
	HANDLE		m_hComPropSheet;
	/// Handle of the page
	HANDLE		m_hPage;

public:
	/**
		@brief Tests if the current page is in device properties mode (as opposed to specific printing properties)
		@return true if the page is in device properties mode
	*/
	bool		IsDevicePage() const {return (m_pDevMode == NULL);};
	/**
		@brief Sets the Common Property Sheet members
		@param pfnComPropSheet Pointer to the Common Property Sheet handler function
		@param hComPropSheet Handle of the common property sheet
		@param hPage Handle of the page
	*/
	void		SetCallbackData(PFNCOMPROPSHEET pfnComPropSheet, HANDLE hComPropSheet, HANDLE hPage) {m_pfnComPropSheet = pfnComPropSheet; m_hComPropSheet = hComPropSheet; m_hPage = hPage;};

protected:
	// Helpers
	/**
		@brief Retrieves a boolean value from the printer settings in the registry
		@param lpSetting Name of the value to retrieve
		@param bDefault Default value returned (if not found in the registry)
		@return The value from the registry, or the default value if not found
	*/
	bool		GetRegistryBool(LPCTSTR lpSetting, bool bDefault) const {return CCPrintRegistry::GetRegistryBool(m_hPrinter, lpSetting, bDefault);};
	/**
		@brief Retrieves an integer value from the printer settings in the registry
		@param lpSetting Name of the value to retrieve
		@param nDefault Default value returned (if not found in the registry)
		@return The value from the registry, or the default value if not found
	*/
	int			GetRegistryInt(LPCTSTR lpSetting, int nDefault) const {return CCPrintRegistry::GetRegistryInt(m_hPrinter, lpSetting, nDefault);};
	/**
		@brief Retrieves a string value from the printer settings in the registry
		@param lpSetting Name of the value to retrieve
		@param lpDefault Default value returned (if not found in the registry)
		@return The value from the registry, or the default value if not found
	*/
	std::tstring GetRegistryString(LPCTSTR lpSetting, LPCTSTR lpDefault) const {return CCPrintRegistry::GetRegistryString(m_hPrinter, lpSetting, lpDefault);};
	/**
		@brief Sets an integer value in the printer settings in the registry
		@param lpSetting Name of the value to set
		@param nValue Value to set
	*/
	void		SetRegistryInt(LPCTSTR lpSetting, int nValue) const {CCPrintRegistry::SetRegistryInt(m_hPrinter, lpSetting, nValue);};

	/**
		@brief Sets the page to changed status (enabling the Apply button)
	*/
	void		SetChanged() {if (m_hDlg != NULL) ::PostMessage(GetParent(m_hDlg), PSM_CHANGED, (WPARAM)m_hDlg, 0);}
};

#endif   //#define _CCPRINTUIPROPPAGE_H_
