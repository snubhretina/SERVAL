
#pragma once
#include "stdafx.h"
#include "utils.h"
#include <io.h>
#include <string.h>
#include "VesselImgTransfer.h"


int CUtils::LoadDataList(DataPathContainer &Path, std::vector<FundusImageData> &fundus_img_data_vec, CListBox &fundus_image_list_box)
{
	CString fundus_data_path(Path.fundus_img_path.c_str());
	CString result_data_path(Path.result_path.c_str());

	fundus_image_list_box.ResetContent();

	std::vector<std::string> fundus_path_vec;
	std::vector<std::string> fundus_name_vec;
	
	std::vector<CString>::iterator format = Path.fundus_image_format_vec.begin();
	for (format ; format != Path.fundus_image_format_vec.end(); format++)
	{
		FindFilesInDir(fundus_data_path, *format, fundus_path_vec, fundus_name_vec);
	}
	
	std::string check_point_root_path;
	std::string fundus_name;
	std::string work_process_path;
	std::string img_result_path;
	CString fundus_name_cstr;

	for (int img_idx = 0; img_idx < fundus_path_vec.size(); img_idx++)
	{
		FundusImageData fundus_img_data;
		
		fundus_img_data.fundus_path = fundus_path_vec[img_idx];
		fundus_img_data.SetEBSize(editing_board_size);

		
		fundus_name = fundus_name_vec[img_idx];
		fundus_img_data.fundus_name = std::string(fundus_name.begin(), fundus_name.end() - 4);
		fundus_img_data.fundus_format = std::string(fundus_name.end() - 4, fundus_name.end());
		
		check_point_root_path = Path.editing_img_path + fundus_img_data.fundus_name + "\\";
		if (!IsPathExist(check_point_root_path.c_str())) {
			MakeDirecotry(check_point_root_path);
		}

		fundus_img_data.AV_mask_path = Path.init_mask_img_path + fundus_img_data.fundus_name + ".png";
		if (IsFileExist(fundus_img_data.AV_mask_path.c_str()))
			fundus_img_data.is_finished_preprocess = TRUE;
		else
			fundus_img_data.is_finished_preprocess = FALSE;


		img_result_path = Path.result_mask_path + fundus_img_data.fundus_name + ".png";
		if (!IsFileExist(img_result_path.c_str())) {
			//MakeDirecotry(img_result_path);
			fundus_img_data.is_finished_editing = FALSE;
		}
		else
		{
			fundus_img_data.is_finished_editing = TRUE;
		}

		fundus_img_data_vec.push_back(fundus_img_data);
		fundus_name_cstr = CString(fundus_name.c_str());
		fundus_image_list_box.InsertString(-1, fundus_name_cstr);
	}
	return 0;
}

void CUtils::FindFilesInDir(CString &dir, CString &ext, std::vector<std::string> &path_list, std::vector<std::string> &file_name_list)
{
	CFileFind finder;
	CString  strFolderPath = dir + "\\*" + ext;
	BOOL bWorking = finder.FindFile(strFolderPath);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strA = finder.GetFileName();
		if ((strA == L".") || (strA == L"..")) continue;

		if (finder.IsDirectory()) 
		{
			FindFilesInDir(finder.GetFilePath(), ext, path_list, file_name_list);
		}
		else 
		{
			CString file_path_cstr = finder.GetFilePath();
			CString file_name_cstr = finder.GetFileName();
			
			std::string file_path = CT2CA(file_path_cstr.operator LPCWSTR());
			std::string file_name = CT2CA(file_name_cstr.operator LPCWSTR());
			path_list.push_back(file_path);
			file_name_list.push_back(file_name);
		}
	}
}

bool CUtils::FindFilesInDir(CString &dir, CString &fname, std::string &path, std::string &file_name)
{
	CFileFind finder;
	CString str = '\\' + fname;
	CString  strFolderPath = dir + str;
	BOOL bWorking = finder.FindFile(strFolderPath);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strA = finder.GetFileName();
		if ((strA == L".") || (strA == L".."));
		else
		{
			CString strFilePath = finder.GetFilePath();
			CString strFileName = finder.GetFileName();
			std::string filePath = CT2CA(strFilePath.operator LPCWSTR());
			std::string fileName = CT2CA(strFileName.operator LPCWSTR());
			path = filePath;
			file_name = fileName;
			return true;
		}
	}
	return false;
}

bool CUtils::DeleteFilesInDir(CString dir, CString fname)
{
	CFileFind finder;
	CString str = '\\' + fname;
	CString  strFolderPath = dir + str;
	BOOL bWorking = finder.FindFile(strFolderPath);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strA = finder.GetFileName();
		if ((strA == L".") || (strA == L".."));
		else
		{
			CString strFilePath = finder.GetFilePath();
			remove(CT2CA(strFilePath.operator LPCWSTR()));
		}
	}
	return false;
}

bool CUtils::DeleteAllFilesInDir(CString dir)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile(dir);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strA = finder.GetFileName();
		if ((strA == L".") || (strA == L".."));
		else
		{
			CString strFilePath = finder.GetFilePath();
			remove(CT2CA(strFilePath.operator LPCWSTR()));
		}
	}
	return false;
}

bool CUtils::IsFileExist(const char *file_name)
{
	std::ifstream infile(file_name);
	return infile.good();
}

