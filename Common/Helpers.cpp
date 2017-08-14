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
#include <tchar.h>
#include "Helpers.h"

/**
	@return true if PDF files have an 'open' handler in the registry
*/
bool CanOpenPDFFiles()
{
	// Check the registry:
	HKEY hKey;
	TCHAR cData[1024];
	DWORD dwLen;
	// Is there a .pdf file entry?
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, _T(".pdf"), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
		return false;

	// Yeah, get the value
	dwLen = 1024;
	if (RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE)cData, &dwLen) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return false;
	}
	RegCloseKey(hKey);

	if (_tcslen(cData) == 0)
		// Nothing there
		return false;

	// Get the handler for this name
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, cData, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
		return false;

	// Do we have a current version?
	HKEY hInnerKey;
	if (RegOpenKeyEx(hKey, _T("CurVer"), 0, KEY_QUERY_VALUE, &hInnerKey) == ERROR_SUCCESS)
	{
		// Yeah, replace the key as this is the one we want
		dwLen = 1024;
		if (RegQueryValueEx(hInnerKey, NULL, NULL, NULL, (LPBYTE)cData, &dwLen) != ERROR_SUCCESS)
		{
			RegCloseKey(hInnerKey);
			RegCloseKey(hKey);
			return false;
		}
		RegCloseKey(hInnerKey);
		RegCloseKey(hKey);
		if (RegOpenKeyEx(HKEY_CLASSES_ROOT, cData, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
			// Damn, can't find it
			return false;
	}

	// OK, check if we have an 'open' command:
	bool bRet = RegOpenKeyEx(hKey, _T("shell\\open"), 0, KEY_QUERY_VALUE, &hInnerKey) == ERROR_SUCCESS;
	if (bRet)
		RegCloseKey(hInnerKey);
	RegCloseKey(hKey);
	return bRet;
}
