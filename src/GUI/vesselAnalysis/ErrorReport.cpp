// ErrorReport.cpp : implementation file
//

#include "stdafx.h"
#include "vesselAnalysis.h"
#include "ErrorReport.h"
#include "afxdialogex.h"
#include "vesselAnalysisDlg.h"


// CErrorReport dialog

IMPLEMENT_DYNAMIC(CErrorReport, CDialogEx)

CErrorReport::CErrorReport(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ErrorReport, pParent)
{

}

CErrorReport::~CErrorReport()
{
}

void CErrorReport::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_report, m_edit_error);
}


BEGIN_MESSAGE_MAP(CErrorReport, CDialogEx)
	ON_BN_CLICKED(IDOK, &CErrorReport::OnBnClickedOk)
END_MESSAGE_MAP()


// CErrorReport message handlers


void CErrorReport::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;
	GetDlgItemText(IDC_EDIT_report, pDlg->m_VesselImgTransfer.m_cstr_errorreport);
	pDlg->m_VesselImgTransfer.SendError();
	EndDialog(IDOK);

	
}
