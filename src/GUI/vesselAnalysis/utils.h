#pragma once
#include "afxwin.h"
#include <tuple>
#include <iostream>
#include <fstream>
#include "C:\opencv\build\include\opencv2\core\mat.hpp"
#include <Ctime>
#include "EllipseFit.h"
#include <sstream>
#include <cstdio>
#include <cmath>
#include "vascularity.hpp"

struct OD_info
{
	cv::Point center;
	int diameter;
	Eigen::VectorXd ellipse_info;
	BOOL is_ellipse;
};

struct CSVStruct
{
	std::string img_path;
	CPoint disc;
	CPoint fovea;
	bool is_left;
};

class DataPathContainer {
	public:
		std::string fundus_folder_name;
		std::string init_av_mask_folder_name;

		std::string result_folder_name;
		std::string result_mask_folder_name;
		std::string result_ROI_mask_folder_name;
		std::string result_metric_folder_name;
		std::string result_disc_info_folder_name;
		std::string result_elapsed_time_folder_name;

		std::string log_folder_name;
		std::string check_point_folder_name;
		std::string dataset_name;

		std::string optic_disc_check_point_name;
		std::string work_process_check_point_name;
		std::string elapsed_time_check_point_name;
		std::string parameter_json_name;

		std::string log_name;

		std::string data_root_path;
		std::string fundus_img_path;
		
		std::string result_mask_path;
		std::string result_ROI_mask_path;
		std::string result_metric_path;
		std::string result_disc_info_path;
		std::string result_elapsed_time_path;

		std::string result_path;
		std::string init_mask_img_path;
		std::string log_path;
		std::string editing_img_path;
		
		std::string test_cache_file_path;
		
		std::vector<CString> fundus_image_format_vec;
		std::string check_point_path;

		void SetCheckPointPath(std::string fundus_name)
		{
			check_point_path = editing_img_path + fundus_name + "\\";
		}

		void SetDirectoryPath(std::string &root_path, std::string &account_name)
		{
			vector<string> words;
			stringstream sstream(root_path);
			string word;

			while (getline(sstream, word, '\\'))
			{
				words.push_back(word);
			}

			data_root_path = root_path;
			dataset_name = words.back();
			fundus_img_path = root_path + fundus_folder_name;
			init_mask_img_path  = root_path + init_av_mask_folder_name + account_name + "\\";

			result_path = root_path + result_folder_name + account_name + "\\";
			result_mask_path = result_path + result_mask_folder_name + "\\";
			result_ROI_mask_path = result_path + result_ROI_mask_folder_name + "\\";
			result_metric_path = result_path + result_metric_folder_name + "\\";
			result_disc_info_path = result_path + result_disc_info_folder_name + "\\";
			result_elapsed_time_path = result_path + result_elapsed_time_folder_name + "\\";

			log_path = root_path + log_folder_name + account_name + "\\";
			editing_img_path = root_path + check_point_folder_name + account_name + "\\";
		}
};

struct FundusImageData
{
	// Common Image Data //
	std::string fundus_name;
	std::string fundus_format;
	std::string fundus_path;

	bool is_finished_preprocess;
	bool is_finished_editing;
	bool is_ellipse_fit;

	int pad_start_point;
	int origin_pad_start_point;

	int pad_length;
	int pad_axis;

	std::string AV_mask_path;

	CTimeSpan elapsed_time;

    // Editing Board Size Image Data //
	int EB_cols;
	int EB_rows;

	cv::Point disc_center_point;
	int disc_diameter;
	int disc_diameter_x;
	int disc_diameter_y;
	double disc_angle;

	int fov_x;
	int fov_y;
	int opt_fov_dist;
	double to_EB_ratio_x;
	double to_EB_ratio_y;

	Eigen::VectorXd disk_ellipse_inform = Eigen::VectorXd::Zero(5);

	// Original Size Image Data //
	int ori_fundus_cols;
	int ori_fundus_rows;

	bool iseditedEllipse;

	cv::Point ori_disc_center_point;
	int ori_disc_diameter_x;
	int ori_disc_diameter_y;
	int ori_disc_diameter;
	int ori_fov_x;
	int ori_fov_y;
	int ori_opt_fov_dist;

	double ori_disc_angle;
	double to_ori_ratio_x;
	double to_ori_ratio_y;

	double zoom_ratio;

	Eigen::VectorXd ori_disk_ellipse_inform = Eigen::VectorXd::Zero(5);

