/**
	@file
	@brief File that contains globals.
			based on globals.cpp
			Printer Driver UI Plugin Sample
			by Microsoft Corporation
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
#include "globals.h"
#include "debug.h"

/// Maximum length of resource string
#define MAX_STR		10000


///////////////////////////////////////
//          Globals
///////////////////////////////////////

/// Module's Instance handle from DLLEntry of process.
HINSTANCE   ghInstance = NULL;

/**
	@param uID Resource ID of the string to load
	@return The loaded string (empty if not found)
*/
std::tstring LoadResourceString(UINT uID)
{
	// Make enough space
	TCHAR cStr[MAX_STR];
	DWORD dwSize = MAX_STR;
	// Load the string
    if (LoadString((HMODULE)ghInstance, uID, cStr, dwSize) == 0)
	{
		// Failed!
		ASSERTMSG(FALSE, "Resource string not found");
		return _T("");
	}
	return cStr;
}
