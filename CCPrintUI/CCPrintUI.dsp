# Microsoft Developer Studio Project File - Name="CCPrintUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CCPrintUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CCPrintUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CCPrintUI.mak" CFG="CCPrintUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CCPrintUI - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CCPrintUI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Development/CCPrinter/CCPrintUI", FRGCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CCPrintUI - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CCPRINTUI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\..\WINDDK\2600.1106\inc\w2k" /I "..\General" /I ".\\" /I "..\Common" /I "..\libpng" /I "..\zlib" /D "STRICT" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "USERMODE_DRIVER" /YX"precomp.h" /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 libpng.lib zlib.lib comctl32.lib user32.lib winspool.lib ole32.lib comdlg32.lib wininet.lib gdi32.lib advapi32.lib /nologo /base:"0x50000000" /entry:"_DllMainCRTStartup@12" /dll /machine:I386 /out:"../Install/DriverFiles/CCPrintUI.dll" /libpath:"..\zlib\projects\visualc6\Win32_LIB_Release" /libpath:"..\libpng\projects\visualc6\Win32_LIB_Release"

!ELSEIF  "$(CFG)" == "CCPrintUI - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CCPRINTUI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\..\..\WINDDK\2600.1106\inc\w2k" /I "..\General" /I ".\\" /I "..\Common" /I "..\libpng" /I "..\zlib" /D "STRICT" /D "_DEBUG" /D "DEBUG" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "USERMODE_DRIVER" /D "VERBOSE_MSG" /YX"precomp.h" /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libpngd.lib zlibd.lib comctl32.lib user32.lib winspool.lib ole32.lib comdlg32.lib wininet.lib gdi32.lib advapi32.lib /nologo /base:"0x50000000" /entry:"_DllMainCRTStartup@12" /dll /debug /machine:I386 /out:"C:\WINNT\system32\spool\drivers\w32x86\3\CCPrintUI.dll" /pdbtype:sept /libpath:"..\zlib\projects\visualc6\Win32_LIB_Debug" /libpath:"..\libpng\projects\visualc6\Win32_LIB_Debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Desc=Stopping Print Spool
PreLink_Cmds=net.exe stop spooler	if exist C:\WINNT\system32\spool\drivers\w32x86\3\CCPrintUI.dll.old del C:\WINNT\system32\spool\drivers\w32x86\3\CCPrintUI.dll.old	if exist C:\WINNT\system32\spool\drivers\w32x86\3\CCPrintUI.dll move C:\WINNT\system32\spool\drivers\w32x86\3\CCPrintUI.dll C:\WINNT\system32\spool\drivers\w32x86\3\CCPrintUI.dll.old
PostBuild_Desc=Starting Print Spool
PostBuild_Cmds=net.exe start spooler
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CCPrintUI - Win32 Release"
# Name "CCPrintUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h;def"
# Begin Group "Pages"

# PROP Default_Filter ""
# Begin Group "Print UI Pages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCLicensePropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicensePropPage.h
# End Source File
# Begin Source File

SOURCE=.\CCPDFPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCPDFPropPage.h
# End Source File
# Begin Source File

SOURCE=.\CCPrintUIPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCPrintUIPropPage.h
# End Source File
# End Group
# Begin Group "License Wizard"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCLicenseWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizard.h
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizBasePage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizBasePage.h
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizJurisdictionPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizJurisdictionPage.h
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizModificationPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizModificationPage.h
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizSamplingPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizSamplingPage.h
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizSummeryPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizSummeryPage.h
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizTypePage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizTypePage.h
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizUsagePage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicenseWizUsagePage.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CCPrintPropPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CCPrintPropPage.h
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CCLicenseLocationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicenseLocationDlg.h
# End Source File
# Begin Source File

SOURCE=.\CCPrintDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CCPrintDlg.h
# End Source File
# Begin Source File