bool CUtils::IsPathExist(const char *path_name)
{	
	int Result = _access(path_name, 0);
	if (Result == 0) {
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CUtils::MakeDirecotry(std::string &path) {
	std::string sub_dir;
	size_t pos = 0;
	while ((pos = path.find_first_of("\\", pos)) != std::string::npos) {
		sub_dir = path.substr(0, pos++);
		if (sub_dir.size() == 0) {
			continue;
		}
		if (!IsPathExist(sub_dir.c_str())) 
		{
			std::wstring sub_dir_wstr = std::wstring(sub_dir.begin(), sub_dir.end());
			_wmkdir(sub_dir_wstr.c_str());
		}
	}

}

void CUtils::ReadCSV(std::string path, std::vector<CSVStruct> &CSV_list)
{
	std::ifstream  data(path);

	std::string line;
	while (std::getline(data, line))
	{
		std::vector<std::string> cur_cell_list;
		std::stringstream  lineStream(line);
		std::string        cell;
		while (std::getline(lineStream, cell, ','))
		{
			cur_cell_list.push_back(cell);
		}

		CSVStruct cur_row;
		cur_row.img_path = cur_cell_list[0];
		cur_row.disc = CPoint(std::stoi(cur_cell_list[1]), std::stoi(cur_cell_list[2]));
		cur_row.fovea = CPoint(std::stoi(cur_cell_list[3]), std::stoi(cur_cell_list[4]));
		if (cur_cell_list[5] == "L")
			cur_row.is_left = true;
		else
			cur_row.is_left = false;

		CSV_list.push_back(cur_row);
	}
}

void CUtils::ReadCSV(std::string path, std::vector<int> &CSV_list)
{
	std::ifstream  data(path);

	std::string line;
	while (std::getline(data, line))
	{
		std::vector<int> cur_cell_list;
		std::stringstream  lineStream(line);
		std::string        cell;
		while (std::getline(lineStream, cell, ','))
		{
			cur_cell_list.push_back(std::stoi(cell));
		}

		CSV_list.push_back(cur_cell_list[0]);
	}
}
void CUtils::ReadCSV(std::string path, std::vector<OD_info> &CSV_list, bool del_fist_row)
{
	std::ifstream  data(path);

	std::string line;
	int cnt = 0;
	std::vector <int> coord;

	OD_info a;
	
	while (std::getline(data, line))
	{
		cnt += 1;
		if (del_fist_row & (cnt == 1))
			continue;
		std::vector<cv::Point> cur_cell_list;
		std::stringstream  lineStream(line);
		std::string        cell;
		while (std::getline(lineStream, cell, ','))
		{
			coord.push_back(std::stoi(cell));
		}
		a.center = cv::Point(coord[0], coord[1]);
		a.diameter = coord[2];
		CSV_list.push_back(a);
		coord.clear();

		//CSV_list.push_back(cur_cell_list);

	}
}

void CUtils::ReadCSV(std::string path, std::vector<std::vector<int>> &CSV_list)
{
	std::ifstream  data(path);

	std::string line;
	while (std::getline(data, line))
	{
		std::vector<int> cur_cell_list;
		std::stringstream  lineStream(line);
		std::string        cell;
		while (std::getline(lineStream, cell, ','))
		{
			cur_cell_list.push_back(std::stoi(cell));
		}

		CSV_list.push_back(cur_cell_list);
	}
}

void CUtils::ReadCSV(std::string path, std::vector<cv::Point> &CSV_list)
{
	std::vector<int> pt;

	std::ifstream  data(path);
	if (data.fail()) {
		return;
	}
	std::string line;
	while (std::getline(data, line))
	{
		std::vector<cv::Point> cur_cell_list;
		std::stringstream  lineStream(line);
		std::string        cell;
		while (std::getline(lineStream, cell, ','))
		{
			pt.push_back(std::stoi(cell));
		}
		CSV_list.push_back(cv::Point(pt[0], pt[1]));
		pt.clear();
	}
}


// Override ReadCSV - 231114 SJ.Go
void CUtils::ReadCSV(std::string path, FundusImageData &fundus_img_data)
{
	std::ifstream  data(path);
	std::string line;
	std::vector<float> coord;

	while (std::getline(data, line))
	{

		std::stringstream lineStream(line);
		std::string cell;

		// Read OpticDisc Information
		// The ellipse information may not be available.
		while (std::getline(lineStream, cell, ','))
		{
			coord.push_back(std::stof(cell));
		}
	}

	int y, x;

	if (fundus_img_data.pad_axis == 0)
	{
		x = int(coord[1]);
		y = int(coord[0]) + fundus_img_data.pad_start_point;
	}
	else if (fundus_img_data.pad_axis == 1)
	{
		x = int(coord[1]) + fundus_img_data.pad_start_point;
		y = int(coord[0]);
	}
	else
	{
		x = int(coord[1]);
		y = int(coord[0]);
	}
	Eigen::VectorXd cur_OD_Ellipse = Eigen::VectorXd::Zero(6);
	cur_OD_Ellipse << y, x, coord[2], coord[3], coord[4], coord[5];

	fundus_img_data.SetOriDiscInfo(cur_OD_Ellipse);
}

void CUtils::WriteCSV(std::string path, std::vector<std::string> fundus_list, std::vector<OD_info>& CSV_list)
{
	FILE *f;
	fopen_s(&f, path.c_str(), "w");
	OD_info a;
	for (int i = 0; i < fundus_list.size(); i++)
	{
		fprintf(f, "%d,", 0);
		fprintf(f, "%d,", 0);
		fprintf(f, "%d,", 80);
		fprintf(f, "\n");
		a.center = cv::Point((0, 0));
		a.diameter = 80;
		CSV_list.push_back(a);
	}
	fclose(f);
}


void CUtils::WriteCSV(std::string path, std::vector<FundusImageData> &CSV_list)
{
	FILE *f;
	fopen_s(&f, path.c_str(), "w");
	for (int i = 0; i < CSV_list.size(); i++)
	{
		fprintf(f, "%d, %d, %d\n", CSV_list[i].disc_center_point.x, CSV_list[i].disc_center_point.y, CSV_list[i].disc_diameter);
	}
	fclose(f);
}

void CUtils::WriteCSV_metric(std::string path, std::string filename, std::string analysis_date, cv::Point opt_pos, std::vector<double> &metric, std::string comment)
{
	FILE *f, *f1, *f2;
	fopen_s(&f, path.c_str(), "a");
	fprintf(f, "%s, %s,", filename.c_str(), analysis_date.c_str());
	fprintf(f, "%d, %d,", int(opt_pos.x), int(opt_pos.y));
	for (int i = 0; i < metric.size(); i++)
	{
		fprintf(f, "%f,", metric[i]);
	}
	fprintf(f, "%s", comment.c_str());
	fprintf(f, "\n");
	fclose(f);
}

void CUtils::WriteCSV_metric(std::string path)
{
	FILE *f;
	fopen_s(&f, path.c_str(), "w");
	fprintf(f, "%s, %s, %s, %s,", "filename", "analysis_date", "x", "y");
	fprintf(f, "%s, %s, %s, %s, %s, %s, %s, %s, %s\n", "BMWa", "BSTDWa", "CMWa", "CSTDWa", "BMWv", "BSTDWv", "CMWv", "CSTDWv", "Comment");
	fclose(f);
}

void CUtils::WriteCSV(std::string path, std::vector<std::vector<int>> CSV_list)
{
	FILE *f;
	fopen_s(&f, path.c_str(), "w");

	for (int i = 0; i < CSV_list.size(); i++)
	{
		for (int j = 0; j < CSV_list[i].size(); j++)
			fprintf(f, "%d,", CSV_list[i][j]);
		fprintf(f, "\n");
	}

	fclose(f);
}

void CUtils::WriteCSV(std::string path, std::vector<cv::Point> CSV_list)
{
	if (CSV_list.empty()) {
		return;
	}
	FILE *f;
	fopen_s(&f, path.c_str(), "w");

	for (int i = 0; i < CSV_list.size(); i++)
	{
		fprintf(f, "%d, %d\n", CSV_list[i].x, CSV_list[i].y);
	}

	fclose(f);
}

// Override WriteCSV - 231115 SJ.Go
// save optic disc information.
//void CUtils::WriteCSV(std::string path, FundusImageData &CSV_list)
//{
//	FILE *f;
//	fopen_s(&f, path.c_str(), "w");
//
//	if (CSV_list.is_ellipse_fit == true) {
//		Eigen::VectorXd m_cartpool = CSV_list.ori_disk_ellipse_inform;
//		fprintf(f, "%f, %f, %d, %f, %f, %f \n", m_cartpool(0), m_cartpool(1), CSV_list.disc_diameter, m_cartpool(2), m_cartpool(3), m_cartpool(4));
//	}
//	else {
//		fprintf(f, "%d, %d, %d\n", CSV_list.ori_disc_center_point.y, CSV_list.ori_disc_center_point.x, CSV_list.ori_disc_diameter);
//	}
//	fclose(f);
//}

// Override WriteCSV - 240801 JM.Chae
// save optic disc information.
void CUtils::WriteCSV(std::string path, FundusImageData &CSV_list)
{
	double x, y;
	FILE *f;
	fopen_s(&f, path.c_str(), "w");

	Eigen::VectorXd m_cartpool = CSV_list.ori_disk_ellipse_inform;
	if (CSV_list.pad_axis == 0)
	{
		x = m_cartpool(1);
		y = m_cartpool(0) - CSV_list.pad_start_point;
	}
	else
	{
		x = m_cartpool(1) - CSV_list.pad_start_point;
		y = m_cartpool(0);
	}

	fprintf(f, "%f, %f, %d, %f, %f, %f \n", y, x, CSV_list.ori_disc_diameter, m_cartpool(2), m_cartpool(3), m_cartpool(4));
	fclose(f);
}

void CUtils::WriteAccountNameCSV(std::string &account_name_csv_path, std::vector<CString> &account_name_vec)
{
	FILE *account_name_csv_file;
	std::wstring account_name_str;

	fopen_s(&account_name_csv_file, account_name_csv_path.c_str(), "w");

	if (account_name_csv_file != NULL)
	{
		for (std::vector<CString>::iterator account_name = account_name_vec.begin(); account_name != account_name_vec.end(); account_name++)
		{
			fwprintf_s(account_name_csv_file, *account_name + L"\n");
		}
		fclose(account_name_csv_file);
	}
	else
	{
		AfxMessageBox(L"������ ������ �� �����ϴ�.", MB_OK);
	}
}

void CUtils::WriteAccountNameCSV(std::string &account_name_csv_path)
{
	FILE *account_name_csv_file;
	CString account_name_wstr = L"";

	fopen_s(&account_name_csv_file, account_name_csv_path.c_str(), "w");

	if (account_name_csv_file != NULL)
	{
		fwprintf_s(account_name_csv_file, account_name_wstr);
		fclose(account_name_csv_file);
	}
	else
	{
		AfxMessageBox(L"������ ������ �� �����ϴ�.", MB_OK);
	}
}

void CUtils::ReadAccountNameCSV(std::string &account_name_csv_path, std::vector<CString> &account_name_vec)
{
	std::wifstream  account_name_csv_file(account_name_csv_path);
	
	if (account_name_csv_file.fail()) {
		return;
	}
	
	std::wstring account_name;
	while (std::getline(account_name_csv_file, account_name, L'\n'))
	{		
		account_name_vec.insert(account_name_vec.begin(), CString(account_name.c_str()));
	}
	account_name_csv_file.close();
}

void CUtils::LoadFundusAndPreprocess(FundusImageData &fundus_img_data, cv::Mat &m_cur_editing_origin_img)
{
	cv::Mat origin_img = cv::imread(fundus_img_data.fundus_path, cv::IMREAD_COLOR);
	cv::Mat padded_img;

	fundus_img_data.ori_fundus_cols = origin_img.cols;
	fundus_img_data.ori_fundus_rows = origin_img.rows;

	PadImage(origin_img, padded_img, fundus_img_data);
	//cv::resize(padded_img, m_cur_editing_EB_img, editing_board_size, cv::InterpolationFlags::INTER_CUBIC);

	padded_img.copyTo(m_cur_editing_origin_img);

	/*fundus_img_data.to_EB_ratio_x = float(editing_board_size.width) / padded_img.cols;*/

	fundus_img_data.SetAspectRatio();

	origin_img.release();
	padded_img.release();
}

void CUtils::LoadMaskAndPreprocess(std::string check_point_path, FundusImageData &fundus_img_data, cv::Mat &m_origin_mask)
{
	cv::Mat origin_mask = cv::imread(check_point_path, cv::IMREAD_COLOR);
	cv::Mat padded_mask;

	PadImage(origin_mask, padded_mask, fundus_img_data);

	padded_mask.copyTo(m_origin_mask);
}

void CUtils::PadImage(cv::Mat &origin_image, cv::Mat &padded_image, FundusImageData &fundus_img_data)
{
	int width, height;
	width = origin_image.cols;
	height = origin_image.rows;
	float aspect_ratio = (float) width / height;
	float EB_aspect_ratio =  (float) editing_board_size.width / editing_board_size.height;

	if (aspect_ratio != EB_aspect_ratio)
		if (aspect_ratio > EB_aspect_ratio) {
			fundus_img_data.pad_length = int(round(width / EB_aspect_ratio));
			fundus_img_data.pad_axis = 0;
		}
		else if (aspect_ratio < EB_aspect_ratio) {
			fundus_img_data.pad_length = int(round(height * EB_aspect_ratio));
			fundus_img_data.pad_axis = 1;
		}
	else
	{
		fundus_img_data.pad_axis = 2;
		fundus_img_data.pad_length = 0;
	}

	if (fundus_img_data.pad_axis != 2) {
		//pad_aixs 0 is width longer then height 1.5 more, 1 is height longer then width more
		if (fundus_img_data.pad_axis == 0) {
			padded_image = cv::Mat::zeros(fundus_img_data.pad_length, width, origin_image.type());
			fundus_img_data.pad_start_point = (fundus_img_data.pad_length / 2 - height / 2) - 1;
			cv::Mat imageROI(padded_image, cv::Rect(0, fundus_img_data.pad_start_point, padded_image.cols, height));
			origin_image.copyTo(imageROI, origin_image);
		}
		else if (fundus_img_data.pad_axis == 1) {
			padded_image = cv::Mat::zeros(height, fundus_img_data.pad_length, origin_image.type());
			fundus_img_data.pad_start_point = (fundus_img_data.pad_length / 2 - width / 2) - 1;
			cv::Mat imageROI(padded_image, cv::Rect(fundus_img_data.pad_start_point, 0, width, padded_image.rows));
			origin_image.copyTo(imageROI, origin_image);
		}
		else
		{
			fundus_img_data.pad_start_point = 0;
			origin_image.copyTo(padded_image);
		}
	}
}

void CUtils::ThinVessel(cv::Mat &src, cv::Mat &dst)
{
	dst = src.clone();
	dst /= 255;         // convert to binary image

	cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
	cv::Mat diff;

	do {
		IterateThinningVessel(dst, 0);
		IterateThinningVessel(dst, 1);
		cv::absdiff(dst, prev, diff);
		dst.copyTo(prev);
	} while (cv::countNonZero(diff) > 0);

	dst *= 255;
}

void CUtils::IterateThinningVessel(cv::Mat &img, int iter)
{
	CV_Assert(img.channels() == 1);
	CV_Assert(img.depth() != sizeof(uchar));
	CV_Assert(img.rows > 3 && img.cols > 3);

	cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);

	int nRows = img.rows;
	int nCols = img.cols;

	if (img.isContinuous()) {
		nCols *= nRows;
		nRows = 1;
	}

	int x, y;
	uchar *pAbove;
	uchar *pCurr;
	uchar *pBelow;
	uchar *nw, *no, *ne;    // north (pAbove)
	uchar *we, *me, *ea;
	uchar *sw, *so, *se;    // south (pBelow)

	uchar *pDst;

	// initialize row pointers
	pAbove = NULL;
	pCurr = img.ptr<uchar>(0);
	pBelow = img.ptr<uchar>(1);

	for (y = 1; y < img.rows - 1; ++y) {
		// shift the rows up by one
		pAbove = pCurr;
		pCurr = pBelow;
		pBelow = img.ptr<uchar>(y + 1);

		pDst = marker.ptr<uchar>(y);

		// initialize col pointers
		no = &(pAbove[0]);
		ne = &(pAbove[1]);
		me = &(pCurr[0]);
		ea = &(pCurr[1]);
		so = &(pBelow[0]);
		se = &(pBelow[1]);

		for (x = 1; x < img.cols - 1; ++x) {
			// shift col pointers left by one (scan left to right)
			nw = no;
			no = ne;
			ne = &(pAbove[x + 1]);
			we = me;
			me = ea;
			ea = &(pCurr[x + 1]);
			sw = so;
			so = se;
			se = &(pBelow[x + 1]);

			int A = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) +
				(*ea == 0 && *se == 1) + (*se == 0 && *so == 1) +
				(*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
				(*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);
			int B = *no + *ne + *ea + *se + *so + *sw + *we + *nw;
			int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we);
			int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);

			if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
				pDst[x] = 1;
		}
	}

	img &= ~marker;
}

