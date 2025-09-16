// CreateIDWindow.cpp: 구현 파일
//

#include "vesselAnalysis.h"
#include "CreateIDWindow.h"
#include "afxdialogex.h"


// CreateIDWindow 대화 상자

IMPLEMENT_DYNAMIC(CreateIDWindow, CDialogEx)

CreateIDWindow::CreateIDWindow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CREATE_ID_WINDOW_DIALOG, pParent)
{

}

void CreateIDWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ID_LIST, m_id_list_box);
	DDX_Control(pDX, IDC_INPUT_ID_TEXT, m_input_name_text);
	DDX_Control(pDX, IDC_ID_INPUT_STATIC_TEXT, m_input_name_static_text);
}


BEGIN_MESSAGE_MAP(CreateIDWindow, CDialogEx)
	
	ON_LBN_SELCHANGE(IDC_ID_LIST, &CreateIDWindow::OnLbnSelchangeIdList)
END_MESSAGE_MAP()

BOOL CreateIDWindow::OnInitDialog() {

}

void 

