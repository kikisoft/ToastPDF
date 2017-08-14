/**
	@file
	@brief 
*/

/*
 * CC PDF Converter: Windows PDF Printer with Creative Commons license support
 * Excel to PDF Converter: Excel PDF printing addin, keeping hyperlinks AND Creative Commons license support
 * Copyright (C) 2007-2010 Guy Hachlili <hguy@cogniview.com>, Cogniview LTD.
 * 
 * This file is part of CC PDF Converter / Excel to PDF Converter
 * 
 * CC PDF Converter and Excel to PDF Converter are free software;
 * you can redistribute them and/or modify them under the terms of the 
 * GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later version.
 * 
 * CC PDF Converter and Excel to PDF Converter are is distributed in the hope 
 * that they will be useful, but WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. * 
 */

#ifndef _CCPRINTINSTALLFUNCTIONS_H_
#define _CCPRINTINSTALLFUNCTIONS_H_

#include "PrinterInstall.h"

/**
    @brief Helper class used to install the port monitor along with the PDF printer
*/
bool IsX64();

class CCPDFPrinterInstaller : public PrinterInstall
{
public:
	// Ctor
	/// Default constructor
	CCPDFPrinterInstaller(HWND hWnd);

protected:
	// Members
	/// Parent window handle
	HWND	m_hWnd;

public:
	// Methods
	/// CC PDF Printer driver install function
	bool	DoInstall(HMODULE hModule = NULL, LPTSTR installationPath = "");
	/// CC PDF Printer driver removal function
	bool	DoRemove();
	/// Tests if the CC PDF Printer driver is installed on this system
	bool	IsPrinterInstalled();

protected:
	/// Display an error message dialog
	void	ShowError(LPCTSTR lpError);
};

#endif   //#define _CCPRINTINSTALLFUNCTIONS_H_