	// disk_ellipse_info -> m_cartpool
	// m_cartpool -> 0: center_y, 1: center_x, 2: diameter_x, 3: diameter_y, 4: angle
	void SetAspectRatio()
	{
		double EB_cols_double = double(EB_cols);
		double EB_rows_double = double(EB_rows);

		if (pad_axis == 0)
		{
			to_EB_ratio_x = EB_cols_double / ori_fundus_cols;
			to_EB_ratio_y = EB_rows_double / pad_length;

			to_ori_ratio_x = ori_fundus_cols / EB_cols_double;
			to_ori_ratio_y = pad_length / EB_rows_double;
		}
		else if (pad_axis == 1)
		{
			to_EB_ratio_x = EB_cols_double / pad_length;
			to_EB_ratio_y = EB_rows_double / ori_fundus_rows;

			to_ori_ratio_x =  pad_length / EB_cols_double;
			to_ori_ratio_y = ori_fundus_rows / EB_rows_double;
		}
		else
		{
			to_EB_ratio_x = EB_cols_double / ori_fundus_cols;;
			to_ori_ratio_y = ori_fundus_rows / EB_rows_double;
		}
	}
	
	std::tuple<cv::Point, int, int, int> CalcEllipseElements(int &x, int &y, double &ratio_x, double &ratio_y, int &diam_x, int &diam_y, bool to_EB_info = TRUE)
	{
		cv::Point center_point;
		int diameter_x;
		int diameter_y;
		int new_diameter;

		if (pad_axis == 0)
		{
			center_point.x = int(x * ratio_x);

			if (to_EB_info)
			{
				center_point.y = int((y + pad_start_point) * ratio_y);
			}
			else
			{
				center_point.y = int((y * ratio_y - pad_start_point));
			}

			center_point.y = int((y + pad_start_point) * ratio_y);
			diameter_x = int(diam_x * ratio_x);
			diameter_y = int(diam_y * ratio_y);
			new_diameter = max(diameter_x, diameter_y);
		}
		else if (pad_axis == 1)
		{
			if (to_EB_info)
			{
				center_point.x = int((x + pad_start_point) * ratio_x);
			}
			else
			{
				center_point.x = int((x * ratio_x - pad_start_point));
			}
			center_point.y = int(y * ratio_y);
			diameter_x = int(diam_x * ratio_x);
			diameter_y = int(diam_y * ratio_y);
			new_diameter = max(diameter_x, diameter_y);
		}
		else
		{
			center_point.x = int(x * ratio_x);
			center_point.y = int(y * ratio_y);
			diameter_x = int(diam_x * ratio_x);
			diameter_y = int(diam_y * ratio_y);
			new_diameter = max(diameter_x, diameter_y);
		}
		
		// Return the variables
		return std::make_tuple(center_point, diameter_x, diameter_y, new_diameter);
	}

	std::tuple<cv::Point, int, int, int, double> CalcEllipseElements(Eigen::VectorXd &ellipse_inform, double &ratio_x, double &ratio_y, bool to_EB_info = TRUE)
	{
		cv::Point center_point;
		int diameter_x;
		int diameter_y;
		int new_diameter;
		double angle;

		if (pad_axis == 0)
		{
			center_point.x = int (round(ellipse_inform(1) * ratio_x));
				
			if (to_EB_info)
			{
				center_point.y = int (round((ellipse_inform(0) + pad_start_point) * ratio_y));
			}
			else
			{
				center_point.y = int (round((ellipse_inform(0) * ratio_y - pad_start_point)));
			}

			diameter_x = int (round(ellipse_inform(2) * ratio_x));
			diameter_y = int (round(ellipse_inform(3) * ratio_y));
			new_diameter = max(diameter_x, diameter_y);
			angle = ellipse_inform(4);
		}
		else if (pad_axis == 1)
		{
			if (to_EB_info)
			{
				center_point.x = int (round((ellipse_inform(1) + pad_start_point) * ratio_x));
			}
			else
			{
				center_point.x = int (round((ellipse_inform(1) * ratio_x - pad_start_point)));
			}

			center_point.y = int (round(ellipse_inform(0) * ratio_y));
			diameter_x = int (round(ellipse_inform(2) * ratio_x));
			diameter_y = int (round(ellipse_inform(3) * ratio_y));
			new_diameter = max(diameter_x, diameter_y);
			angle = ellipse_inform(4);

		}
		else
		{
			center_point.x = int(round(ellipse_inform(1) * ratio_x));
			center_point.y = int(round(ellipse_inform(0) * ratio_y));
			diameter_x = int(round(ellipse_inform(2) * ratio_x));
			diameter_y = int(round(ellipse_inform(3) * ratio_y));
			new_diameter = max(diameter_x, diameter_y);
			angle = ellipse_inform(4);
		}

		
		// Return the variables
		return std::make_tuple(center_point, diameter_x, diameter_y, new_diameter, angle);
	}

