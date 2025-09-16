// Waitwindow.cpp : implementation file
//

#include "stdafx.h"
#include "Waitwindow.h"
#include "afxdialogex.h"


// CWaitwindow dialog

IMPLEMENT_DYNAMIC(CWaitwindow, CDialogEx)

CWaitwindow::CWaitwindow(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WaitWindow, pParent)
	, m_static_wait(_T(""))
{
}

CWaitwindow::~CWaitwindow()
{
}

void CWaitwindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_Wait, m_static_wait);
}

BEGIN_MESSAGE_MAP(CWaitwindow, CDialogEx)
END_MESSAGE_MAP()

BOOL CWaitwindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_is_transferring = true;
	m_is_waiting = true;
	vessel_extract_pthread = NULL;
	view_state_pthread = NULL;
	transfer_waitting_pthread = NULL;
	vessel_extract_pthread = AfxBeginThread(m_VesselImgTransfer->VesselExtractThread, this);
	if (vessel_extract_pthread == NULL)
	{
		AfxMessageBox(L"Error VesselExtractThread");
	}


	view_state_pthread = AfxBeginThread(ViewText, this);
	if (view_state_pthread == NULL)
	{
		AfxMessageBox(L"Error ViewText");
	}
	
	return TRUE;
}

UINT CWaitwindow::ViewText(LPVOID _mothod)
{
	CWaitwindow* m_waitwindow = (CWaitwindow*)_mothod;

	m_waitwindow->GetSystemMenu(NULL)->EnableMenuItem(SC_CLOSE, MF_DISABLED);
	int period = 0;
	while (true) {
		if (m_waitwindow->m_is_transferring == false) {
			m_waitwindow->m_string_wait_text = std::string("Finish! this dialog will closed");
			m_waitwindow->m_static_wait = m_waitwindow->m_string_wait_text.c_str();
			m_waitwindow->SetDlgItemText(IDC_STATIC_Wait, m_waitwindow->m_static_wait);
			m_waitwindow->m_is_waiting = false;

			Sleep(2000);
			m_waitwindow->SendMessage(WM_CLOSE);
			break;
		}
		period += 1;
		m_waitwindow->m_string_wait_text = std::string("analysis") + std::string(period, L'.');
		if (period == 3) {
			period = 0;
		}
		m_waitwindow->m_static_wait = m_waitwindow->m_string_wait_text.c_str();
		m_waitwindow->SetDlgItemText(IDC_STATIC_Wait, m_waitwindow->m_static_wait);
		Sleep(300);
	}
	return 0;
}
