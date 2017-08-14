#include "precomp.h"
#include "Registry.h"
#include <winerror.h>


const static int nMaxValueNameSize = MAX_PATH;
const static int nMaxValueValueSize = 4096;
const static int nMaxClassSize = MAX_PATH;
const static int nMaxKeyNameSize = MAX_PATH;

BOOL Registry::CopyKeys(HKEY hkFrom, HKEY hkTo) 
{
	TCHAR lpstrName[nMaxKeyNameSize];
	TCHAR lpstrClass[nMaxClassSize];

	for (int i = 0;;i++) {
		DWORD nNameSize = nMaxKeyNameSize;
		DWORD nClassSize = nMaxClassSize;
		LONG res = ::RegEnumKeyEx(hkFrom, i, lpstrName, &nNameSize, 0, lpstrClass, &nClassSize, 0);
		if (ERROR_NO_MORE_ITEMS == res) {
			break;
		}
		HKEY hkNew;
		res = ::RegCreateKeyEx(hkTo, lpstrName, 0, lpstrClass, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hkNew, 0); ;
		if (ERROR_SUCCESS != res) {
			return FALSE;		
		}
		::RegCloseKey(hkNew);
		BOOL bRes = CopyKey(hkFrom, lpstrName, hkTo, lpstrName);
		if (! bRes) {
			return FALSE;		
		}
	}

	return TRUE;
}

BOOL Registry::CopyKey(HKEY hkRootFrom, const CString& sFromPath, HKEY hkRootTo, const CString& sToPath)
{
	HKEY hkFrom;
	LONG res = ::RegOpenKeyEx(hkRootFrom, sFromPath, 0, KEY_READ, &hkFrom);
	if (ERROR_SUCCESS != res) {
		return FALSE;	
	}
	HKEY hkTo;
	res = ::RegCreateKeyEx(hkRootTo, sToPath, 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hkTo, 0);
	if (ERROR_SUCCESS != res) {
		::RegCloseKey(hkFrom);
		return FALSE;	
	}
	BOOL bRes = CopyKeys(hkFrom, hkTo) && CopyValues(hkFrom, hkTo);

	::RegCloseKey(hkFrom);
	::RegCloseKey(hkTo);

	return bRes;
}

BOOL Registry::CopyValues(HKEY hkFrom, HKEY hkTo) 
{
	TCHAR lpstrName[nMaxValueNameSize];
	BYTE pValue[nMaxValueValueSize];

	for (int i = 0;;i++) {
		DWORD nType;
		DWORD nNameSize = nMaxValueNameSize;
		DWORD nValueSize = nMaxValueValueSize;
		LONG res = ::RegEnumValue(hkFrom, i, lpstrName, &nNameSize, 0, &nType, pValue, &nValueSize);
		if (ERROR_NO_MORE_ITEMS == res) {
			break;
		}
		res = ::RegSetValueEx(hkTo, lpstrName, 0, nType, pValue, nValueSize);
		if (ERROR_SUCCESS != res) {
			return FALSE;		
		}
	}

	return TRUE;
}
Registry::Registry()
{
	m_hKeyRoot = NULL;
	m_hKey = NULL;
	m_sPath.Empty();
}

Registry::Registry(Registry& regKey)
{
	m_hKeyRoot = NULL;
	m_hKey = NULL;
	m_sPath.Empty();

	try
	{
		Open(regKey.m_hKeyRoot, regKey.m_sPath);
	}
	catch (...)
	{
	}
}

Registry::~Registry()
{
	Close();
}

Registry& Registry::operator=(Registry regKey)
{
	m_hKey = regKey.m_hKey;
	m_sPath = regKey.m_sPath;

    return *this; 
}

LONG Registry::Open(HKEY hKeyRoot, LPCTSTR pszPath)
{
	DWORD dwDisp;
	LONG lResult;

	// if the key has aleady been opened then close it first
	if (m_hKey)
	{
		ASSERT(0);
		return ERROR_ACCESS_DENIED;
	}

	m_sPath = pszPath;
	m_hKeyRoot = hKeyRoot;

	lResult = RegCreateKeyEx(hKeyRoot, pszPath, 0L, NULL,
				REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
				&m_hKey, &dwDisp);

	return lResult;
}

