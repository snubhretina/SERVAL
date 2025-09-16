#pragma once
#include "afxwin.h"
#include "utils.h"
#include "Logger.h"

class VesselAnalysisParams
{
	public:
		VesselAnalysisParams();

		// Message Box Text Param
		CString timer_pause_info_message;
		CString reset_info_message;
		CString direcotry_error_message;
		CString server_connection_error_message;
		CString finish_button_info_message;
		CString finished_image_re_edit_info_message;
		CString ellipse_fit_error_message;

		// Check Point Files Name
		std::string work_process_check_point_name;
		std::string elapsed_time_check_point_name;
		
		// Editing Mode Param
		std::vector<CString>editing_mode_vec;
		DataPathContainer Path;

		// Editing State Param
		std::vector <CString> editing_state_vec;

		// Set Waiting Command Timer Param
		int waiting_time;

		// Server Info
		char* server_ip;
		int server_port_1;
		int server_port_2;

		// Log Text Param
		CString account_name_log_cstr;
		CString closing_serval_log_cstr;
		CString data_loading_log_cstr;
		CString prev_image_button_log_cstr;
		CString next_image_button_log_cstr;
		CString changed_editing_mode_log_cstr;
		CString selected_fundus_image_log_cstr;
		CString finish_fundus_image_log_cstr;
		CString finished_image_re_edit_log_cstr;
		CString reset_log_cstr;

		// Check Point Param
		std::string center_csv_name;
		std::string optic_disc_infor_csv_name;
};