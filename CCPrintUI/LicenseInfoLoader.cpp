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
#include "LicenseInfoLoader.h"
#include "resource.h"

#include "CCPrintProgressDlg.h"
#include "globals.h"
#include "debug.h"

/// Cached XML license information
XDoc LicenseInfoLoader::m_xmlData[LLDT_Count];
/// Flags indicating if the license was retrieved from the CC website in this session
bool LicenseInfoLoader::m_bRetrievedFromWeb[LLDT_Count];

/// List of license data retrieval URLs
LPCTSTR CCINFO_URL[] =
{
	_T("/rest/1.5/license/standard"),
	_T("/rest/1.5/license/recombo"),
	NULL
};

/// List of license image retrieval URLs
LPCTSTR CCINFO_IMAGE_URL[] =
{
	NULL,
	NULL,
	_T("http://i.creativecommons.org/l/by-nc/3.0/88x31.png"),
	_T("http://i.creativecommons.org/l/by-nc-sa/3.0/88x31.png"),
	_T("http://i.creativecommons.org/l/by-nc-nd/3.0/88x31.png"),
	_T("http://i.creativecommons.org/l/by/3.0/88x31.png"),
	_T("http://i.creativecommons.org/l/by-sa/3.0/88x31.png"),
	_T("http://i.creativecommons.org/l/by-nd/3.0/88x31.png"),
	_T("http://i.creativecommons.org/l/sampling/1.0/88x31.png"),
	_T("http://i.creativecommons.org/l/sampling+/1.0/88x31.png"),
	_T("http://i.creativecommons.org/l/nc-sampling+/1.0/88x31.png"),
	NULL
};

/**
	@param x Byte to make hax-byte from
	@return Hex-fixed character for the received one
*/
inline TCHAR toHex(const BYTE &x)
{
	return x > 9 ? x + 55: x + 48;
}

/**
	@param pIn String to encode
	@return URL-encoded string

	This function replaces some characters with Hex representations to conform to the URL specifications
*/
std::tstring URLEncode(LPCTSTR pIn)
{
	// Initialize variables
    std::tstring sOut;
	
    const size_t nLen = _tcslen(pIn) + 1;
    register LPTSTR pOutTmp = NULL;
    LPTSTR pOutBuf = NULL;
    register LPCTSTR pInTmp = NULL;
    LPCTSTR pInBuf = pIn;
    TCHAR b = 0;
	
    // Allocate out buffer
    pOutBuf = (LPTSTR)new TCHAR[nLen * 3];
    if (pOutBuf == NULL)
		// Bad, bad buffer!
		return sOut;

	// Start
    pInTmp	= pInBuf;
	pOutTmp = pOutBuf;
	
	// Do encoding
	while (*pInTmp)
	{
		if(_istalnum(*pInTmp))
			*pOutTmp++ = *pInTmp;
		else
			if(_istspace(*pInTmp))
			*pOutTmp++ = '+';
		else
		{
			*pOutTmp++ = '%';
			*pOutTmp++ = toHex((unsigned char)(*pInTmp)>>4);
			*pOutTmp++ = toHex((unsigned char)(*pInTmp)%16);
		}
		pInTmp++;
	}

	// Finished, set to the output string
	*pOutTmp = '\0';
	sOut = (LPCTSTR)pOutBuf;
	delete [] pOutBuf;

    return sOut;
}

/**
	@return Resource ID of the license image
*/
UINT LicenseInfoLoader::GetLicenseImageID() const
{
	// Which license are we talking about?
	switch (m_eLicense)
	{
		case LTCC:
			// Creative commons:
			if (m_eModification != MTUnknown)
			{
				// OK, start with the modification
				UINT uBase = IDPNG_BY_NC + (int)m_eModification;
				if (m_bCommercialUse)
					// Add commercial use (if allowed)
					uBase += 3;
				// That's it
				return uBase;
			}
			break;
		case LTSampling:
			if (m_eSampling == STUnknown)
				break;
			// Sampling base + type
			return IDPNG_SAMPLING + (int)m_eSampling;
		case LTDevelopingNations:
		case LTPublicDomain:
			// Those don't have specific images
			return IDPNG_SOMERIGHTS;
		default:
			break;
	}

	// None
	return 0;
}

