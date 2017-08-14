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
#include "CCPrintRegistry.h"

#include <winspool.h>
#include <tchar.h>

/**
	@param hPrinter Handle to the printer
	@param lpSetting Name of the key to get data from
	@param nDefault Default value of data (used if the key isn't found)
	@return The value in the registry, or the default if not found (or is of the wrong type)
*/
int	CCPrintRegistry::GetRegistryInt(HANDLE hPrinter, LPCTSTR lpSetting, int nDefault)
{
	// Get the printer key:
	DWORD dwType, dwValue, dwSize = sizeof(DWORD);
	if (GetPrinterData(hPrinter, (LPTSTR)lpSetting, &dwType, (LPBYTE)&dwValue, sizeof(DWORD), &dwSize) != ERROR_SUCCESS)
		return nDefault;

	if (dwType != REG_DWORD)
		return nDefault;

	return (int)dwValue;
}

/**
	@param hPrinter Handle to the printer
	@param lpSetting Name of the key to write data to
	@param nValue The value to write
*/
void CCPrintRegistry::SetRegistryInt(HANDLE hPrinter, LPCTSTR lpSetting, int nValue)
{
	// Get the printer key:
	DWORD dwValue = (DWORD)nValue;
	SetPrinterData(hPrinter, (LPTSTR)lpSetting, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue));
}

/**
	@param hPrinter Handle to the printer
	@param lpSetting Name of the key to get data from
	@param lpDefault Default value of data (used if the key isn't found)
	@return The value in the registry, or the default if not found (or is of the wrong type)
*/
std::tstring CCPrintRegistry::GetRegistryString(HANDLE hPrinter, LPCTSTR lpSetting, LPCTSTR lpDefault)
{
	// Get the printer key:
	DWORD dwType, dwSize;
	LONG lRes = GetPrinterData(hPrinter, (LPTSTR)lpSetting, &dwType, NULL, 0, &dwSize);
	if (((lRes != ERROR_MORE_DATA) && (lRes != ERROR_SUCCESS)) || (dwType != REG_SZ))
		return lpDefault;

	if (dwSize == 0)
		return _T("");

	std::tstring sRet(lpDefault);
	TCHAR* lpData = new TCHAR[(dwSize / sizeof(TCHAR)) + 1];
	if (GetPrinterData(hPrinter, (LPTSTR)lpSetting, NULL, (LPBYTE)lpData, dwSize, &dwSize) == ERROR_SUCCESS)
		sRet = lpData;
	delete [] lpData;

	return sRet;
}

/**
	@param hPrinter Handle to the printer
	@param lpSetting Name of the key to write data to
	@param sValue The value to write
*/
void CCPrintRegistry::SetRegistryString(HANDLE hPrinter, LPCTSTR lpSetting, const std::tstring& sValue)
{
	// Set the value
	SetPrinterData(hPrinter, (LPTSTR)lpSetting, REG_SZ, (LPBYTE)sValue.c_str(), (sValue.size() + 1) * sizeof(TCHAR));
}
