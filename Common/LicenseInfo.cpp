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

#include "LicenseInfo.h"

/**
	
*/
void LicenseInfo::Clear()
{
	// Clean it all up
	m_eLicense = LicenseInfo::LTUnknown;
	m_bCommercialUse = false;
	m_eModification = LicenseInfo::MTUnknown;
	m_cJurisdiction[0] = '\0';
	m_eSampling = LicenseInfo::STUnknown;
	m_cName[0] = '\0';
	m_cURI[0] = '\0';
}

/**
	@param lpJurisdiction The new jurisdiction
*/
void LicenseInfo::SetJurisdiction(LPCTSTR lpJurisdiction)
{
	_tcsncpy(m_cJurisdiction, lpJurisdiction, MAX_JURISDICTION - 1);
	m_cJurisdiction[MAX_JURISDICTION - 1] = '\0';
}

/**
	@param lpName The new name of the license
*/
void LicenseInfo::SetName(LPCTSTR lpName)
{
	_tcsncpy(m_cName, lpName, MAX_NAME - 1);
	m_cName[MAX_NAME - 1] = '\0';
}

/**
	@param lpURI The new URI of the license
*/
void LicenseInfo::SetURI(LPCTSTR lpURI)
{
	_tcsncpy(m_cURI, lpURI, MAX_URI - 1);
	m_cURI[MAX_URI - 1] = '\0';
}
