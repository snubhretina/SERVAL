#include "stdafx.h"
#include "Logger.h"
#include <string>

void CLogger::CurTime()
{
	CTime cTime = CTime::GetCurrentTime();
	cur_time_cstr.Format(time_format_cstr, cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
}

void CLogger::WriteLog()
{
	FILE *log_file;
	log_file = fopen(log_path.c_str() , "a+");

	CurTime();
	log_cstr = cur_time_cstr + log_cstr;

	if (log_file != NULL)
	{
		log_cstr = log_cstr;
		fprintf(log_file, "%S\n", log_cstr);
		fclose(log_file);
	}
}