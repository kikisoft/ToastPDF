
// TTPDFDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

using namespace std;

// TTPDFDlg 대화 상자
class TTPDFDlg : public CDialogEx
{
// 생성입니다.
public:
	TTPDFDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	void ps2pdf(const string &input, const string &output);
	
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOASTPDF_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	static void start(const string& docFilename, const string& pdfFilename, const string& pdfFolder, int totalpage);
	static void workStatus(const string& status);
	static void progress(int page, int totalpage);
	static void finish();

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CProgressCtrl _progressCtrl;
	CStatic _statusStatic;
	CStatic _docFilenameStatic;
	CStatic _pdFilenameStatic;
	CStatic _pdfFolderStatic;
	string  _output;
public:
	afx_msg void OnBnClickedOpen();
	afx_msg void OnBnClickedCancel();
};
