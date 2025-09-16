#pragma once
#include "stdafx.h"
#include "vesselAnalysis.h"
#include "WaitWindowCalcParams.h"
#include "afxdialogex.h"

// WaitWindowCalcParams 대화 상자

class WaitWindowCalcParams : public CDialogEx
{
	DECLARE_DYNAMIC(WaitWindowCalcParams)

public:
	WaitWindowCalcParams(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~WaitWindowCalcParams();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WaitWindow_Calc_Params };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CStatic calc_params_static_text;
	CWinThread *calc_params_pthread, *viewing_static_text_ptrhead;

	static UINT ViewText(LPVOID _mothod);

	bool m_is_calculating;
	bool m_is_waiting;
	static UINT CalcParams(LPVOID _mothod);
}; 
