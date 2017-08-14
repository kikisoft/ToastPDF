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
#include "CCPrintProgressDlg.h"

#include "globals.h"
#include "debug.h"
#include "commctrl.h"

/// ID of progress timer
#define IDT_PROGRESS		0x112

/**
	@param hParent Handle of parent window
	@param request Existing internet request object (to reduce opening connection overhead)
	@param sRequest The request to run
	@param lpServer The server to connect to (can be NULL to connect to use the open connection)
	@param lpOptional Optional headers for the request (can be NULL if none required)
	@param lpHeaders Headers for the request (can be NULL if no special headers needed)
	@return true if the request was completed successfully, false if failed or aborted
*/
bool CCPrintProgressDlg::DoRequest(HWND hParent, InternetRequest& request, const std::tstring& sRequest, LPCTSTR lpServer /* = NULL */, LPCSTR lpOptional /* = NULL */, LPCTSTR lpHeaders /* = NULL */)
{
	// Do we already have a dialog open?
	if (m_hDlg != NULL)
		// Yeah, fail!
		return false;

	// OK, set everyting
	m_bCancel = false;
	m_bSuccess = false;

	m_pRequest = &request;
	m_sRequest = sRequest;
	m_lpServer = lpServer;
	m_lpOptional = lpOptional;
	m_lpHeaders = lpHeaders;

	// Run the modal dialog (actual request will be started when the dialog is displayed)
	DoModal(hParent);

	// When finished, return the flag
	return m_bSuccess;
}

/// Number of steps in the progress bar
#define MAX_STEP		30

/**
	@param uMsg ID of the message
	@param wParam First parameter of the message
	@param lParam Second parameter of the message
	@return TRUE if handled, FALSE to continue handling the message
*/
BOOL CCPrintProgressDlg::PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			{
				// Start the progress:
				// initialize the progress bar
				HWND hProgress = GetDlgItem(IDC_PROGRESS);
				SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, MAX_STEP));
				m_nStep = 0;
				SendMessage(hProgress, PBM_SETPOS, m_nStep, 0);
				// Start the timer
				m_uTimer = SetTimer(m_hDlg, IDT_PROGRESS, 200, NULL);

				// Start the request
				m_pRequest->DoRequestThread(m_sRequest, m_buffer, m_lpServer, m_lpOptional, m_lpHeaders, this);
				return TRUE;
			}
			break;
        case WM_COMMAND:
			// User clicked something!
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
                    switch(LOWORD(wParam))
                    {
						case IDCANCEL:
							// Ah, the cancel button, so cancel
							m_bCancel = true;
							EnableDlgItem(IDCANCEL, FALSE);
							return TRUE;
					}
					break;
			}
			break;
		case WM_TIMER:
			// Is this our timer?
			if (wParam == IDT_PROGRESS)
			{
				// Yeah, move the progress on
				m_nStep++;
				if (m_nStep >= MAX_STEP)
					// Start again if over the edge
					m_nStep = 0;
				SendMessage(GetDlgItem(IDC_PROGRESS), PBM_SETPOS, m_nStep, 0);
			}
			break;
		case WM_DESTROY:
			// We are finished, remove the timer
			if (m_uTimer != 0)
				KillTimer(m_hDlg, m_uTimer);
			break;
	}

	// Call base class for handling
	return CCPrintDlg::PageProc(uMsg, wParam, lParam);
}

/**
	@param bSuccess true if finished successfully, false if failed
*/
void CCPrintProgressDlg::OnFinished(bool bSuccess)
{
	// Remember the flag
	m_bSuccess = bSuccess;
	// Close the dialog...
	EndDialog(m_hDlg, bSuccess ? IDOK : IDCANCEL);
}