std::string CUtils::cur_time(bool is_change_time_format = FALSE) {
	CTime cTime = CTime::GetCurrentTime(); 
	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);
	std::string cur_time_string;

	char year[200];
	char mon[200];
	char day[200];
	char hour[200];
	char min[200];
	char sec[200];
	char msec[200];
	sprintf_s(year, "%04d", cTime.GetYear());
	sprintf_s(mon, "%02d", cTime.GetMonth());
	sprintf_s(day, "%02d", cTime.GetDay());
	sprintf_s(hour, "%02d", cTime.GetHour());
	sprintf_s(min, "%02d", cTime.GetMinute());
	sprintf_s(sec, "%02d", cTime.GetSecond());
	sprintf_s(msec, "%03ld", cur_time.wMilliseconds);

	if (!is_change_time_format)
	{
		cur_time_string = std::string(std::string(year) + std::string(mon) + std::string(day) +
		std::string(hour) + std::string(min) + std::string(sec) + std::string(msec));
	}
	else
	{
		cur_time_string = std::string(std::string(year) + std::string(mon) + std::string(day) + "-" + std::string(hour) + std::string(min) + std::string(sec));
	}

	return cur_time_string;
}

void CUtils::CollapsedFlip(cv::Mat *av_mask)
{
	cv::Mat seg_mask;
	cv::cvtColor(*av_mask, seg_mask, cv::COLOR_BGR2GRAY);
	cv::threshold(seg_mask, seg_mask, 0, 255, CV_THRESH_BINARY);
	cv::Mat labels, stats, centroids;
	cv::connectedComponentsWithStats(seg_mask, labels, stats, centroids);

	for (int i = 0; i < stats.rows; i++)
	{
		int x = stats.at<int>(cv::Point(0, i));
		int y = stats.at<int>(cv::Point(1, i));
		int w = stats.at<int>(cv::Point(2, i));
		int h = stats.at<int>(cv::Point(3, i));
		int pixel_count = stats.at<int>(cv::Point(4, i));

		if (pixel_count / float(w*h) > 0.5 && pixel_count < 50) {
			for (int cur_x = x; cur_x < x + w; cur_x++)
				for (int cur_y = y; cur_y < y + h; cur_y++) {
					av_mask->at<cv::Vec3b>(cur_y, cur_x)[0] = 0;
					av_mask->at<cv::Vec3b>(cur_y, cur_x)[2] = 0;
					seg_mask.at<uchar>(cur_y, cur_x) = 0;
				}
		}
	}
}

