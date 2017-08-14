
// TTPDFDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TTPDFApp.h"
#include "TTPDFDlg.h"
#include "afxdialogex.h"
#include <boost/thread.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// TTPDFDlg 대화 상자



TTPDFDlg::TTPDFDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TOASTPDF_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void TTPDFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, _progressCtrl);

	DDX_Control(pDX, IDC_STATIC_STATUS, _statusStatic);
	DDX_Control(pDX, IDC_STATIC_DOC_NAME, _docFilenameStatic);
	DDX_Control(pDX, IDC_STATIC_PDF_NAME, _pdFilenameStatic);
	DDX_Control(pDX, IDC_STATIC_PDF_FOLDER, _pdfFolderStatic);
}

BEGIN_MESSAGE_MAP(TTPDFDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN, &TTPDFDlg::OnBnClickedOpen)
	ON_BN_CLICKED(IDCANCEL, &TTPDFDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// TTPDFDlg 메시지 처리기

BOOL TTPDFDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void TTPDFDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void TTPDFDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR TTPDFDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void TTPDFDlg::OnBnClickedOpen()
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL, NULL, _output.c_str(), NULL, NULL, SW_NORMAL);
	CDialogEx::OnOK();
}

void TTPDFDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	thePDFWorker.stop();
	CDialogEx::OnCancel();
}

// 콜백 함수들
void TTPDFDlg::start(const string& docFilename, const string& pdfFilename, const string& pdfFolder, int totalpage) {
	TTPDFDlg *pPDFDlg = ((TTPDFDlg *)theApp.m_pMainWnd);
	pPDFDlg->_statusStatic.SetWindowText("PDF 변환 준비 중 ...");
	pPDFDlg->_docFilenameStatic.SetWindowText(string("문서 이름: " + docFilename).c_str());
	pPDFDlg->_pdFilenameStatic.SetWindowText(string("PDF 이름: " + pdfFilename).c_str());
	pPDFDlg->_pdfFolderStatic.SetWindowText(string("PDF 폴더: " + pdfFolder).c_str());
	pPDFDlg->_progressCtrl.SetRange32(0, totalpage);
	pPDFDlg->_progressCtrl.SetPos(0);
}

void TTPDFDlg::workStatus(const string& status) {
	TTPDFDlg *pPDFDlg = ((TTPDFDlg *)theApp.m_pMainWnd);
	pPDFDlg->_statusStatic.SetWindowText(status.c_str());
}
void TTPDFDlg::progress(int page, int totalpage) {
	TTPDFDlg *pPDFDlg = ((TTPDFDlg *)theApp.m_pMainWnd);
	CString text;

	text.Format(_T("PDF 변환 중 ... (%d/%d)"), page, totalpage);
	pPDFDlg->_statusStatic.SetWindowText(text);
	pPDFDlg->_progressCtrl.SetPos(page);
}
void TTPDFDlg::finish() {
	TTPDFDlg *pPDFDlg = ((TTPDFDlg *)theApp.m_pMainWnd);
	pPDFDlg->_statusStatic.SetWindowText("PDF 변환이 완료되었습니다.");
	pPDFDlg->GetDlgItem(IDC_OPEN)->EnableWindow(TRUE);
}

void TTPDFDlg::ps2pdf(const string &input, const string &output) {
	
	_output = output;
	boost::thread pdfWorkerThread([&input, &output]() {
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		thePDFWorker.setCallbacks(TTPDFDlg::start, TTPDFDlg::workStatus, TTPDFDlg::progress, TTPDFDlg::finish);
		thePDFWorker.ps2pdf(input, output);
	});
}
