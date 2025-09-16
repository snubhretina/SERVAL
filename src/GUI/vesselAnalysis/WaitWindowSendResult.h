#pragma once
#include "VesselImgTransfer.h"


// WaitWindowSendResult 대화 상자

class WaitWindowSendResult : public CDialogEx
{
	DECLARE_DYNAMIC(WaitWindowSendResult)

public:
	WaitWindowSendResult(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~WaitWindowSendResult();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WaitWindow_Send_Result };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CStatic send_result_static_text;
	VesselImgTransfer* VesselImgTransfer;
	CWinThread *sending_result_pthread, *viewing_static_text_ptrhead;

	bool m_is_transferring;
	bool m_is_waiting;
	std::string finish_message;
	std::string sending_message;
	static UINT ViewText(LPVOID _mothod);
};
