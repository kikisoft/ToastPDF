/**
	@file
	@brief Implementation of Devmode functions shared with OEM UI and OEM rendering modules.
			Based on:
			Devmode.cpp
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

#include "precomp.h"
#include "debug.h"
#include "devmode.h"
#include "CCCommon.h"

#include "CCPrintLicenseInfo.h"
#include "CCPrintRegistry.h"

/**
	@param bFirstPage true if the requested information is for the first page
	@param szPageSize Size of the page
	@param szLicenseSize Size of the stamp
	@return Location of stamp on the page
*/
POINT LicenseLocationInfo::LocationForPage(bool bFirstPage, SIZE szPageSize, SIZE szLicenseSize) const
{
	LicenseLocation eLocation;
	POINT ptUser;

	// Should we use the first page information?
	if (bFirstPage || (eOtherPages == LLOther))
	{
		// Yeah
		eLocation = eFirstPage;
		ptUser = ptFirstPage;
	}
	else
	{
		// Nope, it's for the other pages
		eLocation = eOtherPages;
		ptUser = ptOtherPages;
	}
	
	// Initialize point
	POINT ptRet;
	ptRet.x = ptRet.y = -1;

	// Calculate according to the stamp location information:
	switch (eLocation)
	{
		case LLTop:
			ptRet.y = 0;
			ptRet.x = (szPageSize.cx - szLicenseSize.cx) / 2;
			break;
		case LLBottom:
			ptRet.y = szPageSize.cy - szLicenseSize.cy;
			ptRet.x = (szPageSize.cx - szLicenseSize.cx) / 2;
			break;
		case LLUserDefined:
			ptRet.x = min(szPageSize.cx - szLicenseSize.cx, ptUser.x);
			ptRet.y = min(szPageSize.cy - szLicenseSize.cy, ptUser.y);
			break;
		default:
			break;
	}

	return ptRet;
}

/**
	@param dwMode Initialization mode information (see IPrintOemPS::DevMode function in the DDK)
	@param pOemDMParam Pointer to an OEMDMPARAM structure containing printer data
	@return S_OK if successful, E_FAIL if failed
*/
HRESULT hrOEMDevMode(DWORD dwMode, POEMDMPARAM pOemDMParam)
{
    POEMDEV pOEMDevIn;
    POEMDEV pOEMDevOut;

    // Verify parameters.
    if( (NULL == pOemDMParam)
        ||
        ( (OEMDM_SIZE != dwMode)
          &&
          (OEMDM_DEFAULT != dwMode)
          &&
          (OEMDM_CONVERT != dwMode)
          &&
          (OEMDM_MERGE != dwMode)
        )
      )
    {
        ERR(ERRORTEXT("DevMode() ERROR_INVALID_PARAMETER.\r\n"));
        VERBOSE(DLLTEXT("\tdwMode = %d, pOemDMParam = %#lx.\r\n"), dwMode, pOemDMParam);

        SetLastError(ERROR_INVALID_PARAMETER);
        return E_FAIL;
    }

    // Cast generic (i.e. PVOID) to OEM private devomode pointer type.
    pOEMDevIn = (POEMDEV) pOemDMParam->pOEMDMIn;
    pOEMDevOut = (POEMDEV) pOemDMParam->pOEMDMOut;

    switch(dwMode)
    {
        case OEMDM_SIZE:
			// Just needs the size of the structure
            pOemDMParam->cbBufSize = sizeof(OEMDEV);
            break;

        case OEMDM_DEFAULT:
			// Fill in with default parameters
			pOEMDevOut->dmOEMExtra.dwSize       = sizeof(OEMDEV);
			pOEMDevOut->dmOEMExtra.dwSignature  = CCPRINT_SIGNATURE;
			pOEMDevOut->dmOEMExtra.dwVersion    = CCPRINT_VERSION;
			pOEMDevOut->cFilename[0]			= '\0';
			ReadOEMDevFromRegistry(pOEMDevOut, pOemDMParam->hPrinter);
            VERBOSE(DLLTEXT("pOEMDevOut after setting default values:\r\n"));
            Dump(pOEMDevOut);
            break;

        case OEMDM_CONVERT:
			// Convert data from old version
            ConvertOEMDevmode(pOEMDevIn, pOEMDevOut, pOemDMParam->hPrinter);
            break;

        case OEMDM_MERGE:
			// Merge data from old and new structures
            ConvertOEMDevmode(pOEMDevIn, pOEMDevOut, pOemDMParam->hPrinter);
            MakeOEMDevmodeValid(pOEMDevOut);
            break;
    }
	
	// Write it up
    Dump(pOemDMParam);

    return S_OK;
}

