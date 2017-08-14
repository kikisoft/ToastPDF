/**
	@file
	@brief Main file for Printer Plugin UI functions
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
#include "resource.h"
#include "debug.h"
#include "oemui.h"
#include "winbase.h"
#include "helpers.h"

#include "CCPDFPropPage.h"
#include "CCLicensePropPage.h"
#include <PRCOMOEM.H>


////////////////////////////////////////////////////////
//      INTERNAL MACROS and DEFINES
////////////////////////////////////////////////////////

/*
typedef struct _tagCBUserData 
{
    HANDLE          hComPropSheet;
    HANDLE          hPropPage;
    POEMUIPSPARAM   pOEMUIParam;
    PFNCOMPROPSHEET pfnComPropSheet;

} CBUSERDATA, *PCBUSERDATA;
*/


/**
    @brief Structure containing information for document property pages
*/
struct DocUserData
{
public:
	// Ctor
	/**
		@param p Pointer to the Plugin's data structure
		@param h Handle to the printer
	*/
	DocUserData(POEMDEV p, HANDLE h) : pDevMode(p), bSave(false), hPrinter(h) {};

public:
	// Members
	/// Pointer to the Plugin's data structure
	POEMDEV			pDevMode;
	/// true to save the data when closing the property sheet
	bool			bSave;
	/// Handle to the printer
	HANDLE			hPrinter;
};

////////////////////////////////////////////////////////
//      INTERNAL Helper functions PROTOTYPES
////////////////////////////////////////////////////////

/// Initializes OptItems to display on the document properties property page UI
static HRESULT hrDocumentPropertyPage(POEMCUIPPARAM pOEMUIParam);
/// Initializes OptItems to display on the device properties property page UI
static HRESULT hrPrinterPropertyPage(POEMCUIPPARAM pOEMUIParam);
/// Callback for device UI option data items changes
LONG APIENTRY OEMPrinterUICallBack(PCPSUICBPARAM pCallbackParam, POEMCUIPPARAM pOEMUIParam);
/// Callback for document UI option data item changes
LONG APIENTRY OEMDocUICallBack(PCPSUICBPARAM pCallbackParam, POEMCUIPPARAM pOEMUIParam);
/// Helper function to create UI option data item
static POPTITEM CreateOptItems(HANDLE hHeap, DWORD dwOptItems);
/// Helper function to initialize UI option data item structures
static void InitOptItems(POPTITEM pOptItems, DWORD dwOptItems);
/// Helper function for creating internal structure for a UI option data item
static POPTTYPE CreateOptType(HANDLE hHeap, WORD wOptParams);
/// Retrieves a string from the resources using the received Heap
static PTSTR GetStringResource(HANDLE hHeap, HANDLE hModule, UINT uResource);



/**
	@param dwMode Type of pages to initialize
	@param pOEMUIParam Pointer to UI data structure
	@return S_OK if all went well, E_FAIL otherwise
*/
HRESULT hrOEMPropertyPage(DWORD dwMode, POEMCUIPPARAM pOEMUIParam)
{
    HRESULT hResult = S_OK;

    VERBOSE(DLLTEXT("hrOEMPropertyPage(%d) entry.\r\n"), dwMode);

    // Validate parameters.
    if( (OEMCUIP_DOCPROP != dwMode)
        &&
        (OEMCUIP_PRNPROP != dwMode)        
      )
    {
        ERR(ERRORTEXT("hrOEMPropertyPage() ERROR_INVALID_PARAMETER.\r\n"));
        VERBOSE(DLLTEXT("\tdwMode = %d, pOEMUIParam = %#lx.\r\n"), dwMode, pOEMUIParam);

        // Return invalid parameter error.
        SetLastError(ERROR_INVALID_PARAMETER);
        return E_FAIL;
    }

    switch(dwMode)
    {
        case OEMCUIP_DOCPROP:
			// Call the document property pages initialization function
            hResult = hrDocumentPropertyPage(pOEMUIParam);
            break;

        case OEMCUIP_PRNPROP:
			// Call the device property pages initialization function
            hResult = hrPrinterPropertyPage(pOEMUIParam);
            break;

        default:
            // Should never reach this!
            ERR(ERRORTEXT("hrOEMPropertyPage() Invalid dwMode, %d"), dwMode);
            SetLastError(ERROR_INVALID_PARAMETER);
            hResult = E_FAIL;
            break;
    }

    return hResult;
}

