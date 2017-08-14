
// TTPDF.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include <boost/regex.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 
#include <boost/process.hpp>
#include <TlHelp32.h>
#include "TTPDFApp.h"
#include "TTPDFDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace boost::program_options;

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

// TTPDFApp

BEGIN_MESSAGE_MAP(TTPDFApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// TTPDFApp 생성

TTPDFApp::TTPDFApp()
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 TTPDFApp 개체입니다.

TTPDFApp theApp;

//
TTPDFWorker thePDFWorker;

// TTPDFApp 초기화
BOOL TTPDFApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	
	AfxEnableControlContainer();

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 파라미터에 따라 적절한 처리
	options_description desc("Allowed options");
	desc.add_options()("action,a", value<string>(), "action : doc2pdf");
	desc.add_options()("input,i", value<string>(), "postscript file");
	desc.add_options()("output,o", value<string>(), "pdf file");
	desc.add_options()("silent,s", "silent");
	
	variables_map vm;
	store(command_line_parser(__argc, __argv).options(desc).extra_parser(reg_foo).run(), vm);

	if (vm.empty() || vm["action"].as<string>() != "doc2pdf") { // 부정확한 파라미터라면 종료
		return FALSE;
	}

	if (vm["input"].empty()) { // 입력파일(Postscript)이 비어있다면

		// 1) 포스트스크립트 출력 파일 설정
		string ps_path = TTUtil::getOutputPostscript();
		if (ps_path.empty()) {
			ps_path = TTUtil::currentPath() + "\\temp\\" + TTUtil::getRandomString() + ".ps";
		}

		// 2) 표준입력 스트림을 포스트스크립트로 저장
		if (!thePDFWorker.stdin2ps(ps_path)) {
			return FALSE;
		}
		
		// 3) Silent 모드가 아니라면 Postscript 파일 설정 후 PDF 변환 시작
		if (!TTUtil::silent() && vm["silent"].empty()) {
			string args;
			args = "-a doc2pdf";
			args += " -i \"" + ps_path + "\"";

			ShellExecute(NULL, NULL, TTUtil::getModuleFileName().c_str(), args.c_str(), NULL, SW_NORMAL);
		}
		
		// 4) 설정 리셋
		TTUtil::reset();

		return FALSE;
	}
	
	// Postscript 입력파일
	string input = vm["input"].as<string>();

	// Silent 모드이고 출력파일 설정이 비어있지 않다면 Silent 모드로 실행
	if ((TTUtil::silent() || !vm["silent"].empty()) && !vm["output"].empty()) {
		thePDFWorker.ps2pdf(input, vm["output"].as<string>());
		return FALSE;
	}
	
	// 파일저장 대화상자로부터 PDF 저장경로를 설정한다.
	string output = TTUtil::getOutputFromSaveDialog(m_hInstance);

	// PDF 저장파일이 비어있다면(사용자에 의한 취소) 종료
	if (output.empty()) {
		return FALSE;
	}

	// PDF 변환 다이얼로그 활성화
	TTPDFDlg dlg;
	m_pMainWnd = &dlg;
	
	dlg.ps2pdf(input, output);

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1)
	{
		
	}

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