void CUtils::ReadCheckPoint(DataPathContainer &Path, FundusImageData &fundus_img_data, cv::Mat &origin_mask, std::vector<WorkProcessStruct> &work_process_vec)
{
	std::string check_point_root_path = Path.editing_img_path + fundus_img_data.fundus_name;
	std::string work_process_path = check_point_root_path + Path.work_process_check_point_name;

	if (!fundus_img_data.is_finished_editing)
	{
		work_process_vec.clear();

		if (IsFileExist(work_process_path.c_str()))
		{
			ReadWorkProcess(work_process_path, work_process_vec);
			
			std::string check_point_path = check_point_root_path + "\\" + work_process_vec.front().prev_editied_mask_path;
			LoadMaskAndPreprocess(check_point_path, fundus_img_data, origin_mask);
		}
		else
		{
			is_modified = TRUE;
			LoadMaskAndPreprocess(fundus_img_data.AV_mask_path, fundus_img_data, origin_mask);
			WriteCheckPoint(Path, origin_mask, fundus_img_data, work_process_vec);
		}

		std::string optic_disc_path = check_point_root_path + "\\" + Path.optic_disc_check_point_name;
		std::string elapsed_time_path = check_point_root_path + "\\" + Path.elapsed_time_check_point_name;

		ReadElapsedTime(elapsed_time_path, fundus_img_data);
		ReadOpticDiscCheckPoint(optic_disc_path, fundus_img_data);
	}
	else
	{
		std::string optic_disc_path = Path.result_disc_info_path + fundus_img_data.fundus_name  + "_" + Path.optic_disc_check_point_name;
		std::string elapsed_time_path = Path.result_elapsed_time_path + fundus_img_data.fundus_name + "_" +  Path.elapsed_time_check_point_name;
		std::string mask_path = Path.result_mask_path + fundus_img_data.fundus_name + ".png";

		if (IsFileExist(mask_path.c_str()))
		{
			LoadMaskAndPreprocess(mask_path, fundus_img_data, origin_mask);
			work_process_vec.clear();

			if (IsFileExist(work_process_path.c_str()))
			{
				ReadWorkProcess(work_process_path, work_process_vec);
			}
			else
			{
				WriteCheckPoint(Path, origin_mask, fundus_img_data, work_process_vec);
				ReadWorkProcess(work_process_path, work_process_vec);
			}

			ReadElapsedTime(elapsed_time_path, fundus_img_data);
			ReadOpticDiscCheckPoint(optic_disc_path, fundus_img_data);
		}
		else
		{
			if (IsFileExist(fundus_img_data.AV_mask_path.c_str()))
			{
				LoadMaskAndPreprocess(fundus_img_data.AV_mask_path, fundus_img_data, origin_mask);
				WriteCheckPoint(Path, origin_mask, fundus_img_data, work_process_vec);
				ReadElapsedTime(elapsed_time_path, fundus_img_data);
				ReadOpticDiscCheckPoint(optic_disc_path, fundus_img_data);

				fundus_img_data.is_finished_editing = FALSE;
			}
			else
			{
				fundus_img_data.is_finished_preprocess = FALSE;
				fundus_img_data.is_finished_editing = FALSE;
			}
		}

	}
	//	if (IsFileExist(work_process_path.c_str()))
	//	{
	//		std::string check_point_path = check_point_root_path + work_process_vec.front().prev_editied_mask_path;

	//		if (!IsFileExist(result_mask_path.c_str()))
	//		{
	//			result_mask_path = result_root_path + "\\" + fundus_img_data.fundus_name + ".png";

	//			cv::Mat init_mask = cv::imread(result_mask_path);
	//			is_modified = TRUE;
	//			WriteCheckPoint(Path, init_mask, fundus_img_data, work_process_vec);
	//		}
	//
	//		if (!IsFileExist(elapsed_time_path.c_str()))
	//		{
	//			elapsed_time_path = check_point_root_path + Path.elapsed_time_check_point_name;
	//		}

	//		ReadElapsedTime(elapsed_time_path, fundus_img_data);

	//		if (IsFileExist(optic_disc_path.c_str()))
	//		{
	//			optic_disc_path = check_point_root_path + Path.optic_disc_check_point_name;
	//		}
	//		ReadOpticDiscCheckPoint(optic_disc_path, fundus_img_data);
	//	}
	//	else
	//	{
	//		is_modified = TRUE;
	//		cv::Mat init_mask = cv::imread(fundus_img_data.AV_mask_path);
	//		WriteCheckPoint(Path, init_mask, fundus_img_data, work_process_vec);
	//	}
	//}

}

