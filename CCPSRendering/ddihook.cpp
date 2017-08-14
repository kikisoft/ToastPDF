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
#include "debug.h"
#include "oemps.h"
#include "resource.h"
#include <PRCOMOEM.H>
#include "CCTChar.h"
#include "CCPrintRegistry.h"

#include "intrface.h"
#include "PngImage.h"
#include "SQLiteDB.h"

/// Instance of module (defined at dllentry.cpp)
extern HINSTANCE ghInstance;

/// Printer Name
#define PRINTER_NAME		 _T("Toast PDF")
/// Port Name
#define PORT_NAME			 _T("ToastPDFPort")
/// Print monitor Name
#define MONITOR_NAME	     _T("ToastPDF_Redmon")

/* Helper functions */

/**
	@brief This function retrieves the resource ID of the image associated with the license
	@param info The license information
	@return Resource ID for the image
*/
/*
UINT GetLicenseImage(const LicenseInfo& info)
{
	switch (info.m_eLicense)
	{
		case LicenseInfo::LTCC:
			if (info.m_eModification != LicenseInfo::MTUnknown)
			{
				UINT uBase = IDPNG_BY_NC + (int)info.m_eModification;
				if (info.m_bCommercialUse)
					uBase += 3;
				return uBase;
			}
			break;
		case LicenseInfo::LTSampling:
			if (info.m_eSampling == LicenseInfo::STUnknown)
				break;
			return IDPNG_SAMPLING + (int)info.m_eSampling;
		case LicenseInfo::LTDevelopingNations:
		case LicenseInfo::LTPublicDomain:
			return IDPNG_SOMERIGHTS;
		default:
			break;
	}
	return 0;
}
*/

/**
	@brief This function copies an image from the source surface to the destination surface
	@param pso The destination surface
	@param pSrc The source surface (the image must be attached to it already)
	@param rectTarget The location to draw the image at
	@return TRUE if drawn successfully, FALSE if failed for any reason
*/
BOOL DrawImage(SURFOBJ* pso, SURFOBJ* pSrc, RECTL& rectTarget)
{
	// Create the translate object (empty)
	XLATEOBJ xlate;
	memset(&xlate, 0, sizeof(xlate));

	// Create the color adjustment structure
	COLORADJUSTMENT clr;
	memset(&clr, 0, sizeof(clr));

	// Create the clip region
	CLIPOBJ clip;
	clip.iUniq = 0;
	clip.rclBounds = rectTarget;
	clip.iDComplexity = DC_RECT;
	clip.iFComplexity = FC_RECT;
	clip.iMode = TC_RECTANGLES;
	clip.fjOptions = 0;

	// Brush location
	POINTL pt;
	pt.x = pt.y = 0;

	// Create source location
	RECTL rectSource;
	rectSource.top = rectSource.left = 0;
	rectSource.right = pSrc->sizlBitmap.cx;
	rectSource.bottom = pSrc->sizlBitmap.cy;

	// Do the work...
	BOOL bRet = EngStretchBlt(pso, pSrc, NULL, &clip, &xlate, &clr, &pt, &rectTarget, &rectSource, &pt, COLORONCOLOR);
	
	return bRet;
}

/**
	@brief This function draws a loaded PNG image onto the received surface in the specified location
	@param pso The surface to draw upon
	@param png The image to draw
	@param rectTarget The drawing location
	@return TRUE if drawn successfully, FALSE if failed
*/
BOOL DrawImage(SURFOBJ* pso, const PngImage& png, RECTL& rectTarget)
{
	SIZEL szBitmap;
	szBitmap.cx = png.GetWidth();
	szBitmap.cy = png.GetHeight();

	UINT uFormat;
	switch (png.GetBitsPerPixel())
	{
		case 1:
			uFormat = BMF_1BPP;
			break;
		case 4:
			uFormat = BMF_4BPP;
			break;
		case 8:
			uFormat = BMF_8BPP;
			break;
		case 16:
			uFormat = BMF_16BPP;
			break;
		case 24:
			uFormat = BMF_24BPP;
			break;
		case 32:
			uFormat = BMF_32BPP;
			break;
	}

	HBITMAP hEngBmp = EngCreateBitmap(szBitmap, png.GetWidthInBytes(), uFormat, BMF_TOPDOWN|BMF_USERMEM, (void*)png.GetBits());
	if (hEngBmp == NULL)
		return FALSE;

	SURFOBJ* pSrc = EngLockSurface((HSURF)hEngBmp);
	BOOL bRet = DrawImage(pso, pSrc, rectTarget);
	EngUnlockSurface(pSrc);
	EngDeleteSurface((HSURF)hEngBmp);

	return bRet;
}