/**
	@param pOEMUIParam Pointer to UI data structure
	@return S_OK
*/
static HRESULT hrDocumentPropertyPage(POEMCUIPPARAM pOEMUIParam)
{
	// Hide some stuff:
	if (pOEMUIParam->pDrvOptItems != NULL)
	{
		for (UINT i=0;i<pOEMUIParam->cDrvOptItems; i++)
		{
			if (pOEMUIParam->pDrvOptItems[i].DMPubID == DMPUB_DEFSOURCE)
				pOEMUIParam->pDrvOptItems[i].Flags |= OPTIF_HIDE;
		}
	}
    
	if(NULL == pOEMUIParam->pOEMOptItems)
    {
        // Fill in the number of OptItems to create for document property UI.
        pOEMUIParam->cOEMOptItems = 1;

        VERBOSE(DLLTEXT("hrDocumentPropertyPage() requesting %d number of items.\r\n"), pOEMUIParam->cOEMOptItems);
    }
    else
    {
        POEMDEV pOEMDev = (POEMDEV) pOEMUIParam->pOEMDM;

        VERBOSE(DLLTEXT("hrDocumentPropertyPage() fill out %d items.\r\n"), pOEMUIParam->cOEMOptItems);

        // Init UI Callback reference.
        pOEMUIParam->OEMCUIPCallback = OEMDocUICallBack;

        // Init OEMOptItmes.
        InitOptItems(pOEMUIParam->pOEMOptItems, pOEMUIParam->cOEMOptItems);

        // Fill out tree view items.
        // Auto-open flag for the document:
        pOEMUIParam->pOEMOptItems[0].Level = 1;
		pOEMUIParam->pOEMOptItems[0].DlgPageIdx = 0;
        pOEMUIParam->pOEMOptItems[0].Flags = OPTIF_CALLBACK;
        pOEMUIParam->pOEMOptItems[0].pName = GetStringResource(pOEMUIParam->hOEMHeap, pOEMUIParam->hModule, IDS_AUTOOPEN);
		if (CanOpenPDFFiles())
			pOEMUIParam->pOEMOptItems[0].Sel = pOEMDev->bAutoOpen ? 1 : 0;
		else
		{
			pOEMUIParam->pOEMOptItems[0].Sel = 0;
			pOEMUIParam->pOEMOptItems[0].Flags |= OPTIF_DISABLED;
		}
		pOEMUIParam->pOEMOptItems[0].DMPubID = DMPUB_USER;

        pOEMUIParam->pOEMOptItems[0].pOptType = CreateOptType(pOEMUIParam->hOEMHeap, 1);

        pOEMUIParam->pOEMOptItems[0].pOptType->Type = TVOT_CHKBOX;
        pOEMUIParam->pOEMOptItems[0].pOptType->pOptParam[0].IconID = 0;
    }

    return S_OK;
}

