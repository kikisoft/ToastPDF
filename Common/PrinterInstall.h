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

#ifndef _PRINTERINSTALL_H_
#define _PRINTERINSTALL_H_

#include "CCTChar.h"
#include <vector>

/// Array of strings
typedef std::vector<std::tstring> STRARRAY;

/// Number of base printer driver file
#define DRIVER_FILE_NUM			0
/// Number of base printer UI file
#define UI_FILE_NUM				1
/// Number of printer driver help file
#define HELP_FILE_NUM			2
/// Number of printer definitions file (PPD or PCD)
#define DATA_FILE_NUM			3

/// Helper function for combining paths
std::tstring ConcatPaths(const std::tstring& sFolder, const std::tstring& sPathname, bool bAllowAbsoluteOverride = true);

/**
    @brief Base class for installing and removing printer drivers
*/
class PrinterInstall
{
public:
	// Ctors
	/// Constructor with installation data
	PrinterInstall(LPCTSTR lpPrinterName, LPCTSTR lpPortName, LPCTSTR lpDriverName, LPCTSTR lpHardwareID, const LPCTSTR* pFiles);
	/// Default constructor
	PrinterInstall();

protected:
	// Members
	/// Printer name
	std::tstring m_sPrinterName;
	/// Printer port name
	std::tstring m_sPortName;
	/// Printer driver name
	std::tstring m_sDriverName;
	/// Hardware ID (should be unique to the driver)
	std::tstring m_sHardwareID;
	/// Port monitor name
	std::tstring m_sPortMonitor;
	/// Array of files to install
	STRARRAY	 m_arFiles;
	/// Last error number
	int			 m_nError;
	/// Last error description
	int			 m_nLastError;
	/// Flag to keep the previous error number and message
	bool		 m_bLockError;
	// Whether the system we're installing on is 64-bit
	bool		 m_bX64;


public:
	// Data Methods
	/**
		@brief Retrieves the last error number
		@return The last error number
	*/
	int			GetError() const {return m_nError;};
	/**
		@brief Retrieves the last error description
		@return The last error description
	*/
	int			GetLastError() const {return m_nLastError;};
	/// Initializes the printer driver data
	void		InitializePrinterData(LPCTSTR lpPrinterName, LPCTSTR lpPortName, LPCTSTR lpDriverName, LPCTSTR lpHardwareID, const LPCTSTR* pFiles);

public:
	// Methods
	/// Install the printer driver
	int			Install(LPCTSTR lpFilesPath, HWND hParent);
	/// Remove the printer driver
	int			Uninstall(HWND hParent);
	/// Installs or removes the printer driver according to the command line parameters
	bool		DoCommandLine(int nArgs, TCHAR** pArgs, bool& bNoUI, LPCTSTR lpRelativeDriverFolder);
	/// Add a port monitor
	bool		AddPortMonitor(LPCTSTR lpMonitorName, LPCTSTR lpMonitorFile, LPCTSTR lpFilePath);
	/// Remove a port monitor
	bool		RemovePortMonitor(LPCTSTR lpMonitorName);
	/// Tests if the printer is installed
	bool		IsPrinterInstalled(bool& bInstalled);

protected:
	// Helpers
	/// Add a printer port to windows
	bool		DoAddPort();
	/// Remove a printer port from windows
	bool		DoDeletePort(HWND hWnd);
	/// Copy driver files from a folder to the windows printer drivers folder
	bool		DoCopyFiles(LPCTSTR lpCopyFrom);
	/// Remove the printer driver files from the windows folder
	bool		DoRemoveFiles();
	/// Install the printer driver
	bool		DoInstallPrinterDriver();
	/// Remove the printer driver
	bool		DoUninstallPrinterDriver();
	/// Add a printer
	bool		DoAddPrinter();
	/// Remove the printer
	bool		DoRemovePrinter(LPTSTR lpName);
	/// Remove all the printers for this driver
	bool		DoRemovePrinters();

	/// Set an error
	void		SetError(int nError);
	/**
		@brief Reset the error members
	*/
	void		ResetError() {m_nError = 0; m_nLastError = 0; m_bLockError = false;};
};

#endif   //#define _PRINTERINSTALL_H_
