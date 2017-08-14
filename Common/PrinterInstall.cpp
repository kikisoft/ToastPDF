/**
	@file
	@brief 
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

#include "precomp.h"
#include <winspool.h>
#include <tchar.h>
#include "PrinterInstall.h"

// Hard-coded string used while installing
/// Supported printer driver OS
#define ENVIRONMENT				TEXT("Windows NT x86")
#define ENVIRONMENT_x64			TEXT("Windows x64")

/// Printer driver data type
#define DRIVER_DATA_TYPE		_T("")
/// Printed material data type
#define PRINTER_DATA_TYPE		_T("RAW")
/// Driver manufacturer
#define DRIVER_MANUFACTURER		_T("KIKI")

/// Default system port monitor
#define DEFAULT_PORT_MONITOR	_T("Local Port")

// Command line flags
/// Install printer flag
#define PARAM_INSTALLPRINTER	_T("/InstallPrinter")
/// Remove printer flag
#define PARAM_REMOVEPRINTER		_T("/UninstallPrinter")
/// No error messages flag
#define PARAM_NOUI				_T("/NoUI")

/**
	@param sFolder Folder to combine
	@param sPathname Path to combine (probably relative to sFolder)
	@param bAllowAbsoluteOverride true to allow sPathname to have an absolute path, false to make sure it's relative
	@return The combined path
*/
std::tstring ConcatPaths(const std::tstring& sFolder, const std::tstring& sPathname, bool bAllowAbsoluteOverride /* = true */)
{
	// Do we have a folder?
	if (sFolder.empty())
		// No, return the second part alone
		return sPathname;
	// Do we have a path?
	if (sPathname.empty())
		// No, return the first part alone
		return sFolder;

	// Allow overriding paths by using a drive letter and colon (c:) or a UNC path (\\)
	if (bAllowAbsoluteOverride)
		if ((sPathname.size() > 2) && (sPathname[1] == ':') || ((sPathname[0] == '\\') && (sPathname[1] == '\\')))
			return sPathname;

	std::tstring sRet = sFolder;

	// Make sure we have a slash at the end of the folder part
	CHAR c = sFolder[sFolder.size()-1];
	if ((c != '\\') && (c != '/'))
		sRet += _T("\\");

	// Make sure we don't have a slash at the beginning of the path part:
	c = sPathname[0];
	if ((c == '\\') || (c == '/'))
		// We do, jump over it
		sRet += sPathname.substr(1);
	else
		// Just add them
		sRet += sPathname;

	return sRet;
}

/**
	@param lpPrinterName Name of printer to install
	@param lpPortName Name of port to create
	@param lpDriverName Name of printer driver
	@param lpHardwareID Unique hardware ID for the printer
	@param pFiles Pointer to array of files to install
*/
PrinterInstall::PrinterInstall(LPCTSTR lpPrinterName, LPCTSTR lpPortName, LPCTSTR lpDriverName, LPCTSTR lpHardwareID, const LPCTSTR* pFiles) : m_sPrinterName(lpPrinterName), m_sPortName(lpPortName), m_sDriverName(lpDriverName), m_sHardwareID(lpHardwareID), m_nError(0), m_nLastError(0), m_bLockError(false), m_sPortMonitor(DEFAULT_PORT_MONITOR)
{
	// Add the files to the files array
	while (*pFiles != NULL)
	{
		m_arFiles.push_back(*pFiles);
		pFiles++;
	}
}

/**
	
*/
PrinterInstall::PrinterInstall() : m_nError(0), m_nLastError(0), m_bLockError(false), m_sPortMonitor(DEFAULT_PORT_MONITOR)
{
}

/**
	@param lpPrinterName Name of printer to install
	@param lpPortName Name of port to create
	@param lpDriverName Name of printer driver
	@param lpHardwareID Unique hardware ID for the printer
	@param pFiles Pointer to array of files to install
*/
void PrinterInstall::InitializePrinterData(LPCTSTR lpPrinterName, LPCTSTR lpPortName, LPCTSTR lpDriverName, LPCTSTR lpHardwareID, const LPCTSTR* pFiles)
{
	// Initialize members
	m_sPrinterName = lpPrinterName;
	m_sPortName = lpPortName;
	m_sDriverName = lpDriverName;
	m_sHardwareID = lpHardwareID;

	// Load the file names into the files array
	m_arFiles.clear();
	while (*pFiles != NULL)
	{
		m_arFiles.push_back(*pFiles);
		pFiles++;
	}
}

