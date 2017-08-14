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

#ifndef _CCPRINTPROGRESSDLG_H_
#define _CCPRINTPROGRESSDLG_H_

#include "ProgressInterface.h"
#include "CCPrintDlg.h"
#include "InternetRequest.h"
#include "resource.h"

class InternetRequest;

/**
    @brief Internet request progress dialog class
*/
class CCPrintProgressDlg : public CCPrintDlg, public ProgressInterface
{
public:
	// Ctor/Dtor
	/**
		@brief Constructor
	*/
	CCPrintProgressDlg() : CCPrintDlg(IDD_PROGRESS), m_bCancel(false), m_bSuccess(false), m_uTimer(0), m_nStep(0) {};
	/**
		@brief Destructor
	*/
	~CCPrintProgressDlg() {};

protected:
	// Members
	/// Cancel flag
	bool			m_bCancel;
	/// Progresss data
	int				m_nStep;
	/// Timer
	UINT			m_uTimer;
	/// true if the request succeeded, false if failed
	bool			m_bSuccess;

	// InternetRequest data:
	/// Pointer to the internet request object
	InternetRequest* m_pRequest;
	/// The request itself
	std::tstring	m_sRequest;
	/// Name of the server to connect to
	LPCTSTR			m_lpServer;
	/// Additional data to request (ANSI only!)
	LPCSTR			m_lpOptional;
	/// Additional headers
	LPCTSTR			m_lpHeaders;

public:
	// Public members
	/// Buffer to fill with the request results
	InternetRequestBuffer m_buffer;

public:
	// Methods
	/// Runs an internet request (displaying the dialog); this is a blocking function (will only return when the request is finished)
	bool			DoRequest(HWND hParent, InternetRequest& request, const std::tstring& sRequest, LPCTSTR lpServer = NULL, LPCSTR lpOptional = NULL, LPCTSTR lpHeaders = NULL);

protected:
	// CCPrintDlg overrides
	/// Called when a windows message is sent to the page
	virtual BOOL	PageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	// ProgressInterface Overrides
	/**
		@brief Called to test if the operation is to be canceled
		@return true to cancel the operation, false to continue (default)
	*/
	virtual bool	GetCancel() {return m_bCancel;};
	/**
		@brief Called when the operation is finished
		@param bSuccess true if finished successfully, false if failed (or canceled by the user)
	*/
	virtual void	OnFinished(bool bSuccess);
};

#endif   //#define _CCPRINTPROGRESSDLG_H_
