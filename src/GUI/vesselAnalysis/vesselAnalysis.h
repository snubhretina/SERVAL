
// vesselAnalysis.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CvesselAnalysisApp:
// �� Ŭ������ ������ ���ؼ��� vesselAnalysis.cpp�� �����Ͻʽÿ�.
//

class CVesselAnalysisApp : public CWinApp
{
public:
	CVesselAnalysisApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CVesselAnalysisApp theApp;