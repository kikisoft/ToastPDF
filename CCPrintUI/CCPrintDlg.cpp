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
#include "CCPrintDlg.h"

#include "globals.h"
#include "debug.h"


/**
@param hDlg Handle to the dialog
@param uMsg ID of the message
@param wParam First parameter of the message
@param lParam Second parameter of the message
@return TRUE if handled, FALSE to continue handling the message
*/
INT_PTR APIENTRY CCPrintDlg::StaticDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Retrieve the object associated with this window
	CCPrintDlg* pThis = (CCPrintDlg*) ::GetWindowLong(hDlg, DWLP_USER);
	// Is this the initial message?
	if ((uMsg == WM_INITDIALOG) && (pThis == NULL))
	{
		// Yeah, we get this information from the lParam instead, so set it up:
		pThis = (CCPrintDlg*)lParam;
		pThis->m_hDlg = hDlg;
		pThis->SetWindowLong(DWLP_USER, (LPARAM)pThis);
	}
	// Do we have an associated object?
	if (pThis == NULL)
	{
		// Not connected yet, this can happen with a few messages
		switch (uMsg)
		{
		case WM_SETFONT:
		case WM_NOTIFYFORMAT:
		case WM_QUERYUISTATE:
			return FALSE;
		default:
			// This shouldn't happen!
			ASSERT(FALSE);
			return FALSE;
		}
	}
	else
	{
		// If we got here, we should ALWAYS have the proper object connected!
		ASSERT(pThis->m_hDlg == hDlg);
	}

	// Call the overridable handler function
	BOOL bRet = pThis->PageProc(uMsg, wParam, lParam);

	// Is this the last message?
	if (uMsg == WM_NCDESTROY)
	{
		// Yes, disconnect the dialog
		pThis->m_hDlg = NULL;
		// And call the overriable handler for post destruction
		pThis->PostDestroy();
	}

	// This is it
	return bRet;
}

/**
	@param uID ID of control to check
	@return TRUE if the check box is checked, FALSE if it's cleared
*/
BOOL CCPrintDlg::GetDlgItemCheck(UINT uID)
{
	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "GetDlgItemCheck Error: check box not found");
		return FALSE;
	}
	// OK, get the status
	return (SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED);
}

/**
	@param uID ID of the control to set
	@param bCheck TRUE to set the check mark, FALSE to clear it
*/
void CCPrintDlg::SetDlgItemCheck(UINT uID, BOOL bCheck)
{
	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "SetDlgItemCheck Error: check box not found");
		return;
	}
	// OK, set it
	SendMessage(hWnd, BM_SETCHECK, bCheck ? BST_CHECKED : BST_UNCHECKED , 0);
}

/**
	@param uID ID of the control to query
	@return The number of items in the control
*/
int CCPrintDlg::GetDlgComboCount(UINT uID)
{
	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "GetDlgComboCount Error: combo box not found");
		return FALSE;
	}
	// OK, get the data
	return SendMessage(hWnd, CB_GETCOUNT, 0, 0);
}

/**
	@param uID ID of the control to query
	@return 
*/
int CCPrintDlg::GetDlgComboSel(UINT uID)
{
	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "GetDlgComboSel Error: combo box not found");
		return FALSE;
	}
	// OK, retrieve the data
	return SendMessage(hWnd, CB_GETCURSEL, 0, 0);
}

/**
	@param uID ID of the control to query
	@return The data of the selected item (0 if not found)
*/
DWORD CCPrintDlg::GetDlgComboSelData(UINT uID)
{
	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "GetDlgComboSelData Error: combo box not found");
		return FALSE;
	}
	// Get the current selection
	int nSel = (int)SendMessage(hWnd, CB_GETCURSEL, 0, 0);
	if (nSel == CB_ERR)
		// None, return 0
		return 0;

	// OK, get it
	return SendMessage(hWnd, CB_GETITEMDATA, nSel, 0);
}

/**
	@param uID ID of combo box control to query
	@return The text of the combo box
*/
std::tstring CCPrintDlg::GetDlgComboText(UINT uID)
{
	std::tstring sRet;

	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "GetDlgComboSelData Error: combo box not found");
		return sRet;
	}
	// Get the selection
	int nSel = (int)SendMessage(hWnd, CB_GETCURSEL, 0, 0);
	if (nSel == CB_ERR)
		// No selection, return empty string
		return sRet;

	// Get the text's length
	int nLen = (int)SendMessage(hWnd, CB_GETLBTEXTLEN, nSel, 0);
	if (nLen < 1)
		// Nothing
		return sRet;

	// Get the text itself
	TCHAR* pStr = new TCHAR[nLen + 1];
	if (SendMessage(hWnd, CB_GETLBTEXT, nSel, (LPARAM)pStr) != nLen)
	{
		// Bad, bad text!
		delete [] pStr;
		return sRet;
	}

	// OK, got it
	sRet.assign(pStr, nLen);
	delete [] pStr;
	return sRet;
}

/**
	@param uID ID of the combo box control to set
	@param nItem Index of the item to select
*/
void CCPrintDlg::SetDlgComboItem(UINT uID, int nItem)
{
	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "SetDlgComboItem Error: combo box not found");
		return;
	}
	// Select it
	SendMessage(hWnd, CB_SETCURSEL, nItem, 0);
}