/**
	@param eType Type of license (CC or Sampling)
	@return The URL to retrieve license creation data from
*/
LPCTSTR	LicenseInfoLoader::GetLicenseURL(LoadedLicenseDataType eType)
{
	if (eType > LLDTSampling)
		// Nothing if it's not CC or Sampling
		return NULL;
	return CCINFO_URL[(int)eType];
}

/**
	@param eType Type of license (only actual CC or Sampling licenses supported)
	@return The URL to retrieve the image from
*/
LPCTSTR	LicenseInfoLoader::GetImageURL(LoadedLicenseDataType eType)
{
	return CCINFO_IMAGE_URL[eType];
}

/**
	@return Name of the license, including jurisdication (if any)
*/
std::tstring LicenseInfoLoader::GetLicenseName() const
{
	// Get the name
	std::tstring sRet = m_cName;
	if (HasJurisdiction())
	{
		// Add jurisdiction
		sRet += _T(", ");
		sRet += m_cJurisdiction;
	}

	return sRet;
}


/**
	@param eType Type of license to check
	@return true if cached license information is valid
*/
bool LicenseInfoLoader::IsValidLicenseInfo(LoadedLicenseDataType eType) 
{
	// Call the generic testing function
	return IsValidLicenseInfo(eType, m_xmlData[eType]);
}

/**
	@param eType Type of license to check
	@param doc XML object to validate the contents of
	@return true if the object holds valid license data, false otherwise
*/
bool LicenseInfoLoader::IsValidLicenseInfo(LoadedLicenseDataType eType, XDoc& doc)
{
	switch (eType)
	{
		case LLDTCC:
		case LLDTSampling:
			// Do we have data and it's of <licenseclass> type?
			return (doc.GetRoot() != NULL) && (doc.GetRoot()->name == _T("licenseclass"));
		default:
			// Later ### - for now, just make sure it ain't empty
			return (doc.GetRoot() != NULL);
	}
}

/**
	@param eType Type of license to load into the cache
	@param pXML XML string to load
	@return true if XML loaded successfully and the license data is valid, false otherwise
*/
bool LicenseInfoLoader::LoadData(LoadedLicenseDataType eType, LPCTSTR pXML)
{
	// Use the generic loading function
	return LoadAndValidateData(eType, m_xmlData[eType], pXML);
}

/**
	@param eType Type of license to load
	@param doc XML object to fill with the license data
	@param pXML XML string to load
	@return true if XML object loaded with valid data, false otherwise
*/
bool LicenseInfoLoader::LoadAndValidateData(LoadedLicenseDataType eType, XDoc& doc, LPCTSTR pXML)
{
	// Load the XML
	PARSEINFO info;
	doc.Close();
	doc.Load(pXML, &info);
	if (info.erorr_occur)
		// Error!
		return false;

	// Make sure it's valid
	return IsValidLicenseInfo(eType, doc);
}

/**
	@return The type of license needed according to the information in the members
*/
LicenseInfo::LoadedLicenseDataType LicenseInfoLoader::GetCCLicenseType() const
{
	// Is this a Creative Commons license?
	if (m_eLicense == LTCC)
	{
		// Yeah, what kind of modifications allowed?
		switch (m_eModification)
		{
			case MTAllow:
				// All; return by commerical use
				return m_bCommercialUse ? LLDTCC_by : LLDTCC_by_nc;
			case MTShareAlike:
				// Share-alike; return by commerical use
				return m_bCommercialUse ? LLDTCC_by_nc_sa : LLDTCC_by_sa;
			case MTNo:
				// None; return by commerical use
				return m_bCommercialUse ? LLDTCC_by_nc_nd : LLDTCC_by_nd;
			default:
				// Weird!
				return LLDT_Count;
		}
	}
	else if (m_eLicense == LTSampling)
	{
		// It's a sampling license:
		switch (m_eSampling)
		{
			case STSampling:
				// Just sampling
				return LLDTSampling_by;
			case STSamplingPlus:
				// Sampling plus
				return LLDTSampling_by_plus;
			case STSamplingNC:
				// Sampling plus with no commercial allowed:
				return LLDTSampling_by_nc_plus;
		}
	}

	// Not something we can cache:
	return LLDT_Count;
}

/**
	
*/
void LicenseInfoLoader::InitLicenseRetrievedData()
{
	for (int i=0;i<LLDT_Count;i++)
		m_bRetrievedFromWeb[i] = false;
}

