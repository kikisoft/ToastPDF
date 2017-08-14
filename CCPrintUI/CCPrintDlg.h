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

#ifndef _CCPRINTDLG_H_
#define _CCPRINTDLG_H_

#include "CCTChar.h"

/**
    @brief Base class for Print UI dialogs and pages
*/
class CCPrintDlg
{
public:
	// Ctors
	/**
		@brief Constructor
		@param uResourceID Resource ID of the dialog or page
	*/
	CCPrintDlg(UINT uResourceID) : m_uResourceID(uResourceID), m_hDlg(NULL) {};

protected:
	// Members
	/// Resource ID
	UINT		m_uResourceID;
	/// Handle of dialog (valid only while running)
	HWND		m_hDlg;

protected:
	// Overridables
	/// Called when a windows message is sent to the page
	virtual BOOL PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	/**
		@brief Called after the last message was sent to the window and it is closed
	*/
	virtual void PostDestroy() {};

public:
	// Public overridables
	/// Runs the dialog
	virtual int DoModal(HWND hParent);

protected:
	// Helpers
	/**
		@brief Returns the window handle of a control by the control ID
		@param uID The control ID
		@return The window handle
	*/
	HWND		GetDlgItem(UINT uID) {return ::GetDlgItem(m_hDlg, uID);};
	/**
		@brief Enables or disables a control by the control ID
		@param uID The control ID
		@param bEnable TRUE to enable the control, FALSE to disable it
	*/
	void		EnableDlgItem(UINT uID, BOOL bEnable) {EnableWindow(GetDlgItem(uID), bEnable);};

	/**
		@brief Retrieves the text of a control in the dialog
		@param uID The ID of the control
		@param lpString String to put the text in
		@param nMaxSize Length of the string
		@return The number of characters copied to the string, or 0 if failed
	*/
	UINT		GetDlgItemText(UINT uID, LPTSTR lpString, int nMaxSize) {return ::GetDlgItemText(m_hDlg, uID, lpString, nMaxSize);};
	/**
		@brief Sets the text of a control in the dialog
		@param uID ID of the control
		@param lpString The text to put in the control
	*/
	void		SetDlgItemText(UINT uID, LPCTSTR lpString) {::SetDlgItemText(m_hDlg, uID, lpString);};

	/// Retrieves the check status of a check box control
	BOOL		GetDlgItemCheck(UINT uID);
	/// Sets or clears the check mark in a check box control
	void		SetDlgItemCheck(UINT uID, BOOL bCheck);

	/// Retrieves which radio button is selected in a range of radio button controls
	int			GetCheckedRadioBtn(UINT uIDStart, UINT uIDEnd);

	/// Retrieves the number of items in a combo box control
	int			GetDlgComboCount(UINT uID);
	/// Retrieves the index of the selected item in a combo box control (-1 if none selected)
	int			GetDlgComboSel(UINT uID);
	/// Retrieves the Data associated with the selected item in a combo box control (0 if none selected)
	DWORD		GetDlgComboSelData(UINT uID);
	/// Retrieves the text of the currently selected item of a combo box control
	std::tstring GetDlgComboText(UINT uID);
	/// Sets the selection of a combo box control
	void		SetDlgComboItem(UINT uID, int nItem);
	/// Sets the selection of a combo box control by the item's Data
	void		SetDlgComboSel(UINT uID, DWORD dwData);
	/// Sets the selection of a combo box control by the item's text
	void		SetDlgComboSel(UINT uID, LPCTSTR lpString);
	/// Adds an item to a combo box control using a resource string
	void		AddDlgComboString(UINT uID, UINT uStringID, DWORD dwData);
	/// Adds an item to a combo box control
	void		AddDlgComboString(UINT uID, LPCTSTR lpString, DWORD dwData);
	/// Removes all items from a combo box control
	void		CleanDlgCombo(UINT uID);

	/**
		@brief Displays a message box
		@param lpText The message box text
		@param lpCaption The message box caption
		@param uType The message box type (buttons, icon)
		@return The return value (which button was pressed)
	*/
	int			MessageBox(LPCTSTR lpText, LPCTSTR lpCaption, UINT uType) {return ::MessageBox(m_hDlg, lpText, lpCaption, uType);};

	/**
		@brief Sets the value of one of the dialog box's Windows attributes; see SetWindowLong in MSDN for more information
		@param nIndex The index of the attribute
		@param dwNewLong The new value
	*/
	void		SetWindowLong(int nIndex, LONG dwNewLong) {::SetWindowLong(m_hDlg, nIndex, dwNewLong);};
	/**
		@brief Retrieves the value of one of the dialog box's Windows attributes; see GetWindowLong in MSDN for more information
		@param nIndex The index of the attributes
		@return The value of the attribute
	*/
	DWORD		GetWindowLong(int nIndex) {return ::GetWindowLong(m_hDlg, nIndex);};

	// Callbacks
	/// Called by Windows when the dialog box receives a windows message
	
	static INT_PTR APIENTRY StaticDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

};

#endif   //#define _CCPRINTDLG_H_