/**
	@param uID ID of the combo box control to set
	@param dwData Data of the item to select
*/
void CCPrintDlg::SetDlgComboSel(UINT uID, DWORD dwData)
{
	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "SetDlgComboSel Error: combo box not found");
		return;
	}
	// Get count of items
	int nCount = SendMessage(hWnd, CB_GETCOUNT, 0, 0);
	if (nCount == CB_ERR)
		// No items
		return;

	// Find the item with the data
	for (int i = 0; i < nCount; i++)
	{
		// Is this it?
		if (dwData == (DWORD)SendMessage(hWnd, CB_GETITEMDATA, i, 0))
		{
			// Yeah, select it
			SendMessage(hWnd, CB_SETCURSEL, i, 0);
			return;
		}
	}
}

/**
	@param uID ID of combo box control to set
	@param lpString The text of the item to select
*/
void CCPrintDlg::SetDlgComboSel(UINT uID, LPCTSTR lpString)
{
	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "SetDlgComboSel Error: combo box not found");
		return;
	}
	// Get item count
	int nCount = SendMessage(hWnd, CB_GETCOUNT, 0, 0);
	if (nCount == CB_ERR)
		// No items
		return;
	int nLen = 64, nCurrentLen;
	TCHAR* pStr = new TCHAR[nLen];
	// Find the item with the text
	int i;
	for (i = 0; i < nCount; i++)
	{
		// Get the length of the text
		nCurrentLen = (int)SendMessage(hWnd, CB_GETLBTEXTLEN, i, 0) + 1;
		if (nCurrentLen < 2)
			// Nothing to get
			continue;
		// Do we need to make the buffer longer?
		if (nCurrentLen > nLen)
		{
			// Yeah, multiple it until we have enough room
			while (nLen < nCurrentLen)
				nLen *= 2;
			// Replace it
			delete [] pStr;
			pStr = new TCHAR[nLen];
		}
		// Get the text
		if (SendMessage(hWnd, CB_GETLBTEXT, i, (LPARAM)pStr) != nCurrentLen - 1)
			// Error!
			continue;

		// OK, compare
		pStr[nCurrentLen - 1] = '\0';
		if (_tcscmp(pStr, lpString) == 0)
			// Found it!
			break;
	}
	delete [] pStr;
	if (i == nCount)
		// Not found
		return;
	// Select it
	SendMessage(hWnd, CB_SETCURSEL, i, 0);
}

/**
	@param uID ID of the combo box control to add the item to
	@param uStringID Resource ID of the text string
	@param dwData The data to set for the new item
*/
void CCPrintDlg::AddDlgComboString(UINT uID, UINT uStringID, DWORD dwData)
{
	// Load the string
	std::tstring sStr = LoadResourceString(uStringID);
	if (sStr.empty())
		// Empty, do nothing
		return;

	// Add it
	AddDlgComboString(uID, sStr.c_str(), dwData);
}

/**
	@param uID ID of the combo box control to add the item to
	@param lpString The text of the new item
	@param dwData The data of the new item
*/
void CCPrintDlg::AddDlgComboString(UINT uID, LPCTSTR lpString, DWORD dwData)
{
	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "AddDlgComboString Error: combo box not found");
		return;
	}
	// Add the item
	int nItem = SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)lpString);
	if (nItem == CB_ERR)
	{
		// Failed!
		ASSERTMSG(FALSE, "AddDlgComboString Error: could not add string");
		return;
	}
	// Set the item's data
	if (dwData != 0)
		SendMessage(hWnd, CB_SETITEMDATA, nItem, dwData);
}

/**
	@param uID ID of the combo box to clear
*/
void CCPrintDlg::CleanDlgCombo(UINT uID)
{
	// Get the window handle of the control
	HWND hWnd = GetDlgItem(uID);
	if (hWnd == NULL)
	{
		// Not found!
		ASSERTMSG(FALSE, "AddDlgComboString Error: combo box not found");
		return;
	}
	// Clear it!
	SendMessage(hWnd, CB_RESETCONTENT, 0, 0);
}

/**
	@param uIDStart ID of the first radio button control in the set
	@param uIDEnd ID of the last radio button control in the set
	@return The number of the selected radio button (or -1 if none are selected)
*/
int CCPrintDlg::GetCheckedRadioBtn(UINT uIDStart, UINT uIDEnd)
{
	// Make sure we are going in the right direction
	ASSERT(uIDStart <= uIDEnd);
	// We'll check them all
	for (UINT uID = uIDStart; uID <= uIDEnd; uID++)
	{
		// Until one is found selected
		if (IsDlgButtonChecked(m_hDlg, uID))
			// OK, return its number
			return uID - uIDStart;
	}

	// None selected
	return -1;
}

/**
	@param hParent Handle of parent window
	@return The exit code (see DialogBox in MSDN)
*/
int CCPrintDlg::DoModal(HWND hParent)
{
	return (int)DialogBoxParam(ghInstance, MAKEINTRESOURCE(m_uResourceID), hParent, StaticDlgProc, (LPARAM)this);
}

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCPrintDlg::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_COMMAND:
			// User clicked something
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
					// It's a button!
                    switch(LOWORD(wParam))
                    {
						case IDCANCEL:
						case IDOK:
							// Default handling of OK and Cancel buttons is to close the dialog with the appropriate exit code
							EndDialog(m_hDlg, LOWORD(wParam));
							return TRUE;
					}
					break;
			}
			break;
	}

	// We don't handle anything else
	return FALSE;
}
