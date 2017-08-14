/**
	@file
	@brief 
*/

/*
 * CC PDF Converter: Windows PDF Printer with Creative Commons license support
 * and CCInfo Acrobat plugin: wizard for adding Create Commons licenses to documents
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

#ifndef __LICENSEINFO_H__
#define __LICENSEINFO_H__

#include "XMLite.h"

#define MAX_JURISDICTION		128
#define MAX_NAME				256
#define MAX_URI					256

/**
    @brief License information class

	Static members of this class hold cached XML license information; an instance of this class
	holds license information that the user set in the license wizard
*/
struct LicenseInfo
{
public:
	/**
		@brief Default constructor: initialize the structure
	*/
	LicenseInfo() : m_eLicense(LicenseInfo::LTUnknown), m_bCommercialUse(false), m_eModification(LicenseInfo::MTUnknown), m_eSampling(LicenseInfo::STUnknown) {m_cJurisdiction[0] = '\0'; m_cURI[0] = '\0'; m_cName[0] = '\0';};
	/**
		@brief Copy constructor
		@param info The license information to copy
	*/
	LicenseInfo(const LicenseInfo& info) : m_eLicense(info.m_eLicense), m_bCommercialUse(info.m_bCommercialUse), m_eModification(info.m_eModification), m_eSampling(info.m_eSampling) {_tcscpy(m_cJurisdiction, info.m_cJurisdiction); _tcscpy(m_cURI, info.m_cURI); _tcscpy(m_cName, info.m_cName);};

public:
	// Definitions
	/// License type
	typedef enum 
	{
		LTCC = 0,
		LTSampling = 1,
		LTPublicDomain = 2,
		LTDevelopingNations = 3,
		LTNone = 4,
		LTUnknown
	} LicenseType;
	/// Allowed modifictions
	typedef enum
	{
		MTAllow = 0,
		MTShareAlike = 1,
		MTNo = 2,
		MTUnknown
	} ModificationType;
	/// Sampling license type
	typedef enum
	{
		STSampling,
		STSamplingPlus,
		STSamplingNC,
		STUnknown
	} SamplingType;
	/// List of cached licenses
	typedef enum
	{
		LLDTCC = 0,
		LLDTSampling,
		LLDTCC_by_nc,
		LLDTCC_by_nc_sa,
		LLDTCC_by_nc_nd,
		LLDTCC_by,
		LLDTCC_by_sa,
		LLDTCC_by_nd,
		LLDTSampling_by,
		LLDTSampling_by_plus,
		LLDTSampling_by_nc_plus,
		LLDT_Count
	} LoadedLicenseDataType;

public:
	// Members
	/// Type of license the user wants to set
	LicenseInfo::LicenseType			m_eLicense;
	/// Does this license allow commercial use of the matterial?
	bool				m_bCommercialUse;
	/// Type of modifications allowed in the license
	LicenseInfo::ModificationType	m_eModification;
	/// Type of sampling license
	LicenseInfo::SamplingType	m_eSampling;
	/// Name of jurisdiction the laws of which the license will be subject to
	TCHAR				m_cJurisdiction[MAX_JURISDICTION];

	// License contents
	/// Name of the license
	TCHAR				m_cName[MAX_NAME];
	/// URI of the license
	TCHAR				m_cURI[MAX_URI];

public:
	/// Reset the license information to a clean license
	void				Clear();
	/**
		@brief Tests if this license has a specific jurisdiction
		@return true if the license has a specific jurisdiction, false if not
	*/
	bool				HasJurisdiction() const {return (m_cJurisdiction[0] != '\0');};
	/// Sets the license jurisdiction
	void				SetJurisdiction(LPCTSTR lpJurisdiction);
	/// Sets the license name
	void				SetName(LPCTSTR lpName);
	/// Sets the license URI
	void				SetURI(LPCTSTR lpURI);

	/**
		@brief Copy operator for the license information structure
		@param other The license information to copy
		@return Refernece to this license information structure
	*/
	const LicenseInfo&	operator=(const LicenseInfo& other) {m_eLicense = other.m_eLicense; m_bCommercialUse = other.m_bCommercialUse; m_eModification = other.m_eModification; m_eSampling = other.m_eSampling; _tcscpy(m_cJurisdiction, other.m_cJurisdiction); _tcscpy(m_cName, other.m_cName); _tcscpy(m_cURI, other.m_cURI); return *this;};
};



#endif /* __LICENSEINFO_H__ */