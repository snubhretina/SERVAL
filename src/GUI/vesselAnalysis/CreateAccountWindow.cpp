// CreateAccountWindow.cpp: 구현 파일
//
#include "stdafx.h"
#include "vesselAnalysis.h"
#include "CreateAccountWindow.h"
#include "afxdialogex.h"
#include "utils.h"


// CreateAccountWindow 대화 상자

IMPLEMENT_DYNAMIC(CreateAccountWindow, CDialogEx)

CreateAccountWindow::CreateAccountWindow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CREATE_ACCOUNT_WINDOW_DIALOG, pParent)
{
}


void CreateAccountWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACCOUNT_LIST_BOX, m_account_list_box);
	DDX_Control(pDX, IDC_INPUT_NAME_STATIC_TEXT, m_input_name_static_text);
	DDX_Control(pDX, IDC_INPUT_NAME_TEXT_EDITOR, m_input_name_text_editor);
	DDX_Control(pDX, IDC_LOGIN_BUTTON, m_login_button);
}

BOOL CreateAccountWindow::OnInitDialog() {
	CDialog::OnInitDialog();

	m_account_name_scv_path = ".\\account_name.csv";
	
	if (utils.IsFileExist(m_account_name_scv_path.c_str())) {
		std::vector<CString>::iterator account_name;
		account_name = m_account_name_vec.begin();
		utils.ReadAccountNameCSV(m_account_name_scv_path, m_account_name_vec);

		for (account_name = m_account_name_vec.begin(); account_name != m_account_name_vec.end(); account_name++)
		{
			m_account_list_box.InsertString(0, *account_name);
		};
		m_account_list_box.SetCurSel(0);
	}
	else {
		utils.WriteAccountNameCSV(m_account_name_scv_path);
	}

	if (m_account_list_box.GetCount() != 0) {
		GetDlgItem(IDC_LOGIN_BUTTON)->EnableWindow(TRUE);
	}
	else {
		GetDlgItem(IDC_LOGIN_BUTTON)->EnableWindow(FALSE);
	}

	return TRUE;
}


BEGIN_MESSAGE_MAP(CreateAccountWindow, CDialogEx)
	ON_BN_CLICKED(IDC_CREATE_ACCOUNT_BUTTON, &CreateAccountWindow::OnBnClickedCreateAccountButton)
	ON_BN_CLICKED(IDC_LOGIN_BUTTON, &CreateAccountWindow::OnBnClickedLoginButton)
	ON_BN_CLICKED(IDC_DELETE_ACCOUNT_BUTTON, &CreateAccountWindow::OnBnClickedDeleteAccountButton)
END_MESSAGE_MAP()

void CreateAccountWindow::OnBnClickedCreateAccountButton()
{
	CString input_account_name = L"";
	GetDlgItemText(IDC_INPUT_NAME_TEXT_EDITOR, input_account_name);
	
	if (input_account_name == L"") {
		return;
	}
	else {
		int account_index = m_account_list_box.FindStringExact(-1, input_account_name);
		if (account_index == LB_ERR) {
			m_account_list_box.InsertString(0, input_account_name);
			m_account_list_box.SetCurSel(0);
			m_account_name_vec.insert(m_account_name_vec.begin(), input_account_name);
			utils.WriteAccountNameCSV(m_account_name_scv_path, m_account_name_vec);
		}
		else {
			AfxMessageBox(_T("중복된 이름의 계정이 존재합니다."));
		}
	};

	SetDlgItemText(IDC_INPUT_NAME_TEXT_EDITOR, _T(""));
	if (m_account_list_box.GetCount() != 0) {
		GetDlgItem(IDC_LOGIN_BUTTON)->EnableWindow(TRUE);
	}
}

void CreateAccountWindow::OnBnClickedDeleteAccountButton()
{
	CString deleted_account_name;
	int account_index = m_account_list_box.GetCurSel();
	m_account_list_box.GetText(account_index, deleted_account_name);
	m_account_list_box.DeleteString(account_index);

	std::vector<CString>::iterator find_account_name = std::find(m_account_name_vec.begin(), m_account_name_vec.end(), deleted_account_name);
	m_account_name_vec.erase(find_account_name);
	
	utils.WriteAccountNameCSV(m_account_name_scv_path, m_account_name_vec);

	if (m_account_list_box.GetCount() == 0) {
		GetDlgItem(IDC_LOGIN_BUTTON)->EnableWindow(FALSE);
	}
	else {
		m_account_list_box.SetCurSel(0);
	}
}

void CreateAccountWindow::OnBnClickedLoginButton()
{
	int account_index = m_account_list_box.GetCurSel();
	if (account_index != LB_ERR) {
		m_account_list_box.GetText(account_index, m_account_name);
		CDialog::OnOK();
	}
	else {
		AfxMessageBox(_T("계정 이름을 선택해 주세요."));
	}
}


