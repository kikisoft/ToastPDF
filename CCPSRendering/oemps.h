/**
	@file
	@brief Public definitions for printer driver rendering hooks
			Based on oemps.h
			Printer Driver Rendering Plugin Sample
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

#ifndef _OEMPS_H
#define _OEMPS_H

#include "DEVMODE.H"
#include "debug.h"

////////////////////////////////////////////////////////
//      OEM Defines
////////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// Warning: the following enum order must match the 
//          order in OEMHookFuncs[].
///////////////////////////////////////////////////////
typedef enum tag_Hooks {
    UD_DrvStartPage,
    UD_DrvSendPage,
    UD_DrvStartDoc,
    UD_DrvEndDoc,

    MAX_DDI_HOOKS,

} ENUMHOOKS;


typedef struct _OEMPDEV {
    //
    // define whatever needed, such as working buffers, tracking information,
    // etc.
    //
    // This test DLL hooks out every drawing DDI. So it needs to remember
    // PS's hook function pointer so it call back.
    //
    PFN     pfnPS[MAX_DDI_HOOKS];

    //
    // define whatever needed, such as working buffers, tracking information,
    // etc.
    //
    DWORD     dwReserved[1];
	class IOemPS* pOemPS;
    struct IPrintOemDriverPS*  pOEMHelp;
	UINT		nPage;

} OEMPDEV, *POEMPDEV;


#endif





