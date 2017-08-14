/**
	@file
	@brief Precompiled header header ;D
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

#ifndef _PRECOMP_H
#define _PRECOMP_H


// Necessary for compiling under VC.
#if(!defined(WINVER) || (WINVER < 0x0500))
	#undef WINVER
	#define WINVER          0x0500
#endif
#if(!defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0500))
	#undef _WIN32_WINNT
	#define _WIN32_WINNT    0x0500
#endif
#if !defined(WINNT)
	#define WINNT
#endif


// Required header files that shouldn't change often.

#pragma warning (disable: 4786)


#include <STDDEF.H>
#include <STDLIB.H>
#include <OBJBASE.H>
#include <STDARG.H>
#include <STDIO.H>
#include <WINDEF.H>
#include <WINERROR.H>
#include <WINBASE.H>
#include <WINGDI.H>
#include <WINDDI.H>
#include <WINSPOOL.H>
#include <TCHAR.H>
#include <EXCPT.H>
#include <ASSERT.H>
#include <PRSHT.H>
#include "COMPSTUI.H"
#include <WINDDIUI.H>
#include <PRINTOEM.H>

// Define from ntdef.h in Win2K SDK.
// NT 4 may not have this defined
// in the public headers.
#ifndef NOP_FUNCTION
  #if (_MSC_VER >= 1210)
    #define NOP_FUNCTION __noop
  #else
    #define NOP_FUNCTION (void)0
  #endif
#endif


#define COUNTOF(p)  (sizeof(p)/sizeof(*(p)))



#endif



