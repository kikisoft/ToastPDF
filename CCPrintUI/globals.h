/**
	@file
	@brief Lists of globals declared in Globals.cpp.
			based on globals.h
			Printer Driver UI Plugin Sample
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

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "CCTChar.h"

///////////////////////////////////////
//          Globals
///////////////////////////////////////

/// Module's Instance handle from DLLEntry of process.
extern HINSTANCE   ghInstance;

/// Function to load a string from the resources
std::tstring LoadResourceString(UINT uID);

#endif


