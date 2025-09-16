// CreateNameWindow.cpp: 구현 파일
//

#include "vesselAnalysis.h"
#include "CreateNameWindow.h"
#include "afxdialogex.h"


// CreateNameWindow 대화 상자

IMPLEMENT_DYNAMIC(CreateNameWindow, CDialogEx)

CreateNameWindow::CreateNameWindow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CREATE_NAME_WINDOW_DIALOG, pParent)
{

}

CreateNameWindow::~CreateNameWindow()
{
}

void CreateNameWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CreateNameWindow, CDialogEx)
END_MESSAGE_MAP()


// CreateNameWindow 메시지 처리기
