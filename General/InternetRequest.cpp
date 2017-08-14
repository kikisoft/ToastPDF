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

#include "InternetRequest.h"

/// Initial size of buffer
#define REQUESTBUFFER_MINSIZE		1024


/**
	@param pBuffer Pointer to the data to add to the buffer
	@param dwSize Size of data to add
	@return true if added successfully, false if buffer is too large to be allocated
*/
bool InternetRequestBuffer::AddData(const char* pBuffer, unsigned int dwSize)
{
	// Should we increase the buffer size?
	if (m_dwSize + dwSize > m_dwRealSize)
	{
		// Yeah, calculate the new size (it's ALWAYS a multiplication of the initial buffer size, so we won't enlarge it too often)
		unsigned int dwNewSize = (m_dwRealSize == 0) ? REQUESTBUFFER_MINSIZE : m_dwRealSize * 2;
		while (dwNewSize < m_dwSize + dwSize)
			dwNewSize *= 2;

		// Create the new buffer
		char* pNewBuffer = new char[dwNewSize];
		if (pNewBuffer == NULL)
			// Error!
			return false;
		// Copy current data if any
		if (m_dwSize > 0)
			memcpy(pNewBuffer, m_pData, m_dwSize);

		// Set the size
		m_dwRealSize = dwNewSize;

		// Delete old buffer and set the new
		delete [] m_pData;
		m_pData = pNewBuffer;
	}

	// Add the new data to the end of the current buffer data
	memcpy(m_pData + m_dwSize, pBuffer, dwSize);
	// And set the size
	m_dwSize += dwSize;
	return true;
}





/**
	
*/
InternetRequest::InternetRequest() : m_hSession(NULL), m_hConnection(NULL), m_hRequest(NULL),
	m_bInRequest(false), m_bInCreation(false)
{
}

/**
	
*/
InternetRequest::~InternetRequest()
{
	// Close the wininet handles and clean up
	CloseAll();
}

/**
	
*/
void InternetRequest::CloseAll()
{
	// We don't want to crash because of wininet, so we wrap the code with a try/catch
	try
	{
		// Do we have a session?
		if (m_hSession != NULL)
		{
			// Remove the status callback if we have it
			if (m_hRequest != NULL)
				::InternetSetStatusCallback(m_hRequest, NULL);
			if (m_hConnection != NULL)
				::InternetSetStatusCallback(m_hConnection, NULL);
			::InternetSetStatusCallback(m_hSession, NULL);
		}

		// Do we have a current request running?
		if (m_hRequest != NULL)
		{
			// Close the request
			InternetCloseHandle(m_hRequest);
			m_hRequest = NULL;
		}
		// Do we have a connection?
		if (m_hConnection != NULL)
		{
			// Close it
			InternetCloseHandle(m_hConnection);
			m_hConnection = NULL;
		}
		// Do we have a session?
		if (m_hSession != NULL)
		{
			// Close that too
			InternetCloseHandle(m_hSession);
			m_hSession = NULL;
		}
	}
	catch(...)
	{
	}
}