BOOL Registry::KeyExists(HKEY hKeyRoot, LPCTSTR pszPath)
{
	BOOL bExistingKey;
	DWORD dwDisp;
	HKEY hTemp;

	// open a temporary key
	RegCreateKeyEx(hKeyRoot, pszPath, 0L, NULL,
				REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
				&hTemp, &dwDisp);
	
	// make sure we close it
	RegCloseKey(hTemp);

	// test for existing
	bExistingKey = (dwDisp == REG_OPENED_EXISTING_KEY);

	// and then delete the physical entry from the registry if new
	if (!bExistingKey)
		Delete(hKeyRoot, pszPath);

	return bExistingKey;
}

LONG Registry::RecurseDeleteKey(HKEY key, LPCTSTR lpszKey)
{
	HKEY rslt;
	LONG lRes;
	FILETIME time;
	TCHAR szBuffer[256];
	DWORD dwSize = 256;

	lRes = RegOpenKeyEx(key, lpszKey, 0, KEY_ALL_ACCESS, &rslt);

	if (lRes != ERROR_SUCCESS)
		return lRes;

	while (RegEnumKeyEx(rslt, 0, szBuffer, &dwSize, NULL, NULL, NULL, &time) == ERROR_SUCCESS)
	{
		lRes = RecurseDeleteKey(rslt, szBuffer);

		if (lRes != ERROR_SUCCESS)
			return lRes;

		dwSize = 256;
	}

	RegCloseKey(rslt);

	return RegDeleteKey(key, lpszKey);
}

LONG Registry::Delete(HKEY hKeyRoot, LPCTSTR pszPath)
{
	return RecurseDeleteKey(hKeyRoot, pszPath);
}

void Registry::Close()
{
	if (m_hKey)
	{
		RegCloseKey(m_hKey);
	}
		
	m_hKey = NULL;
}

LONG Registry::Write(LPCTSTR pszItem, DWORD dwVal) 
{
	ASSERT(m_hKey);
	ASSERT(pszItem);
	return RegSetValueEx(m_hKey, pszItem, 0L, REG_DWORD,
		(CONST BYTE*) &dwVal, sizeof(DWORD));
}

DWORD Registry::GetValueType(LPCTSTR pszItem) const
{
	ASSERT(m_hKey);
	ASSERT(pszItem);

	DWORD dwType;

	LONG lRet = RegQueryValueEx (m_hKey, pszItem, NULL, &dwType, NULL, NULL);

	if (lRet == ERROR_SUCCESS)
		return dwType;

	return REG_NONE;
}

LONG Registry::Write(LPCTSTR pszItem, LPCTSTR pszVal, DWORD dwType) 
{
	ASSERT(m_hKey);
	ASSERT(pszItem);
	ASSERT(pszVal);

#ifndef _NOT_USING_MFC_
	ASSERT(AfxIsValidAddress(pszVal, _tcslen(pszVal), FALSE));
#endif

	return RegSetValueEx(m_hKey, pszItem, 0L, dwType,
		(CONST BYTE*) pszVal, _tcslen(pszVal) + 1);
}

LONG Registry::Write(LPCTSTR pszItem, const BYTE* pData, DWORD dwLength, DWORD dwType) 
{
	ASSERT(m_hKey);
	ASSERT(pszItem);
	ASSERT(pData && dwLength > 0);

#ifndef _NOT_USING_MFC_
	ASSERT(AfxIsValidAddress(pData, dwLength, FALSE));
#endif
	return RegSetValueEx (m_hKey, pszItem, 0L, dwType,
		pData, dwLength);
}

LONG Registry::Read(LPCTSTR pszItem, DWORD& dwVal) const
{
	ASSERT(m_hKey);
	ASSERT(pszItem);

	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest;

	LONG lRet = RegQueryValueEx (m_hKey, pszItem, NULL, 
		&dwType, (BYTE *) &dwDest, &dwSize);

	if (lRet == ERROR_SUCCESS)
		dwVal = dwDest;

	return lRet;
}

LONG Registry::Read(LPCTSTR pszItem, CString& sVal) const
{
	ASSERT(m_hKey);
//	ASSERT(pszItem);

	DWORD dwType;
	DWORD dwSize = 200;
	char  string[200];

	LONG lReturn = RegQueryValueEx (m_hKey, pszItem, NULL,
		&dwType, (BYTE *) string, &dwSize);

	if (lReturn == ERROR_SUCCESS)
		sVal = string;

	return lReturn;
}