/**
	@param pOEMUIParam Pointer to UI data structure
	@return S_OK
*/
static HRESULT hrPrinterPropertyPage(POEMCUIPPARAM pOEMUIParam)
{
    if(NULL == pOEMUIParam->pOEMOptItems)
    {
        // Fill in the number of OptItems to create for device property UI.
        pOEMUIParam->cOEMOptItems = 1;

        VERBOSE(DLLTEXT("hrPrinterPropertyPage() requesting %d number of items.\r\n"), pOEMUIParam->cOEMOptItems);
    }
    else
    {
		DWORD dwValue, dwSize, dwRet;

        VERBOSE(DLLTEXT("hrPrinterPropertyPage() fill out %d items.\r\n"), pOEMUIParam->cOEMOptItems);

        // Get device settings value from printer.
		dwSize = sizeof(dwValue);
        dwRet = GetPrinterData(pOEMUIParam->hPrinter, (LPTSTR)SETTINGS_AUTOOPEN, NULL, (LPBYTE)&dwValue, sizeof(DWORD), &dwSize);
        if (dwRet != ERROR_SUCCESS)
			dwValue = 1;
		bool bAutoOpen = (dwValue == 1);

        // Init UI Callback reference.
        pOEMUIParam->OEMCUIPCallback = OEMPrinterUICallBack;

        // Init OEMOptItmes.
        InitOptItems(pOEMUIParam->pOEMOptItems, pOEMUIParam->cOEMOptItems);

        // Fill out tree view items.
        // Auto-open flag for the document:
        pOEMUIParam->pOEMOptItems[0].Level = 1;
		pOEMUIParam->pOEMOptItems[0].DlgPageIdx = 0;
        pOEMUIParam->pOEMOptItems[0].Flags = OPTIF_CALLBACK;
        pOEMUIParam->pOEMOptItems[0].pName = GetStringResource(pOEMUIParam->hOEMHeap, pOEMUIParam->hModule, IDS_AUTOOPEN);
		if (CanOpenPDFFiles())
			pOEMUIParam->pOEMOptItems[0].Sel = bAutoOpen ? 1 : 0;
		else
		{
			pOEMUIParam->pOEMOptItems[0].Sel = 0;
			pOEMUIParam->pOEMOptItems[0].Flags |= OPTIF_DISABLED;
		}
		pOEMUIParam->pOEMOptItems[0].DMPubID = DMPUB_USER;

        pOEMUIParam->pOEMOptItems[0].pOptType = CreateOptType(pOEMUIParam->hOEMHeap, 1);

        pOEMUIParam->pOEMOptItems[0].pOptType->Type = TVOT_CHKBOX;
        pOEMUIParam->pOEMOptItems[0].pOptType->pOptParam[0].IconID = 0;
		pOEMUIParam->pOEMOptItems[0].pOptType->Count = 1;
    }

    return S_OK;
}


/**
	@param pPSUIInfo Pointer to the property pages data structure
	@param lParam Additional data (depends on values in pPSUIInfo
	@param pOEMHelp Pointer to the core PS printer object
	@return S_OK if all went well, E_FAIL otherwise
*/
HRESULT hrOEMDocumentPropertySheets(PPROPSHEETUI_INFO pPSUIInfo, LPARAM lParam, IPrintOemDriverUI* pOEMHelp)
{
    LONG_PTR    lResult;

    // Validate parameters.
    if( (NULL == pPSUIInfo)
        ||
        (PROPSHEETUI_INFO_VERSION != pPSUIInfo->Version)
      )
    {
        ERR(ERRORTEXT("OEMDocumentPropertySheets() ERROR_INVALID_PARAMETER.\r\n"));

        // Return invalid parameter error.
        SetLastError(ERROR_INVALID_PARAMETER);
        return  E_FAIL;
    }

    VERBOSE(DLLTEXT("OEMDocumentPropertySheets(Reason: %d) entry.\r\n"), pPSUIInfo->Reason);


    // Do action.
    switch(pPSUIInfo->Reason)
    {
        case PROPSHEETUI_REASON_INIT:
            {
				// Initialize: add property pages
                POEMUIPSPARAM   pOEMUIParam = (POEMUIPSPARAM) pPSUIInfo->lParamInit;
                POEMDEV         pOEMDev = (POEMDEV) pOEMUIParam->pOEMDM;
	
                PROPSHEETPAGE   Page;

				/*
				// Add another property page
				CCLicensePropPage* pLicense = new CCLicensePropPage(pOEMUIParam->hPrinter, pOEMHelp, pOEMDev, pOEMUIParam->pPublicDM);
				pLicense->PreparePage(Page);

                // Add property sheets.
                lResult = pPSUIInfo->pfnComPropSheet(pPSUIInfo->hComPropSheet, CPSFUNC_ADD_PROPSHEETPAGE, (LPARAM)&Page, 0);
				pLicense->SetCallbackData(pPSUIInfo->pfnComPropSheet, pPSUIInfo->hComPropSheet, (HANDLE)lResult);
				*/

                // Init property page.
				CCPDFPropPage* pPDF = new CCPDFPropPage(pOEMUIParam->hPrinter, pOEMHelp, pOEMDev);
				pPDF->PreparePage(Page);

                // Add property sheets.
                lResult = pPSUIInfo->pfnComPropSheet(pPSUIInfo->hComPropSheet, CPSFUNC_ADD_PROPSHEETPAGE, (LPARAM)&Page, 0);
				pPDF->SetCallbackData(pPSUIInfo->pfnComPropSheet, pPSUIInfo->hComPropSheet, (HANDLE)lResult);

				pPSUIInfo->UserData = (DWORD)new DocUserData(pOEMDev, pOEMUIParam->hPrinter);
			}
            break;

        case PROPSHEETUI_REASON_GET_INFO_HEADER:
            {
				// Get information: do nothing here
                PPROPSHEETUI_INFO_HEADER    pHeader = (PPROPSHEETUI_INFO_HEADER) lParam;

                lResult = TRUE;
            }
            break;

        case PROPSHEETUI_REASON_GET_ICON:
            // No icon
            lResult = 0;
            break;

        case PROPSHEETUI_REASON_SET_RESULT:
            {
				// Remember to save the data
                PSETRESULT_INFO pInfo = (PSETRESULT_INFO) lParam;

                lResult = pInfo->Result;
				if (lResult == 1)
				{
					if (pPSUIInfo->UserData != NULL)
					{
						DocUserData* pData = (DocUserData*)pPSUIInfo->UserData;
						pData->bSave = true;
					}
				}
            }
            break;

        case PROPSHEETUI_REASON_DESTROY:
			// Finished:
            lResult = TRUE;
			if (pPSUIInfo->UserData != NULL)
			{
				DocUserData* pData = (DocUserData*)pPSUIInfo->UserData;
				// Check if we should save
				if (pData->bSave)
					WriteOEMDevToRegistry(pData->pDevMode, pData->hPrinter);
				// And clean up
				delete pData;
				pPSUIInfo->UserData = 0;
			}
            break;
    }

    pPSUIInfo->Result = lResult;
    return S_OK;
}