/**
	@brief This function draws a loaded BITMAP image onto the received surface in the specified location
	@param pso The surface to draw upon
	@param hBmp The bitmap to draw
	@param rectTarget The drawing location
	@return TRUE if drawn successfully, FALSE if failed
*/
BOOL DrawImage(SURFOBJ* pso, HBITMAP hBmp, RECTL& rectTarget)
{
	BITMAP bmp;
	::GetObject(hBmp, sizeof(bmp), &bmp);
	SIZEL szBitmap;
	szBitmap.cx = bmp.bmWidth;
	szBitmap.cy = bmp.bmHeight;
	UINT uFormat;
	switch (bmp.bmBitsPixel)
	{
		case 1:
			uFormat = BMF_1BPP;
			break;
		case 4:
			uFormat = BMF_4BPP;
			break;
		case 8:
			uFormat = BMF_8BPP;
			break;
		case 16:
			uFormat = BMF_16BPP;
			break;
		case 24:
			uFormat = BMF_24BPP;
			break;
		case 32:
			uFormat = BMF_32BPP;
			break;
	}

	HBITMAP hEngBmp = EngCreateBitmap(szBitmap, bmp.bmWidthBytes, uFormat, BMF_USERMEM, (void*)bmp.bmBits);
	if (hEngBmp == NULL)
		return FALSE;

	SURFOBJ* pSrc = EngLockSurface((HSURF)hEngBmp);
	BOOL bRet = DrawImage(pso, pSrc, rectTarget);
	EngUnlockSurface(pSrc);
	EngDeleteSurface((HSURF)hEngBmp);

	return bRet;
}

/// PDFMark URL link box definition
#define URLBOX	"\n[ /Rect [%d %d %d %d]\n\
	/Action << /Subtype /URI /URI (%s) >>\n\
	/Border [0 0 2]\n\
	/Color [.7 0 0]\n\
	/Subtype /Link\n\
	/ANN pdfmark\n"

/// PDFMark hyperlink function definition
#define HYPERLINK_FUNC "/cc_hyperlink { dup show stringwidth pop neg\n\
   gsave 0 currentfont dup\n\
      /FontInfo get /UnderlineThickness get exch\n\
      /FontMatrix get dtransform setlinewidth 0 currentfont dup\n\
      /FontInfo get /UnderlinePosition get exch\n\
      /FontMatrix get dtransform rmoveto rlineto stroke\n\
   grestore\n\
} def\n"

