/**
	@file
	@brief Define COM interface for User Mode Printer Customization DLL.
			Based on intrface.h
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

#ifndef _INTERFACE_H
#define _INTERFACE_H

/**
    @brief Interface for PostScript OEM sample rendering module
*/
class IOemPS : public IPrintOemPS
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

	/// Method for OEM to specify DDI hook out
    STDMETHOD(EnableDriver)  (THIS_ DWORD DriverVersion, DWORD cbSize, PDRVENABLEDATA pded);

    /// Method to notify OEM plugin that it is no longer required
    STDMETHOD(DisableDriver) (THIS);

    /// Method for OEM to contruct its own PDEV
    STDMETHOD(EnablePDEV)    (THIS_ PDEVOBJ pdevobj, PWSTR pPrinterName, ULONG cPatterns, HSURF *phsurfPatterns, ULONG cjGdiInfo, GDIINFO *pGdiInfo, ULONG cjDevInfo, DEVINFO *pDevInfo, DRVENABLEDATA *pded, OUT PDEVOEM *pDevOem);

    /// Method for OEM to free any resource associated with its PDEV
    STDMETHOD(DisablePDEV)   (THIS_ PDEVOBJ pdevobj);

    /// Method for OEM to transfer from old PDEV to new PDEV
    STDMETHOD(ResetPDEV)     (THIS_ PDEVOBJ pdevobjOld, PDEVOBJ pdevobjNew);


    /// Get OEM dll related information
    STDMETHOD(GetInfo) (THIS_ DWORD dwMode, PVOID pBuffer, DWORD cbSize, PDWORD pcbNeeded);

    /// OEMDevMode
    STDMETHOD(DevMode) (THIS_ DWORD dwMode, POEMDMPARAM pOemDMParam);

    /// OEMCommand - PSCRIPT only, return E_NOTIMPL on Unidrv
    STDMETHOD(Command) (THIS_ PDEVOBJ pdevobj, DWORD dwIndex, PVOID pData, DWORD cbSize, OUT DWORD *pdwResult);

	/// Constructor
    IOemPS();
	/// Destructor
    ~IOemPS();

protected:
	// Members
	/// Count of references for this object
    long                m_cRef;
	/// Pointer to the core printer driver object
    IPrintOemDriverPS*  m_pOEMHelp;

public:
	// Data Access
};





#endif


