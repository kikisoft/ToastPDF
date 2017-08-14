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

#ifndef _LICENSEINFOLOADER_H_
#define _LICENSEINFOLOADER_H_

#include "LicenseInfo.h"

/**
    @brief Helper class for loading license information from the CC website or from saved data
*/
struct LicenseInfoLoader : public LicenseInfo
{
public:
	/**
		@brief Default constructor
	*/
	LicenseInfoLoader() : LicenseInfo() {};
	/**
		@brief Copy constructor
		@param info The license information object to copy
	*/
	LicenseInfoLoader(const LicenseInfo& info) : LicenseInfo(info) {};

protected:
	// Static members
	/// Cached XML license information
	static XDoc			m_xmlData[LLDT_Count];
	/// Flags indicating if the license was retrieved from the CC website in this session
	static bool			m_bRetrievedFromWeb[LLDT_Count];

public:
	// Methods
	/// Retrieves the type of XML license information that this object can use (according to the member values)
	LoadedLicenseDataType GetCCLicenseType() const;

	/// Returns the license name
	std::tstring		GetLicenseName() const;
	/// Returns the license image resource ID 
	UINT				GetLicenseImageID() const;

	/// Loads the license information from the cache, CC website (if not found) or saved resource (if cannot access)
	bool				LoadLicenseData(HWND hParent);

protected:
	// Helpers
	/// Loads the license information from the CC website
	bool				RequestLicense(HWND hParent, XDoc& doc, bool& bCancel);

public:
	// Static methods
	/// Initializes the static data
	static void			InitLicenseRetrievedData();

	/**
		@brief Retrieves an XML object holding the cached license
		@param eType Type of license information needed
		@return Reference to the cached XML object
	*/
	static const XDoc&	GetLicenseXML(LoadedLicenseDataType eType) {return m_xmlData[eType];};
	/// Retrieves the license type information from the CC website
	static bool			RequestLicenseType(LoadedLicenseDataType eType, HWND hWnd, bool& bCancel);

protected:
	// Static Helpers
	/// Checks if the license specified is valid
	static bool			IsValidLicenseInfo(LoadedLicenseDataType eType);
	/// Checks if this license was loaded from the web (or at least tried to)
	/**
		@brief Checks if the license data was retrieved from the website in this session
		@param eType The type of license data to check
		@return true if the license data was loaded from the website in this session, false if not
	*/
	static bool			IsLicenseRetrievedData(LoadedLicenseDataType eType) {return m_bRetrievedFromWeb[eType];};
	/// Loads an XML string as the cached license
	static bool			LoadData(LoadedLicenseDataType eType, LPCTSTR pXML);

	/// Loads an XML string into the cached license object and makes sure it's valid
	static bool			LoadAndValidateData(LoadedLicenseDataType eType, XDoc& doc, LPCTSTR pXML);
	/// Loads a license data from a resource XML string
	static bool			LoadInternalLicenseInfo(LoadedLicenseDataType eType);
	/// Tests to see if the received XML object holds valid license data information
	static bool			IsValidLicenseInfo(LoadedLicenseDataType eType, XDoc& doc);
	/**
		@brief Sets the license retrieval flag
		@param eType Type of license retrieved
	*/
	static void			SetLicenseRetrieved(LoadedLicenseDataType eType) {m_bRetrievedFromWeb[eType] = true;};

	/// Retrieves the license retrieval URL by the license type
	static LPCTSTR		GetLicenseURL(LoadedLicenseDataType eType);
	/// Retrieves the license image's URL by the license type
	static LPCTSTR		GetImageURL(LoadedLicenseDataType eType);
};

#endif   //#define _LICENSEINFOLOADER_H_
