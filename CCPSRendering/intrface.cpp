/**
	@file
	@brief Implements COM interface for User Mode Printer Customization DLL.
			Based on intrface.cpp
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

#include "precomp.h"
#include <INITGUID.H>
#include <PRCOMOEM.H>

#include "oemps.h"
#include "debug.h"
#include "intrface.h"
#include "CCCommon.h"



////////////////////////////////////////////////////////
//      Internal Globals
////////////////////////////////////////////////////////

/// Count of activate IOemPS components
static long g_cComponents;
/// Count of locks
static long g_cServerLocks;


////////////////////////////////////////////////////////////////////////////////
//
// IOemPS body
//
/**
	Constructor of IOemPS - increments component and reference counts
*/
IOemPS::IOemPS() 
{ 
    VERBOSE(DLLTEXT("IOemPS::IOemPS() entered.\r\n"));

    // Increment COM component count.
    InterlockedIncrement(&g_cComponents);
    InterlockedIncrement(&m_cRef);

    m_pOEMHelp = NULL;

    VERBOSE(DLLTEXT("IOemPS::IOemPS() leaving.\r\n"));
}


/**
	Destructor of IOemPS - frees helper object (if used), decrements locks and reference count
*/
IOemPS::~IOemPS()
{
    // Make sure that helper interface is released.
    if(NULL != m_pOEMHelp)
    {
        m_pOEMHelp->Release();
        m_pOEMHelp = NULL;
    }

    // If this instance of the object is being deleted, then the reference 
    // count should be zero.
    assert(0 == m_cRef);

    // Decrement COM compontent count.
    InterlockedDecrement(&g_cComponents);
}

