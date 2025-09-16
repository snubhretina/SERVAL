#pragma once
#include "afxwin.h"
#include "utils.h"
#include "VesselAnalysisParams.h"

// CreateAccountWindow 대화 상자

class CreateAccountWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CreateAccountWindow)

public:
	CreateAccountWindow(CWnd* pParent = nullptr);   // 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_ACCOUNT_WINDOW_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_account_list_box;
	CStatic m_input_name_static_text;
	CEdit m_input_name_text_editor;
	CString m_account_name;
	CButton m_login_button;
	CUtils utils;

	std::string m_account_name_scv_path;
	std::vector<CString> m_account_name_vec;

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnBnClickedCreateAccountButton();
	afx_msg void OnBnClickedLoginButton();
	afx_msg void OnBnClickedDeleteAccountButton();
};