/**
	@return true if a wininet session was initialized successfully, false if failed
*/
bool InternetRequest::OpenSession()
{
	// Close existing session
	CloseAll();

	try
	{
		// Start the session
		m_hSession = ::InternetOpen(_T("CogniView CCInfo/0.1"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (m_hSession == NULL)
		{
			// Error
			m_sError = _T("Cannot connect to the internet");
			return false;
		}

		// Set status callback
		if (::InternetSetStatusCallback(m_hSession, InternetCallback) == INTERNET_INVALID_STATUS_CALLBACK)
		{
			// Error
			m_sError = _T("Cannot set connection callback");
			return false;
		}
	}
	catch (...)
	{
		return false;
	}

	// All is well
	return true;
}

/**
	@param hInternet Handle to the wininet object that requested the callback
	@param dwContext Client-defined data (the pointer to the InternetRequest object, in our case)
	@param dwInternetStatus Callback stauts
	@param lpvStatusInformation Pointer to additional callback information
	@param dwStatusInformationLength Size of information object
*/
void __stdcall InternetRequest::InternetCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	// Get the request object
	InternetRequest* pRequest = (InternetRequest*)dwContext;
	// Use the callback
	pRequest->OnInternetCallback(dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
}

/**
	@param dwInternetStatus Callback status
	@param lpvStatusInformation Pointer to additional callback information
	@param dwStatusInformationLength Size of information object
*/
void InternetRequest::OnInternetCallback(DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	// What status?
	switch (dwInternetStatus)
	{
		case INTERNET_STATUS_HANDLE_CREATED:
			// Created connection/request handle
			m_bInCreation = false;
			return;
		case INTERNET_STATUS_HANDLE_CLOSING:
			// Closed the connecton/request handle
			m_bInRequest = false;
			m_bInCreation = false;
			return;
		case INTERNET_STATUS_REQUEST_COMPLETE:
			// Request finished - for ASYNC requests (which we don't actually do yet)
			if (dwStatusInformationLength < sizeof(INTERNET_ASYNC_RESULT))
			{
				m_sError = _T("Error while receving information");
				m_bInRequest = false;
			}
			else
			{
				LPINTERNET_ASYNC_RESULT pResult = (LPINTERNET_ASYNC_RESULT)lpvStatusInformation;
				if (pResult->dwError != ERROR_SUCCESS)
				{
					TCHAR c[128];
					_stprintf(c, _T("Receiving information error %d"), pResult->dwError);
					m_sError = c;
				}
				m_bInRequest = false;
			}
			return;
	}
}

/**
	@param sRequest The page requested from the server
	@param buffer Buffer object to fill with the request response
	@param lpServer Server name (can be NULL; if so, will use the current connection if any or fail if there is none)
	@param lpOptional Additional data for POST (can be NULL)
	@param lpHeaders Additional headers (can be NULL)
	@param pProgress Pointer to the progress interface (can be NULL)
	@return true if the request was successful, false if failed
*/
bool InternetRequest::DoRequest(const std::tstring& sRequest, InternetRequestBuffer& buffer, LPCTSTR lpServer /* = NULL */, LPCSTR lpOptional /* = NULL */, LPCTSTR lpHeaders /* = NULL */, ProgressInterface* pProgress /* = &piDefault */)
{
	// Create a data structure for the request information
	RequestData* pData = new RequestData(this, sRequest, pProgress, buffer, lpServer, lpOptional, lpHeaders);
	// Run the request
	return DoRequest(pData);
}

/**
	@param sRequest The page requested from the server
	@param buffer Buffer object to fill with the request response
	@param lpServer Server name (can be NULL; if so, will use the current connection if any or fail if there is none)
	@param lpOptional Additional data for POST (can be NULL)
	@param lpHeaders Additional headers (can be NULL)
	@param pProgress Pointer to the progress interface (can be NULL)
	@return true if the request worker thread started, false if it failed to start
*/
bool InternetRequest::DoRequestThread(const std::tstring& sRequest, InternetRequestBuffer& buffer, LPCTSTR lpServer /* = NULL */, LPCSTR lpOptional /* = NULL */, LPCTSTR lpHeaders /* = NULL */, ProgressInterface* pProgress /* = &piDefault */)
{
	// Create a data structure for the request information
	RequestData* pData = new RequestData(this, sRequest, pProgress, buffer, lpServer, lpOptional, lpHeaders);

	// Create a worker thread
	DWORD dwThreadID;
	HANDLE hThread = ::CreateThread(NULL, 0, RequestThreadProc, (LPVOID)pData, 0, &dwThreadID);
	if (hThread == NULL)
	{
		// Failed!
		delete pData;
		m_sError = _T("Cannot create request thread");
		return false;
	}

	// OK, just return now
	::CloseHandle(hThread);
	return true;
}

/**
	@param lpData Pointer to the thread data structure (RequestData in this case)
	@return 0
*/
DWORD WINAPI InternetRequest::RequestThreadProc(LPVOID lpData)
{
	// Get the request data pointer
	RequestData* pData = (RequestData*)lpData;
	// Do the request
	pData->m_pThis->DoRequest(pData);
	return 0;
}

/**
	@param pData Pointer to the request data structure
	@return true if request returned the data, false if failed
*/
bool InternetRequest::DoRequest(RequestData* pData)
{
	// Run the request
	bool bRet = InnerDoRequest(pData);
	// Notify the progress interface about the end of the request
	pData->m_pProgress->OnFinished(bRet);

	// Finish up
	delete pData;
	return bRet;
}

/**
	@param pData Pointer to the request data structure
	@return true if request returned the data, false if failed
*/
bool InternetRequest::InnerDoRequest(RequestData* pData)
{
	m_sError = _T("");

	// Wrap this so it won't crash if wininet fails
	try
	{
		// Do we have a session handle?
		if (m_hSession == NULL)
		{
			// No, create one
			if (!OpenSession())
				return false;
		}

		// Do we have a request running?
		if (m_hRequest != NULL)
		{
			// Yeah, stop it first
			::InternetCloseHandle(m_hRequest);
			m_hRequest = NULL;
		}

		// Does the request specify a server?
		if (pData->m_sServer.empty())
		{
			// No, do we HAVE a server from before?
			if (m_sServer.empty())
			{
				// No, so we can't do it
				m_sError = _T("No server defined for the request");
				return false;
			}
		}
		else if (m_sServer != pData->m_sServer)
		{
			// Different server; close the current connection
			if (m_hConnection != NULL)
				::InternetCloseHandle(m_hConnection);
			m_hConnection = NULL;

			// Open the connection:
			m_bInCreation = true;
			m_hConnection = ::InternetConnect(m_hSession, pData->m_sServer.c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD)this);
			while (m_bInCreation && !pData->m_pProgress->GetCancel())
				::Sleep(100);
			if (!m_sError.empty())
				return false;
			// OK, remember the server name for next time
			m_sServer = pData->m_sServer;
		}

		// Start the request from the server (POST if optional data is not empty)
		m_bInCreation = true;
		m_hRequest = ::HttpOpenRequest(m_hConnection, pData->m_sOptional.empty() ? NULL : _T("POST"), pData->m_sRequest.c_str(), _T("HTTP/1.1"), NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_NO_COOKIES|INTERNET_FLAG_NO_UI|INTERNET_FLAG_PRAGMA_NOCACHE, (DWORD)this);
		while (m_bInCreation && !pData->m_pProgress->GetCancel())
			::Sleep(100);

		if (m_hRequest == NULL)
		{
			// Error
			m_sError = _T("Cannot create request");
			return false;
		}

		// Should we add headers?
		if (!pData->m_sHeaders.empty())
		{
			// Yeah, add them
			if (!::HttpAddRequestHeaders(m_hRequest, pData->m_sHeaders.c_str(), -1, HTTP_ADDREQ_FLAG_ADD|HTTP_ADDREQ_FLAG_REPLACE))
			{
				// Error!
				m_sError = _T("Cannot send request (2)");
				return false;
			}
		}

		// Get the POST data if we have it
		LPVOID lpOptional = pData->m_sOptional.empty() ? NULL : (LPVOID)pData->m_sOptional.c_str();
		// Do the request
		if (!::HttpSendRequest(m_hRequest, NULL, 0, lpOptional, (lpOptional == NULL) ? 0 : pData->m_sOptional.size()))
		{
			// Failed!
			if (m_sError.empty())
				m_sError = _T("Cannot send request");
			return false;
		}

		// We get here after the request has been sent
		if (!m_sError.empty())
			return false;
	}
	catch (...)
	{
		// Catch crashes here!
		m_sError = _T("Unknown error while connecting");
		return false;
	}

	char* pBuffer = NULL;
	// Wrap this so it won't crash if wininet fails
	try
	{
		DWORD dwRead, dwSize;
		do
		{
			// Should we stop?
			if (pData->m_pProgress->GetCancel())
				// Yeah, stop
				return false;

			// block until new data arrives, or until end of file
			if (!::InternetQueryDataAvailable(m_hRequest, &dwSize, 0, 0))
			{
				m_sError = _T("Cannot retrieve data from query");
				return false;
			}
			// Is this the end?
			if (dwSize == 0)
				// Yeah, stop now
				break;

			// Create a buffer for the data
			pBuffer = new char[dwSize];
			// Retrieve it
			if (!::InternetReadFile(m_hRequest, pBuffer, dwSize, &dwRead))
			{
				// Error!
				m_sError = _T("Cannot retrieve data from query (2)");
				delete [] pBuffer;
				return false;
			}

			// If we didn't get any data, we're finished too
			if (dwRead == 0)
				break;

			// Add it to the buffer
			if (!pData->m_pBuffer->AddData(pBuffer, dwRead))
			{
				// Couldn't!
				m_sError = _T("Cannot add data to query buffer");
				delete [] pBuffer;
				return false;
			}

			// Finished with the reading buffer
			delete [] pBuffer;
			pBuffer = NULL;

		} while (true);

		// Add a string-ending character to make things easier for the readers of the request
		pData->m_pBuffer->AddData("\0", 1);
	}
	catch (...)
	{
		// Something bad happend, delete the temporary buffer...
		if (pBuffer != NULL)
			delete [] pBuffer;

		// And set the error
		m_sError = _T("Unknown error while retriving data");
		return false;
	}

	// Success!!!
	return true;
}