/**
	@param pPSUIInfo Pointer to the property pages data structure
	@param lParam Additional data (depends on values in pPSUIInfo
	@param pOEMHelp Pointer to the core PS printer object
	@return S_OK if all went well, E_FAIL otherwise
*/
HRESULT hrOEMDevicePropertySheets(PPROPSHEETUI_INFO pPSUIInfo, LPARAM lParam, IPrintOemDriverUI* pOEMHelp)
{
    LONG_PTR    lResult;

    VERBOSE(DLLTEXT("hrOEMDevicePropertySheets(%#x, %#x) entry\r\n"), pPSUIInfo, lParam);

    // Validate parameters.
    if( (NULL == pPSUIInfo)
        ||
        (PROPSHEETUI_INFO_VERSION != pPSUIInfo->Version)
      )
    {
        ERR(ERRORTEXT("hrOEMDevicePropertySheets() ERROR_INVALID_PARAMETER.\r\n"));

        // Return invalid parameter error.
        SetLastError(ERROR_INVALID_PARAMETER);
        return E_FAIL;
    }

    Dump(pPSUIInfo);

    // Do action.
    switch(pPSUIInfo->Reason)
    {
        case PROPSHEETUI_REASON_INIT:
			// We don't do nothing here, really
            break;

        case PROPSHEETUI_REASON_GET_INFO_HEADER:
            {
				// Nothing here either
                PPROPSHEETUI_INFO_HEADER    pHeader = (PPROPSHEETUI_INFO_HEADER) lParam;

                lResult = TRUE;
            }
            break;

        case PROPSHEETUI_REASON_GET_ICON:
            // No icon
            lResult = 0;
            break;

        case PROPSHEETUI_REASON_SET_RESULT:
            {
				// Just pass the result along
                PSETRESULT_INFO pInfo = (PSETRESULT_INFO) lParam;

                lResult = pInfo->Result;
            }
            break;

        case PROPSHEETUI_REASON_DESTROY:
			// Finished
            lResult = TRUE;
            break;
    }

    pPSUIInfo->Result = lResult;
    return S_OK;
}

