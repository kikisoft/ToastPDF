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
#include "oemps.h"
#include "debug.h"

HINSTANCE ghInstance;


// Need to export these functions as c declarations.
extern "C" {


///////////////////////////////////////////////////////////
//
// DLL entry point
//

/**
	@param hInst Handle to the DLL instance
	@param wReason Reason of call
	@param lpReserved Not used
	@return TRUE
*/
BOOL WINAPI DllMain(HINSTANCE hInst, WORD wReason, LPVOID lpReserved)
{
	switch(wReason)
 	{
		case DLL_PROCESS_ATTACH:
            VERBOSE(DLLTEXT("Process attach.\r\n"));
			ghInstance = hInst;
            break;

		case DLL_THREAD_ATTACH:
            VERBOSE(DLLTEXT("Thread attach.\r\n"));
			break;

		case DLL_PROCESS_DETACH:
            VERBOSE(DLLTEXT("Process detach.\r\n"));
			break;

		case DLL_THREAD_DETACH:
            VERBOSE(DLLTEXT("Thread detach.\r\n"));
			break;
	}

	return TRUE;
}

/**
	@param ulReason Reason for call
	@return TRUE
*/
BOOL WINAPI DllInitialize(ULONG ulReason)
{
    BOOL    bRet = TRUE;

	switch(ulReason)
	{
		case DLL_PROCESS_ATTACH:
            VERBOSE(DLLTEXT("Process attach.\r\n"));
            break;

		case DLL_THREAD_ATTACH:
            VERBOSE(DLLTEXT("Thread attach.\r\n"));
			break;

		case DLL_PROCESS_DETACH:
            VERBOSE(DLLTEXT("Process detach.\r\n"));
			break;

		case DLL_THREAD_DETACH:
            VERBOSE(DLLTEXT("Thread detach.\r\n"));
			break;
	}

	return bRet;
}



}  // extern "C" closing bracket