	void CalcEBEllipseInfoFromOri()
	{
		std::tuple<cv::Point, int, int, int> result_tuple = CalcEllipseElements(ori_disc_center_point.x, ori_disc_center_point.y, to_EB_ratio_x, to_EB_ratio_y, ori_disc_diameter_x, ori_disc_diameter_y);

		disc_center_point = std::get<0>(result_tuple);
		disc_diameter_x = std::get<1>(result_tuple);
		disc_diameter_y = std::get<2>(result_tuple);
		disc_diameter = std::get<3>(result_tuple);
		disc_angle = ori_disc_angle;
	}

	void CalcEBEllipseInfoFromOri(Eigen::VectorXd &ellipse_inform)
	{
		std::tuple<cv::Point, int, int, int, double> result_tuple = CalcEllipseElements(ellipse_inform, to_EB_ratio_x, to_EB_ratio_y, TRUE);

		disk_ellipse_inform << std::get<0>(result_tuple).y, std::get<0>(result_tuple).x, std::get<1>(result_tuple), std::get<2>(result_tuple), std::get<4>(result_tuple);
		disc_diameter = int (round(std::get<4>(result_tuple)));
	}

	void ConvertOriEllipseInfoToEB()
	{
		SetAspectRatio();
		CalcEBEllipseInfoFromOri();
		disk_ellipse_inform << disc_center_point.y, disc_center_point.x, disc_diameter_x, disc_diameter_y, disc_angle;
		is_ellipse_fit = TRUE;
	}

	void ConvertOriEllipseInfoToEB(Eigen::VectorXd &ori_disk_ellipse_inform)
	{
		SetAspectRatio();
		CalcEBEllipseInfoFromOri(ori_disk_ellipse_inform);
		disk_ellipse_inform << disc_center_point.y, disc_center_point.x, disc_diameter_x, disc_diameter_y, disc_angle;
	}

	void CalcOriEllipseInfoFromEB()
	{
		std::tuple<cv::Point, int, int, int> result_tuple = CalcEllipseElements(disc_center_point.x, disc_center_point.y, to_ori_ratio_x, to_ori_ratio_y, disc_diameter_x, disc_diameter_y, FALSE);
		ori_disc_center_point = std::get<0>(result_tuple);
		ori_disc_diameter_x = std::get<1>(result_tuple);
		ori_disc_diameter_y = std::get<2>(result_tuple);
		ori_disc_diameter = std::get<3>(result_tuple);
		ori_disc_angle = disc_angle;
	}
	
	void CalcOriEllipseInfoFromEB(Eigen::VectorXd &ellipse_inform)
	{
		std::tuple<cv::Point, int, int, int, double> result_tuple = CalcEllipseElements(ellipse_inform =  ellipse_inform, to_ori_ratio_x, to_ori_ratio_y, FALSE);
		ori_disc_center_point = std::get<0>(result_tuple);
		ori_disc_diameter_x = std::get<1>(result_tuple);
		ori_disc_diameter_y = std::get<2>(result_tuple);
		ori_disc_diameter = std::get<3>(result_tuple);
		ori_disc_angle = std::get<4>(result_tuple);
	}

	void ConvertEBEllipseInfoToOri()
	{
		SetAspectRatio();
		CalcOriEllipseInfoFromEB();
		ori_disk_ellipse_inform = Eigen::VectorXd::Zero(5);
		ori_disk_ellipse_inform << ori_disc_center_point.y, ori_disc_center_point.x, ori_disc_diameter_y, ori_disc_angle;
	}

	void ConvertEBEllipseInfoToOri(Eigen::VectorXd &ellipse_inform)
	{
		SetAspectRatio();
		CalcOriEllipseInfoFromEB(ellipse_inform);
		ori_disk_ellipse_inform = Eigen::VectorXd::Zero(5);
		ori_disk_ellipse_inform << ori_disc_center_point.y, ori_disc_center_point.x, ori_disc_diameter_x, ori_disc_diameter_y, ori_disc_angle;
	}

	void SetInitOriDiscInfo()
	{
		ori_disc_angle = 0;
		ori_disk_ellipse_inform = Eigen::VectorXd::Zero(5);
		ori_disc_diameter_x = ori_disc_diameter;
		ori_disc_diameter_y = ori_disc_diameter;
		ori_disk_ellipse_inform << ori_disc_center_point.y, ori_disc_center_point.x, ori_disc_diameter, ori_disc_diameter, ori_disc_angle;
	}