/**
	@param pCallbackParam Callback information structure
	@param pOEMUIParam Pointer to the UI option data items structure
	@return CPSUICB_ACTION_NONE or CPSUICB_ACTION_ITEMS_APPLIED if changes were applied
*/
LONG APIENTRY OEMPrinterUICallBack(PCPSUICBPARAM pCallbackParam, POEMCUIPPARAM pOEMUIParam)
{
    LONG    lReturn = CPSUICB_ACTION_NONE;
    POEMDEV pOEMDev = (POEMDEV) pOEMUIParam->pOEMDM;

    VERBOSE(DLLTEXT("OEMPrinterUICallBack() entry, Reason is %d.\r\n"), pCallbackParam->Reason);

    switch(pCallbackParam->Reason)
    {
        case CPSUICB_REASON_APPLYNOW:
            {
                DWORD dwValue = pOEMUIParam->pOEMOptItems[0].Sel;

                // Store OptItems state in printer data.
                if (SetPrinterData(pOEMUIParam->hPrinter, SETTINGS_AUTOOPEN, REG_DWORD, (PBYTE)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS)
					// Success!
					lReturn = CPSUICB_ACTION_ITEMS_APPLIED;
            }
            break;

        default:
            break;
    }

    return lReturn;
}

/**
	@param pCallbackParam Callback information structure
	@param pOEMUIParam Pointer to the UI option data items structure
	@return Appropriate action value (see _CPSUICALLBACK in MSDN)
*/
LONG APIENTRY OEMDocUICallBack(PCPSUICBPARAM pCallbackParam, POEMCUIPPARAM pOEMUIParam)
{
	// Initialize response
    LONG    lReturn = CPSUICB_ACTION_NONE;
    POEMDEV pOEMDev = (POEMDEV) pOEMUIParam->pOEMDM;

    VERBOSE(DLLTEXT("OEMDocUICallBack() entry, Reason is %d.\r\n"), pCallbackParam->Reason);

    switch(pCallbackParam->Reason)
    {
		case CPSUICB_REASON_SEL_CHANGED:
			// We don't handle this
			break;

        case CPSUICB_REASON_APPLYNOW:
            // Store OptItems state in DEVMODE.
			pOEMDev->bAutoOpen = pOEMUIParam->pOEMOptItems[0].Sel == 1;
			// Store the data in the registry...
			WriteOEMDevToRegistry(pOEMDev, pOEMUIParam->hPrinter);
			lReturn = CPSUICB_ACTION_ITEMS_APPLIED;
            break;

        case CPSUICB_REASON_KILLACTIVE:
			// Remember the flag
            pOEMDev->bAutoOpen = pOEMUIParam->pOEMOptItems[0].Sel == 1;
            break;

        case CPSUICB_REASON_SETACTIVE:
		case CPSUICB_REASON_ITEMS_REVERTED:
			// Set the flag
            pOEMUIParam->pOEMOptItems[0].Sel = pOEMDev->bAutoOpen ? 1 : 0;
            pOEMUIParam->pOEMOptItems[0].Flags |= OPTIF_CHANGED;
            lReturn = CPSUICB_ACTION_OPTIF_CHANGED;
            break;

        default:
            break;
    }

    return lReturn;
}

/**
	@param hHeap Handle to the heap used to allocate the option data item's structures
	@param dwOptItems Number of option data items to create
	@return 
*/
static POPTITEM CreateOptItems(HANDLE hHeap, DWORD dwOptItems)
{
    POPTITEM    pOptItems = NULL;

    // Allocate memory for OptItems;
    pOptItems = (POPTITEM) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(OPTITEM) * dwOptItems);
    if(NULL != pOptItems)
    {
		// OK, initilize the structure
        InitOptItems(pOptItems, dwOptItems);
    }
    else
    {
        ERR(ERRORTEXT("CreateOptItems() failed to allocate memory for OPTITEMs!\r\n"));
    }

    return pOptItems;
}