/**
	@param eType Type of license creation information to load (only CC or Sampling allowed)
	@param hWnd Handle of parent window
	@param bCancel Reference to the cancel flag; will be set to true if the user canceled the request
	@return true if license retrieved successfully, false if failed
*/
bool LicenseInfoLoader::RequestLicenseType(LoadedLicenseDataType eType, HWND hWnd, bool& bCancel)
{
	bCancel = false;
	if (eType > LLDTSampling)
		// Only allow CC or Sampling
		return false;

	if (IsLicenseRetrievedData(eType))
		// Already retreived, don't try again
		return true;

	// Try to get the data from the Web Service:
	// Display a progress dialog so the user won't feel we are stuck; also for canceling
	CCPrintProgressDlg dlg;
	// Run the request from the dialog
	InternetRequest request;
	std::tstring sURL = GetLicenseURL(eType);
	if (dlg.DoRequest(hWnd, request, sURL, _T("api.creativecommons.org")))
	{
		// Got it:
		unsigned int dwSize;
		const char* pBuffer = dlg.m_buffer.GetData(dwSize);
		std::tstring sXML = MakeTString(std::string(pBuffer, dwSize));
		// Try to load it:
		if (!LoadData(eType, sXML.c_str()))
			// Failed... so load the cached version
			if (!LoadInternalLicenseInfo(eType))
				return false;
	}
	else
	{
		// Did the user cancel?
		if (dlg.GetCancel())
			// Yeah
			bCancel = true;
		// Try to load from the internal cache
		if (!LoadInternalLicenseInfo(eType))
			return false;
	}

	// Remember that we have already retrieved the data, so we won't do it again in this session
	SetLicenseRetrieved(eType);
	return true;
}

/**
	@param eType Type of license information to retrieve
	@return true if loaded from resource successfully, false if failed
*/
bool LicenseInfoLoader::LoadInternalLicenseInfo(LoadedLicenseDataType eType)
{
	// Did we load it already?
	if (IsValidLicenseInfo(eType))
		// Yeah, OK
		return true;

	// Find the XML resource:
	HRSRC hRes = FindResource(ghInstance, MAKEINTRESOURCE(IDX_STANDARD + (int)eType), _T("XML"));
	if (hRes == NULL)
		// Not found!
		return false;

	// Retrieve the data
	DWORD dwSize = SizeofResource(ghInstance, hRes);
	HGLOBAL hXML = LoadResource(ghInstance, hRes);
	if (hXML == NULL)
		// Error!
		return false;

	// Lock it and put it in a string
	LPVOID pXML = LockResource(hXML);
	std::tstring s = MakeTString(std::string((const char*)pXML, dwSize));
	UnlockResource(hXML);

	// Now load the data into the license cache
	return LoadData(eType, s.c_str());
}

