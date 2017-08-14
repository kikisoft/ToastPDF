// toast_vpdinst.cpp : Defines the entry point for the console application.
//

#include "precomp.h"
#include <boost/regex.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/filesystem.hpp>

#include "CCPrintInstallFunctions.h"
#include "Registry.h"
#include "ToastPDFInstaller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;
using namespace boost::program_options;


enum action
{
	NONE = 0,
	INSTALL = 1,
	UNINSTALL
};

/*  This custom option parse function recognize gcc-style
option "-fbar" / "-fno-bar".
*/
pair<string, string> reg_foo(const string& s)
{
	if (s.find("-f") == 0) {
		if (s.substr(2, 3) == "no-")
			return make_pair(s.substr(5), string("false"));
		else
			return make_pair(s.substr(2), string("true"));
	}
	else {
		return make_pair(string(), string());
	}
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
			options_description desc("Allowed options");
			desc.add_options()("help,h", "produce a help message");
			desc.add_options()("action,a", value<string>(), "action : install | uninstall");

			variables_map vm;
			store(command_line_parser(argc, argv).options(desc).extra_parser(reg_foo).run(), vm);

			if (vm.empty()) {
				return 0;
			}

			if (vm.count("help")) {
				cout << desc << endl;
				return 0;
			}

			if (vm["action"].empty()) {
				return 0;
			}

			action act = action::NONE;

			if (vm["action"].as<string>() == "install")
			{
				act = action::INSTALL;
			}

			if (vm["action"].as<string>() == "uninstall")
			{
				act = action::UNINSTALL;
			}
			
			if (act == action::NONE)
			{
				cout << "action error!" << endl;
				return -1;
			}

			if (act == action::INSTALL)
			{
				BOOL disableWow64FsRedirection = FALSE;
				if (IsX64()) {
					LPVOID oldValue;
					BOOL bResult = Wow64DisableWow64FsRedirection(&oldValue);
					disableWow64FsRedirection = TRUE;
				}

				CCPDFPrinterInstaller installer(NULL);
				if (installer.DoInstall())
				{
					cout << "install success." << endl;
				}
				else
				{
					cout << "install failed." << endl;
				}

				if (disableWow64FsRedirection) {
					LPVOID oldValue;
					Wow64RevertWow64FsRedirection(&oldValue);
				}

				Registry registry;
				if (registry.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\GPL Ghostscript\\9.16")) == ERROR_SUCCESS) {
					CString gs_dll;
					boost::filesystem::path dir(boost::filesystem::current_path());

					gs_dll.Format(_T("%s\\gsdll32.dll"), dir.string());
					registry.Write(_T("GS_DLL"), gs_dll);
					CString gs_lib;
					gs_lib.Format(_T("%s\\tools\\gs9.16\\bin;"), dir.string());
					gs_lib.AppendFormat(_T("%s\\tools\\gs9.16\\lib;"), dir.string());
					gs_lib.AppendFormat(_T("%s\\tools\\gs9.16\\fonts"), dir.string());
					registry.Write(_T("GS_LIB"), gs_lib);
					registry.Close();
				}
			}
			else if (act == action::UNINSTALL)
			{
				BOOL disableWow64FsRedirection = FALSE;
				if (IsX64()) {
					LPVOID oldValue;
					BOOL bResult = Wow64DisableWow64FsRedirection(&oldValue);
					disableWow64FsRedirection = TRUE;
				}

				CCPDFPrinterInstaller installer(NULL);
				if (installer.DoRemove())
				{
					cout << "uninstall success." << endl;
				}
				else
				{
					cout << "uninstall failed." << endl;
				}

				if (disableWow64FsRedirection) {
					LPVOID oldValue;
					Wow64RevertWow64FsRedirection(&oldValue);
				}
			}
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
