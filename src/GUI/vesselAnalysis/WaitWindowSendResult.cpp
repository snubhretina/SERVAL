// WaitWindowSendResult.cpp: 구현 파일
//

#include "stdafx.h"
#include "vesselAnalysis.h"
#include "WaitWindowSendResult.h"
#include "afxdialogex.h"


// WaitWindowSendResult 대화 상자

IMPLEMENT_DYNAMIC(WaitWindowSendResult, CDialogEx)

WaitWindowSendResult::WaitWindowSendResult(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WaitWindow_Send_Result, pParent)
{

}

WaitWindowSendResult::~WaitWindowSendResult()
{
}

void WaitWindowSendResult::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEND_RESULT_STATIC_TEXT, send_result_static_text);
}


BEGIN_MESSAGE_MAP(WaitWindowSendResult, CDialogEx)
END_MESSAGE_MAP()

BOOL WaitWindowSendResult::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	sending_result_pthread = NULL;
	viewing_static_text_ptrhead = NULL;
	m_is_transferring = true;
	m_is_waiting = true;

	sending_result_pthread = AfxBeginThread(VesselImgTransfer->SendResult, this);
	if (sending_result_pthread == NULL)
	{
		AfxMessageBox(L"결과 전송 실패.");
	}

	viewing_static_text_ptrhead = AfxBeginThread(ViewText, this);
	if (viewing_static_text_ptrhead == NULL)
	{
		AfxMessageBox(L"Error ViewText");
	}

	return TRUE;
}

UINT WaitWindowSendResult::ViewText(LPVOID _mothod)
{
	WaitWindowSendResult* m_waitwindow = (WaitWindowSendResult*)_mothod;
	m_waitwindow->GetSystemMenu(NULL)->EnableMenuItem(SC_CLOSE, MF_DISABLED);
	
	std::string finish_message = "결과 전송이 완료되었습니다.\n잠시후 창이 종료됩니다.";
	std::string sending_message = "서버로 전송하는 중";
	std::string progrss_message;
	int period = 0;
	while (true) {
		if (m_waitwindow->m_is_transferring == false) {
			m_waitwindow->SetDlgItemText(IDC_SEND_RESULT_STATIC_TEXT, CString(finish_message.c_str()));
			m_waitwindow->m_is_waiting = false;

			Sleep(3000);
			m_waitwindow->SendMessage(WM_CLOSE);
			break;
		}
		period += 1;
		progrss_message = sending_message + std::string(period, L'.');
		if (period == 5) {
			period = 0;
		}
		m_waitwindow->SetDlgItemText(IDC_SEND_RESULT_STATIC_TEXT, CString(progrss_message.c_str()));
		Sleep(300);
	}
	return 0;
}