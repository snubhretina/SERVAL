#pragma once


// CreateNameWindow 대화 상자

class CreateNameWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CreateNameWindow)

public:
	CreateNameWindow(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CreateNameWindow();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_NAME_WINDOW_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