SOURCE=.\CCPrintProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CCPrintProgressDlg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CCLicenseLocationWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\CCLicenseLocationWnd.h
# End Source File
# Begin Source File

SOURCE=.\CCPrintUI.def
# End Source File
# Begin Source File

SOURCE=.\CCPrintUI.h
# End Source File
# Begin Source File

SOURCE=.\dllentry.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\intrface.cpp
# End Source File
# Begin Source File

SOURCE=.\intrface.h
# End Source File
# Begin Source File

SOURCE=.\LicenseInfoLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\LicenseInfoLoader.h
# End Source File
# Begin Source File

SOURCE=.\oemui.cpp
# End Source File
# Begin Source File

SOURCE=.\oemui.h
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
# Begin Group "License cache"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\General\res\CC-by-nc-nd.xml"
# End Source File
# Begin Source File

SOURCE="..\General\res\CC-by-nc-sa.xml"
# End Source File
# Begin Source File

SOURCE="..\General\res\CC-by-nc.xml"
# End Source File
# Begin Source File

SOURCE="..\General\res\CC-by-nd.xml"
# End Source File
# Begin Source File

SOURCE="..\General\res\CC-by-sa.xml"
# End Source File
# Begin Source File

SOURCE="..\General\res\CC-by.xml"
# End Source File
# Begin Source File

SOURCE=..\General\res\recombo.xml
# End Source File
# Begin Source File

SOURCE=..\General\res\Sampling.xml
# End Source File
# Begin Source File

SOURCE="..\General\res\SamplingPlus-nc.xml"
# End Source File
# Begin Source File

SOURCE=..\General\res\SamplingPlus.xml
# End Source File
# Begin Source File

SOURCE=..\General\res\standard.xml
# End Source File
# End Group
# Begin Group "License Icons"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\General\res\CC.ico
# End Source File
# Begin Source File

SOURCE=..\General\res\Derivative.ico
# End Source File
# Begin Source File

SOURCE=..\General\res\DevNations.ico
# End Source File
# Begin Source File

SOURCE="..\General\res\non-commercial.ico"
# End Source File
# Begin Source File

SOURCE=..\General\res\PublicDomain.ico
# End Source File
# Begin Source File

SOURCE=..\General\res\Sampling.ico
# End Source File
# Begin Source File

SOURCE=..\General\res\SamplingPlus.ico
# End Source File
# Begin Source File

SOURCE=..\General\res\share_alike.ico
# End Source File
# End Group
# Begin Group "License Images"

# PROP Default_Filter ""
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
# Begin Source File

SOURCE=..\General\res\attribution.ico
# End Source File
# Begin Source File

SOURCE=..\General\res\cc.bmp
# End Source File
# Begin Source File

SOURCE=.\CCPrintUI.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
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

SOURCE=..\Common\Helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Helpers.h
# End Source File
# Begin Source File

SOURCE=..\Common\LicenseInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\LicenseInfo.h
# End Source File
# Begin Source File

SOURCE=..\Common\XMLite.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\XMLite.h
# End Source File
# End Group
# Begin Group "General Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\General\InternetRequest.cpp
# End Source File
# Begin Source File

SOURCE=..\General\InternetRequest.h
# End Source File
# Begin Source File

SOURCE=..\General\PngImage.cpp
# End Source File
# Begin Source File

SOURCE=..\General\PngImage.h
# End Source File
# Begin Source File

SOURCE=..\General\ProgressInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\General\ProgressInterface.h
# End Source File
# End Group
# Begin Group "Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\libpng\png.h
# End Source File
# Begin Source File

SOURCE=..\libpng\pngconf.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib.h
# End Source File
# End Group
# Begin Group "DDK Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\WINDDK\2600.1106\inc\w2k\compstui.h
# End Source File
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
# Begin Source File

SOURCE=..\..\..\..\WINDDK\2600.1106\inc\w2k\winddiui.h
# End Source File
# End Group
# End Target
# End Project