/**
	@param hParent Handle to parent window
	@param doc Reference to XML document to fill
	@param bCancel Reference to cancel flag; will be set to true if the user cancels the request
	@return true if the license information was retrieved successfully, false if failed
*/
bool LicenseInfoLoader::RequestLicense(HWND hParent, XDoc& doc, bool& bCancel)
{
	// Check what kind of license are we after:
	LoadedLicenseDataType eType = GetCCLicenseType();
	if (eType == LLDT_Count)
	{
		// Error: not something we can define
#ifdef _DEBUG
		assert(false);
#endif
		return false;
	}

	// Can we use the cached version?
	if (!HasJurisdiction() && IsLicenseRetrievedData(eType))
	{
		// Yes, no jurisdiction and we already got this one before in this session:
		doc = GetLicenseXML(eType);
		return true;
	}

	// Try to get the data

	// Initialize veriables
	TCHAR cHeaders[1024];
	XDoc answers;
	std::tstring sJuri, sRequestURL;

	// Build the answers header that will be sent to the Web Service
	// More information can be found at http://api.creativecommons.org/docs/readme_15.html
	LPXNode pAnswers = answers.AppendChild(_T("answers"));
	LoadedLicenseDataType eBaseType;
	LPXNode pClass;
	if (m_eLicense == LTCC)
	{
		// This is a Creative Commons standard license, add information:
		pClass = pAnswers->AppendChild(_T("license-standard"));
		pClass->AppendChild(_T("commercial"), m_bCommercialUse ? _T("y") : _T("n"));
		pClass->AppendChild(_T("derivatives"), (m_eModification == MTAllow) ? _T("y") : (m_eModification == MTShareAlike) ? _T("sa") : _T("n"));
		eBaseType = LLDTCC;
		sRequestURL = _T("/rest/1.5/license/standard/issue");
	}
	else
	{
		// This is a sampling license:
		pClass = pAnswers->AppendChild(_T("license-recombo"));
		pClass->AppendChild(_T("sampling"), (m_eSampling == STSampling) ? _T("sampling") : (m_eSampling == STSamplingPlus) ? _T("samplingplus") : _T("ncsamplingplus"));
		eBaseType = LLDTSampling;
		sRequestURL = _T("/rest/1.5/license/recombo/issue");
	}

	// Did the user specify a jurisdiction?
	DISP_OPT opt;
	opt.newline = false;
	if (HasJurisdiction())
	{
		// Yeah, find it in the license definition so we can get the ID we need to send to the Web Service
		const LPXNode pJuri = GetLicenseXML(eBaseType).Find(_T("field"), _T("jurisdiction"));
		if (pJuri != NULL)
		{
			// Go over the jurisdiction nodes:
			for (XNodes::const_iterator i = pJuri->childs.begin(); sJuri.empty() && (i != pJuri->childs.end()); i++)
			{
				// Does it have an id?
				const LPXAttr pID = (*i)->GetAttr(_T("id"));
				if ((pID == NULL) || pID->value.empty())
					// Na, so it's the general one, continue
					continue;
				const LPXNode pLabel = (*i)->GetChild(_T("label"));
				if (pLabel != NULL)
				{
					// Is this the one?
					if (pLabel->GetText(&opt) == m_cJurisdiction)
						// Yeah, use this id
						sJuri = pID->value;
				}
			}
		}
	}

	// OK, add the jurisdiction:
	pClass->AppendChild(_T("jurisdiction"), sJuri.c_str());

	// Get the answers XML string and encode it for transfer to the Web Service
	opt.reference_value = true;
	opt.newline = false;
	std::tstring sOptional = answers.GetXML(&opt);
	sOptional = URLEncode(sOptional.c_str());
	if (sOptional.empty())
		// Error!
		return false;

	// OK, create the headers we'll need:
	sOptional = _T("answers=") + sOptional;
	_stprintf(cHeaders, _T("Content-Type: application/x-www-form-urlencoded\r\nContent-Length: %d"), sOptional.size());

	// Run the request will displaying the progress dialog
	CCPrintProgressDlg dlg;
	std::tstring sError;
	InternetRequest request;
	bool bSuccess = false;
	if (dlg.DoRequest(hParent, request, sRequestURL, _T("api.creativecommons.org"), MakeAnsiString(sOptional).c_str(), cHeaders))
	{
		// Success! Retrieve the data:
		unsigned int dwSize;
		const char* pBuffer = dlg.m_buffer.GetData(dwSize);
		std::tstring sXML = MakeTString(std::string(pBuffer, dwSize));
		// Load it into the XML object and make sure we can use it
		bSuccess = LoadAndValidateData(eType, doc, sXML.c_str());
		if (bSuccess)
		{
			if (!HasJurisdiction())
				// Good license data and no jurisdiction, so replace the cached data:
				LoadData(eType, sXML.c_str());
		}
		else
		{
			if (HasJurisdiction())
				sError = LoadResourceString(IDS_ERROR_RETRIEVEDATA_JURISDICTION);
			else
				sError = LoadResourceString(IDS_ERROR_RETRIEVEDATA);
		}
	}
	else
	{
		if (dlg.GetCancel())
		{
			// User canceled: notify about using cached data:
			if (HasJurisdiction())
				sError = LoadResourceString(IDS_WARNING_USERCANCEL_JURISDICTION);
			else
				sError = LoadResourceString(IDS_WARNING_USERCANCEL);
		}
		else
			sError = LoadResourceString(IDS_ERROR_RETRIEVELICENSE);
	}

	if (!bSuccess)
	{
		// Not a valid license information object, so use the cached one and notify the user
		bSuccess = LoadInternalLicenseInfo(eType);
		if (bSuccess)
		{
			doc = GetLicenseXML(eType);
			SetJurisdiction(_T(""));
		}
		else
		{
			// Could not even use the internal license info!!!
			ASSERT(FALSE);
			MessageBox(hParent, LoadResourceString(IDS_ERROR_RETRIEVELICENSE).c_str(), _T("CC PDF Printer"), MB_ICONWARNING|MB_OK);
			return false;
		}
	}

	if (bSuccess)
	{
		// Did we retrieve something we can cache?
		if (!HasJurisdiction())
			// Yeah, so set the flag
			SetLicenseRetrieved(eType);
	}

	// Is there something to notify?
	if (!sError.empty())
		// Yeah...
		MessageBox(hParent, sError.c_str(), _T("CC PDF Printer"), MB_ICONWARNING|MB_OK);

	return true;
}