bool CUtils::ReadWorkProcess(std::string &work_process_path, std::vector<WorkProcessStruct> &work_process_vec)
{
	FILE *work_process_check_point_csv_file;
	fopen_s(&work_process_check_point_csv_file, (work_process_path).c_str(), "r");

	if (work_process_check_point_csv_file == NULL) {
		return FALSE;
	}
	char work_process_path_line[1024];

	while (fgets(work_process_path_line, 1024, work_process_check_point_csv_file)) {
		WorkProcessStruct work_proc;

		work_process_path_line[strcspn(work_process_path_line, "\n")] = 0;
		std::string work_process_line(work_process_path_line);
		
		std::size_t pos = work_process_line.find("\\");
		if (pos != std::string::npos)
		{
			std::istringstream iss(work_process_line);
			std::string buffer;
			std::vector<std::string> splited_str;
			
			char delimiter = '\\\\';

			while (getline(iss, buffer, delimiter))
			{
				splited_str.push_back(buffer);
			}
			work_proc.prev_editied_mask_path = splited_str.back();
		}
		else
		{
			work_proc.prev_editied_mask_path = work_process_line;
		}
		work_process_vec.push_back(work_proc);
	}
	fclose(work_process_check_point_csv_file);

	return TRUE;
}

void CUtils::RemoveOldCheckPoint(std::string &check_point_root_path, std::vector<WorkProcessStruct> &work_process_vec)
{
	CString work_process_path_cstr = CString(check_point_root_path.c_str()) + L"\\*.png";
	CFileFind finder;
	BOOL bWorking = finder.FindFile(work_process_path_cstr);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strA = finder.GetFileName();
		if ((strA == L".") || (strA == L"..")) continue;

		if (finder.IsDirectory())
		{
			continue;
		}
		else
		{
			CString file_path_cstr = finder.GetFilePath();
			std::string file_path = CT2CA(file_path_cstr.operator LPCWSTR());
		}
	}
}

