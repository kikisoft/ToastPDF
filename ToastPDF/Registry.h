#if !defined(AFX_REGKEY_H__E451D194_D4C3_11D1_AB17_0000E8425C3E__INCLUDED_)
#define AFX_REGKEY_H__E451D194_D4C3_11D1_AB17_0000E8425C3E__INCLUDED_

#include <winreg.h>
#include <afxtempl.h>

class Registry
{

// Construction
public:
	Registry();
	Registry(Registry& regKey);
	virtual ~Registry();

	Registry& operator=(Registry regKey); 

// Operations
public:
	// note: calling open on an already open key will assert
	// you must call Close() first if you want to re-use Registry objects
	LONG Open(HKEY hKeyRoot, LPCTSTR pszPath); 
	void Close();
	CString GetPath() const { return m_sPath; }
	HKEY GetKey() const { return m_hKey; }
	HKEY GetKeyRoot() const { return m_hKeyRoot; }
		
	LONG Write(LPCTSTR pszItem, DWORD dwVal);
	LONG Write(LPCTSTR pszItem, LPCTSTR pszVal, DWORD dwType = REG_SZ);
	LONG Write(LPCTSTR pszItem, const BYTE* pData, DWORD dwLength, DWORD dwType = REG_BINARY);
		
	LONG Read(LPCTSTR pszItem, DWORD& dwVal) const;
	LONG Read(LPCTSTR pszItem, CString& sVal) const;
	LONG Read(LPCTSTR pszItem, BYTE* pData, DWORD& dwLength) const;
		
	static LONG Delete(HKEY hKeyRoot, LPCTSTR pszPath);
	static BOOL KeyExists(HKEY hKeyRoot, LPCTSTR pszPath);
		
	int GetSubkeyNames(CStringArray& aNames) const;
	int GetValueNames(CStringArray& aNames) const;
	BOOL HasValues() const;
	DWORD GetValueType(LPCTSTR pszItem) const;
		
	BOOL ExportToIni(LPCTSTR szIniPath) const;
	BOOL ImportFromIni(LPCTSTR szIniPath);
		
	static BOOL CopyKey(HKEY hkRootFrom, const CString& sFromPath, HKEY hkRootTo, const CString& sToPath);
		
		
#ifndef _NOT_USING_MFC_
	static CString GetAppRegPath(LPCTSTR szAppName = NULL);
#endif

protected:
	HKEY 	m_hKey;
	HKEY 	m_hKeyRoot;
	CString m_sPath;

protected:
	static LONG RecurseDeleteKey(HKEY key, LPCTSTR lpszKey);
	BOOL ExportKeyToIni(LPCTSTR pszKey, CStdioFile& file) const;
	BOOL ExportValueToIni(DWORD nIndex, CStdioFile& file) const;
	BOOL ImportSectionFromIni(const CString& sSection, CStdioFile& file, CString& sNextSection);	   

	static BOOL CopyKeys(HKEY hkFrom, HKEY hkTo);
	static BOOL CopyValues(HKEY hkFrom, HKEY hkTo);
};


#endif