	void SetOriDiscInfoFromCheckPoint()
	{
		ori_disk_ellipse_inform = Eigen::VectorXd::Zero(5);
		ori_disk_ellipse_inform << ori_disc_center_point.y, ori_disc_center_point.x, ori_disc_diameter_x, ori_disc_diameter_y, ori_disc_angle;
	}

	void SetOriDiscInfo(Eigen::VectorXd &ellipse_inform)
	{
		ori_disc_center_point = cv::Point(int(ellipse_inform(1)), int(ellipse_inform(0)));
		ori_disc_diameter_x = int(ellipse_inform(2));
		ori_disc_diameter_y = int(ellipse_inform(3));
		ori_disc_diameter = max(ori_disc_diameter_x, ori_disc_diameter_y);
		ori_disc_angle = ellipse_inform(4);
		
		is_ellipse_fit = TRUE;
		ori_disk_ellipse_inform = Eigen::VectorXd::Zero(5);
		ori_disk_ellipse_inform << ori_disc_center_point.y, ori_disc_center_point.x, ori_disc_diameter_x, ori_disc_diameter_y, ori_disc_angle;
	}

	void SetEBDiscInfo(Eigen::VectorXd &ellipse_inform)
	{
		disc_center_point = cv::Point(int(ellipse_inform(1)), int(ellipse_inform(0)));
		disc_diameter_x = int(ellipse_inform(2));
		disc_diameter_y = int(ellipse_inform(3));
		disc_diameter = max(disc_diameter_x, disc_diameter_y);
		disc_angle = ellipse_inform(4);

		disk_ellipse_inform = Eigen::VectorXd::Zero(5);
		disk_ellipse_inform << disc_center_point.y, disc_center_point.x, disc_diameter_x, disc_diameter_y, disc_angle;
		is_ellipse_fit = TRUE;
	}

	void ResetAllDiscInfo()
	{
		ori_disc_center_point = cv::Point(0, 0);
		ori_disc_diameter_x = 0;
		ori_disc_diameter_y = 0;
		ori_disc_angle = 0;
		ori_disk_ellipse_inform << ori_disc_center_point.y, ori_disc_center_point.x, ori_disc_diameter_x, ori_disc_diameter_y, ori_disc_angle;

		disc_center_point = cv::Point(0, 0);
		disc_diameter_x = 0;
		disc_diameter_y = 0;
		disc_angle = 0;
		disk_ellipse_inform << disc_center_point.y, disc_center_point.x, disc_diameter_x, disc_diameter_y, disc_angle;
	}

	void SetEBSize(CRect &cur_editing_board_size)
	{
		EB_cols = cur_editing_board_size.Width();
		EB_rows = cur_editing_board_size.Height();
	}

	void SetEBSize(cv::Size &cur_editing_board_size)
	{
		EB_cols = cur_editing_board_size.width;
		EB_rows = cur_editing_board_size.height;
	}
};


struct WorkProcessStruct
{
	std::string prev_editied_mask_path;
};

class CUtils
{
	public:
		// load file list
		int LoadDataList(DataPathContainer &Path, std::vector<FundusImageData> &fundus_img_data_vec, CListBox &fundus_image_list_box);
		// find file index
		void FindFilesInDir(CString &dir, CString &ext, std::vector<std::string> &path_list, std::vector<std::string> &file_name_list);
		bool FindFilesInDir(CString &dir, CString &fname, std::string &path, std::string &file_name);
		bool DeleteFilesInDir(CString dir, CString fname);
		bool DeleteAllFilesInDir(CString dir);
		void MakeDirecotry(std::string &path);

		// exsit file
		bool IsFileExist(const char *file_name);
		// exist path
		bool IsPathExist(const char *path_name);
		

		// read csv file
		void ReadCSV(std::string path, std::vector<CSVStruct> &CSV_list);
		void ReadCSV(std::string path, std::vector<int> &CSV_list);
		void ReadCSV(std::string path, std::vector<OD_info> &CSV_list, bool del_fist_row);
		void ReadCSV(std::string path, std::vector<std::vector<int>> &CSV_list);

		void ReadCSV(std::string path, std::vector<cv::Point> &CSV_list);
		void ReadCSV(std::string path, FundusImageData &fundus_img_data);