bool CUtils::ReadElapsedTime(std::string &elapsed_path, FundusImageData &fundus_img_data)
{
	std::ifstream file(elapsed_path);
	if (!file.is_open()) {
		WriteInitElapsedTime(elapsed_path);

		fundus_img_data.elapsed_time = CTimeSpan(0, 0, 0, 0);
		return true;
	}
	else
	{
		std::string line;
		std::getline(file, line); // Skip header line
		std::string lastLine;
		while (std::getline(file, line)) {
			lastLine = line;
		}

		std::istringstream iss(lastLine);
		std::string save_time, hour, minute, second;

		if (std::getline(iss, save_time, ',') &&
			std::getline(iss, hour, ',') &&
			std::getline(iss, minute, ',') &&
			std::getline(iss, second, ',')) {
			// Process the data here
			fundus_img_data.elapsed_time = CTimeSpan(0, std::stoi(hour), std::stoi(minute), std::stoi(second));
		}
		file.close();
		return true;
	}
}

void CUtils::WriteInitElapsedTime(std::string &elapsed_time_path)
{
	std::ofstream elapsed_time_file(elapsed_time_path);
	if (elapsed_time_file.is_open()) {
		elapsed_time_file << "Save_Time, Hours, Minutes, Seconds" << std::endl;
		elapsed_time_file << cur_time(TRUE) << ", 0, 0, 0"  << std::endl;
		elapsed_time_file.close();
	}
}

bool CUtils::ReadOpticDiscCheckPoint(std::string &optic_disc_path, FundusImageData &fundus_img_data)
{
	std::ifstream data(optic_disc_path);
	std::string line;

	while (std::getline(data, line))
	{
		std::vector<float> coord;
		std::stringstream lineStream(line);
		std::string cell;

		// Read OpticDisc Information
		// The ellipse information may not be available.
		while (std::getline(lineStream, cell, ','))
		{
			coord.push_back(std::stof(cell));
		}
		if (fundus_img_data.pad_axis == 0)
		{
			fundus_img_data.ori_disc_center_point = cv::Point(int(coord[1]), int(coord[0]) + fundus_img_data.pad_start_point);
		}
		else if (fundus_img_data.pad_axis == 1)
		{
			fundus_img_data.ori_disc_center_point = cv::Point(int(coord[1]) + fundus_img_data.pad_start_point, int(coord[0]));
		}
		else
		{
			fundus_img_data.ori_disc_center_point = cv::Point(int(coord[1]), int(coord[0]));
		}
		fundus_img_data.ori_disc_diameter_x = coord[3];
		fundus_img_data.ori_disc_diameter_y = coord[4];

		fundus_img_data.ori_disc_diameter = max(fundus_img_data.ori_disc_diameter_x, fundus_img_data.ori_disc_diameter_y);
		fundus_img_data.ori_disc_angle = coord[5];
		fundus_img_data.is_ellipse_fit = TRUE;

		fundus_img_data.SetOriDiscInfoFromCheckPoint();		
		fundus_img_data.ConvertOriEllipseInfoToEB();
		coord.clear();
	}

	return TRUE;
}

cv::Mat CUtils::UnpadImage(FundusImageData &fundus_img_data, cv::Mat padded_img)
{
	cv::Mat unpadded_img;
	if (fundus_img_data.pad_axis == 0)
	{
		unpadded_img = padded_img(cv::Rect(0, fundus_img_data.pad_start_point, fundus_img_data.ori_fundus_cols, fundus_img_data.ori_fundus_rows));
	}
	else
	{
		unpadded_img = padded_img(cv::Rect(fundus_img_data.pad_start_point, 0, fundus_img_data.ori_fundus_cols, fundus_img_data.ori_fundus_rows));
	}
	return unpadded_img;
}

void CUtils::WriteCheckPoint(DataPathContainer &Path, cv::Mat &cur_editing_mask, FundusImageData &fundus_img_data, std::vector<WorkProcessStruct> &work_process_vec)
{
	std::string check_point_root_path = Path.editing_img_path + fundus_img_data.fundus_name;

	if (is_modified)
	{
		std::string editing_mask_name = cur_time() + ".png";
		std::string editing_mask_path = check_point_root_path + "\\" + editing_mask_name;
		std::string work_process_path = check_point_root_path + Path.work_process_check_point_name;

		cv::Mat unpadded_mask = UnpadImage(fundus_img_data, cur_editing_mask);
		
		WriteEditingMask(editing_mask_path, unpadded_mask);
		
		RecordWorkProcess(check_point_root_path, editing_mask_name, work_process_vec);
		WriteWorkProcess(work_process_path, editing_mask_path, work_process_vec);
		is_modified = FALSE;
	}
}

