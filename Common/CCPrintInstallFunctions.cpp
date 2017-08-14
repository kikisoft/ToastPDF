/**
	@file
	@brief 
*/

/*
 * CC PDF Converter: Windows PDF Printer with Creative Commons license support
 * Excel to PDF Converter: Excel PDF printing addin, keeping hyperlinks AND Creative Commons license support
 * Copyright (C) 2007-2010 Guy Hachlili <hguy@cogniview.com>, Cogniview LTD.
 * 
 * This file is part of CC PDF Converter / Excel to PDF Converter
 * 
 * CC PDF Converter and Excel to PDF Converter are free software;
 * you can redistribute them and/or modify them under the terms of the 
 * GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later version.
 * 
 * CC PDF Converter and Excel to PDF Converter are is distributed in the hope 
 * that they will be useful, but WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. * 
 */

#include "precomp.h"
#include "CCPrintInstallFunctions.h"

#include "CCPrintRegistry.h"
#include "Helpers.h"
#include "CCCommon.h"

#include <winspool.h>
#include <tchar.h>
#include <atlchecked.h>

/// List of driver files
static LPCTSTR pPrinterFilelist[] =
{
	_T("PSCRIPT5.DLL"),
	_T("PS5UI.DLL"),
	_T("PSCRIPT.HLP"),
#ifdef CC_PDF_CONVERTER
	_T("TTPDFPrn.PPD"),
	_T("TTPDFConverter.ini"),
	_T("TTPrintUI.DLL"),
	_T("TTPSRendering.DLL"),
#elif EXCEL_TO_PDF
	_T("XLPDFPrn.PPD"),
	_T("XL2PDFConverter.ini"),
	_T("XL2PDFPrintUI.DLL"),
	_T("XL2PDFPSRendering.DLL"),
#else
#error "Please select one of the printer types"
#endif
	_T("PSCRIPT.NTF"),
	_T("PSCRPTFE.NTF"),
	NULL
};

#ifdef CC_PDF_CONVERTER
/// Printer Name
#define PRINTER_NAME		 _T("Toast PDF")
/// Port Name
#define PORT_NAME			 _T("ToastPDFPort")
/// Driver name
#define DRIVER_NAME			 _T("Toast PDF Converter")
/// Hardware ID
#define HARDWARE_ID			 _T("ToastPDF_Driver0101")
/// Print monitor Name
#define MONITOR_NAME	     _T("ToastPDF_Redmon")
/// Print monitor DLL filename
#define MONITOR_DLL			 _T("TTPDF_redmonnt.dll")
/// PDF Convert exe filename
#define CC_PDF_CONVERTER_EXE _T("ToastPDF.exe")


#elif EXCEL_TO_PDF
/// Printer Name
#define PRINTER_NAME	_T("Excel to PDF Converter")
/// Port Name
#define PORT_NAME		_T("XL2PDFPort")
/// Driver Name
#define DRIVER_NAME		_T("Excel to PDF Virtual Printer")
/// Hardware ID
#define HARDWARE_ID		_T("XL2PDFConverter_Driver0101")
/// Print Monitor Name
#define MONITOR_NAME	_T("XL2PDF_Redmon")
/// Print monitor DLL filename
#define MONITOR_DLL		_T("XL2PDF_redmonnt.dll")
#else
#error "Please select one of the printer types"
#endif