/**
	@param nError The error to set
*/
void PrinterInstall::SetError(int nError)
{
	// Do we keep the current error?
	if (m_bLockError)
		// Yeah, leave it
		return;

	// Save it
	m_nError = nError;
	m_nLastError = GetLastError();
	// Don't overwrite errors
	m_bLockError = true;
}

/**
	@return true if the port was added succesfully, false if failed
*/
bool PrinterInstall::DoAddPort()
{
	// Initialize the port creation data structure
	PRINTER_DEFAULTS  pd;
	ZeroMemory(&pd, sizeof(PRINTER_DEFAULTS)); 
	pd.DesiredAccess = SERVER_ACCESS_ADMINISTER;
	HANDLE hXcv = INVALID_HANDLE_VALUE;

	// Open the base port monitor
	std::tstring s = _T(",XcvMonitor ") + m_sPortMonitor;
	if (!::OpenPrinter((TCHAR*)s.c_str(), &hXcv, &pd))
	{
		SetError(-1001);
#ifdef TRACE
		TRACE(_T("Open printer failed: %d\n"), GetLastError());
#endif
		return false;
	}

	// Issue the port adding command
	DWORD cbneed,cbstate;
	TCHAR cOutputData[MAX_PATH];
	if (!::XcvData(hXcv, L"AddPort", (PBYTE)MakeWideString(m_sPortName).c_str(), (m_sPortName.size() + 1) * sizeof(WCHAR), (LPBYTE)cOutputData, MAX_PATH, &cbneed, &cbstate))
	{
		SetError(-1002);
#ifdef TRACE
		TRACE(_T("XcdData failed: %d\n"), cbstate);
#endif
		::ClosePrinter(hXcv);
		return false;
	}

    ::ClosePrinter(hXcv);
	return true;
}

/**
	@param hWnd Handle to the parent window
	@return true if the port was deleted successfully, false if failed
*/
bool PrinterInstall::DoDeletePort(HWND hWnd)
{
	// Delete the port
	if (!::DeletePortW(NULL, hWnd, (WCHAR*)MakeWideString(m_sPortName).c_str()))
	{
		if (GetLastError() != 0)
		{
#ifdef TRACE
			TRACE(_T("Delete port failed: %d\n"), GetLastError());
#endif
			// Must be another printer on this port... But we'll leave the port be
		}
	}
	return true;
}

/**
	@param lpCopyFrom Folder to copy the printer driver files from
	@return true if copied successfully, false if failed
*/
bool PrinterInstall::DoCopyFiles(LPCTSTR lpCopyFrom)
{
	TCHAR lpPath[MAX_PATH + 1];
	DWORD uSize=0;

	// Get the Windows printer driver folder
	if (!GetPrinterDriverDirectory(NULL, NULL, 1, (LPBYTE)lpPath, MAX_PATH + 1, &uSize))
	{
		// Failed?!
		SetError(-1201);
#ifdef TRACE
		TRACE(_T("Could not get printer driver directory: %d\n"), GetLastError());
#endif
		return false;
	}

	// Copy the files from the lpCopyFrom dir to the destination dir:
	std::tstring sFrom, sTo;
	for (STRARRAY::const_iterator i = m_arFiles.begin(); i != m_arFiles.end(); i++)
	{
		// Create the full path name for the files
		sFrom = ConcatPaths(lpCopyFrom, (*i));
		sTo = ConcatPaths(lpPath, (*i));

		// Now copy
		if (!::CopyFile(sFrom.c_str(), sTo.c_str(), FALSE))
		{
			SetError(-1202);
#ifdef TRACE
			TRACE(_T("Cannot copy file: %d\n"), GetLastError());
#endif
			return false;
		}
	}

	return true;
}

