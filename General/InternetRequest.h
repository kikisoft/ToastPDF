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

#ifndef __INTERNETREQUEST_H__
#define __INTERNETREQUEST_H__

#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include "CCTChar.h"

#include "ProgressInterface.h"

/**
    @brief Self-resizing buffer for internet requests data
*/
class InternetRequestBuffer
{
public:
	// Ctor/Dtor
	/**
		@brief Default constructor; Initializes variables
	*/
	InternetRequestBuffer() : m_pData(NULL), m_dwSize(0), m_dwRealSize(0) {};
	/**
		@brief Deletes the buffer
	*/
	~InternetRequestBuffer() {if (m_pData != NULL) delete [] m_pData;};

protected:
	// Members
	/// Pointer to the data
	char*			m_pData;
	/// Size of data held in the buffer
	unsigned int	m_dwSize;
	/// Size of buffer (same or larger then m_dwSize)
	unsigned int	m_dwRealSize;

public:
	// Methods
	/// Add data to the buffer, increasing the size if necessary
	bool			AddData(const char* pBuffer, unsigned int dwSize);
	/**
		@brief Retrieves a pointer to the data, including the size
		@param dwSize Set to hold the size of the data (in bytes) upon return
		@return Pointer to the data
	*/
	const char*		GetData(unsigned int& dwSize) {dwSize = m_dwSize; return m_pData;};
	/**
		@brief Cleans the buffer (but doesn't resize it)
	*/
	void			Clear() {m_dwSize = 0;};
};

/**
    @brief Class for requesting data from an internet server via HTTP
*/
class InternetRequest
{
public:
	// Ctor/Dtor
	/// Constructor
	InternetRequest();
	/// Destructor
	~InternetRequest();

protected:
	// Definitions
	/**
	    @brief Structure for holding request information; used mostly for requests on a worker thread
	*/
	struct RequestData
	{
	public:
		/**
			@brief Constructor
			@param pThis Pointer to the InternetRequest object that will handle the request
			@param sRequest The page requested from the server
			@param pProgress Pointer to the progress interface
			@param buffer Buffer object to fill with the request response
			@param lpServer Server name (can be NULL)
			@param lpOptional Additional data for POST (can be NULL)
			@param lpHeaders Additional headers (can be NULL)
		*/
		RequestData(InternetRequest* pThis, const std::tstring& sRequest, ProgressInterface* pProgress, InternetRequestBuffer& buffer, LPCTSTR lpServer = NULL, LPCSTR lpOptional = NULL, LPCTSTR lpHeaders = NULL)
			: m_pThis(pThis), m_sRequest(sRequest), m_pProgress(pProgress), m_pBuffer(&buffer), m_sServer(lpServer == NULL ? _T("") : lpServer), m_sOptional(lpOptional == NULL ? "" : lpOptional), m_sHeaders(lpHeaders == NULL ? _T("") : lpHeaders) 
			{};
	public:
		/// Pointer to the InternetRequest object that will handle the request
		InternetRequest*		m_pThis;
		/// The page requested from the server
		std::tstring			m_sRequest;
		/// Pointer to the progress interface
		ProgressInterface*		m_pProgress;
		/// Pointer to the buffer object to fill with the request response
		InternetRequestBuffer*	m_pBuffer;
		/// Server name (can be NULL)
		std::tstring			m_sServer;
		/// Additional data for POST (can be NULL)
		std::string				m_sOptional;
		/// Additional headers (can be NULL)
		std::tstring			m_sHeaders;
	};

protected:
	// Members
	/// The wininet session object
	HINTERNET		m_hSession;
	/// The wininet connection object (reused for each different server)
	HINTERNET		m_hConnection;
	/// The wininet current request (reused for each request)
	HINTERNET		m_hRequest;
	/// true if currently in a request, false if not
	bool			m_bInRequest;
	/// true while creating a connection, false if not
	bool			m_bInCreation;
	/// Name of server used for the current connection
	std::tstring	m_sServer;
	/// Last error for the request
	std::tstring	m_sError;

public:
	// Data Access
	/**
		@brief Retrieves the last error string
		@return The last error string
	*/
	const std::tstring& GetError() {return m_sError;};

public:
	// Methods
	/// Runs an internet request and returns when it is finished
	bool			DoRequest(const std::tstring& sRequest, InternetRequestBuffer& buffer, LPCTSTR lpServer = NULL, LPCSTR lpOptional = NULL, LPCTSTR lpHeaders = NULL, ProgressInterface* pProgress = &gpiDefault);
	/// Runs an internet request in a worker thread and returns immediatly
	bool			DoRequestThread(const std::tstring& sRequest, InternetRequestBuffer& buffer, LPCTSTR lpServer = NULL, LPCSTR lpOptional = NULL, LPCTSTR lpHeaders = NULL, ProgressInterface* pProgress = &gpiDefault);

protected:
	// Helpers
	/// Called when the request changes status
	void			OnInternetCallback(DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	/// Opens a wininet session
	bool			OpenSession();
	/// Closes all wininet handles
	void			CloseAll();
	/// Runs the request and calles the progress interface when it is finished
	bool			DoRequest(RequestData* pData);
	/// Runs the request (does the actual work)
	bool			InnerDoRequest(RequestData* pData);

protected:
	// Static callbacks
	/// Callback for wininet status changes
	static void __stdcall InternetCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	/// The request worker thread main function
	static DWORD WINAPI RequestThreadProc(LPVOID lpData);
};


#endif /* __INTERNETREQUEST_H__ */