//////////////////////////////////////////////////////////////////////////
bool IsX64()
{
	HMODULE hKernel32 = ::LoadLibrary(_T("KERNEL32.DLL"));
	if (hKernel32 == NULL)
		return false;

	VOID (WINAPI* pfnGetNativeSystemInfo)(LPSYSTEM_INFO);

	*(FARPROC*)&pfnGetNativeSystemInfo = ::GetProcAddress(hKernel32, "GetNativeSystemInfo");
	if (pfnGetNativeSystemInfo == NULL)
	{
		::FreeLibrary(hKernel32);
		return false;
	}

	SYSTEM_INFO si;
	(*pfnGetNativeSystemInfo)(&si);

	::FreeLibrary(hKernel32);

	return (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
}

/**
	@param hWnd Handle to the parent window
*/
CCPDFPrinterInstaller::CCPDFPrinterInstaller(HWND hWnd) : PrinterInstall(PRINTER_NAME, PORT_NAME, DRIVER_NAME, HARDWARE_ID, pPrinterFilelist), m_hWnd(hWnd)
{
	m_bX64 = IsX64();
}

/**
	@param lpError Error string
*/
void CCPDFPrinterInstaller::ShowError(LPCTSTR lpError)
{
	::MessageBox(m_hWnd, lpError, _T("Installing printer"), MB_ICONERROR|MB_OK);
}

/**
	@param hWnd Handle to the parent window
	@return true if the CC PDF Printer driver is installed on the system, false if not
*/
bool CCPDFPrinterInstaller::IsPrinterInstalled()
{
	// Create an installer instance
	bool bInstalled;
	// Test for installed driver
	if (!PrinterInstall::IsPrinterInstalled(bInstalled))
	{
		// Error
		ShowError(_T("Error while testing for existing printer"));
		return false;
	}

	// Return the value
	return bInstalled;
}

/**
	@param hModule Handle to the module that should be used to retrieve the driver files path from; NULL to use this module
	@return true if successfully installed, false if failed
*/
bool CCPDFPrinterInstaller::DoInstall(HMODULE hModule /* = NULL */,  LPTSTR installationPath /*= ""*/)
{
	// Retrieve the file path to the module
	TCHAR cPath[MAX_PATH + 2], cError[2048];
	std::tstring sInstallPath;
	
	
	if (_tcscmp(installationPath,""))
	{
		_tcscpy_s (cPath, MAX_PATH, installationPath);
		_tcscat_s (cPath, MAX_PATH, "\\");
	}
	else
	{
		if (!::GetModuleFileName(hModule, cPath, MAX_PATH + 1))
		{
			ShowError("Cannot retrieve path");
			return false;
		}

		// OK, we want to remove the actual module name as we need just the path
		TCHAR* pSlash = _tcsrchr(cPath, '\\');
		if (pSlash == NULL)
		{
			ShowError(_T("Error while retrieving path"));
			return false;
		}
		*(pSlash + 1) = '\0';
	}

	// Remember the path
	sInstallPath = cPath;

	// And add the DriverFiles folder so we can install the files from there
	_tcscat(cPath, m_bX64? _T("tools\\pdfprinter\\x64") : _T("tools\\pdfprinter\\x86"));
	//_tcscat(cPath, _T("DriverFiles"));
	// Start installing:

	// Add the redmon port monitor
	if (!AddPortMonitor(MONITOR_NAME, MONITOR_DLL, cPath))
	{
		_stprintf(cError, _T("Printer installation error %d (%d)"), GetError(), GetLastError());
		//ShowError(cError);
		//return false;
	}

	// Install the actual printer driver
	if (Install(cPath, m_hWnd) != 0)
	{
		// Failed
		_stprintf(cError, _T("Printer installation error %d (%d)"), GetError(), GetLastError());
		ShowError(cError);
		RemovePortMonitor(MONITOR_NAME);
		return false;
	}

	// Set up the redmon settings:
	// Open the key
	HKEY hKey;
	if (::RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\") MONITOR_NAME _T("\\Ports\\" PORT_NAME), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) != ERROR_SUCCESS)
	{
		// Can't... fail
		ShowError(_T("Could not access the port registry settings"));
		Uninstall(m_hWnd);
		RemovePortMonitor(MONITOR_NAME);
		return false;
	}
	// We want to run the CCPDFConverter to eat the PostScript, so set it up:
#ifdef CC_PDF_CONVERTER
	std::tstring sAppPath = ::ConcatPaths(sInstallPath, CC_PDF_CONVERTER_EXE);
#elif EXCEL_TO_PDF
	std::tstring sAppPath = ::ConcatPaths(sInstallPath, _T("XL2PDFConverter.exe"));
#else
#error "Please select one of the printer types"
#endif
	if (::RegSetValueEx(hKey, _T("Command"), 0, REG_SZ, (const BYTE*)sAppPath.c_str(), (sAppPath.size() + 1) * sizeof(TCHAR)) != ERROR_SUCCESS)
	{
		// Can't
		ShowError(_T("Could not write the port registry settings (1)"));
		::RegCloseKey(hKey);
		Uninstall(m_hWnd);
		RemovePortMonitor(MONITOR_NAME);
		return false;
	}
	
	// Set the delay value
	DWORD dwValue = 300;
	if (::RegSetValueEx(hKey, _T("Delay"), 0, REG_DWORD, (const BYTE*)&dwValue, sizeof(dwValue)) != ERROR_SUCCESS)
	{
		ShowError(_T("Could not write the port registry settings (2)"));
		::RegCloseKey(hKey);
		Uninstall(m_hWnd);
		RemovePortMonitor(MONITOR_NAME);
		return false;
	}
	// We want to run as a user, not as the printer; also see the changes in the redmon source code
	dwValue = 1;
	if (::RegSetValueEx(hKey, _T("RunUser"), 0, REG_DWORD, (const BYTE*)&dwValue, sizeof(dwValue)) != ERROR_SUCCESS)
	{
		ShowError(_T("Could not write the port registry settings (3)"));
		::RegCloseKey(hKey);
		Uninstall(m_hWnd);
		RemovePortMonitor(MONITOR_NAME);
		return false;
	}
	// No need to display a window:
	dwValue = 0;
	if (::RegSetValueEx(hKey, _T("ShowWindow"), 0, REG_DWORD, (const BYTE*)&dwValue, sizeof(dwValue)) != ERROR_SUCCESS)
	{
		ShowError(_T("Could not write the port registry settings (4)"));
		::RegCloseKey(hKey);
		Uninstall(m_hWnd);
		RemovePortMonitor(MONITOR_NAME);
		return false;
	}

	// Set the arguments
	std::tstring sArguments = _T("-a doc2pdf");
	if (::RegSetValueEx(hKey, _T("Arguments"), 0, REG_SZ, (const BYTE*)sArguments.c_str(), (sArguments.size() + 1) * sizeof(TCHAR)) != ERROR_SUCCESS)
	{
		ShowError(_T("Could not write the port registry settings (5)"));
		::RegCloseKey(hKey);
		Uninstall(m_hWnd);
		RemovePortMonitor(MONITOR_NAME);
		return false;
	}

	::RegCloseKey(hKey);	

	// Also, set up the DB and images paths:
	// Get the printer handle
	BOOL bRet = FALSE;
	PRINTER_DEFAULTS  pd;
	HANDLE  hPrinter;
	ZeroMemory(&pd, sizeof(PRINTER_DEFAULTS)); 
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	if (!::OpenPrinter(PRINTER_NAME, &hPrinter, &pd))
	{
		ShowError(_T("Could not open the printer for setting paths (1)"));
		return false;
	}
	
	// And set the values
	CCPrintRegistry::SetRegistryString(hPrinter, _T("DB Path"), ::ConcatPaths(sInstallPath, _T("cc2.db3")).c_str());
	CCPrintRegistry::SetRegistryString(hPrinter, _T("Image Path"), ::ConcatPaths(sInstallPath, _T("Images\\")).c_str());
	CCPrintRegistry::SetRegistryBool(hPrinter, (LPTSTR)SETTINGS_AUTOOPEN, CanOpenPDFFiles());
	::ClosePrinter(hPrinter);

	return true;
}

/**
	@return true if uninstalled successfully, false if failed
*/
bool CCPDFPrinterInstaller::DoRemove()
{
	TCHAR cError[2048];

	// Uninstall the printer driver files
	if (Uninstall(m_hWnd) != 0)
	{
		_stprintf(cError, _T("Printer removal error %d (%d)"), GetError(), GetLastError());
		ShowError(cError);
		return false;
	}

	// And remove the redmon port monitor
	if (!RemovePortMonitor(MONITOR_NAME))
	{
		_stprintf(cError, _T("Printer removal error %d (%d)"), GetError(), GetLastError());
		ShowError(cError);
		return false;
	}

	return true;
}
