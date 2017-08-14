/**
	@file
	@brief Define common data types, and external function prototypes
			for devmode functions in the UI and rendering plugins
			Based on:
			Devmode.h
			Printer Driver Plugin Sample
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

#ifndef _DEVMODE_H
#define _DEVMODE_H

#include "LicenseInfo.h"

////////////////////////////////////////////////////////
//      OEM Devmode Defines
////////////////////////////////////////////////////////




////////////////////////////////////////////////////////
//      OEM Devmode Type Definitions
////////////////////////////////////////////////////////

/// License stamp location enumeration
typedef enum {LLTop = 0, LLBottom, LLUserDefined, LLNone, LLOther, LLLast} LicenseLocation;

/**
    @brief Structure holding license stamp location data
*/
struct LicenseLocationInfo
{
	/**
		@brief Constructor: sets default value for stamp location
	*/
	LicenseLocationInfo() : eFirstPage(LLNone), eOtherPages(LLOther) {ptFirstPage.x = ptFirstPage.y = ptOtherPages.x = ptOtherPages.y = 0;};

	// Members
	/// Location of first page stamp
	LicenseLocation		eFirstPage;
	/// Location of user-defined first-page stamp (only used if eFirstPage is LLUserDefined)
	POINT				ptFirstPage;
	/// Location of the other pages stamp
	LicenseLocation		eOtherPages;
	/// Location of user-defined other-page stamp (only used if eOtherPages is LLUserDefined)
	POINT				ptOtherPages;

	/**
		@brief Resets the stamp location object
	*/
	void				Clear() {eFirstPage = LLNone; eOtherPages = LLOther; ptFirstPage.x = ptFirstPage.y = ptOtherPages.x = ptOtherPages.y = 0;};
	/// Calculates the current stamp location according to the page and stamp sizes (and the location information)
	POINT				LocationForPage(bool bFirstPage, SIZE szPageSize, SIZE szLicenseSize) const;
};

/**
    @brief Structure for extra plugin data 
*/
typedef struct tagOEMDEV
{
	/// Extra data header: must be included according to the DDK
    OEM_DMEXTRAHEADER   dmOEMExtra;
	/// Filename of the document to be created
	WCHAR				cFilename[MAX_PATH + 1];
	/// TRUE to open the document after creation, FALSE not to
	BOOL				bAutoOpen;
	/// TRUE to add license information to the document's XMP information, FALSE not to
	BOOL				bSetProperties;
	/// The location of the license stamp
	LicenseLocationInfo location;
	/// The license information
	LicenseInfo			info;

} OEMDEV, *POEMDEV;

/// Pointer to a constant structure of plugin data
typedef const OEMDEV *PCOEMDEV;



/////////////////////////////////////////////////////////
//		ProtoTypes
/////////////////////////////////////////////////////////

/// Handler for plugin data structure creation and initialization
HRESULT hrOEMDevMode(DWORD dwMode, POEMDMPARAM pOemDMParam);
/// Handler for copy/initialization of plugin data
BOOL ConvertOEMDevmode(PCOEMDEV pOEMDevIn, POEMDEV pOEMDevOut, HANDLE hPrinter);
/// Handler for validation of plugin data
BOOL MakeOEMDevmodeValid(POEMDEV pOEMDevmode);
/// This function dumps the plugin data structure for testing purpuses
void Dump(PCOEMDEV pOEMDevIn);

/// Reads the plugin data from the registry
void ReadOEMDevFromRegistry(POEMDEV pDev, HANDLE hPrinter);
/// Writes the plugin data to the registry
void WriteOEMDevToRegistry(const POEMDEV pDev, HANDLE hPrinter);

#endif


