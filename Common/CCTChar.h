/**
	@file
	@brief This file contains Unicode/MultiByte competability functions and a generic string class (tstring)
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

#ifndef _CCTCHAR_H_
#define _CCTCHAR_H_

#ifdef _UNICODE
/// tstring for Unicode compilations: wstring
#define tstring wstring
#else
/// tstrign for MultiByte complications: string
#define tstring string
#endif

#include <string>

/// Returns a MultiByte string from a generic string object
std::string MakeAnsiString(const std::tstring& sString);
/// Returns a MultiByte string from a generic NULL-terminated buffer
std::string MakeAnsiString(LPCTSTR pString);
/// Returns a Unicode string from a generic string object
std::wstring MakeWideString(const std::tstring& sString);
/// Returns a Unicode string from a generic NULL-terminated buffer
std::wstring MakeWideString(LPCTSTR pString);

/// Returns a generic string object from a MultiByte string
std::tstring MakeTString(const std::string& sString);
/// Returns a generic string object from a Unicode string
std::tstring MakeTString(const std::wstring& sString);

#endif   //#define _CCTCHAR_H_