/**
	@param pOptItems Pointer to the option data items structure
	@param dwOptItems Number of option data items
*/
static void InitOptItems(POPTITEM pOptItems, DWORD dwOptItems)
{
    VERBOSE(DLLTEXT("InitOptItems() entry.\r\n"));
	if (dwOptItems == 0)
		return;

    // Zero out memory.
    memset(pOptItems, 0, sizeof(OPTITEM) * dwOptItems);

    // Set each OptItem's size, and Public DM ID.
    for(DWORD dwCount = 0; dwCount < dwOptItems; dwCount++)
    {
        pOptItems[dwCount].cbSize = sizeof(OPTITEM);
        pOptItems[dwCount].DMPubID = DMPUB_NONE;
    }
}

/**
	@param hHeap Handle to the heap used to allocate the option data item's structures
	@param wOptParams Option data item parameters
	@return Pointer to the option data item structure created
*/
static POPTTYPE CreateOptType(HANDLE hHeap, WORD wOptParams)
{
    POPTTYPE    pOptType = NULL;

    VERBOSE(DLLTEXT("CreateOptType() entry.\r\n"));

    // Allocate memory from the heap for the OPTTYPE; the driver will take care of clean up.
    pOptType = (POPTTYPE) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(OPTTYPE));
    if(NULL != pOptType)
    {
        // Initialize OPTTYPE.
        pOptType->cbSize = sizeof(OPTTYPE);
        pOptType->Count = wOptParams;

        // Allocate memory from the heap for the OPTPARAMs for the OPTTYPE.
        pOptType->pOptParam = (POPTPARAM) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, wOptParams * sizeof(OPTPARAM));
        if(NULL != pOptType->pOptParam)
        {
            // Initialize the OPTPARAMs.
            for(WORD wCount = 0; wCount < wOptParams; wCount++)
            {
                pOptType->pOptParam[wCount].cbSize = sizeof(OPTPARAM);
            }
        }
        else
        {
            ERR(ERRORTEXT("CreateOptType() failed to allocated memory for OPTPARAMs!\r\n"));

            // Free allocated memory and return NULL.
            HeapFree(hHeap, 0, pOptType);
            pOptType = NULL;
        }
    }
    else
    {
        ERR(ERRORTEXT("CreateOptType() failed to allocated memory for OPTTYPE!\r\n"));
    }

    return pOptType;
}

/**
	@param hHeap Handle to the heap used to allocate memory for the string
	@param hModule Handle to the module to load string from
	@param uResource ID of string resource
	@return Pointer to the loaded string
*/
static PTSTR GetStringResource(HANDLE hHeap, HANDLE hModule, UINT uResource)
{
    int     nResult;
    DWORD   dwSize = MAX_PATH;
    PTSTR   pszString = NULL;

    VERBOSE(DLLTEXT("GetStringResource(%#x, %#x, %d) entered.\r\n"), hHeap, hModule, uResource);

    // Allocate buffer for string resource from heap; let the driver clean it up.
    pszString = (PTSTR) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwSize * sizeof(TCHAR));
    if(NULL != pszString)
    {
        // Load string resource; resize after loading so as not to waste memory.
        nResult = LoadString((HMODULE)hModule, uResource, pszString, dwSize);
        if(nResult > 0)
        {
            PTSTR   pszTemp;


            VERBOSE(DLLTEXT("LoadString() returned %d!\r\n"), nResult);
            VERBOSE(DLLTEXT("String load was \"%s\".\r\n"), pszString);

            pszTemp = (PTSTR) HeapReAlloc(hHeap, HEAP_ZERO_MEMORY, pszString, (nResult + 1) * sizeof(TCHAR));
            if(NULL != pszTemp)
            {
                pszString = pszTemp;
            }
            else
            {
                ERR(ERRORTEXT("GetStringResource() HeapReAlloc() of string retrieved failed! (Last Error was %d)\r\n"), GetLastError());
            }
        }
        else
        {
            ERR(ERRORTEXT("LoadString() returned %d! (Last Error was %d)\r\n"), nResult, GetLastError());
            ERR(ERRORTEXT("GetStringResource() failed to load string resource %d!\r\n"), uResource);

            pszString = NULL;
        }
    }
    else
    {
        ERR(ERRORTEXT("GetStringResource() failed to allocate string buffer!\r\n"));
    }

    return pszString;
}