/**
	@param hParent Handle to parent window
	@return true if license data parsed successfully, false if failed
*/
bool LicenseInfoLoader::LoadLicenseData(HWND hParent)
{
	XDoc doc;
	bool bCancel = false, bError = false;
	LPXNode pResult = NULL;
	switch (m_eLicense)
	{
		case LicenseInfo::LTCC:
			// Creative Commons standard license:
			// Update the license data from the Web Service and get it:
			if (!RequestLicense(hParent, doc, bCancel))
			{
				// Cannot!
				MessageBox(hParent, LoadResourceString(IDS_ERROR_RETRIEVEDATA_SHORT).c_str(), _T("CC PDF Printer"), MB_ICONWARNING|MB_OK);
				return false;
			}

			// Make sure we have the expected information in the XML:
			pResult = doc.Find(_T("result"));
			if (pResult == NULL)
				// Nope
				bError = true;
			else
			{
				// OK, retrieve the license URL and name:
				DISP_OPT opt;
				opt.reference_value = false;
				LPXNode pNode = pResult->GetChild(_T("license-uri"));
				if (pNode == NULL)
					bError = true;
				else
					SetURI(pNode->GetText(&opt).c_str());
				pNode = pResult->GetChild(_T("license-name"));
				if (pNode == NULL)
					bError = true;
				else
					SetName(pNode->GetText(&opt).c_str());
			}
			if (bError)
			{
				// Error - bail out
				MessageBox(hParent, LoadResourceString(IDS_ERROR_RETRIEVELICENSE).c_str(), _T("CC PDF Printer"), MB_ICONWARNING|MB_OK);
				return false;
			}
			break;
		case LicenseInfo::LTSampling:
			// Creative Commons sampling license:
			// Update the license inforamtion from the Web Service and get it:
			if (!RequestLicense(hParent, doc, bCancel))
			{
				// Cannot
				MessageBox(hParent, LoadResourceString(IDS_ERROR_RETRIEVEDATA_SHORT).c_str(), _T("CC PDF Printer"), MB_ICONWARNING|MB_OK);
				return false;
			}

			// Make sure we have the expected information in the XML:
			pResult = doc.Find(_T("result"));
			if (pResult == NULL)
				// Nope
				bError = true;
			else
			{
				// OK, retrieve the URL and name
				DISP_OPT opt;
				opt.reference_value = false;
				LPXNode pNode = pResult->GetChild(_T("license-uri"));
				if (pNode == NULL)
					bError = true;
				else
					SetURI(pNode->GetText(&opt).c_str());
				pNode = pResult->GetChild(_T("license-name"));
				if (pNode == NULL)
					bError = true;
				else
					SetName(pNode->GetText(&opt).c_str());
			}
			if (bError)
			{
				// An error:
				MessageBox(hParent, LoadResourceString(IDS_ERROR_RETRIEVELICENSE).c_str(), _T("CC PDF Printer"), MB_ICONWARNING|MB_OK);
				return false;
			}
			break;
		case LicenseInfo::LTDevelopingNations:
			// Creative Commons Developing Nations license:
			SetURI(_T("http://creativecommons.org/licenses/devnations/2.0/"));
			SetName(_T("Developing Nations"));
			SetJurisdiction(_T(""));
			break;
		case LicenseInfo::LTPublicDomain:
			// Creative Commons Public Domain license:
			SetURI(_T("http://creativecommons.org/licenses/publicdomain/"));
			SetName(_T("Public Domain"));
			SetJurisdiction(_T(""));
			break;
		case LicenseInfo::LTUnknown:
			ASSERT(FALSE);
			// Fall down
		case LicenseInfo::LTNone:
			// Remove license
			SetURI(_T(""));
			SetName(_T(""));
			SetJurisdiction(_T(""));
			break;
	}

	return true;
}