/**
	@return true if the printer driver files were removed successfully, false if failed
*/
bool PrinterInstall::DoRemoveFiles()
{
	TCHAR lpPath[MAX_PATH + 1];
	DWORD uSize=0;

	// Get the Windows printer driver folder
	if (!::GetPrinterDriverDirectory(NULL, NULL, 1, (LPBYTE)lpPath, MAX_PATH + 1, &uSize))
	{
		// Failed!
		SetError(-1301);
#ifdef TRACE
		TRACE(_T("Cannot get printer driver directory: %d\n"), GetLastError());
#endif
		return false;
	}

	// Remove all the files:
	std::tstring sFullPath;
	for (STRARRAY::const_iterator i = m_arFiles.begin(); i != m_arFiles.end(); i++)
	{
		// Create the full path to the file
		sFullPath = ConcatPaths(lpPath, (*i));
		// Delete it
		
		if (!::DeleteFile(sFullPath.c_str()))
		{
#ifdef TRACE
			TRACE(_T("Warning: cannot delete file %s: %d\n"), (*i).c_str(), GetLastError());
#endif
		}
	}
	return true;
}

/**
	@return true if the printer driver is installed successfully, false if failed
*/
bool PrinterInstall::DoInstallPrinterDriver()
{
	// Initialize data structure
	DRIVER_INFO_6 info;
	memset(&info, 0, sizeof(info));

	TCHAR lpPath[MAX_PATH];
	DWORD uSize=0; 

	// Get the Windows printer driver folder
	if (!::GetPrinterDriverDirectory(NULL, NULL, 1, (LPBYTE)lpPath, MAX_PATH, &uSize))
	{
		// Can't...
		SetError(-1401);
#ifdef TRACE
		TRACE(_T("Could not get printer driver directory: %d\n"), GetLastError());
#endif
		return false;
	}

	
	// Add a slash
	int nLen = _tcslen(lpPath);
	if ((nLen > 0) || (lpPath[nLen - 1] != '\\'))
		_tcscat(lpPath, _T("\\"));

	TCHAR cDriverPath[MAX_PATH], cConfigPath[MAX_PATH], cHelpPath[MAX_PATH], cDataPath[MAX_PATH];
	// Create the file names of the main driver file...
	_tcscpy(cDriverPath, lpPath);
	_tcscat(cDriverPath, m_arFiles[DRIVER_FILE_NUM].c_str());
	// ... the printer UI file ...
	_tcscpy(cConfigPath, lpPath);
	_tcscat(cConfigPath, m_arFiles[UI_FILE_NUM].c_str());
	// ... the printer help file ...
	_tcscpy(cHelpPath, lpPath);
	_tcscat(cHelpPath, m_arFiles[HELP_FILE_NUM].c_str());
	// ... and the printer configuration file
	_tcscpy(cDataPath, lpPath);
	_tcscat(cDataPath, m_arFiles[DATA_FILE_NUM].c_str());

	// Add all the other files into a buffer
	int nSize = 0;
	for (STRARRAY::const_iterator i = m_arFiles.begin(); i != m_arFiles.end(); i++)
		nSize += ((*i).size() + 1);
	nSize++;
	TCHAR* pBuffer = new TCHAR[nSize];
	nSize = 0;
	for (STRARRAY::const_iterator i = m_arFiles.begin(); i != m_arFiles.end(); i++)
	{

		
		memcpy(pBuffer + nSize, (*i).c_str(), (*i).size() * sizeof(TCHAR));
		nSize += (*i).size();
		pBuffer[nSize] = '\0';
		nSize++;
	}
	pBuffer[nSize] = '\0';

	// Set the data structure members
	info.cVersion = 3;
	info.pName = (TCHAR*)m_sDriverName.c_str();
	info.pEnvironment = m_bX64 ? ENVIRONMENT_x64 : ENVIRONMENT;
	info.pDriverPath = cDriverPath;
	info.pDataFile = cDataPath;
	info.pConfigFile = cConfigPath;
	info.pHelpFile = cHelpPath;
	info.pDependentFiles = pBuffer;
	info.pMonitorName = NULL;
	info.pDefaultDataType = DRIVER_DATA_TYPE;
	info.dwlDriverVersion = 0x0000000200000000I64;
	info.pszMfgName = DRIVER_MANUFACTURER;
	info.pszHardwareID = (TCHAR*)m_sHardwareID.c_str();
	info.pszProvider = DRIVER_MANUFACTURER;

	// Now actually install the driver:
	if (!::AddPrinterDriver(NULL, 6, (LPBYTE)&info))
	{
		// Bah!
		delete [] pBuffer;
		SetError(-1402);
#ifdef TRACE
		TRACE(_T("AddPrinterDriver failed: %d\n"), GetLastError());
#endif
		return false;
	}
	delete [] pBuffer;

	// OK!
	return true;
}