/**
	@param pOEMDevIn Pointer to the structure to read data from
	@param pOEMDevOut Pointer to the structure to write data into
	@param hPrinter Handle to the printer
	@return TRUE if all went well, FALSE if there's a problem
*/
BOOL ConvertOEMDevmode(PCOEMDEV pOEMDevIn, POEMDEV pOEMDevOut, HANDLE hPrinter)
{
    if( (NULL == pOEMDevIn)
        ||
        (NULL == pOEMDevOut)
      )
    {
        ERR(ERRORTEXT("ConvertOEMDevmode() invalid parameters.\r\n"));
        return FALSE;
    }

    // Check OEM Signature, if it doesn't match ours,
    // then just assume DMIn is bad and use defaults.
    if(pOEMDevIn->dmOEMExtra.dwSignature == pOEMDevOut->dmOEMExtra.dwSignature)
    {
        VERBOSE(DLLTEXT("Converting private OEM Devmode.\r\n"));
        VERBOSE(DLLTEXT("pOEMDevIn:\r\n"));
        Dump(pOEMDevIn);

        // Copy the old structure in to the new using which ever size is the smaller.
        // Devmode maybe from newer Devmode (not likely since there is only one), or
        // Devmode maybe a newer Devmode, in which case it maybe larger,
        // but the first part of the structure should be the same.

        // DESIGN ASSUMPTION: the private DEVMODE structure only gets added to;
        // the fields that are in the DEVMODE never change only new fields get added to the end.

		DWORD dwWrite = __min(pOEMDevOut->dmOEMExtra.dwSize, pOEMDevIn->dmOEMExtra.dwSize);
        memcpy(pOEMDevOut, pOEMDevIn, dwWrite);

        // Set the devmode defaults so that anything the isn't copied over will
        // be set to the default value.
		if (((pOEMDevIn->dmOEMExtra.dwSize < sizeof(OEMDEV)) || (pOEMDevIn->dmOEMExtra.dwVersion != CCPRINT_VERSION)) && (pOEMDevOut->dmOEMExtra.dwSize == sizeof(OEMDEV)))
		{
			dwWrite = sizeof(OEMDEV);
			pOEMDevOut->cFilename[0] = '\0';
			ReadOEMDevFromRegistry(pOEMDevOut, hPrinter);
		}

        // Re-fill in the size and version fields to indicated 
        // that the DEVMODE is the current private DEVMODE version.
        pOEMDevOut->dmOEMExtra.dwSize       = dwWrite;
        pOEMDevOut->dmOEMExtra.dwVersion    = CCPRINT_VERSION;
    }
    else
    {
        WARNING(DLLTEXT("Unknown DEVMODE signature, pOEMDMIn ignored.\r\n"));

        // Don't know what the input DEVMODE is, so just use defaults.
        pOEMDevOut->dmOEMExtra.dwSize       = sizeof(OEMDEV);
        pOEMDevOut->dmOEMExtra.dwSignature  = CCPRINT_SIGNATURE;
        pOEMDevOut->dmOEMExtra.dwVersion    = CCPRINT_VERSION;
		pOEMDevOut->cFilename[0]			= '\0';
		ReadOEMDevFromRegistry(pOEMDevOut, hPrinter);
    }

    return TRUE;
}