		// write csv file
		void WriteCSV(std::string path, std::vector<int> CSV_list);
		void WriteCSV(std::string path, std::vector<std::vector<int>> CSV_list);
		void WriteCSV(std::string path, std::vector<cv::Point> CSV_list);
		void WriteCSV(std::string path, std::vector<std::string> fundus_list);
		void WriteCSV(std::string path, std::vector<std::string> fundus_list, std::vector<OD_info> &CSV_list);
		void WriteCSV(std::string path, std::vector<FundusImageData> &CSV_list);
		void WriteCSV(std::string path, FundusImageData &CSV_list);
		void WriteCSV_metric(std::string path);
		void WriteCSV_metric(std::string path, std::string filename, std::string analysis_date, cv::Point opt_pos, std::vector<double> &metric, std::string comment);
		void WriteAccountNameCSV(std::string &ccount_name_csv_path, std::vector<CString> &account_name);
		void WriteAccountNameCSV(std::string &ccount_name_csv_path);
		void ReadAccountNameCSV(std::string &account_name_csv_path, std::vector<CString> &account_name_vec);
		
		//padding image
		void LoadFundusAndPreprocess(FundusImageData &fundus_img_data, cv::Mat &m_origin_img);

		void LoadMaskAndPreprocess(std::string check_point_path, FundusImageData &fundus_img_data, cv::Mat &m_origin_mask);

		void PadImage(cv::Mat &origin_image, cv::Mat &padded_image, FundusImageData &fundus_img_data);

		//auto editing function
		void ThinVessel(cv::Mat& src, cv::Mat& dst);


		std::string OD_inform_csv, metric_csv, center_csv;
		OD_info OD_inform;

		//pad variable
		double pad_length;
		int pad_axis, pad_top, pad_left, pad_width, pad_height;
		bool pad_option;
		cv::Size pad_size, org_size, unresized_size;
		cv::Rect pad_rect;
		int work_process_idx;
		cv::Size editing_board_size;

		std::vector<OD_info> vec_center_point;

		bool is_result_data_existed;
		bool is_init_mask_existed;

		//auto editing function
		cv::Mat main_vessel_endp_list, collapsed_vessel_endp_list, main_branch_list;
		std::string cur_time(bool is_change_time_format);
		void IterateThinningVessel(cv::Mat &img, int iter);
		void CollapsedFlip(cv::Mat *av_mask);

		bool is_modified;
		int work_process_num;

		void ReadCheckPoint(DataPathContainer &Path, FundusImageData &fundus_img_data, cv::Mat &origin_mask, std::vector<WorkProcessStruct> &work_process_vec);
		bool ReadElapsedTime(std::string &elapsed_time_check_point_path, FundusImageData &fundus_img_data);
		void WriteInitElapsedTime(std::string &elapsed_time_path);
		bool ReadWorkProcess(std::string &work_process_path, std::vector<WorkProcessStruct> &work_process_vec);
		bool ReadOpticDiscCheckPoint(std::string &optic_disc_path, FundusImageData &fundus_img_data);
		void RemoveOldCheckPoint(std::string &work_process_path, std::vector<WorkProcessStruct> &work_process_vec);

		cv::Mat GetROIFromMask(cv::Mat &cur_editing_mask, cv::Mat &diameter_mask);
		void SaveResult(DataPathContainer &Path, cv::Mat &unpadded_mask, cv::Mat &unpadded_ROI_mask, FundusImageData &fundus_img_data, vascularity *example);
		void SaveResult(DataPathContainer &Path, cv::Mat &unpadded_mask, cv::Mat &unpadded_ROI_mask, FundusImageData &fundus_img_data);
		void RemoveResult(DataPathContainer &Path, FundusImageData &fundus_img_data);

		void WriteCheckPoint(DataPathContainer &Path, cv::Mat &cur_editing_mask, FundusImageData &fundus_img_data, std::vector<WorkProcessStruct> &work_process_vec);
		void WriteElapsedTime(std::string &elapsed_time_path, CTimeSpan &elapsed_time_check_point);
		void WriteElapsedTime(DataPathContainer &Path, FundusImageData &fundus_img_data);

		void WriteWorkProcess(std::string &work_process_path, std::string &editing_mask_path, std::vector<WorkProcessStruct> &work_process_vec);
		void WriteWorkProcess(std::string &work_process_path, std::string &editing_mask_path);
		void WriteEditingMask(std::string &init_editing_mask_path, cv::Mat &cur_editing_mask);
		void RecordWorkProcess(std::string &check_point_path,  std::string &editing_mask_check_point_name, std::vector<WorkProcessStruct> &work_process_vec);
		
		void ResetCheckPoint(DataPathContainer &Path, FundusImageData &fundus_img_data, std::vector<WorkProcessStruct> &work_process_vec);
		cv::Mat UnpadImage(FundusImageData &fundus_img_data, cv::Mat padded_img);

};