LONG Registry::Read(LPCTSTR pszItem, BYTE* pData, DWORD& dwLen) const
{
	ASSERT(m_hKey);
	ASSERT(pszItem);

	DWORD dwType;

	return RegQueryValueEx(m_hKey, pszItem, NULL,
		&dwType, pData, &dwLen);

}

int Registry::GetSubkeyNames(CStringArray& aNames) const
{
	ASSERT(m_hKey);

   aNames.RemoveAll();

   DWORD nKey = 0;
   TCHAR szName[512];
   LONG lResult = ERROR_SUCCESS;
   DWORD nNameLen = sizeof(szName)/sizeof(TCHAR);

   while (lResult == ERROR_SUCCESS)
   { 
      lResult = ::RegEnumKey(m_hKey, nKey, szName, nNameLen);
      
      // we have a valid key name
      if (lResult == ERROR_SUCCESS)
         aNames.Add(szName);

      nKey++; // next 
   }

   return aNames.GetSize();
}

int Registry::GetValueNames(CStringArray& aNames) const
{
	ASSERT(m_hKey);

   DWORD nVal = 0;
   TCHAR szName[512];
   LONG lResult = ERROR_SUCCESS;

   while (lResult == ERROR_SUCCESS)
   { 
      DWORD nNameLen = sizeof(szName)/sizeof(TCHAR);

      lResult = ::RegEnumValue(m_hKey, nVal, szName, &nNameLen,
                               NULL, NULL, NULL, NULL);
      
      // we have a valid key name
      if (lResult == ERROR_SUCCESS)
         aNames.Add(szName);

      nVal++; // next 
   } 

   return aNames.GetSize();
}

BOOL Registry::HasValues() const
{
	ASSERT(m_hKey);

   TCHAR szName[512];
   DWORD nNameLen = sizeof(szName)/sizeof(TCHAR);

   return (::RegEnumValue(m_hKey, 0, szName, &nNameLen,
                          NULL, NULL, NULL, NULL) == ERROR_SUCCESS);
}

#ifndef _NOT_USING_MFC_

CString Registry::GetAppRegPath(LPCTSTR szAppName)
{
	ASSERT(AfxGetApp()->m_pszRegistryKey);
	CString sRegPath, sAppName;

	if (szAppName && _tcslen(szAppName))
		sAppName = szAppName;
	else
		sAppName = AfxGetAppName();

	// construct reg path
	sRegPath = "Software\\";
	sRegPath += CString(AfxGetApp()->m_pszRegistryKey);
	sRegPath += '\\';
	sRegPath += sAppName;
	sRegPath += '\\';

	return sRegPath;
}

#endif

BOOL Registry::ExportToIni(LPCTSTR szIniPath) const
{
	ASSERT (m_hKey);
	
	if (!m_hKey)
		return FALSE;
	
	CStdioFile file;
	
	if (!file.Open(szIniPath, CFile::modeCreate | CFile::modeWrite))
		return FALSE;
	
	// process top level keys. ie we ignore any values in the root
	CStringArray aSubKeys;
	
	if (GetSubkeyNames(aSubKeys))
	{
		for (int nKey = 0; nKey < aSubKeys.GetSize(); nKey++)
		{
			CString sName = aSubKeys[nKey];
			
			if (!ExportKeyToIni(sName, file))
				return FALSE;
		}
	}
	
	return TRUE;
}

BOOL Registry::ExportKeyToIni(LPCTSTR pszKey, CStdioFile& file) const
{
	ASSERT (pszKey && *pszKey);
	Registry reg;
	
	if (reg.Open(m_hKey, pszKey) == ERROR_SUCCESS)
	{
		BOOL bSectionHasValues = reg.HasValues();
		
		if (bSectionHasValues)
		{
			// write out section heading
			CString sSection;
			sSection.Format(_T("[%s]\n"), pszKey);
			file.WriteString(sSection);

			// write out values
			int nIndex = 0;
			
			while (reg.ExportValueToIni(nIndex, file))
				nIndex++;
		}
		
		// write out subkeys
		CStringArray aSubKeys;
		
		if (reg.GetSubkeyNames(aSubKeys))
		{
			for (int nKey = 0; nKey < aSubKeys.GetSize(); nKey++)
			{
				CString sName = aSubKeys[nKey];
				
				// process subkey
				CString sKeyName;
				sKeyName.Format(_T("%s\\%s"), pszKey, sName);
				
				if (!ExportKeyToIni(sKeyName, file))
					return FALSE;
			}
		}
		
		return TRUE;
	}
	
	// else
	return FALSE;
}

