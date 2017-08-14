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
#include "CCPrintLicenseInfo.h"

#include "CCPrintRegistry.h"

/// Registry key for license
#define REG_LICENSE_BASE_NAME			_T("License")

/// Registry key: type of license
#define REG_LICENSE_TYPE_NAME			_T("Type")
/// Registry key: license: commercial usage
#define REG_LICENSE_COMMERCIAL_NAME		_T("Commercial")
/// Registry key: license: modificiation options
#define REG_LICENSE_MODIFICATION_NAME	_T("Modification")
/// Registry key: license: jurisdiction of license
#define REG_LICENSE_JURISDICTION_NAME	_T("Jurisdiction")
/// Registry key: license: Sampling license type
#define REG_LICENSE_SAMPLING_NAME		_T("Sampling")
/// Registry key: license: Name
#define REG_LICENSE_NAME_NAME			_T("Name")
/// Registry key: license: URI
#define REG_LICENSE_URI_NAME			_T("URI")

/// Registry key for license stamp location
#define REG_LOCATION_BASE_NAME			_T("Location")

/// Registry key: stamp: Put on first page
#define REG_LOCATION_FIRSTPAGE_NAME		_T("FirstPage")
/// Registry key: stamp: Location on first page (X)
#define REG_LOCATION_FIRSTPAGE_X_NAME	_T("FirstPageX")
/// Registry key: stamp: Location on first page (Y)
#define REG_LOCATION_FIRSTPAGE_Y_NAME	_T("FirstPageY")
/// Registry key: stamp: Put on other pages
#define REG_LOCATION_OTHERPAGES_NAME	_T("OtherPages")
/// Registry key: stamp: Location on other pages (X)
#define REG_LOCATION_OTHERPAGES_X_NAME	_T("OtherPagesX")
/// Registry key: stamp: Location on other pages (Y)
#define REG_LOCATION_OTHERPAGES_Y_NAME	_T("OtherPagesY")

/**
	@param hPrinter Handle to the printer
	@param info License information structure to fill
*/
void CCPrintLicenseInfo::ReadFromRegistry(HANDLE hPrinter, LicenseInfo& info)
{
	// Clean up first
	info.Clear();

	// Retrieve license data
	info.m_eLicense = (LicenseType)CCPrintRegistry::GetRegistryInt(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_TYPE_NAME, LTUnknown);
	info.m_bCommercialUse = CCPrintRegistry::GetRegistryBool(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_COMMERCIAL_NAME, false);
	info.m_eModification = (ModificationType)CCPrintRegistry::GetRegistryInt(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_MODIFICATION_NAME, MTUnknown);
	info.SetJurisdiction(CCPrintRegistry::GetRegistryString(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_JURISDICTION_NAME, _T("")).c_str());
	info.m_cJurisdiction[MAX_JURISDICTION - 1] = '\0';
	info.m_eSampling = (SamplingType)CCPrintRegistry::GetRegistryInt(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_SAMPLING_NAME, STUnknown);
	info.SetName(CCPrintRegistry::GetRegistryString(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_NAME_NAME, _T("")).c_str());
	info.SetURI(CCPrintRegistry::GetRegistryString(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_URI_NAME, _T("")).c_str());
}

/**
	@param hPrinter Handle to the printer
	@param info License information to write to registry
*/
void CCPrintLicenseInfo::WriteToRegistry(HANDLE hPrinter, const LicenseInfo& info)
{
	// Write license data
	CCPrintRegistry::SetRegistryInt(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_TYPE_NAME, (int)info.m_eLicense);
	CCPrintRegistry::SetRegistryBool(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_COMMERCIAL_NAME, info.m_bCommercialUse);
	CCPrintRegistry::SetRegistryInt(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_MODIFICATION_NAME, (int)info.m_eModification);
	CCPrintRegistry::SetRegistryString(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_JURISDICTION_NAME, info.m_cJurisdiction);
	CCPrintRegistry::SetRegistryInt(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_SAMPLING_NAME, (int)info.m_eSampling);
	CCPrintRegistry::SetRegistryString(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_NAME_NAME, info.m_cName);
	CCPrintRegistry::SetRegistryString(hPrinter, REG_LICENSE_BASE_NAME REG_LICENSE_URI_NAME, info.m_cURI);
}

/**
	@param hPrinter Handle to the printer
	@param info License location information structure to fill
*/
void CCPrintLicenseInfo::ReadFromRegistry(HANDLE hPrinter, LicenseLocationInfo& info)
{
	// Clear first
	info.Clear();
	// Write first page data
	info.eFirstPage = (LicenseLocation)CCPrintRegistry::GetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_FIRSTPAGE_NAME, LLNone);
	if (info.eFirstPage == LLUserDefined)
	{
		// Only write the location if it's user defined
		info.ptFirstPage.x = CCPrintRegistry::GetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_FIRSTPAGE_X_NAME, 0);
		info.ptFirstPage.y = CCPrintRegistry::GetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_FIRSTPAGE_Y_NAME, 0);
	}
	// Write the other page's data
	info.eOtherPages = (LicenseLocation)CCPrintRegistry::GetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_OTHERPAGES_NAME, LLNone);
	if (info.eOtherPages == LLUserDefined)
	{
		// Only write the location if it's user defined
		info.ptOtherPages.x = CCPrintRegistry::GetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_OTHERPAGES_X_NAME, 0);
		info.ptOtherPages.y = CCPrintRegistry::GetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_OTHERPAGES_Y_NAME, 0);
	}
}

/**
	@param hPrinter Handle to the printer
	@param info License location information structure to fill
*/
void CCPrintLicenseInfo::WriteToRegistry(HANDLE hPrinter, const LicenseLocationInfo& info)
{
	// Read the first page location data
	CCPrintRegistry::SetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_FIRSTPAGE_NAME, (int)info.eFirstPage);
	if (info.eFirstPage == LLUserDefined)
	{
		// User defined: read the X and Y
		CCPrintRegistry::SetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_FIRSTPAGE_X_NAME, (int)info.ptFirstPage.x);
		CCPrintRegistry::SetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_FIRSTPAGE_Y_NAME, (int)info.ptFirstPage.y);
	}
	// Read the other pages location data
	CCPrintRegistry::SetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_OTHERPAGES_NAME, (int)info.eOtherPages);
	if (info.eOtherPages == LLUserDefined)
	{
		// User defined: read X and Y
		CCPrintRegistry::SetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_OTHERPAGES_X_NAME, (int)info.ptOtherPages.x);
		CCPrintRegistry::SetRegistryInt(hPrinter, REG_LOCATION_BASE_NAME REG_LOCATION_OTHERPAGES_Y_NAME, (int)info.ptOtherPages.y);
	}
}
