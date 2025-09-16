#pragma once
#include "afxwin.h"
#include <iostream>
#include "vesselAnalysis.h"
#include "VesselImgTransfer.h"

// CWaitwindow dialog

class CWaitwindow : public CDialogEx
{
	DECLARE_DYNAMIC(CWaitwindow)

public:
	CWaitwindow(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaitwindow();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WaitWindow };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_static_wait;
	std::string m_string_wait_text;
	int thread_type;
	VesselImgTransfer* m_VesselImgTransfer;	

	static UINT ViewText(LPVOID _mothod);
	//static UINT StopTransferWaiting(LPVOID _mothod);
	CWinThread *vessel_extract_pthread, *view_state_pthread, *transfer_waitting_pthread;

	DWORD return_result;
	bool m_is_transferring;
	bool m_is_waiting;
	virtual BOOL OnInitDialog();
};
