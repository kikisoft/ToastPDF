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
#include "CCPDFPropPage.h"
#include "globals.h"

#include "helpers.h"

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCPDFPropPage::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Which message is this?
    switch (uMsg)
    {
		case WM_INITDIALOG:
			// Page creation:
			// Initialize page controls
			InitControls();
			SetFocus(GetDlgItem(IDC_FILENAME));
			return FALSE;

        case WM_COMMAND:
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
					// Some button/box clicked:
                    switch(LOWORD(wParam))
                    {
                        case IDC_BROWSE:
							// User wants to browse for a filename;
							// Make sure this isn't a Device Properties page (in which case we don't allow this)
							if (!IsDevicePage())
							{
								// OK, get the current filename
								TCHAR cPath[MAX_PATH + 2];
								TCHAR cFilename[MAX_PATH + 2];
								GetDlgItemText(IDC_FILENAME, cFilename, MAX_PATH + 1);
								LPTSTR pName;
								if (GetFullPathName(cFilename, MAX_PATH + 1, cPath, &pName) > 0)
								{
									DWORD dw = GetFileAttributes(cPath);
									if ((dw != ((DWORD)-1)) && (dw & FILE_ATTRIBUTE_DIRECTORY))
										pName = cPath + _tcslen(cPath);
									else if (pName > cPath)
										*(pName - 1) = '\0';
									_tcscpy(cFilename, pName);
								}
								else
								{
									cFilename[0] = '\0';
									cPath[0] = '\0';
								}

								// Fill up the Open File structure
								OPENFILENAME info;
								memset(&info, 0, sizeof(info));
								info.lStructSize = sizeof(info);
								info.hInstance = ghInstance;
								info.hwndOwner = m_hDlg;
								info.lpstrFilter = _T("PDF Files (*.pdf)\0*.pdf\0All Files (*.*)\0*.*\0\0");
								info.lpstrInitialDir = cPath;
								info.lpstrFile = cFilename;
								info.nMaxFile = MAX_PATH + 1;
								info.lpstrTitle = _T("Select a filename to write into");
								info.Flags = OFN_ENABLESIZING|OFN_EXPLORER|OFN_NOREADONLYRETURN|OFN_PATHMUSTEXIST;
								info.lpstrDefExt = _T("pdf");

								// Display the dialog:
								if (GetSaveFileName(&info))
								{
									// OK, get the data
									GetDlgItemText(IDC_FILENAME, cPath, MAX_PATH + 1);
									SetDlgItemText(IDC_FILENAME, cFilename);
									// Is it valid?
									if (!TestValidFilename())
										// No, reset to previous data
										SetDlgItemText(IDC_FILENAME, cPath);
									else
									{
										// Yes, set up in the page
										SetDlgItemText(IDC_FILENAME, m_sPath.c_str());
										SetChanged();
									}
								}
							}
							break;
						case IDC_AUTOOPEN:
							// User changed the open-on-print option
							SetChanged();
							break;
                    }
                    break;

				case EN_CHANGE:
					// Edit box changed
					if (LOWORD(wParam) == IDC_FILENAME)
					{
						// This is the file name box, so check it
						TCHAR cPath[MAX_PATH + 2];
						if (GetDlgItemText(IDC_FILENAME, cPath, MAX_PATH + 1) != 0)
						{
							// Is it the same as we already got?
							if (_tcsicmp(cPath, m_pDevMode->cFilename) == 0)
								// Yes, ignore the change
								break;
						}
						SetChanged();
					}
					break;

                default:
					// Nothing we can handle
                    return FALSE;
            }
			// We got here, meaning we handled it
            return TRUE;

        case WM_NOTIFY:
            {
				// Notification, what is it?
                switch (((LPNMHDR)lParam)->code)  // type of notification message
                {
                    case PSN_SETACTIVE:
						// Page made active, reload the data
						InitControls();
                        break;
    
                    case PSN_KILLACTIVE:
						// Check and see if we have a valid filename
						if (!TestValidFilename())
						{
							// Not valid, so stop the page change

#ifdef _WIN64
							SetWindowLong(DWLP_MSGRESULT, TRUE);
#else
							SetWindowLong(DWL_MSGRESULT, TRUE);
#endif
							
							return TRUE;
						}
						// OK, get the data
						_tcscpy(m_pDevMode->cFilename, m_sPath.c_str());
						m_pDevMode->bAutoOpen = GetDlgItemCheck(IDC_AUTOOPEN);
						return TRUE;

                    case PSN_APPLY:
						// Apply, we want to save the data
						if (!TestValidFilename())
						{
							// Not valid, we want to make sure this does NOT apply!
							(*m_pfnComPropSheet)(m_hComPropSheet, CPSFUNC_SET_RESULT, (LPARAM)m_hPage, CPSUI_CANCEL);
						}
						else
						{
							// OK, copy the data
							_tcscpy(m_pDevMode->cFilename, m_sPath.c_str());
							m_pDevMode->bAutoOpen = GetDlgItemCheck(IDC_AUTOOPEN);
							// Notify the page handle that it applies
							(*m_pfnComPropSheet)(m_hComPropSheet, CPSFUNC_SET_RESULT, (LPARAM)m_hPage, CPSUI_OK);
						}
						// Handled here
						return TRUE;

                    case PSN_RESET:
						// Move along, nothing to see here
                        break;
                }
            }
            break;
	}

	// Got here: could not handle it
	return FALSE;
}