BOOL Registry::ExportValueToIni(DWORD nIndex, CStdioFile& file) const
{
	TCHAR szName[512];
	DWORD nNameLen = sizeof(szName)/sizeof(TCHAR);
	DWORD dwType;
	
	LONG lResult = ::RegEnumValue(m_hKey, nIndex, szName, &nNameLen,
		NULL, &dwType, NULL, NULL);
	
	// we have a valid key name
	if (lResult == ERROR_SUCCESS)
	{
		CString sValueLine;
		
		switch (dwType)
		{
		case REG_DWORD:
			{
				DWORD dwValue;
				Read(szName, dwValue);
				sValueLine.Format(_T("%s = %d\n"), szName, dwValue);
			}
			break;
			
		case REG_SZ:
			{
				CString sValue;
				Read(szName, sValue);
				sValueLine.Format(_T("%s = \"%s\"\n"), szName, sValue);
			}
		}
		
		if (!sValueLine.IsEmpty())
			file.WriteString(sValueLine);
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL Registry::ImportFromIni(LPCTSTR szIniPath)
{
	ASSERT (m_hKey);
	
	if (!m_hKey)
		return FALSE;
	
	CStdioFile file;
	
	if (!file.Open(szIniPath, CFile::modeRead))
		return FALSE;

	CString sSection, sNextSection;
	BOOL bRes = TRUE;

	do 
	{
		// detect invalid ini files
		bRes &= ImportSectionFromIni(sSection, file, sNextSection);
		sSection = sNextSection;
	} 
	while(bRes && !sSection.IsEmpty());
	
	return bRes;
}

BOOL Registry::ImportSectionFromIni(const CString& sSection, CStdioFile& file, CString& sNextSection)
{
	sNextSection.Empty();
	BOOL bRoot = (sSection.IsEmpty());
	Registry reg;
	
	// open the reg key if not root
	if (!bRoot)
	{
		TRACE (" Registry::ImportSectionFromIni(%s)\n", sSection);
		if (reg.Open(m_hKey, sSection) != ERROR_SUCCESS)
			return FALSE;
	}
	
	CString sLine;
	
	while (file.ReadString(sLine))
	{
		sLine.TrimLeft();
		sLine.TrimRight();
		
		if (sLine.IsEmpty())
			continue;

		else if (sLine[0] == '[')
		{
			// check for end tag
			if (sLine[sLine.GetLength() - 1] == ']')
			{
				sNextSection = sLine.Mid(1, sLine.GetLength() - 2);
				return TRUE;
			}
			
			// else
			return FALSE;
		}
		else if (!bRoot) // can't have values in the root
		{      
			CString sName, sValue;
			int nEquals = sLine.Find('=');
			
			if (nEquals > 0)
			{
				sName = sLine.Left(nEquals);
				sValue = sLine.Mid(nEquals + 1);
				
				sName.TrimLeft();
				sName.TrimRight();
				sValue.TrimLeft();
				sValue.TrimRight();
				
				// name must not be empty
				if (!sName.IsEmpty())
				{
					// if value contains only digits optionally beginning 
					// with a minus sign then its a DWORD else a string
					BOOL bString = FALSE;

					if (sValue.IsEmpty())
						bString = TRUE;
					else
					{
						// traverse the chars
						for (int nChar = 0; nChar < sValue.GetLength() && !bString; nChar++)
						{
							switch (sValue[nChar])
							{
							case '-':
								bString = (nChar > 0);
								break;

							case '0':
							case '1':
							case '2':
							case '3':
							case '4':
							case '5':
							case '6':
							case '7':
							case '8':
							case '9':
								break; // okay

							default:
								bString = TRUE; // everything else
								break;

							}
						}
					}

					if (bString)
					{
						// remove possible leading and trailing quotes
						if (sValue.GetLength() && sValue[0] == '\"' && sValue[sValue.GetLength() - 1] == '\"')
							reg.Write(sName, sValue.Mid(1, sValue.GetLength() - 2));
						else
							reg.Write(sName, sValue);
					}
					else // DWORD
						reg.Write(sName, (DWORD)_ttoi(sValue));
				}
			}
		}
		else // invalid file
			return FALSE;
	}
	
	return TRUE;
}