void CUtils::WriteEditingMask(std::string &editing_mask_path, cv::Mat &cur_editing_mask)
{
	
	cv::imwrite(editing_mask_path, cur_editing_mask);
}

void CUtils::RecordWorkProcess(std::string & check_point_path, std::string &editing_mask_name, std::vector<WorkProcessStruct> &work_process_vec)
{
	WorkProcessStruct cur_work_proc;
	cur_work_proc.prev_editied_mask_path = editing_mask_name;
	
	if (work_process_vec.size() == 0)
	{
		work_process_vec.insert(work_process_vec.begin(), cur_work_proc);
		work_process_idx = 0;
	}
	else
	{
		std::vector<WorkProcessStruct> erase_data_vector(work_process_vec.begin(), work_process_vec.begin() + work_process_idx);
		for (WorkProcessStruct work_process : erase_data_vector)
		{
			std::string before_check_point_path = check_point_path + "\\" + work_process.prev_editied_mask_path;
			std::remove(before_check_point_path.c_str());
		}

		work_process_vec.erase(work_process_vec.begin(), work_process_vec.begin() + work_process_idx);
		work_process_vec.insert(work_process_vec.begin(), cur_work_proc);

		if (work_process_vec.size() > work_process_num)
		{
			std::vector<WorkProcessStruct> trash_work_process(work_process_vec.begin() + work_process_num, work_process_vec.end());
			for (WorkProcessStruct work_process : trash_work_process)
			{
				std::string before_check_point_path = check_point_path + "\\" + work_process.prev_editied_mask_path;
				std::remove(before_check_point_path.c_str());
			}
			work_process_vec.erase(work_process_vec.begin() + work_process_num, work_process_vec.end());
			trash_work_process.clear();
		}
		work_process_idx = 0;
	}
}

void CUtils::WriteElapsedTime(std::string &elapsed_time_path, CTimeSpan &elapsed_time_check_point)
{
	std::ofstream elapsed_time_file(elapsed_time_path, std::ios::app); // Open the file in append mode
	if (elapsed_time_file.is_open()) {
		elapsed_time_file << "Save_Time, Hours, Minutes, Seconds" << std::endl;
		elapsed_time_file << cur_time(TRUE) << ", " << elapsed_time_check_point.GetHours() << ", " << elapsed_time_check_point.GetMinutes() << ", " << elapsed_time_check_point.GetSeconds() << std::endl;
		elapsed_time_file.close();
	}
}

void CUtils::WriteElapsedTime(DataPathContainer &Path, FundusImageData &fundus_img_data)
{
	std::string elapsed_time_path = Path.editing_img_path + fundus_img_data.fundus_name + "\\" + Path.elapsed_time_check_point_name;
	CTimeSpan elapsed_time_check_point = fundus_img_data.elapsed_time;

	std::ofstream elapsed_time_file(elapsed_time_path, std::ios::app); // Open the file in append mode
	if (elapsed_time_file.is_open()) {
		elapsed_time_file << cur_time(TRUE) << ',' << elapsed_time_check_point.GetHours() << "," << elapsed_time_check_point.GetMinutes() << ", " << elapsed_time_check_point.GetSeconds() << std::endl;
		elapsed_time_file.close();
	}
}

void CUtils::WriteWorkProcess(std::string &work_process_path, std::string &editing_mask_path, std::vector<WorkProcessStruct> &work_process_vec)
{
	FILE *work_process_check_point_csv_file;
	fopen_s(&work_process_check_point_csv_file, (work_process_path).c_str(), "w");

	for (WorkProcessStruct work_process : work_process_vec)
	{
		fprintf(work_process_check_point_csv_file, "%s\n", work_process.prev_editied_mask_path.c_str());
	}
	fclose(work_process_check_point_csv_file);
}

void CUtils::WriteWorkProcess(std::string &work_process_path, std::string &editing_mask_path)
{
	FILE *work_process_check_point_csv_file;
	fopen_s(&work_process_check_point_csv_file, (work_process_path).c_str(), "w");
	fprintf(work_process_check_point_csv_file, "%s\n", editing_mask_path.c_str());
	fclose(work_process_check_point_csv_file);
}

void CUtils::ResetCheckPoint(DataPathContainer &Path, FundusImageData &fundus_img_data, std::vector<WorkProcessStruct> &work_process_vec)
{
	std::string editing_mask_name = cur_time() + ".png";

	std::string check_point_root_path = Path.editing_img_path + fundus_img_data.fundus_name + "\\";
	std::string center_csv_path = Path.init_mask_img_path + fundus_img_data.fundus_name + "_center.csv";

	const char* check_point_root_path_cchar = check_point_root_path.c_str();
	std::string elapsed_time_path = check_point_root_path + Path.elapsed_time_check_point_name;

	if (IsPathExist(check_point_root_path_cchar))
	{
		std::string editing_mask_path = check_point_root_path + "\\" + editing_mask_name;
		std::string work_process_path = check_point_root_path + Path.work_process_check_point_name;
		
		ReadCSV(center_csv_path, fundus_img_data);
		
		CString all_file(L"*.*");
		DeleteFilesInDir(CString(check_point_root_path_cchar), all_file);
		
		work_process_vec.clear();
		fundus_img_data.elapsed_time = CTimeSpan(0, 0, 0, 0);

		cv::Mat init_mask = cv::imread(fundus_img_data.AV_mask_path, cv::IMREAD_COLOR);
		
		WriteCSV(check_point_root_path + "\\" + Path.optic_disc_check_point_name, fundus_img_data);
		WriteEditingMask(editing_mask_path, init_mask);
		init_mask.release();

		RecordWorkProcess(check_point_root_path, editing_mask_name, work_process_vec);
		WriteWorkProcess(work_process_path, editing_mask_path, work_process_vec);
		WriteElapsedTime(elapsed_time_path, fundus_img_data.elapsed_time);
	}
	return;
}


