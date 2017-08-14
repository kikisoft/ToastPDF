/**
	@file
	@brief Header of interface for PScript UI plugin
			based on intrface.h
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

#ifndef _INTRFACE_H_
#define _INTRFACE_H_

/**
    @brief Class implementing the PScript UI plugin interface
*/
class IOemUI: public IPrintOemUI
{
public:
    // *** IUnknown methods ***
	/// This function returns the requested interface for the object, if available
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj);
	/// This function adds a reference to the object
    STDMETHOD_(ULONG,AddRef)  (THIS);
	/// This function removes a reference of the object, deleting it if necessary
    STDMETHOD_(ULONG,Release) (THIS);

	/// Method for retrieving the Core Print Driver interface.
    STDMETHOD(PublishDriverInterface)(THIS_ IUnknown *pIUnknown);

    /// Get OEM dll related information
    STDMETHOD(GetInfo) (THIS_ DWORD dwMode, PVOID  pBuffer, DWORD  cbSize, PDWORD pcbNeeded);

	/// Creates, copies and updates the printer's DevMode structure
    STDMETHOD(DevMode) (THIS_  DWORD dwMode, POEMDMPARAM pOemDMParam) ;

    //
    // OEMCommonUIProp
    //
	/// Adds, deletes and updates the generic UI property pages
    STDMETHOD(CommonUIProp) (THIS_ DWORD dwMode, POEMCUIPPARAM pOemCUIPParam);

	/// Called for the updating the document property pages
    STDMETHOD(DocumentPropertySheets) (THIS_ PPROPSHEETUI_INFO pPSUIInfo, LPARAM lParam);

	/// Called for the updating the device property pages
    STDMETHOD(DevicePropertySheets) (THIS_ PPROPSHEETUI_INFO pPSUIInfo, LPARAM lParam);

	/// Called to test if the print job is printable
    STDMETHOD(DevQueryPrintEx) (THIS_ POEMUIOBJ poemuiobj, PDEVQUERYPRINT_INFO pDQPInfo, PDEVMODE pPublicDM, PVOID pOEMDM);

	/// Called to retrieve the device capabilities
    STDMETHOD(DeviceCapabilities) (THIS_ POEMUIOBJ poemuiobj, HANDLE hPrinter, PWSTR pDeviceName, WORD wCapability, PVOID pOutput, PDEVMODE pPublicDM, PVOID pOEMDM, DWORD dwOld, DWORD *dwResult);

	/// Called to upgrade registry information (not implemented)
    STDMETHOD(UpgradePrinter) (THIS_ DWORD dwLevel, PBYTE pDriverUpgradeInfo);

	/// Called when a printer event occurs (adding, removing, changing printers) (not implemented)
    STDMETHOD(PrinterEvent) (THIS_ PWSTR pPrinterName, INT iDriverEvent, DWORD dwFlags, LPARAM lParam);

	/// Called when the driver is initialized or deleted (not implemented)
    STDMETHOD(DriverEvent)(THIS_ DWORD dwDriverEvent, DWORD dwLevel, LPBYTE pDriverInfo, LPARAM lParam);
 
	/// Called to obtain the ICC profile to use (not implemented)
    STDMETHOD(QueryColorProfile) (THIS_ HANDLE hPrinter, POEMUIOBJ poemuiobj, PDEVMODE pPublicDM, PVOID pOEMDM, ULONG ulReserved, VOID *pvProfileData, ULONG *pcbProfileData, FLONG *pflProfileData);

	/// Called to display the font installer dialog (not implemented)
    STDMETHOD(FontInstallerDlgProc) (THIS_  HWND hWnd, UINT usMsg, WPARAM wParam, LPARAM lParam);

	/// Called to update additional fonts (not implemented)
    STDMETHOD(UpdateExternalFonts) (THIS_ HANDLE hPrinter, HANDLE hHeap, PWSTR pwstrCartridges);

	/**
		@brief Constructor
	*/
    IOemUI() { m_cRef = 1; m_pOEMHelp = NULL; };
	/// Destructor: cleans up
    ~IOemUI();

protected:
	// Members
	/// Number of references to the object
    LONG                m_cRef;
	/// Pointer to the Print Core object
    IPrintOemDriverUI*  m_pOEMHelp;
};

#endif   //#define _INTRFACE_H_
