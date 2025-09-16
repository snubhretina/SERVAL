#pragma once
#include "afxwin.h"
//#include "vesselAnalysisDlg.h"


// CErrorReport dialog

class CErrorReport : public CDialogEx
{
	DECLARE_DYNAMIC(CErrorReport)

public:
	CErrorReport(CWnd* pParent = NULL);   // standard constructor
	virtual ~CErrorReport();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ErrorReport };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_edit_error, m_cstr_error;
};