/**
	@return true if the printer driver was uninstalled successfully, false if failed
*/
bool PrinterInstall::DoUninstallPrinterDriver()
{
	// Try to uninstall
	if (!::DeletePrinterDriverEx(NULL, ENVIRONMENT, (TCHAR*)m_sDriverName.c_str(), DPD_DELETE_UNUSED_FILES, 3)) 
	{
		// Did we actually fail?
		DWORD dw = GetLastError();
		if (dw != 0)
		{
			// Yeah
			SetError(-1501);
#ifdef TRACE
			TRACE(_T("DeletePrinterDriver failed: %d\n"), dw);
#endif
			return false;
		}
	} 
	return true;
}

/**
	@return true if the printer was added successfully, false if failed
*/
bool PrinterInstall::DoAddPrinter()
{
	// Initialize the data structure
	PRINTER_INFO_2 pi2;
	ZeroMemory(&pi2, sizeof(PRINTER_INFO_2)); 

	// Set the variables
	pi2.pServerName= NULL; 
	pi2.pPrinterName = (TCHAR*)m_sPrinterName.c_str(); 
	pi2.pPortName = (TCHAR*)m_sPortName.c_str(); 
	pi2.pDriverName = (TCHAR*)m_sDriverName.c_str();
	pi2.pPrintProcessor = _T("WinPrint");
	pi2.pDatatype = PRINTER_DATA_TYPE;
	pi2.Attributes = PRINTER_ATTRIBUTE_DIRECT;

	// Add the printer
	HANDLE hPrinter = ::AddPrinter(NULL, 2, (LPBYTE)&pi2);
	if (hPrinter == NULL)
	{
		// Failed!
		SetError(-1601);
#ifdef TRACE
		TRACE(_T("AddPrinter failed: %d\n"), GetLastError());
#endif
		return false;
	}
	::ClosePrinter(hPrinter);

	return true;
}

/**
	@param lpName Name of printer to delete
	@return true if the printer was deleted successfully, false if failed
*/
bool PrinterInstall::DoRemovePrinter(LPTSTR lpName)
{
	BOOL bRet = FALSE;
	PRINTER_DEFAULTS  pd;
	HANDLE  hPrinter;

	ZeroMemory(&pd, sizeof(PRINTER_DEFAULTS)); 
	pd.DesiredAccess = PRINTER_ALL_ACCESS;

	// Retrieve the printer handle
	if (!::OpenPrinter(lpName, &hPrinter, &pd))
	{
		// Can't - maybe it ain't there...
		SetError(-1701);
#ifdef TRACE
		TRACE(_T("OpenPrinter failed while removing printer %s: %d"), lpName, GetLastError());
#endif
		return false;
	}

	// Delete it
	if (!::DeletePrinter(hPrinter))
	{
		// Failed!
		SetError(-1702);
#ifdef TRACE
		TRACE(_T("DeletePrinter for printer %s failed: %d"), lpName, GetLastError());
#endif
		return false;
	}
	::ClosePrinter(hPrinter);

	return true;
}

/**
	@param bInstalled Reference to the installed flag, filled upon return
	@return true if tested successfully, false if failed to test
*/
bool PrinterInstall::IsPrinterInstalled(bool& bInstalled)
{
	PRINTER_INFO_2* pInfo;
	DWORD dwSize, dwCount;
	bInstalled = false;

	// Retrieve the number of available printers
	::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, NULL, 0, &dwSize, &dwCount);
	if (dwSize == 0)
	{
		// No printers to find
		return true;
	}

	// OK, we want the printer data
	pInfo = (PRINTER_INFO_2*)new char[dwSize];
	if (!::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, (LPBYTE)pInfo, dwSize, &dwSize, &dwCount))
	{
		// Cannot...
		SetError(-1801);
		delete [] pInfo;
		return false;
	}

	// Now, compare our driver name to each printer
	for (int i=0;!bInstalled && (i<(int)dwCount);i++)
	{
		if (_tcscmp(pInfo[i].pDriverName, m_sDriverName.c_str()) == 0)
			// Aha! Found one!
			bInstalled = true;
	}

	// OK
	delete [] pInfo;
	return true;
}