/**
	@return TRUE if the file name in the edit box is valid, FALSE otherwise
*/
BOOL CCPDFPropPage::TestValidFilename()
{
	// Get the data from the box
	TCHAR cPath[MAX_PATH + 2];
	if (GetDlgItemText(IDC_FILENAME, cPath, MAX_PATH + 1) == 0)
	{
		// We allow empty filename, the user will be asked by the PDF creation application later...
		m_sPath = _T("");
		return TRUE;
	}

	// OK, do we have an extension here?
	TCHAR* pPos = _tcsrchr(cPath, '.');
	if (pPos != NULL)
	{
		TCHAR* pSlash = _tcsrchr(cPath, '\\');
		if ((pSlash != NULL) && (pSlash > pPos))
			pPos = NULL;
	}
	if (pPos == NULL)
		// No, add a .pdf to the filename
		_tcscat(cPath, _T(".pdf"));

	// Is this the same as we already got?
	if (_tcsicmp(m_sPath.c_str(), cPath) == 0)
		// Yes, keep it
		return TRUE;

	// Try and find if the file exists
	DWORD dw = GetFileAttributes(cPath);
	if (dw == 0xFFFFFFFF)
	{
		// Filename doesn't exist, go make sure the PATH is valid
		pPos = _tcsrchr(cPath, '\\');
		if (pPos != NULL)
		{
			*pPos = '\0';
			dw = GetFileAttributes(cPath);
			if (dw == 0xFFFFFFFF)
			{
				MessageBox(_T("Directory not found"), _T("Filename error"), MB_OK|MB_ICONERROR);
				return FALSE;
			}
			if ((dw & FILE_ATTRIBUTE_READONLY) != 0)
			{
				MessageBox(_T("Cannot write to directory"), _T("Filename error"), MB_OK|MB_ICONERROR);
				return FALSE;
			}
			if ((dw & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				MessageBox(_T("Invalid directory"), _T("Filename error"), MB_OK|MB_ICONERROR);
				return FALSE;
			}

			// It's OK
			*pPos = '\\';
			m_sPath = cPath;
			return TRUE;
		}
		else
		{
			MessageBox(_T("Invalid filename"), _T("Filename error"), MB_OK|MB_ICONERROR);
			return FALSE;
		}
	}

	// This exists, is it a file?
	if ((dw & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		// Nope
		MessageBox(_T("Please enter a filename"), _T("Filename error"), MB_OK|MB_ICONERROR);
		return FALSE;
	}

	if ((dw & FILE_ATTRIBUTE_READONLY) != 0)
	{
		// It's read only, can't save on it
		MessageBox(_T("Cannot replace file"), _T("Filename error"), MB_OK|MB_ICONERROR);
		return FALSE;
	}

	// File exist, ask to replace:
	if (MessageBox(_T("File exists\nDo you wish to replace it?"), _T("Filename"), MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2) != IDYES)
		return FALSE;

	// OK, set it up and return
	m_sPath = cPath;
	return TRUE;
}

/**
	
*/
void CCPDFPropPage::InitControls()
{
	// Get the path from the data structure
	m_sPath = m_pDevMode->cFilename;
	// And set the page controls
	SetDlgItemText(IDC_FILENAME, m_sPath.c_str());
	SetDlgItemCheck(IDC_AUTOOPEN, m_pDevMode->bAutoOpen);

	// Do we have a handle for PDF files?
	if (!CanOpenPDFFiles())
	{
		// No, so disable the option to auto-open them
		SetDlgItemCheck(IDC_AUTOOPEN, FALSE);
		EnableDlgItem(IDC_AUTOOPEN, FALSE);
	}
}
