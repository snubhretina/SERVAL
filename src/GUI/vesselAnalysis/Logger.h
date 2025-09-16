#pragma once
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include "vesselAnalysis.h"
#include "utils.h"

using namespace std;

class CLogger
{
public:
	std::string log_path;
	CString time_format_cstr;
	CString cur_time_cstr;
	CString log_cstr;

	void CurTime();
	void WriteLog();
};