/**
	@return true if all the printers connected to this printer driver were removed, false if failed
*/
bool PrinterInstall::DoRemovePrinters()
{
	PRINTER_INFO_2* pInfo;
	DWORD dwSize, dwCount;

	// Find the number of installed printers
	::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, NULL, 0, &dwSize, &dwCount);
	if (dwSize == 0)
	{
		// No printers to remove
		return true;
	}

	// Retreive the printers data
	pInfo = (PRINTER_INFO_2*)new char[dwSize];
	if (!::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, (LPBYTE)pInfo, dwSize, &dwSize, &dwCount))
	{
		// Failed
		SetError(-1801);
		delete [] pInfo;
		return false;
	}

	// Go over printers
	for (int i=0;i<(int)dwCount;i++)
	{
		// Does this one uses our driver?
		if (_tcscmp(pInfo[i].pDriverName, m_sDriverName.c_str()) == 0)
		{
			// Yeah, remove it
			if (!DoRemovePrinter(pInfo[i].pPrinterName))
			{
				// Damn! Failed!
				delete [] pInfo;
				return false;
			}
		}
	}

	delete [] pInfo;
	return true;
}

/**
	@param lpFilesPath Path of printer driver files
	@param hParent Handle to parent window
	@return 0 if succeeded, any other number for error
*/
int PrinterInstall::Install(LPCTSTR lpFilesPath, HWND hParent)
{
	// Reset the errors
	ResetError();
	// Do we have a printer name?
	if (m_sPrinterName.empty())
	{
		// No, can't continue
		SetError(-2);
		return m_nError;
	}

	// Do we have a folder to read files from?
	if ((lpFilesPath == NULL) || (_tcslen(lpFilesPath) == 0))
	{
		// No, can't continue
		SetError(-1);
		return m_nError;
	}

	// Add the port
	if (!DoAddPort())
		return m_nError;

	// Copy the printer driver files
	if (!DoCopyFiles(lpFilesPath))
	{
		DoRemoveFiles();
		DoDeletePort(hParent);
		return m_nError;
	}

	// Install the printer driver
	if (!DoInstallPrinterDriver())
	{
		DoRemoveFiles();
		DoDeletePort(hParent);
		return m_nError;
	}

	// Finally, add a printer
	if (!DoAddPrinter())
	{
		DoUninstallPrinterDriver();
		DoDeletePort(hParent);
		return m_nError;
	}

	return 0;
}

/**
	@param hParent Handle to the parent window
	@return 0 if succeeded, any other number for error
*/
int PrinterInstall::Uninstall(HWND hParent)
{
	// Reset the previous error, if any
	ResetError();

	// Do we have a printer name?
	if (m_sPrinterName.empty())
	{
		// No, can't continue
		SetError(-2);
		return m_nError;
	}

	// Try to remove all printers that use this driver
	if (!DoRemovePrinters())
	{
		// Can't, just try the rest of the stuff and fail
		DoUninstallPrinterDriver();
		DoDeletePort(hParent);
		return m_nError;
	}

	// Remove the printer driver itself
	if (!DoUninstallPrinterDriver())
	{
		// Can't
		int nRet = m_nError;
		DoDeletePort(hParent);
		return m_nError;
	}

	// And finally, the port monitor
	if (!DoDeletePort(hParent))
		return m_nError;

	return 0;
}