/**
	@param pOEMDevmode Pointer to the printer data structure to initialize
	@return TRUE if initialized successfully, FALSE if failed
*/
BOOL MakeOEMDevmodeValid(POEMDEV pOEMDevmode)
{
    if(NULL == pOEMDevmode)
    {
		// Bad parameter!
        return FALSE;
    }

    // ASSUMPTION: pOEMDevmode is large enough to contain OEMDEV structure.

    // Make sure that dmOEMExtra indicates the current OEMDEV structure.
    pOEMDevmode->dmOEMExtra.dwSize       = sizeof(OEMDEV);
    pOEMDevmode->dmOEMExtra.dwSignature  = CCPRINT_SIGNATURE;
    pOEMDevmode->dmOEMExtra.dwVersion    = CCPRINT_VERSION;

	int i;
	for (i=0;i<MAX_PATH+1;i++)
	{
		if (pOEMDevmode->cFilename[i] == '\0')
			break;
	}
	if (i == MAX_PATH + 1)
		pOEMDevmode->cFilename[0] = '\0';

    return TRUE;
}

/**
	@param pOEMDevmode Pointer to the printer data structure to dump
*/
void Dump(PCOEMDEV pOEMDevmode)
{
    if( (NULL != pOEMDevmode)
        &&
        (pOEMDevmode->dmOEMExtra.dwSize >= sizeof(OEMDEV))
        &&
        (CCPRINT_SIGNATURE == pOEMDevmode->dmOEMExtra.dwSignature)
      )
    {
        VERBOSE(__TEXT("\tdmOEMExtra.dwSize      = %d\r\n"), pOEMDevmode->dmOEMExtra.dwSize);
        VERBOSE(__TEXT("\tdmOEMExtra.dwSignature = %#x\r\n"), pOEMDevmode->dmOEMExtra.dwSignature);
        VERBOSE(__TEXT("\tdmOEMExtra.dwVersion   = %#x\r\n"), pOEMDevmode->dmOEMExtra.dwVersion);
        VERBOSE(__TEXT("\tOutputFile             = %.*s\r\n"), MAX_PATH, pOEMDevmode->cFilename);
        VERBOSE(__TEXT("\tAutoOpen               = %s\r\n"), pOEMDevmode->bAutoOpen ? __TEXT("Yes") : __TEXT("No"));
        VERBOSE(__TEXT("\tWriteProperties        = %s\r\n"), pOEMDevmode->bSetProperties ? __TEXT("Yes") : __TEXT("No"));
        VERBOSE(__TEXT("\tLocation               = %u\r\n"), (UINT)pOEMDevmode->location.eFirstPage);
        VERBOSE(__TEXT("\tLocation(2)            = %u\r\n"), (UINT)pOEMDevmode->location.eOtherPages);
        VERBOSE(__TEXT("\tLicense                = %u\r\n"), (UINT)pOEMDevmode->info.m_eLicense);
    }
    else
    {
        ERR(ERRORTEXT("Dump(POEMDEV) unknown private OEM DEVMODE.\r\n"));
    }
}

/**
	@param pDev Pointer to the plugin data
	@param hPrinter Handle to the printer
*/
void ReadOEMDevFromRegistry(POEMDEV pDev, HANDLE hPrinter)
{
	// Read the auto open and XMP settings
	pDev->bAutoOpen = CCPrintRegistry::GetRegistryBool(hPrinter, (LPTSTR)SETTINGS_AUTOOPEN, true);
	pDev->bSetProperties = CCPrintRegistry::GetRegistryBool(hPrinter, (LPTSTR)SETTINGS_WRITEPROPERTIES, true);

	// Read the license and stamp locations
	CCPrintLicenseInfo::ReadFromRegistry(hPrinter, pDev->info);
	CCPrintLicenseInfo::ReadFromRegistry(hPrinter, pDev->location);
}

/**
	@param pDev Pointer to the plugin data
	@param hPrinter Handle to the printer
*/
void WriteOEMDevToRegistry(const POEMDEV pDev, HANDLE hPrinter)
{
	// Write the auto open and XMP settings
	CCPrintRegistry::SetRegistryBool(hPrinter, (LPTSTR)SETTINGS_AUTOOPEN, pDev->bAutoOpen ? true : false);
	CCPrintRegistry::SetRegistryBool(hPrinter, (LPTSTR)SETTINGS_WRITEPROPERTIES, pDev->bSetProperties ? true : false);

	// Write the license and stamp locations
	CCPrintLicenseInfo::WriteToRegistry(hPrinter, pDev->info);
	CCPrintLicenseInfo::WriteToRegistry(hPrinter, pDev->location);
}

