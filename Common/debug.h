/**
	@file
	@brief Define common data types, and external function prototypes
			for debugging functions.
			Based on:
			Debug.H 
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

#ifndef _DEBUG_H
#define _DEBUG_H


// VC and Build use different debug defines.
// The following makes it so either will
// cause the inclusion of debugging code.
#if !defined(_DEBUG) && defined(DBG)
    #define _DEBUG      DBG
#elif defined(_DEBUG) && !defined(DBG)
    #define DBG         _DEBUG
#endif

#ifdef KERNEL_MODE
#define DLLTEXT(s)      __TEXT("CCPSRendering:  ") __TEXT(s)
#else
#define DLLTEXT(s)      __TEXT("CCPrintUI:  ") __TEXT(s)
#endif
#define ERRORTEXT(s)    __TEXT("ERROR ") DLLTEXT(s)


/////////////////////////////////////////////////////////
//		Macros
/////////////////////////////////////////////////////////

//
// These macros are used for debugging purposes. They expand
// to white spaces on a free build. Here is a brief description
// of what they do and how they are used:
//
// giDebugLevel
//  Global variable which set the current debug level to control
//  the amount of debug messages emitted.
//
// VERBOSE(msg)
//  Display a message if the current debug level is <= DBG_VERBOSE.
//
// TERSE(msg)
//  Display a message if the current debug level is <= DBG_TERSE.
//
// WARNING(msg)
//  Display a message if the current debug level is <= DBG_WARNING.
//  The message format is: WRN filename (linenumber): message
//
// ERR(msg)
//  Similiar to WARNING macro above - displays a message
//  if the current debug level is <= DBG_ERROR.
//
// ASSERT(cond)
//  Verify a condition is true. If not, force a breakpoint.
//
// ASSERTMSG(cond, msg)
//  Verify a condition is true. If not, display a message and
//  force a breakpoint.
//
// RIP(msg)
//  Display a message and force a breakpoint.
//
// Usage:
//  These macros require extra parantheses for the msg argument
//  example, ASSERTMSG(x > 0, ("x is less than 0\n"));
//           WARNING(("App passed NULL pointer, ignoring...\n"));
//

#define DBG_VERBOSE 1
#define DBG_TERSE   2
#define DBG_WARNING 3
#define DBG_ERROR   4
#define DBG_RIP     5
#define DBG_NONE    6

#if DBG

    #define DebugMsg    DebugMessage

    //
    // Strip the directory prefix from a filename (ANSI version)
    //

    PCSTR
    StripDirPrefixA(
        IN PCSTR    pstrFilename
        );

    extern INT giDebugLevel;


    #define DBGMSG(level, prefix, msg) { \
                if (giDebugLevel <= (level)) { \
                    DebugMsg("%s %s (%d): ", prefix, StripDirPrefixA(__FILE__), __LINE__); \
                    DebugMsg(msg); \
                } \
            }

    #define DBGPRINT(level, msg) { \
                if (giDebugLevel <= (level)) { \
                    DebugMsg(msg); \
                } \
            }

    #define VERBOSE         if(giDebugLevel <= DBG_VERBOSE) DebugMsg
    #define TERSE           if(giDebugLevel <= DBG_TERSE) DebugMsg
    #define WARNING         if(giDebugLevel <= DBG_WARNING) DebugMsg
    #define ERR             if(giDebugLevel <= DBG_ERROR) DebugMsg

    #define ASSERT(cond) { \
                if (! (cond)) { \
                    RIP(("\n")); \
                } \
            }

    #define ASSERTMSG(cond, msg) { \
                if (! (cond)) { \
                    RIP(msg); \
                } \
            }

    #define RIP(msg) { \
                DBGMSG(DBG_RIP, "RIP", msg); \
                DebugBreak(); \
            }


#else // !DBG

    #define DebugMsg    NOP_FUNCTION

    #define VERBOSE     NOP_FUNCTION
    #define TERSE       NOP_FUNCTION
    #define WARNING     NOP_FUNCTION
    #define ERR         NOP_FUNCTION

    #define ASSERT(cond)

    #define ASSERTMSG(cond, msg)
    #define RIP(msg)
    #define DBGMSG(level, prefix, msg)
    #define DBGPRINT(level, msg)

#endif




/////////////////////////////////////////////////////////
//		ProtoTypes
/////////////////////////////////////////////////////////

BOOL DebugMessage(LPCSTR, ...);
BOOL DebugMessage(LPCWSTR, ...);
void Dump(PPUBLISHERINFO pPublisherInfo);
void Dump(POEMDMPARAM pOemDMParam);
#ifndef KERNEL_MODE
void Dump(PPROPSHEETUI_INFO pPSUIInfo);
#endif

#endif