/**
	@param nArgs Number of command line arguments
	@param pArgs Pointer to an array of command line arguments
	@param bNoUI Reference to a silent command flag, filled upon exit
	@param lpRelativeDriverFolder Folder name (relative to the current module) where the printer driver files can be found
	@return true if any command lines variables were detected and acted upon

	Command line usage:
	To install the printer:
	<app.exe> [/NoUI] /InstallPrinter
	To uninstall the printer:
	<app.exe> [/NoUI] /UninstallPrinter 
	Note that the object must be initialized for this to work!
*/
bool PrinterInstall::DoCommandLine(int nArgs, TCHAR** pArgs, bool& bNoUI, LPCTSTR lpRelativeDriverFolder)
{
	// Initialize stuff
	bNoUI = false;
	ResetError();

	// Go over variables
	for (int i = 1; i < nArgs; i++)
	{
		if (_tcscmp(pArgs[i], PARAM_NOUI) == 0)
			// NoUI (silent) parameter
			bNoUI = true;
		else if (_tcscmp(pArgs[i], PARAM_INSTALLPRINTER) == 0)
		{
			// Printer installation:
			TCHAR cPath[MAX_PATH + 2];
			// Retrieve the current module path
			if (!::GetModuleFileName(NULL, cPath, MAX_PATH + 1))
				// Can't?!
				SetError(-100);
			else
			{
				// Remove the actual module name (we need the folder)
				TCHAR* pSlash = _tcsrchr(cPath, '\\');
				if (pSlash == NULL)
					SetError(-101);
				else
				{
					// Add the printer driver folder
					*(pSlash + 1) = '\0';
					_tcscat(cPath, lpRelativeDriverFolder);
					// And install
					Install(cPath, NULL);
				}
			}
			// We did something here
			return true;
		}
		else if (_tcscmp(__targv[i], PARAM_REMOVEPRINTER) == 0)
		{
			// Printer removal
			Uninstall(NULL);
			return true;
		}
	}

	// We didn't do nothing
	return false;
}

/**
	@param lpMonitorName Name of port monitor
	@param lpMonitorFile Port monitor DLL filename
	@param lpFilePath Path where the port monitor file currently resides
	@return true if installed successfully, false if failed
*/
bool PrinterInstall::AddPortMonitor(LPCTSTR lpMonitorName, LPCTSTR lpMonitorFile, LPCTSTR lpFilePath)
{
	TCHAR lpPath[MAX_PATH + 1];

	
	// Get the Windows system path (where we will install the port monitor)
	if (!GetSystemDirectory(lpPath, MAX_PATH + 1))
	{
		// Failed
		SetError(-2001);
#ifdef TRACE
		TRACE(_T("Could not get system directory: %d\n"), GetLastError());
#endif
		return false;
	}

	// Copy the file into the system directory
	std::tstring sFrom, sTo;
	sFrom = ConcatPaths(lpFilePath, lpMonitorFile);
	sTo = ConcatPaths(lpPath, lpMonitorFile);
	if (!::CopyFile(sFrom.c_str(), sTo.c_str(), FALSE))
	{
		// Couldn't!
		SetError(-2002);
#ifdef TRACE
		TRACE(_T("Cannot copy file: %d\n"), GetLastError());
#endif
		return false;
	}
	
	// Initialize the data structure
	MONITOR_INFO_2 info;
	info.pName = (TCHAR*)lpMonitorName;
	info.pEnvironment = m_bX64 ? ENVIRONMENT_x64 : ENVIRONMENT;
	info.pDLLName = (TCHAR*)lpMonitorFile;
	
	// Add the new monitor
	if (!::AddMonitor(NULL, 2, (LPBYTE)&info))
	{
		// Failed...
		SetError(-2003);
#ifdef TRACE
		TRACE(_T("Cannot add a port monitor: %d\n"), GetLastError());
#endif
		return false;
	}
	

	// Remember the name!
	m_sPortMonitor = lpMonitorName;
	return true;
}

/**
	@param lpMonitorName Name of port monitor
	@return true if port monitor was deleted successfully, false if failed
*/
bool PrinterInstall::RemovePortMonitor(LPCTSTR lpMonitorName)
{
	// Try to delete
	if (!::DeleteMonitor(NULL, m_bX64 ? ENVIRONMENT_x64 : ENVIRONMENT, (TCHAR*)lpMonitorName))
	{
		// Did we actually fail?
		DWORD dw = GetLastError();
		if (dw != 0)
		{
			// Yep
			SetError(-2101);
#ifdef TRACE
			TRACE(_T("Cannot remove a port monitor: %d\n"), dw);
#endif
			return false;
		}
	}

	return true;
}
