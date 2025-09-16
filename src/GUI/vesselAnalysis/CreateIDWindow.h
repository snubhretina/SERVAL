#pragma once

#include "afxwin.h"
// CreateIDWindow 대화 상자

class CreateIDWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CreateIDWindow)

public:
	CreateIDWindow(CWnd* pParent = nullptr);   // 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_ID_WINDOW_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_id_list_box;
	CEdit m_input_id_text;
	afx_msg void OnLbnSelchangeIdList();
	CEdit m_input_name_text;
	CStatic m_input_name_static_text;
};
