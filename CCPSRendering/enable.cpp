/**
	@file
	@brief 
*/

#include "precomp.h"
#include "debug.h"
#include "oemps.h"



////////////////////////////////////////////////////////
//      Internal Constants
////////////////////////////////////////////////////////

/// Hook functions (must mach the order of ENUMHOOKS enum
static const DRVFN OEMHookFuncs[] =
{
    { INDEX_DrvStartPage,                   (PFN) OEMStartPage                  },
    { INDEX_DrvSendPage,                    (PFN) OEMSendPage                   },
    { INDEX_DrvStartDoc,                    (PFN) OEMStartDoc                   },
    { INDEX_DrvEndDoc,                      (PFN) OEMEndDoc                     },
};




/**
	@param pdevobj Pointer to the DEVOBJ structure
	@param pPrinterName Pointer to the printer name
	@param cPatterns Number of surface patterns in phsurtPatterns
	@param phsurfPatterns Pointer to surface patters handles array
	@param cjGdiInfo Size of pGdiInfo structure
	@param pGdiInfo Pointer to GDIINFO structure
	@param cjDevInfo Size of pDevInfo structure
	@param pDevInfo Pointer to a DEVINFO structure
	@param pded Pointer to a DRVENABLEDATA structure to fill with hooking function pointers
	@return Pointer to the private PDEV structure, if successful, or NULL if failed
*/
PDEVOEM APIENTRY OEMEnablePDEV(PDEVOBJ pdevobj, PWSTR pPrinterName, ULONG cPatterns, HSURF *phsurfPatterns, ULONG cjGdiInfo, GDIINFO *pGdiInfo, ULONG cjDevInfo, DEVINFO *pDevInfo, DRVENABLEDATA *pded)
{
    POEMPDEV    poempdev;
    INT         i, j;
    DWORD       dwDDIIndex;
    PDRVFN      pdrvfn;

    VERBOSE(DLLTEXT("OEMEnablePDEV() entry.\r\n"));

    //
    // Allocate the OEMDev
    //
    poempdev = new OEMPDEV;
    if (NULL == poempdev)
    {
        return NULL;
    }

    //
    // Fill in OEMDEV as you need
    //
	poempdev->nPage = 0;

    //
    // Fill in OEMDEV
    //
    for (i = 0; i < MAX_DDI_HOOKS; i++)
    {
        //
        // search through Unidrv's hooks and locate the function ptr
        //
        dwDDIIndex = OEMHookFuncs[i].iFunc;
        for (j = pded->c, pdrvfn = pded->pdrvfn; j > 0; j--, pdrvfn++)
        {
            if (dwDDIIndex == pdrvfn->iFunc)
            {
                poempdev->pfnPS[i] = pdrvfn->pfn;
                break;
            }
        }
        if (j == 0)
        {
            //
            // didn't find the Unidrv hook. Should happen only with DrvRealizeBrush
            //
            poempdev->pfnPS[i] = NULL;
        }

    }

    return (POEMPDEV) poempdev;
}

/**
	@param pdevobj Pointer to the DEVOBJ structure
*/
VOID APIENTRY OEMDisablePDEV(PDEVOBJ pdevobj)
{
    VERBOSE(DLLTEXT("OEMDisablePDEV() entry.\r\n"));


    //
    // Free memory for OEMPDEV and any memory block that hangs off OEMPDEV.
    //
    assert(NULL != pdevobj->pdevOEM);
    delete pdevobj->pdevOEM;
}

/**
	@param pdevobjOld Pointer to the original DEVOBJ structure
	@param pdevobjNew Pointer to the new DEVOBJ structure
	@return 
*/
BOOL APIENTRY OEMResetPDEV(PDEVOBJ pdevobjOld, PDEVOBJ pdevobjNew)
{
    VERBOSE(DLLTEXT("OEMResetPDEV() entry.\r\n"));


    //
    // If you want to carry over anything from old pdev to new pdev, do it here.
    //

    return TRUE;
}

/**
*/
VOID APIENTRY OEMDisableDriver()
{
    VERBOSE(DLLTEXT("OEMDisableDriver() entry.\r\n"));
}

/**
	@param dwOEMintfVersion Version of driver (should be PRINTER_OEMINTF_VERSION)
	@param dwSize Size of DEVENABLEDATA structure in pded
	@param pded Pointer to a DEVENABLEDATA structure
	@return TRUE
*/
BOOL APIENTRY OEMEnableDriver(DWORD dwOEMintfVersion, DWORD dwSize, PDRVENABLEDATA pded)
{
    VERBOSE(DLLTEXT("OEMEnableDriver() entry.\r\n"));

    // List DDI functions that are hooked.
    pded->iDriverVersion =  PRINTER_OEMINTF_VERSION;
    pded->c = sizeof(OEMHookFuncs) / sizeof(DRVFN);
    pded->pdrvfn = (DRVFN *) OEMHookFuncs;

    return TRUE;
}
