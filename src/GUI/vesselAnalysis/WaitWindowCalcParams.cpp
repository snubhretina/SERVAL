// WaitWindowCalcParams.cpp: 구현 파일
//

#include "stdafx.h"
#include "vesselAnalysis.h"
#include "WaitWindowCalcParams.h"
#include "afxdialogex.h"
#include "vesselAnalysisDlg.h"


// WaitWindowCalcParams 대화 상자

IMPLEMENT_DYNAMIC(WaitWindowCalcParams, CDialogEx)

WaitWindowCalcParams::WaitWindowCalcParams(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WaitWindow_Calc_Params, pParent)
{

}

WaitWindowCalcParams::~WaitWindowCalcParams()
{
}

void WaitWindowCalcParams::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CALC_PARAMS_STATIC_TEXT, calc_params_static_text);
}


BEGIN_MESSAGE_MAP(WaitWindowCalcParams, CDialogEx)
END_MESSAGE_MAP()

BOOL WaitWindowCalcParams::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;

	calc_params_pthread = NULL;
	viewing_static_text_ptrhead = NULL;
	m_is_waiting = TRUE;;
	m_is_calculating = TRUE;

	calc_params_pthread = AfxBeginThread(CalcParams, this);
	if (calc_params_pthread == NULL)
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

UINT WaitWindowCalcParams::CalcParams(LPVOID _mothod)
{
	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;
	
	pDlg->example = new vascularity(pDlg->m_unpadded_ROI_mask, pDlg->m_unpadded_ROI_mask);

	pDlg->m_WaitWindowCalcParams.m_is_calculating = false;

	return TRUE;
}

UINT WaitWindowCalcParams::ViewText(LPVOID _mothod)
{
	WaitWindowCalcParams* m_waitwindow = (WaitWindowCalcParams*)_mothod;
	m_waitwindow->GetSystemMenu(NULL)->EnableMenuItem(SC_CLOSE, MF_DISABLED);

	std::string finish_message = "파라미터 계산이 완료되었습니다.";
	std::string sending_message = "파라미터 계산 중";
	std::string progrss_message;

	int period = 0;
	while (TRUE) {
		if (m_waitwindow->m_is_calculating == FALSE) {
			m_waitwindow->SetDlgItemText(IDC_CALC_PARAMS_STATIC_TEXT, CString(finish_message.c_str()));
			m_waitwindow->m_is_waiting = FALSE;

			Sleep(3000);
			m_waitwindow->SendMessage(WM_CLOSE);
			break;
		}
		period += 1;
		progrss_message = sending_message + std::string(period, L'.');
		if (period == 5) {
			period = 0;
		}
		m_waitwindow->SetDlgItemText(IDC_CALC_PARAMS_STATIC_TEXT, CString(progrss_message.c_str()));
		Sleep(300);
	}
	return 0;
}