/**
	@param iid Reference to the interface ID
	@param ppv Pointer to the interface pointer to be filled
	@return E_NOINTERFACE if failed, S_OK if this object supports the interface
*/
HRESULT __stdcall IOemPS::QueryInterface(const IID& iid, void** ppv)
{    
    if (iid == IID_IUnknown)
    {
        *ppv = static_cast<IUnknown*>(this); 
        VERBOSE(DLLTEXT("IOemPS::QueryInterface IUnknown.\r\n")); 
    }
    else if (iid == IID_IPrintOemPS)
    {
        *ppv = static_cast<IPrintOemPS*>(this);
        VERBOSE(DLLTEXT("IOemPS::QueryInterface IPrintOemPs.\r\n")); 
    }
    else
    {
#if DBG && defined(USERMODE_DRIVER)
        TCHAR szOutput[80] = {0};
        StringFromGUID2(iid, szOutput, COUNTOF(szOutput)); // can not fail!
        WARNING(DLLTEXT("IOemPS::QueryInterface %s not supported.\r\n"), szOutput); 
#endif
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

/**
	@return The current lock count of the object
*/
ULONG __stdcall IOemPS::AddRef()
{
    VERBOSE(DLLTEXT("IOemPS::AddRef() entry.\r\n"));
    return InterlockedIncrement(&m_cRef);
}

/**
	@return The current lock count of the object
*/
ULONG __stdcall IOemPS::Release() 
{
    VERBOSE(DLLTEXT("IOemPS::Release() entry.\r\n"));
    if (InterlockedDecrement(&m_cRef) == 0)
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

/**
	@param dwMode What kind of information is queried
	@param pBuffer Pointer to buffer to put the information into
	@param cbSize Size of buffer to fill
	@param pcbNeeded Pointer to DWORD to put the needed size of buffer into
	@return E_FAIL for wrong parameters, S_OK for success
*/
HRESULT __stdcall IOemPS::GetInfo(DWORD dwMode, PVOID pBuffer, DWORD cbSize, PDWORD pcbNeeded)
{
    VERBOSE(DLLTEXT("IOemPS::GetInfo(%d) entry.\r\n"), dwMode);

    // Validate parameters.
    if( (NULL == pcbNeeded)
        ||
        ( (OEMGI_GETSIGNATURE != dwMode)
          &&
          (OEMGI_GETVERSION != dwMode)
          &&
          (OEMGI_GETPUBLISHERINFO != dwMode)
        )
      )
    {
        ERR(DLLTEXT("IOemPS::GetInfo() exit pcbNeeded is NULL!\r\n"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return E_FAIL;
    }

    // Set expected buffer size.
    if(OEMGI_GETPUBLISHERINFO != dwMode)
    {
        *pcbNeeded = sizeof(DWORD);
    }
    else
    {
        *pcbNeeded = sizeof(PUBLISHERINFO);
        return E_FAIL;
    }

    // Check buffer size is sufficient.
    if((cbSize < *pcbNeeded) || (NULL == pBuffer))
    {
        ERR(DLLTEXT("IOemPS::GetInfo() exit insufficient buffer!\r\n"));
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return E_FAIL;
    }

    switch(dwMode)
    {
        // OEM DLL Signature
        case OEMGI_GETSIGNATURE:
            *(PDWORD)pBuffer = CCPRINT_SIGNATURE;
            break;

        // OEM DLL version
        case OEMGI_GETVERSION:
            *(PDWORD)pBuffer = CCPRINT_VERSION;
            break;

        case OEMGI_GETPUBLISHERINFO:
            Dump((PPUBLISHERINFO)pBuffer);
            // Fall through to default case.

        // dwMode not supported.
        default:
            // Set written bytes to zero since nothing was written.
            ERR(DLLTEXT("IOemPS::GetInfo() exit, mode not supported.\r\n"));
            *pcbNeeded = 0;
            SetLastError(ERROR_NOT_SUPPORTED);
            return E_FAIL;
    }

    VERBOSE(DLLTEXT("IOemPS::GetInfo() exit S_OK, (*pBuffer is %#x).\r\n"), *(PDWORD)pBuffer);

    return S_OK;
}

/**
	@param pIUnknown Pointer to the parent driver interface
	@return S_OK if parent driver interface retrieved successfully, E_FAIL if not
*/
HRESULT __stdcall IOemPS::PublishDriverInterface(IUnknown *pIUnknown)
{
    VERBOSE(DLLTEXT("IOemPS::PublishDriverInterface() entry.\r\n"));

    // Need to store pointer to Driver Helper functions, if we already haven't.
    if (this->m_pOEMHelp == NULL)
    {
        HRESULT hResult;


        // Get Interface to Helper Functions.
        hResult = pIUnknown->QueryInterface(IID_IPrintOemDriverPS, (void** ) &(this->m_pOEMHelp));

        if(!SUCCEEDED(hResult))
        {
            // Make sure that interface pointer reflects interface query failure.
            this->m_pOEMHelp = NULL;

            return E_FAIL;
        }

    }

    return S_OK;
}


/**
	@param dwDriverVersion Version of printer driver
	@param cbSize Size of structure pointed to by pded
	@param pded Pointer to a DRVENABLEDATA structure
	@return S_OK
*/
HRESULT __stdcall IOemPS::EnableDriver(DWORD dwDriverVersion, DWORD cbSize, PDRVENABLEDATA pded)
{
    VERBOSE(DLLTEXT("IOemPS::EnableDriver() entry.\r\n"));

    OEMEnableDriver(dwDriverVersion, cbSize, pded);

    // Even if nothing is done, need to return S_OK so 
    // that DisableDriver() will be called, which releases
    // the reference to the Printer Driver's interface.
    return S_OK;
}

/**
	@return S_OK
*/
HRESULT __stdcall IOemPS::DisableDriver()
{
    VERBOSE(DLLTEXT("IOemPS::DisaleDriver() entry.\r\n"));

    OEMDisableDriver();

    // Release reference to Printer Driver's interface.
    if (this->m_pOEMHelp)
    {
        this->m_pOEMHelp->Release();
        this->m_pOEMHelp = NULL;
    }

    return S_OK;
}

/**
	@param pdevobj Pointer to a DEVOBJ structure
	@return S_OK
*/
HRESULT __stdcall IOemPS::DisablePDEV(PDEVOBJ pdevobj)
{
    VERBOSE(DLLTEXT("IOemPS::DisablePDEV() entry.\r\n"));

    OEMDisablePDEV(pdevobj);

    return S_OK;
};

/**
	@param pdevobj Pointer to a DEVOBJ structure
	@param pPrinterName Name of the printer
	@param cPatterns Number of surface patterns in phsurtPatterns
	@param phsurfPatterns Pointer to surface patters handles array
	@param cjGdiInfo Size of pGdiInfo structure
	@param pGdiInfo Pointer to GDIINFO structure
	@param cjDevInfo Size of pDevInfo structure
	@param pDevInfo Pointer to a DEVINFO structure
	@param pded Pointer to a DRVENABLEDATA structure to fill with hooking function pointers
	@param pDevOem Pointer to fill with the private PDEV structure
	@return S_OK if successful, E_FAIL if failed
*/
HRESULT __stdcall IOemPS::EnablePDEV(PDEVOBJ pdevobj, PWSTR pPrinterName, ULONG cPatterns, HSURF *phsurfPatterns, ULONG cjGdiInfo, GDIINFO *pGdiInfo, ULONG cjDevInfo, DEVINFO *pDevInfo, DRVENABLEDATA *pded, OUT PDEVOEM *pDevOem)
{
    VERBOSE(DLLTEXT("IOemPS::EnablePDEV() entry.\r\n"));

    *pDevOem = OEMEnablePDEV(pdevobj, pPrinterName, cPatterns,  phsurfPatterns,
                             cjGdiInfo, pGdiInfo, cjDevInfo, pDevInfo, pded);

	if ((*pDevOem) != NULL)
	{
		((OEMPDEV*)(*pDevOem))->pOemPS = this;
		((OEMPDEV*)(*pDevOem))->pOEMHelp = m_pOEMHelp;
	}

    return (NULL != *pDevOem ? S_OK : E_FAIL);
}

/**
	@param pdevobjOld Pointer to the original DEVOBJ structure
	@param pdevobjNew Pointer to the new DEVOBJ structure
	@return S_OK if successful, E_FAIL if failed
*/
HRESULT __stdcall IOemPS::ResetPDEV(PDEVOBJ pdevobjOld, PDEVOBJ pdevobjNew)
{
    BOOL    bResult;


    VERBOSE(DLLTEXT("IOemPS::ResetPDEV() entry.\r\n"));

    bResult = OEMResetPDEV(pdevobjOld, pdevobjNew);

    return (bResult ? S_OK : E_FAIL);
}

/**
	@param dwMode Type of information/action to perform on the OEMDMPARAM structure
	@param pOemDMParam Pointer to the OEMDMPARAM structure
	@return S_OK if operation was successful, E_FAIL if failed
*/
HRESULT __stdcall IOemPS::DevMode(DWORD dwMode, POEMDMPARAM pOemDMParam)
{   
    VERBOSE(DLLTEXT("IOemPS:DevMode(%d, %#x) entry.\n"), dwMode, pOemDMParam); 
    return hrOEMDevMode(dwMode, pOemDMParam);
}

/**
	@param pdevobj Pointer to the DEVOBJ structure
	@param dwIndex Index of command
	@param pData Not used
	@param cbSize Not used
	@param pdwResult Pointer to command result
	@return S_OK
*/
HRESULT __stdcall IOemPS::Command(PDEVOBJ pdevobj, DWORD dwIndex, PVOID pData, DWORD cbSize, OUT DWORD *pdwResult)
{
    VERBOSE(DLLTEXT("IOemPS::Command() entry.\r\n"));
	// We actually ignore this, as we don't use the printer commands for anything here
    *pdwResult = ERROR_SUCCESS;
    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// oem class factory
//
/**
    @brief This class is the factory for IOemPS objects
*/
class IOemCF : public IClassFactory
{
public:
    // *** IUnknown methods ***
	/// This function queries the OLE interface
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj);
	/// This function adds a reference for the object factory
    STDMETHOD_(ULONG,AddRef)  (THIS);
	/// The function removes a reference of the object factory
    STDMETHOD_(ULONG,Release) (THIS);
   
    // *** IClassFactory methods ***
	/// Creates an instance of the IOemPS OLE object
    STDMETHOD(CreateInstance) (THIS_ 
                               LPUNKNOWN pUnkOuter,
                               REFIID riid,
                               LPVOID FAR* ppvObject);
	/// Locks the server (do not permit unloading)
    STDMETHOD(LockServer)     (THIS_ BOOL bLock);


    // Constructor
	/// Creates the factory object
    IOemCF();
	/// Deletes the factory object
    ~IOemCF();

protected:
	/// Reference count (number of factory objects)
    long    m_cRef;
};

///////////////////////////////////////////////////////////
//
// Class factory body
//
/**
	
*/
IOemCF::IOemCF()
{ 
    VERBOSE(DLLTEXT("IOemCF::IOemCF() entered.\r\n"));

    InterlockedIncrement(&m_cRef);
}

/**
	
*/
IOemCF::~IOemCF() 
{ 
    VERBOSE(DLLTEXT("IOemCF::~IOemCF() entered.\r\n"));

    // If this instance of the object is being deleted, then the reference 
    // count should be zero.
    assert(0 == m_cRef);
}

/**
	@param iid Reference to the interface ID
	@param ppv Pointer to the interface pointer to be filled
	@return E_NOINTERFACE if failed, S_OK if this object supports the interface
*/
HRESULT __stdcall IOemCF::QueryInterface(const IID& iid, void** ppv)
{    
    VERBOSE(DLLTEXT("IOemCF::QueryInterface entered.\r\n"));

    if ((iid == IID_IUnknown) || (iid == IID_IClassFactory))
    {
        *ppv = static_cast<IOemCF*>(this); 
    }
    else
    {
#if DBG && defined(USERMODE_DRIVER)
        TCHAR szOutput[80] = {0};
        StringFromGUID2(iid, szOutput, COUNTOF(szOutput)); // can not fail!
        WARNING(DLLTEXT("IOemCF::QueryInterface %s not supported.\r\n"), szOutput); 
#endif

        *ppv = NULL;
        return E_NOINTERFACE;
    }

    reinterpret_cast<IUnknown*>(*ppv)->AddRef();

    VERBOSE(DLLTEXT("IOemCF::QueryInterface leaving.\r\n"));

    return S_OK;
}

/**
	@return The current lock count of the object
*/
ULONG __stdcall IOemCF::AddRef()
{
    VERBOSE(DLLTEXT("IOemCF::AddRef() called.\r\n"));
    return InterlockedIncrement(&m_cRef);
}

/**
	@return The current lock count of the object
*/
ULONG __stdcall IOemCF::Release() 
{
    VERBOSE(DLLTEXT("IOemCF::Release() called.\r\n"));

    if (InterlockedDecrement(&m_cRef) == 0)
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

/**
	@param pUnknownOuter Pointer to whether object is or isn't part of an aggregate (must be NULL in this case)
	@param iid Reference to the interface ID for the created object
	@param ppv Pointer to the interface pointer to be filled
	@return S_OK if successfully created an object for this interface, error value otherwise
*/
HRESULT __stdcall IOemCF::CreateInstance(IUnknown* pUnknownOuter, const IID& iid, void** ppv) 
{
    VERBOSE(DLLTEXT("Class factory:  Create component.\r\n"));

    // Cannot aggregate.
    if (pUnknownOuter != NULL)
    {
        WARNING(DLLTEXT("Class factory:  non-Null pUnknownOuter.\r\n"));

        return CLASS_E_NOAGGREGATION;
    }

    // Create component.
    IOemPS* pOemCP = new IOemPS;
    if (pOemCP == NULL)
    {
        ERR(ERRORTEXT("Class factory:  failed to allocate IOemPS.\r\n"));

        return E_OUTOFMEMORY;
    }

    // Get the requested interface.
    HRESULT hr = pOemCP->QueryInterface(iid, ppv);

    // Release the IUnknown pointer.
    // (If QueryInterface failed, component will delete itself.)
    pOemCP->Release();
    return hr;
}

/**
	@param bLock TRUE to lock the server, FALSE to release it
	@return S_OK
*/
HRESULT __stdcall IOemCF::LockServer(BOOL bLock) 
{
    VERBOSE(DLLTEXT("IOemCF::LockServer(%d) entered.\r\n"), bLock);

    if (bLock)
    {
        InterlockedIncrement(&g_cServerLocks);
    }
    else
    {
        InterlockedDecrement(&g_cServerLocks);
    }

    VERBOSE(DLLTEXT("IOemCF::LockServer() leaving.\r\n"));
    return S_OK;
}


//
// Registration functions
//

//
// Can DLL unload now?
//
/**
	@return S_OK if we can unload (not locked and has no created objects), S_FALSE otherwise
*/
STDAPI DllCanUnloadNow()
{
    VERBOSE(DLLTEXT("DllCanUnloadNow entered.\r\n"));

    if ((g_cComponents == 0) && (g_cServerLocks == 0))
        return S_OK;

	return S_FALSE;
}

//
// Get class factory
//
/**
	@param clsid OLE class ID for the object (factory)
	@param iid Reference to the interface ID for the created object
	@param ppv Pointer to the interface pointer to be filled
	@return S_OK if created successfully, error code otherwise
*/
STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv)
{
    VERBOSE(DLLTEXT("DllGetClassObject:  Create class factory entered.\r\n"));

    // Can we create this component?
    if (clsid != CLSID_OEMRENDER)
    {
        ERR(ERRORTEXT("DllGetClassObject:  doesn't support clsid %#x!\r\n"), clsid);
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    // Create class factory.
    IOemCF* pFontCF = new IOemCF;  // Reference count set to 1
                                         // in constructor
    if (pFontCF == NULL)
    {
        ERR(ERRORTEXT("DllGetClassObject:  memory allocation failed!\r\n"));
        return E_OUTOFMEMORY;
    }

    // Get requested interface.
    HRESULT hr = pFontCF->QueryInterface(iid, ppv);
    pFontCF->Release();


    VERBOSE(DLLTEXT("DllGetClassObject:  Create class factory leaving.\r\n"));

    return hr;
}

