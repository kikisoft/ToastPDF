# Microsoft Developer Studio Project File - Name="CCPSRendering" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CCPSRendering - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CCPSRendering.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CCPSRendering.mak" CFG="CCPSRendering - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CCPSRendering - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CCPSRendering - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Development/CCPrinter/CCPSRendering", AVGCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CCPSRendering - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CCPSRENDERING_EXPORTS" /YX /FD /c
# ADD CPP /nologo /Gz /MT /W3 /GX /O2 /I ".\\" /I "D:\WINDDK\2600.1106\inc\w2k" /I "..\Common" /I "..\General" /I "..\libpng" /I "..\zlib" /I "..\DB" /D "STRICT" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "OEMCOM" /D "USERMODE_DRIVER" /D "WINNT" /D "KERNEL_MODE" /Yu"precomp.h" /FD /c
# SUBTRACT CPP /WX /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 umpdddi.lib kernel32.lib user32.lib ole32.lib uuid.lib gdi32.lib winspool.lib libpng.lib zlib.lib /nologo /base:"0x70000000" /entry:"_DllMainCRTStartup@12" /dll /machine:I386 /out:"../Install/DriverFiles/CCPSRendering.dll" /libpath:"..\zlib\projects\visualc6\Win32_LIB_Release" /libpath:"..\libpng\projects\visualc6\Win32_LIB_Release"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "CCPSRendering - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CCPSRENDERING_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /GX /Zi /Od /Gy /I ".\\" /I "D:\WINDDK\2600.1106\inc\w2k" /I "..\Common" /I "..\General" /I "..\libpng" /I "..\zlib" /I "..\DB" /D "STRICT" /D "_DEBUG" /D "DEBUG" /D "_UNICODE" /D "UNICODE" /D "OEMCOM" /D "USERMODE_DRIVER" /D "WINNT" /D "KERNEL_MODE" /D "VERBOSE_MSG" /Yu"precomp.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 umpdddi.lib kernel32.lib user32.lib ole32.lib uuid.lib gdi32.lib winspool.lib libpngd.lib zlibd.lib /nologo /base:"0x70000000" /entry:"_DllMainCRTStartup@12" /dll /debug /machine:I386 /out:"C:\WINNT\system32\spool\drivers\w32x86\3\CCPSRendering.dll" /pdbtype:sept /libpath:"..\..\..\..\WINDDK\2600.1106\lib\w2k\i386" /libpath:"..\zlib\projects\visualc6\Win32_LIB_Debug" /libpath:"..\libpng\projects\visualc6\Win32_LIB_Debug"
# SUBTRACT LINK32 /incremental:no /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Stopping Print Spool
PreLink_Cmds=net.exe stop spooler	if exist C:\WINNT\system32\spool\drivers\w32x86\3\CCPSRendering.dll.old del C:\WINNT\system32\spool\drivers\w32x86\3\CCPSRendering.dll.old	if exist C:\WINNT\system32\spool\drivers\w32x86\3\CCPSRendering.dll move C:\WINNT\system32\spool\drivers\w32x86\3\CCPSRendering.dll C:\WINNT\system32\spool\drivers\w32x86\3\CCPSRendering.dll.old
PostBuild_Desc=Starting Print Spool
PostBuild_Cmds=net.exe start spooler
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CCPSRendering - Win32 Release"
# Name "CCPSRendering - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Source File

SOURCE=.\CCPSRendering.def
# End Source File
# Begin Source File

SOURCE=.\CCPSRendering.h
# End Source File
# Begin Source File

SOURCE=.\ddihook.cpp
# End Source File
# Begin Source File

SOURCE=.\dllentry.cpp
# End Source File
# Begin Source File

SOURCE=.\enable.cpp
# End Source File
# Begin Source File

SOURCE=.\intrface.cpp
# End Source File
# Begin Source File

SOURCE=.\intrface.h
# End Source File
# Begin Source File

SOURCE=.\oemps.h
# End Source File
# Begin Source File

SOURCE=.\precomp.cpp
# ADD CPP /Yc"precomp.h"
# End Source File
# Begin Source File

SOURCE=.\precomp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;rc"
# Begin Source File

SOURCE=..\General\res\By.png
# End Source File
# Begin Source File

SOURCE=..\General\res\ByNc.png
# End Source File
# Begin Source File

SOURCE=..\General\res\ByNcNd.png
# End Source File
# Begin Source File

SOURCE=..\General\res\ByNcSa.png
# End Source File
# Begin Source File

SOURCE=..\General\res\ByNd.png
# End Source File
# Begin Source File

SOURCE=..\General\res\BySa.png
# End Source File
# Begin Source File

SOURCE=.\CCPSRendering.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\General\res\Sampling.png
# End Source File
# Begin Source File

SOURCE=..\General\res\SamplingPlus.png
# End Source File
# Begin Source File

SOURCE=..\General\res\SamplingPlusNC.png
# End Source File
# Begin Source File

SOURCE=..\General\res\Somerights.png
# End Source File
# End Group
# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Common\CCCommon.h
# End Source File
# Begin Source File

SOURCE=..\Common\CCPrintLicenseInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\CCPrintLicenseInfo.h
# End Source File
# Begin Source File

SOURCE=..\Common\CCPrintRegistry.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\CCPrintRegistry.h
# End Source File
# Begin Source File

SOURCE=..\Common\CCTChar.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\CCTChar.h
# End Source File
# Begin Source File

SOURCE=..\Common\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\debug.h
# End Source File
# Begin Source File

SOURCE=..\Common\devmode.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\devmode.h
# End Source File
# Begin Source File

SOURCE=..\Common\LicenseInfo.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Common\LicenseInfo.h
# End Source File
# Begin Source File

SOURCE=..\libpng\png.h
# End Source File
# Begin Source File

SOURCE=..\libpng\pngconf.h
# End Source File
# Begin Source File

SOURCE=..\General\PngImage.cpp
# End Source File
# Begin Source File

SOURCE=..\General\PngImage.h
# End Source File
# Begin Source File

SOURCE=..\Common\XMLite.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Common\XMLite.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib.h
# End Source File
# End Group
# Begin Group "DB Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\DB\sqlite3.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\DB\sqlite3.h
# End Source File
# Begin Source File

SOURCE=..\DB\SQLiteDB.cpp
# End Source File
# Begin Source File

SOURCE=..\DB\SQLiteDB.h
# End Source File
# End Group
# Begin Group "DDK Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\WINDDK\2600.1106\inc\w2k\d3dnthal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WINDDK\2600.1106\inc\w2k\ddrawint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WINDDK\2600.1106\inc\w2k\prcomoem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WINDDK\2600.1106\inc\w2k\printoem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WINDDK\2600.1106\inc\w2k\winddi.h
# End Source File
# End Group
# End Target
# End Project