cv::Mat CUtils::GetROIFromMask(cv::Mat &cur_editing_mask, cv::Mat &diameter_mask)
{
	cv::Mat ROI_mask;
	cur_editing_mask.copyTo(ROI_mask);
	ROI_mask.setTo(cv::Vec3b(0, 0, 0), diameter_mask);

	return ROI_mask;
}
	

void CUtils::SaveResult(DataPathContainer &Path, cv::Mat &unpadded_mask, cv::Mat &unpadded_ROI_mask, FundusImageData &fundus_img_data, vascularity *example)
{
	std::string metric_path = Path.result_metric_path + fundus_img_data.fundus_name + "_" + Path.parameter_json_name;
	std::string optic_disc_path = Path.result_disc_info_path + fundus_img_data.fundus_name + "_" + Path.optic_disc_check_point_name;
	std::string elapsed_time_path = Path.result_elapsed_time_path + fundus_img_data.fundus_name + "_" +Path.elapsed_time_check_point_name;

	cv::Mat splited_mask[3], splited_ROI_mask[3], ROI_mask;
	
	WriteCSV(optic_disc_path, fundus_img_data);
	WriteElapsedTime(elapsed_time_path, fundus_img_data.elapsed_time);

	/*example->write(metric_path);*/

	cv::split(unpadded_mask, splited_mask);
	cv::split(unpadded_ROI_mask, splited_ROI_mask);

	cv::imwrite(Path.result_mask_path + fundus_img_data.fundus_name + ".png", unpadded_mask);
	cv::imwrite(Path.result_mask_path + fundus_img_data.fundus_name + "_artery.png", splited_mask[2]);
	cv::imwrite(Path.result_mask_path + fundus_img_data.fundus_name + "_vein.png", splited_mask[0]);
	
	cv::imwrite(Path.result_ROI_mask_path + "ROI_" + fundus_img_data.fundus_name + ".png", unpadded_ROI_mask);
	cv::imwrite(Path.result_ROI_mask_path + "ROI_" + fundus_img_data.fundus_name + "_artery.png", splited_ROI_mask[2]);
	cv::imwrite(Path.result_ROI_mask_path + "ROI_" + fundus_img_data.fundus_name + "_vein.png", splited_ROI_mask[0]);
}

void CUtils::SaveResult(DataPathContainer &Path, cv::Mat &unpadded_mask, cv::Mat &unpadded_ROI_mask, FundusImageData &fundus_img_data)
{
	std::string metric_path = Path.result_metric_path + fundus_img_data.fundus_name + "_" + Path.parameter_json_name;
	std::string optic_disc_path = Path.result_disc_info_path + fundus_img_data.fundus_name + "_" + Path.optic_disc_check_point_name;
	std::string elapsed_time_path = Path.result_elapsed_time_path + fundus_img_data.fundus_name + "_" + Path.elapsed_time_check_point_name;

	cv::Mat splited_mask[3], splited_ROI_mask[3], ROI_mask;

	WriteCSV(optic_disc_path, fundus_img_data);
	WriteElapsedTime(elapsed_time_path, fundus_img_data.elapsed_time);

	cv::split(unpadded_mask, splited_mask);
	cv::split(unpadded_ROI_mask, splited_ROI_mask);

	cv::imwrite(Path.result_mask_path + fundus_img_data.fundus_name + ".png", unpadded_mask);
	cv::imwrite(Path.result_mask_path + fundus_img_data.fundus_name + "_artery.png", splited_mask[2]);
	cv::imwrite(Path.result_mask_path + fundus_img_data.fundus_name + "_vein.png", splited_mask[0]);

	cv::imwrite(Path.result_ROI_mask_path + "ROI_" + fundus_img_data.fundus_name + ".png", unpadded_ROI_mask);
	cv::imwrite(Path.result_ROI_mask_path + "ROI_" + fundus_img_data.fundus_name + "_artery.png", splited_ROI_mask[2]);
	cv::imwrite(Path.result_ROI_mask_path + "ROI_" + fundus_img_data.fundus_name + "_vein.png", splited_ROI_mask[0]);
}

void CUtils::RemoveResult(DataPathContainer &Path, FundusImageData &fundus_img_data)
{
	std::string mask_files_path = Path.result_mask_path + fundus_img_data.fundus_name + "_*.*";
	std::string mask_file_path = Path.result_mask_path + fundus_img_data.fundus_name + ".png";

	std::string ROI_mask_file_paths = Path.result_ROI_mask_path + "ROI_" + fundus_img_data.fundus_name + "_*.*";

	std::string metric_path = Path.result_metric_path + fundus_img_data.fundus_name + "_" + Path.parameter_json_name;
	std::string optic_disc_path = Path.result_disc_info_path + fundus_img_data.fundus_name + "_" + Path.optic_disc_check_point_name;
	std::string elapsed_time_path = Path.result_elapsed_time_path + fundus_img_data.fundus_name + "_" + Path.elapsed_time_check_point_name;

	CString mask_files_path_cstr(mask_files_path.c_str());
	CString mask_file_path_cstr(mask_file_path.c_str());
	CString ROI_mask_file_paths_cstr(ROI_mask_file_paths.c_str());
	CString metric_path_cstr(metric_path.c_str());
	CString optic_disc_path_cstr(optic_disc_path.c_str());
	CString elapsed_time_path_cstr(elapsed_time_path.c_str());
	
	DeleteAllFilesInDir(mask_files_path_cstr);
	DeleteAllFilesInDir(mask_file_path_cstr);
	DeleteAllFilesInDir(ROI_mask_file_paths_cstr);
	DeleteAllFilesInDir(metric_path_cstr);
	DeleteAllFilesInDir(optic_disc_path_cstr);
	DeleteAllFilesInDir(elapsed_time_path_cstr);
}

//void CUtils::SerializeData(vascularity example)
//{
//	example.
//}