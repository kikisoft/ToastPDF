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

#ifndef _CCPRINTREGISTRY_H_
#define _CCPRINTREGISTRY_H_

#include "CCTChar.h"

namespace CCPrintRegistry
{

/// Reads integer data from the printer's registry settings
int			GetRegistryInt(HANDLE hPrinter, LPCTSTR lpSetting, int nDefault);
/// Reads string data from the printer's registry settings
std::tstring GetRegistryString(HANDLE hPrinter, LPCTSTR lpSetting, LPCTSTR lpDefault);
/**
	@brief Reads boolean data from the printer's registry settings
	@param hPrinter Handle to the printer
	@param lpSetting Name of the key to read from
	@param bDefault Default value of the flag if not found in the registry
	@return The value of the data in the registry, or the default value if not found there
*/
inline bool	GetRegistryBool(HANDLE hPrinter, LPCTSTR lpSetting, bool bDefault) {return (GetRegistryInt(hPrinter, lpSetting, bDefault ? 1 : 0) == 1);};

/// Writes integer data to the printer's registry settings
void		SetRegistryInt(HANDLE hPrinter, LPCTSTR lpSetting, int nValue);
/**
	@brief Writes boolean data to the printer's registry settings
	@param hPrinter Handle to the printer
	@param lpSetting Name of the key to write to
	@param bValue Value to set
*/
inline void	SetRegistryBool(HANDLE hPrinter, LPCTSTR lpSetting, bool bValue) {SetRegistryInt(hPrinter, lpSetting, bValue ? 1 : 0);};
/// Writes string data to the printer's registry settings
void		SetRegistryString(HANDLE hPrinter, LPCTSTR lpSetting, const std::tstring& sValue);

};

#endif   //#define _CCPRINTREGISTRY_H_