/// PDFMark hyperlink start definition
#define HYPERLINK_START	"\ncurrentpoint\n\
  %d sub\n\
  currentcolor 0 0 128 setrgbcolor\
  ("

/// PDFMark hyperlink close definition
#define HYPERLINK_END ") cc_hyperlink\n\
  setcolor\n\
  currentpoint\n\
  [ /Rect 6 -4 roll 4 array astore\n\
    /Action << /Subtype /URI /URI (%s) >>\n\
    /Border [0 0 2]\n\
    /Color [.7 0 0]\n\
    /Subtype /Link\n\
    /ANN pdfmark\n"

/// PDFMark internal document link box definition
#define JUMPBOX "\n[ /Rect [%d %d %d %d]\n\
	/Border [0 0 2]\n\
	/Color [.7 0 0]\n\
	/Dest /%s\n\
	/Title (%s)\n\
	/Subtype /Link\n\
	/ANN pdfmark\n"

/// PDFMark internal document destination definition
#define JUMPDEST "\n[ /Dest /%s\n\
	/View [/Fit]\n\
	/DEST pdfmark\n"

/// Postscript text writing (with font) start definition
#define PS_TEXT_START "/Times-Roman findfont [%d 0 0 -%d 0 0 ] makefont setfont\n\
%d %d moveto\n\
("
/// Postscript text (with font) writing end definition
#define PS_TEXT_END ") show\n"

/// Postscript text writing start definition
#define PS_JUSTTEXT_START "("
/// Postscript text writing end definition
#define PS_JUSTTEXT_END ") show \n"

/// Postscript text writing (centered) helper function definition
#define PS_TEXT_END_CENTER ")\n\
dup stringwidth pop\n\
%d exch sub\n\
2 div\n\
0 rmoveto\n\
show\n"

/// Postscript text writing (centered) start definition
#define PS_CALC_CENTER_START "/Times-Roman findfont [%d 0 0 -%d 0 0 ] makefont setfont\n\
%d %d moveto\n\
("

/// Postscript text writing (centered) end definition
#define PS_CALC_CENTER_END ") stringwidth pop\n\
%d exch sub\n\
2 div\n\
0 rmoveto\n"

/// Postscript circle definition
#define PS_CIRCLE "newpath %d %d %d 0 360 arc fill closepath\n"

/// PostScript image start definition
#define PS_IMAGE_START "gsave\n\
%d %d translate\n\
%d %d scale\n\
%d %d %d [%d 0 0 -%d 0 %d] {<\n"

/// PostScript image end definition
#define PS_IMAGE_END "\n>} image\n\
grestore\n";

/// Created by text
#define CREATEDBY_TEXT "The document was created by "
/// Created by link
#define CREATEDBY_LINK "CC PDF Converter"

/// Postscript public domain license information data: (note - special fix in ghostscript to handle this!)
#define PS_NO_LICENSE_INFO "\n[ /Rights (False)\n\
	/RightsURL ("

/// Postscript public domain license information data: (note - special fix in ghostscript to handle this!)
#define PS_LICENSE_INFO_START "\n[ /Rights (True)\n\
	/RightsURL ("
#define PS_LICENSE_INFO_CONTINUE ")\n\
	/RightsStatement (This work is licensed under a "
#define PS_LICENSE_INFO_END ")\n\
	/DOCINFO pdfmark\n"

/**
	@brief This function escapes text to be able to write it in PostScript 
	@param lpString The string to fill with the escaped text
	@param lpText The original text
	@param dwLen Length of original text
*/
void AddPSText(LPSTR lpString, LPCSTR lpText, DWORD dwLen)
{
	int nLoc = strlen(lpString);
	for (int i=0;i<dwLen;i++)
	{
		switch (lpText[i])
		{
			case '\n':
			case '\r':
			case '\t':
			case '\b':
			case '\f':
			case '\\':
			case '(':
			case ')':
				lpString[nLoc++] = '\\';
				break;
		}
		lpString[nLoc++] = lpText[i];
	}
	lpString[nLoc] = '\0';
}

/**
	@brief This function escapes text to be able to write it in PostScript 
	@param lpString The string to fill with the escaped text
	@param s The original string to write
*/
void AddPSText(LPSTR lpString, const std::string& s)
{
	AddPSText(lpString, s.c_str(), s.size());
}

/**
	@brief This function writes text to the PostScript file
	@param pdevobj Pointer to the device object representing the PostScript printer
	@param pDevOEM Pointer to the CC PDF Converter render plugin object
	@param lpText The text to write
*/
void PrintPS(PDEVOBJ pdevobj, POEMPDEV pDevOEM, LPCSTR lpText)
{
	DWORD dwResult, dwLen = strlen(lpText);
	pDevOEM->pOEMHelp->DrvWriteSpoolBuf(pdevobj, (void*)lpText, dwLen, &dwResult);
}

/**
	@brief This function creates a circle on the specified location in the PostScript file
	@param pdevobj Pointer to the device object representing the PostScript printer
	@param pDevOEM Pointer to the CC PDF Converter render plugin object
	@param nX X location of the center of the circle
	@param nY Y location of the center of the circle
	@param nRadius Radius of the circle
*/
void PrintCircle(PDEVOBJ pdevobj, POEMPDEV pDevOEM, int nX, int nY, int nRadius)
{
	char cCircle[128];
	sprintf(cCircle, PS_CIRCLE, nX, nY, nRadius);
	PrintPS(pdevobj, pDevOEM, cCircle);
}

/**
	@brief This function writes a centered text string into the PostScript file
	@param pdevobj Pointer to the device object representing the PostScript printer
	@param pDevOEM Pointer to the CC PDF Converter render plugin object
	@param nFontSize Size of the font to print at
	@param nX X Location of the start of the string's bound box
	@param nY Y location of the string's bound box
	@param nWidth Width of the string's bound box
	@param lpText The text to write
*/
void CenterText(PDEVOBJ pdevobj, POEMPDEV pDevOEM, int nFontSize, int nX, int nY, int nWidth, LPCSTR lpText)
{
	char cStr[1024];
	sprintf(cStr, PS_CALC_CENTER_START, nFontSize, nFontSize, nX, nY + nFontSize);
	AddPSText(cStr, lpText, strlen(lpText));
	char cEnd[1024];
	sprintf(cEnd, PS_CALC_CENTER_END, nWidth);
	strcat(cStr, cEnd);
	PrintPS(pdevobj, pDevOEM, cStr);
}

/**
	@brief This function writes an inner-document link box into the PostScript file
	@param pdevobj Pointer to the device object representing the PostScript printer
	@param pDevOEM Pointer to the CC PDF Converter render plugin object
	@param lpDestination Name of the link's destination
	@param rectTarget Location of the link's box
	@param lpName Name of the location (will be displayed in a tooltip in Acrobat)
*/
void PrintJumpLink(PDEVOBJ pdevobj, POEMPDEV pDevOEM, LPCSTR lpDestination, const RECTL& rectTarget, LPCSTR lpName = NULL)
{
	if (lpName == NULL)
		lpName = lpDestination;
	char cLink[1024];
	sprintf(cLink, JUMPBOX, rectTarget.left, rectTarget.top, rectTarget.right, rectTarget.bottom, lpDestination, lpName);
	PrintPS(pdevobj, pDevOEM, cLink);
}

/**
	@brief This function writes a destination mark into the PostScript file (target for inner-document links)
	@param pdevobj Pointer to the device object representing the PostScript printer
	@param pDevOEM Pointer to the CC PDF Converter render plugin object
	@param lpDestination Name of the destination (must be NULL terminated)
*/
void PrintJumpDestination(PDEVOBJ pdevobj, POEMPDEV pDevOEM, LPCSTR lpDestination)
{
	char cLink[1024];
	sprintf(cLink, JUMPDEST, lpDestination);
	PrintPS(pdevobj, pDevOEM, cLink);
}

/**
	@brief This function writes a URL link box into the PostScript file
	@param pdevobj Pointer to the device object representing the PostScript printer
	@param pDevOEM Pointer to the CC PDF Converter render plugin object
	@param lpURL The target URL (must be NULL terminated)
	@param rectTarget Location of the link's box
*/
void PrintURLLink(PDEVOBJ pdevobj, POEMPDEV pDevOEM, LPCSTR lpURL, const RECTL& rectTarget)
{
	char cLink[1024];
	sprintf(cLink, URLBOX, rectTarget.left, rectTarget.top, rectTarget.right, rectTarget.bottom, lpURL);
	DWORD dwResult, dwLen = strlen(cLink);
	pDevOEM->pOEMHelp->DrvWriteSpoolBuf(pdevobj, cLink, dwLen, &dwResult);
}

/**
	@brief This function writes a Hyperlinked text into the PostScript file
	@param pdevobj Pointer to the device object representing the PostScript printer
	@param pDevOEM Pointer to the CC PDF Converter render plugin object
	@param nFontSize Size of the font to write in
	@param lpText Text of the link
	@param dwLen Length of the text
	@param lpURL The URL of the link (must be NULL terminated)
*/
void PrintHyperlink(PDEVOBJ pdevobj, POEMPDEV pDevOEM, int nFontSize, LPCSTR lpText, DWORD dwLen, LPCSTR lpURL)
{
	char cStr[2048];
	sprintf(cStr, HYPERLINK_START, nFontSize);
	AddPSText(cStr, lpText, dwLen);
	char cEnd[1024];
	sprintf(cEnd, HYPERLINK_END, lpURL);
	strcat(cStr, cEnd);
	PrintPS(pdevobj, pDevOEM, cStr);
}

/**
	@brief This function prepares a text string so it can be written into PostScript
	@param lpString The string to fill
	@param nFontSize Size of the font to use
	@param nX X location of the start of the text
	@param nY Y location of the start of the text
	@param lpText The text to write
	@param dwLen Length of the text string
	@param nWidth Width of the text's bounding box (-1 so it won't be centered)
	@return The length of the new string

	Note that not specifying nWidth (or setting it to -1) will write the text as it is, and specifying nWidth
	will center the text inside a box starting at nX and being nWidth wide
*/
DWORD PrepareWriteString(LPSTR lpString, int nFontSize, int nX, int nY, LPCSTR lpText, DWORD dwLen, int nWidth = -1)
{
	sprintf(lpString, PS_TEXT_START, nFontSize, nFontSize, nX, nY);
	AddPSText(lpString, lpText, dwLen);
	if (nWidth == -1)
		strcat(lpString, PS_TEXT_END);
	else
	{
		char c[128];
		sprintf(c, PS_TEXT_END_CENTER, nWidth);
		strcat(lpString, c);
	}
	return strlen(lpString);
}

/**
	@brief This function writes text into the postscript file (without a specific location)
	@param pdevobj Pointer to the device object representing the PostScript printer
	@param pDevOEM Pointer to the CC PDF Converter render plugin object
	@param lpText The text to write
*/
void PrintText(PDEVOBJ pdevobj, POEMPDEV pDevOEM, LPCSTR lpText)
{
	char cStr[1024];
	sprintf(cStr, PS_JUSTTEXT_START);
	AddPSText(cStr, lpText, strlen(lpText));
	strcat(cStr, PS_JUSTTEXT_END);
	PrintPS(pdevobj, pDevOEM, cStr);
}

/**
	@brief This function writes text into the postscript file in a specific location, breaking into lines if necessary
	@param pdevobj Pointer to the device object representing the PostScript printer
	@param pDevOEM Pointer to the CC PDF Converter render plugin object
	@param nFontSize Size of the font to write with
	@param nX The X location of the bound box
	@param nY The Y loation of the bound box
	@param nWidth The width of the bound box
	@param nLineHeight Height of each text line
	@param lpText The text to write
	@param bCenter true to center the text, false (default) to align to the left
	@return The height of the printed text
*/
int PrintText(PDEVOBJ pdevobj, POEMPDEV pDevOEM, int nFontSize, int nX, int nY, int nWidth, int nLineHeight, LPCSTR lpText, bool bCenter = false)
{
	char cStr[1024];
	// Calculate how much text we can put in the width:
	DWORD dwResult, dwLen = strlen(lpText), dwWriteLen;
	int nRet = 0;
	while ((dwLen * nFontSize / 2) > nWidth)
	{
		// Find the last space we can break on:
		DWORD dwBreak = max(1, nWidth / (nFontSize / 2));
		while ((dwBreak > 1) && (lpText[dwBreak] != ' '))
			dwBreak--;
		// Write what we have
		nRet += nLineHeight;
		dwWriteLen = PrepareWriteString(cStr, nFontSize, nX, nY + nRet, lpText, dwBreak, bCenter ? nWidth : -1);
		pDevOEM->pOEMHelp->DrvWriteSpoolBuf(pdevobj, cStr, dwWriteLen, &dwResult);
		lpText += dwBreak + 1;
		dwLen -= dwBreak - 1;
	}
	
	if (dwLen > 0)
	{
		nRet += nLineHeight;
		dwWriteLen = PrepareWriteString(cStr, nFontSize, nX, nY + nRet, lpText, dwLen, bCenter ? nWidth : -1);
		dwWriteLen = strlen(cStr);
		pDevOEM->pOEMHelp->DrvWriteSpoolBuf(pdevobj, cStr, dwWriteLen, &dwResult);
	}

	return nRet;
}

/**
	@brief This function writes a bitmap image directly into the PostScript file
	@param pdevobj Pointer to the device object representing the PostScript printer
	@param pDevOEM Pointer to the CC PDF Converter render plugin object
	@param hBmp The bitmap to write
	@param rectTargetArea In: Horizontal bound box, Y location of the bitmap; Out: Actual location of the bitmap
	@return TRUE if written successfully, FALSE if failed
*/
BOOL PrintImage(PDEVOBJ pdevobj, POEMPDEV pDevOEM, HBITMAP hBmp, RECTL& rectTargetArea)
{
	double dMultiplier = 1.0;
	if (pdevobj->pPublicDM->dmPrintQuality > 0)
		dMultiplier = min(2.0, pdevobj->pPublicDM->dmPrintQuality / 72.0);

	DIBSECTION dib;
	if (::GetObject(hBmp, sizeof(dib), &dib) == 0)
		return FALSE;

	int nDrawWidth = dib.dsBmih.biWidth * dMultiplier;
	int nDrawHeight = dib.dsBmih.biHeight * dMultiplier;
	rectTargetArea.left = ((rectTargetArea.left + rectTargetArea.right) / 2) - (nDrawWidth / 2);
	rectTargetArea.right = rectTargetArea.left + nDrawWidth;
	rectTargetArea.bottom = rectTargetArea.top + nDrawHeight;

	char cStr[1024];
	sprintf(cStr, PS_IMAGE_START, rectTargetArea.left, rectTargetArea.top, 
		nDrawWidth, nDrawHeight, 
		dib.dsBm.bmWidth, dib.dsBm.bmHeight, dib.dsBm.bmBitsPixel, dib.dsBm.bmWidth, dib.dsBm.bmHeight, dib.dsBm.bmHeight);
	std::string sWrite(cStr);

	int nByteWidth = dib.dsBmih.biSizeImage / dib.dsBmih.biHeight;
	int nRealByteWidth = dib.dsBmih.biBitCount == 0 ? 8 : (dib.dsBm.bmWidth * 8) / dib.dsBmih.biBitCount;
	for (int i=0;i<dib.dsBm.bmHeight;i++)
	{
		for (int j=0;j<nRealByteWidth;j++)
		{
			sprintf(cStr, "%02x", *(((unsigned char*)dib.dsBm.bmBits) + (i * nByteWidth) + j));
			sWrite += cStr;
		}
		sWrite += "\n";
	}

	sWrite += PS_IMAGE_END;
	DWORD dwWriteLen = sWrite.size(), dwResult;
	DWORD dwRes = pDevOEM->pOEMHelp->DrvWriteSpoolBuf(pdevobj, (void*)sWrite.c_str(), dwWriteLen, &dwResult);
	return (dwRes == S_OK) && (dwWriteLen == dwResult);
}

/**
	@brief This function adds the license page to the PostScript file
	@param pso Pointer to the surface object representing the writing PostScript file
	@return TRUE if written successfully, FALSE if failed to write
*/
/*
BOOL DoLicensePage(SURFOBJ* pso)
{
    PDEVOBJ     pdevobj;
    POEMPDEV    poempdev;

    pdevobj = (PDEVOBJ)pso->dhpdev;
    poempdev = (POEMPDEV)pdevobj->pdevOEM;
	PCOEMDEV pDevMode = (PCOEMDEV)pdevobj->pOEMDM;

	// Print the license destination, as it should reach here:
	PrintJumpDestination(pdevobj, poempdev, "TheLicense");

	// ### later - mind the language
	int nLang = 6;

	// Retrieve the text we wonna write on the page:
	SQLite::DB db;
	std::tstring sPath = CCPrintRegistry::GetRegistryString(pdevobj->hPrinter, _T("DB Path"), _T(""));
	if (sPath.empty())
		return FALSE;
	if (!db.Open(sPath.c_str()))
		return FALSE;

	// Write stuff in proper location:
	int nHeight = pso->sizlBitmap.cy;
	int nLineHeight = nHeight / 60;
	int nFontSize = nLineHeight - 2;
	int nY = nLineHeight * 6;
	int nX = nLineHeight * 2;

	// Find license type:
	TCHAR cQuery[256];
	int nMode;
	switch (pDevMode->info.m_eLicense)
	{
		case LicenseInfo::LTCC:
			nMode = 0;
			_stprintf(cQuery, _T("SELECT * FROM TblLicenseType WHERE commercial = %d AND derivs = %d AND mode = %d"), pDevMode->info.m_bCommercialUse ? 1 : 0, pDevMode->info.m_eModification, nMode);
			break;
		case LicenseInfo::LTSampling:
			nMode = 1;
			_stprintf(cQuery, _T("SELECT * FROM TblLicenseType WHERE derivs = %d AND mode = %d"), pDevMode->info.m_eSampling, nMode);
			break;
		case LicenseInfo::LTDevelopingNations:
			nMode = 2;
			_stprintf(cQuery, _T("SELECT * FROM TblLicenseType WHERE mode = %d"), nMode);
			break;
		default:
			return FALSE;
	}

	SQLite::Recordset records = db.Query(cQuery);
	if (!records.IsValid() || (records.GetRecordCount() == 0))
	{
		std::tstring s = db.GetLastError();
		return FALSE;
	}

	int nLicenseID = records.GetRecord(0).GetNumField(_T("LicenseTypeID"));
	std::string sLicenseShortName = MakeAnsiString(records.GetRecord(0).GetField(_T("LicenseShortName")));
	_stprintf(cQuery, _T("SELECT LicenseName FROM tblLicenseName WHERE LicenseTypeID = %d AND LangID = %d"), nLicenseID, nLang);
	records = db.Query(cQuery);
	if (!records.IsValid() || (records.GetRecordCount() == 0))
	{
		std::tstring s = db.GetLastError();
		return FALSE;
	}
	std::tstring sLicenseName = records.GetRecord(0).GetField(_T("LicenseName"));

	int nJuri = 1;
	if (pDevMode->info.HasJurisdiction())
	{
		// Use the jurisdiction name to find the ID
		_stprintf(cQuery, _T("SELECT jurisdictionsID FROM tblJurisdictionName WHERE LanguageID = 6 AND JurisdictionName = '%s'"), pDevMode->info.m_cJurisdiction);
		records = db.Query(cQuery);
		if (records.IsValid() && (records.GetRecordCount() > 0))
			nJuri = records.GetRecord(0).GetNumField(_T("jurisdictionsID"));
	}

	_stprintf(cQuery, _T("SELECT * FROM tblJurisdiction WHERE JurisdictionID = %d AND mode = %d"), nJuri, nMode);
	records = db.Query(cQuery);
	SQLite::Record recJuri;
	if (records.IsValid() && (records.GetRecordCount() > 0))
		recJuri = records.GetRecord(0);

	std::string sVersion, sJuriShortName;
	if (recJuri.IsValid())
	{
		sVersion = MakeAnsiString(recJuri.GetField(_T("Version")));
		sJuriShortName = MakeAnsiString(recJuri.GetField(_T("ShortName")));
		sLicenseName += _T(" ") + MakeTString(sVersion);
	}
	
	_stprintf(cQuery, _T("SELECT JurisdictionName FROM TblJurisdictionName WHERE JurisdictionsID = %d AND LanguageID = %d"), nJuri, nLang);
	records = db.Query(cQuery);
	if (records.IsValid() && (records.GetRecordCount() > 0))
	{
		sLicenseName += _T(" ") + records.GetRecord(0).GetField(_T("JurisdictionName"));
	}
	else
	{
		std::tstring s = db.GetLastError();
		return FALSE;
	}

	// Print the logo on the top
	sPath = CCPrintRegistry::GetRegistryString(pdevobj->hPrinter, _T("Image Path"), sPath.c_str());
	if (sPath.empty() || (*sPath.rbegin() != '\\'))
		sPath += '\\';
	RECTL rectTarget;
	rectTarget.left = 0;
	rectTarget.right = pso->sizlBitmap.cx;
	rectTarget.top = nY;
	rectTarget.bottom = nY + 1;
	HBITMAP hBmp = (HBITMAP)LoadImage(ghInstance, (sPath + _T("CCLogo.bmp")).c_str(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE);
	if (hBmp != NULL)
	{
		if (PrintImage(pdevobj, poempdev, hBmp, rectTarget))
			nY = rectTarget.bottom + nLineHeight;
		::DeleteObject(hBmp);
	}

	// Write hyperlink function...
	PrintPS(pdevobj, poempdev, HYPERLINK_FUNC);
	int nTextHeight;

	CenterText(pdevobj, poempdev, (nFontSize * 3) / 2, 0, nY, pso->sizlBitmap.cx, MakeAnsiString(sLicenseName).c_str());
	std::string sLink = "http://creativecommons.org/licenses/" + sLicenseShortName + "/";
	if (!sVersion.empty())
		sLink += sVersion + "/";
	if (!sJuriShortName.empty())
		sLink += sJuriShortName + "/";
	PrintHyperlink(pdevobj, poempdev, (nFontSize * 3) / 2, MakeAnsiString(sLicenseName).c_str(), sLicenseName.size(), sLink.c_str());

	nY += (nFontSize * 3) / 2;
	nY += nLineHeight;
	
	// Now start writing the text:
	_stprintf(cQuery, _T("SELECT * FROM tblTextOrder INNER JOIN tblText ON tblText.Text_ID = tblTextOrder.textid LEFT JOIN tblImages ON tblText.Imageid = tblImages.ImageID WHERE tblTextOrder.LicenseTypeID = %d AND tblText.Lang = %d ORDER BY textorder"), nLicenseID, nLang);
	SQLite::Recordset setText = db.Query(cQuery);
	if (!setText.IsValid())
	{
		std::tstring s = db.GetLastError();
		return FALSE;
	}
	
	for (int iText = 0; iText < setText.GetRecordCount(); iText++)
	{
		SQLite::Record recText = setText.GetRecord(iText);
		if (recText.GetNumField(_T("header")) == 1)
		{
			nY += PrintText(pdevobj, poempdev, (nFontSize * 5) / 4, nX, nY, pso->sizlBitmap.cx - (2 * nX), (nLineHeight * 5) / 4, MakeAnsiString(recText.GetField(_T("Data"))).c_str());
		}
		else
		{
			int nImageHeight = 0;
			std::tstring sImage = recText.GetField(_T("ImageFile"));
			if (!sImage.empty())
			{
				hBmp = (HBITMAP)LoadImage(ghInstance, (sPath + sImage).c_str(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION|LR_LOADFROMFILE);
				if (hBmp != NULL)
				{
					nY += nLineHeight / 2;
					rectTarget.left = nX;
					rectTarget.top = nY;
					rectTarget.right = nX + nLineHeight * 5;
					rectTarget.bottom = rectTarget.top + 1;
					if (PrintImage(pdevobj, poempdev, hBmp, rectTarget))
						nImageHeight = rectTarget.bottom - nY;
					::DeleteObject(hBmp);
				}
			}
			nTextHeight = PrintText(pdevobj, poempdev, nLineHeight, nX + nLineHeight * 5, nY, pso->sizlBitmap.cx - ((2 * nX) + (nLineHeight * 5)), nLineHeight, MakeAnsiString(recText.GetField(_T("Data"))).c_str());
			if (nImageHeight == 0)
			{
				// Put bullet:
				PrintCircle(pdevobj, poempdev, nX + (nLineHeight * 5 / 2), nY + (nLineHeight / 2), nLineHeight / 4);
			}
			nY += max(nTextHeight, nImageHeight);
		}
		nY += nLineHeight / 2;
	}

	// Bottom of page: put a link to cc pdf converter
	CenterText(pdevobj, poempdev, nFontSize, 0, nHeight - 2 * nLineHeight, pso->sizlBitmap.cx, CREATEDBY_TEXT CREATEDBY_LINK);
	PrintText(pdevobj, poempdev, CREATEDBY_TEXT);
	PrintHyperlink(pdevobj, poempdev, nFontSize, CREATEDBY_LINK, strlen(CREATEDBY_LINK), "http://www.cogniview.com/cc-pdf-converter.php");

	// Don't do the regular page operations...
    return (((PFN_DrvSendPage)(poempdev->pfnPS[UD_DrvSendPage]))(pso));
}
*/

/**
	@brief This function is called when a new page is printed into the PostScript file
	@param pso Pointer to the surface object representing the writing PostScript file
	@return TRUE if written successfully, FALSE if failed to write
*/
BOOL APIENTRY OEMStartPage(SURFOBJ* pso)
{
    PDEVOBJ     pdevobj;
    POEMPDEV    poempdev;

    VERBOSE(DLLTEXT("OEMStartPage() entry.\r\n"));

    pdevobj = (PDEVOBJ)pso->dhpdev;
    poempdev = (POEMPDEV)pdevobj->pdevOEM;
	poempdev->nPage++;

    //
    // turn around to call PS
    //

    return (((PFN_DrvStartPage)(poempdev->pfnPS[UD_DrvStartPage]))(pso));

}

/**
	@brief This function is called when a page has been printed into the PostScript file
	@param pso Pointer to the surface object representing the writing PostScript file
	@return TRUE if written successfully, FALSE if failed to write
*/
BOOL APIENTRY OEMSendPage(SURFOBJ* pso)
{
	
    PDEVOBJ     pdevobj;
    POEMPDEV    poempdev;

    VERBOSE(DLLTEXT("OEMSendPage() entry.\r\n"));

    pdevobj = (PDEVOBJ)pso->dhpdev;
    poempdev = (POEMPDEV)pdevobj->pdevOEM;
	PCOEMDEV pDevMode = (PCOEMDEV)pdevobj->pOEMDM;
	
	/*
	bool bFirstPage = poempdev->nPage == 1;
	LicenseLocation eLocation = LLNone;
	if (bFirstPage)
		eLocation = pDevMode->location.eFirstPage;
	else
	{
		eLocation = pDevMode->location.eOtherPages;
		if (eLocation == LLOther)
			eLocation = pDevMode->location.eFirstPage;
	}

	if (bFirstPage && pDevMode->bSetProperties)
	{
		// Write the license info!
		switch (pDevMode->info.m_eLicense)
		{
			case LicenseInfo::LTPublicDomain:
				// Put a public domain license notice:
				{
					char c[2048];
					sprintf(c, PS_NO_LICENSE_INFO);
					std::string sURL = MakeAnsiString(pDevMode->info.m_cURI);
					AddPSText(c, sURL);
					strcat(c, PS_LICENSE_INFO_END);
					PrintPS(pdevobj, poempdev, c);
				}
				break;
			case LicenseInfo::LTNone:
			case LicenseInfo::LTUnknown:
				// Do nothing!
				break;
			default:
				// Put the license information notice:
				{
					char c[2048];
					sprintf(c, PS_LICENSE_INFO_START);
					std::string sURL = MakeAnsiString(pDevMode->info.m_cURI), sName = MakeAnsiString(pDevMode->info.m_cName);
					AddPSText(c, sURL);
					strcat(c, PS_LICENSE_INFO_CONTINUE);
					AddPSText(c, sName);
					strcat(c, " license ");
					AddPSText(c, sURL);
					strcat(c, PS_LICENSE_INFO_END);
					PrintPS(pdevobj, poempdev, c);
				}
				break;
		}

	}

	if (eLocation != LLNone)
	{
		// Load and use PNG
		UINT uImage = GetLicenseImage(pDevMode->info);
		if (uImage > 0)
		{
			// 1. Load the bitmap
			PngImage png;
			if (png.LoadFromResource(uImage, true, ghInstance))
			{
				// Create the target location
				RECTL rectTarget;
				SIZEL szTarget;
				double dMultiplier = 1.0;
				if (pdevobj->pPublicDM->dmPrintQuality > 0)
					dMultiplier = pdevobj->pPublicDM->dmPrintQuality / 72.0;
				szTarget.cx = png.GetWidth() * dMultiplier;
				szTarget.cy = png.GetHeight() * dMultiplier;

				POINT ptTarget = pDevMode->location.LocationForPage(bFirstPage, pso->sizlBitmap, szTarget);
				rectTarget.left = ptTarget.x;
				rectTarget.top = ptTarget.y;
				rectTarget.right = rectTarget.left + szTarget.cx;
				rectTarget.bottom = rectTarget.top + szTarget.cy;

				DrawImage(pso, png, rectTarget);

				// Make this a link:
				switch (pDevMode->info.m_eLicense)
				{
					case LicenseInfo::LTCC:
					case LicenseInfo::LTSampling:
					case LicenseInfo::LTDevelopingNations:
						PrintJumpLink(pdevobj, poempdev, "TheLicense", rectTarget);
						break;
					default:
						if (pDevMode->info.m_cURI[0] != '\0')
							PrintURLLink(pdevobj, poempdev, MakeAnsiString(&pDevMode->info.m_cURI[0]).c_str(), rectTarget);
						break;
				}
			}
		}
	}
	*/
    //
    // turn around to call PS
    //

    return (((PFN_DrvSendPage)(poempdev->pfnPS[UD_DrvSendPage]))(pso));

}

/**
	@brief This function is called when a print job has started
	@param pso Pointer to the surface object representing the writing PostScript file
	@param pwszDocName Name of the document printed
	@param dwJobId Print job ID
	@return TRUE if started successfully, FALSE if failed
*/
BOOL APIENTRY OEMStartDoc(SURFOBJ* pso, PWSTR pwszDocName,DWORD dwJobId)
{
    PDEVOBJ     pdevobj;
    POEMPDEV    poempdev;

    VERBOSE(DLLTEXT("OEMStartDoc() entry.\r\n"));

    pdevobj = (PDEVOBJ)pso->dhpdev;
    poempdev = (POEMPDEV)pdevobj->pdevOEM;

    //
    // turn around to call PS
    //
	
	if (!(((PFN_DrvStartDoc)(poempdev->pfnPS[UD_DrvStartDoc])) (
            pso,
            pwszDocName,
            dwJobId)))
		return FALSE;
	

	PCOEMDEV pDevMode = (PCOEMDEV)pdevobj->pOEMDM;
	
	std::string sDocFilename = MakeAnsiString(pwszDocName);
	std::string sFilename = MakeAnsiString(pDevMode->cFilename);

	bool bAutoOpen = true;
	HKEY ooKey;
	
	LPTSTR ooSubKey = _T("SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\") MONITOR_NAME _T("\\Ports\\" PORT_NAME);
	DWORD ooPathSize = 512;
	TCHAR PDF_OUTPUT_PATH[512]={0};
	long lresult;

	lresult = RegOpenKey(HKEY_LOCAL_MACHINE, ooSubKey, &ooKey);
	if(lresult == 0){

		lresult = RegQueryValueEx(ooKey, _T("Output"), NULL, NULL, (LPBYTE)PDF_OUTPUT_PATH, &ooPathSize);
		if(lresult == 0 && lstrlen(PDF_OUTPUT_PATH)!=0){			
			bAutoOpen = false;
			sFilename = MakeAnsiString(PDF_OUTPUT_PATH);
		}
		
		RegSetValueEx(ooKey, _T("Document"), 0L, REG_SZ, (CONST BYTE*)pwszDocName, sizeof(pwszDocName) * sizeof(PWSTR));

		::RegCloseKey(ooKey);
	}

	if (!sFilename.empty())
	{
		sFilename = "%%File: " + sFilename + "\r\n";
		DWORD dwResult, dwLen = sFilename.size();

		poempdev->pOEMHelp->DrvWriteSpoolBuf(pdevobj, (LPVOID)sFilename.c_str(), dwLen, &dwResult);
		if (dwResult != dwLen)
			return FALSE;

		// Also write the auto open command, if we should:
		if (pDevMode->bAutoOpen && bAutoOpen)
		{
			sFilename = "%%FileAutoOpen\r\n";
			dwLen = sFilename.size();
			poempdev->pOEMHelp->DrvWriteSpoolBuf(pdevobj, (LPVOID)sFilename.c_str(), dwLen, &dwResult);
			if (dwResult != dwLen)
				return FALSE;

		}
	}

	return TRUE;
}

/**
	@brief This function is called the printing has ended
	@param pso Pointer to the surface object representing the writing PostScript file
	@param fl Status of the print job
	@return TRUE if written successfully, FALSE if failed to write
*/
BOOL APIENTRY OEMEndDoc(SURFOBJ* pso, FLONG fl)
{
    PDEVOBJ     pdevobj;
    POEMPDEV    poempdev;

    VERBOSE(DLLTEXT("OEMEndDoc() entry.\r\n"));

    pdevobj = (PDEVOBJ)pso->dhpdev;
    poempdev = (POEMPDEV)pdevobj->pdevOEM;
	PCOEMDEV pDevMode = (PCOEMDEV)pdevobj->pOEMDM;

	if (fl != ED_ABORTDOC)
	{
		switch (pDevMode->info.m_eLicense)
		{
			case LicenseInfo::LTCC:
			case LicenseInfo::LTSampling:
			case LicenseInfo::LTDevelopingNations:
				// Add another page:
				if (!OEMStartPage(pso))
					return FALSE;				
				/*
				if (!DoLicensePage(pso))
					return FALSE;
					*/
				
				break;
		}
	}

    //
    // turn around to call PS
    //
	
	HKEY ooKey;
	LPTSTR ooSubKey = _T("SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\") MONITOR_NAME _T("\\Ports\\" PORT_NAME);
	long lresult;
	lresult = RegOpenKey(HKEY_LOCAL_MACHINE, ooSubKey, &ooKey);
	if (lresult == 0) {
		RegSetValueEx(ooKey, _T("Pages"), 0L, REG_DWORD, (CONST BYTE*)&poempdev->nPage, sizeof(DWORD));
		::RegCloseKey(ooKey);
	}
	
    return (((PFN_DrvEndDoc)(poempdev->pfnPS[UD_DrvEndDoc])) (
            pso,
            fl));

}
