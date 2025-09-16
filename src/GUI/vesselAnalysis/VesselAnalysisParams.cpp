#include "stdafx.h"
#include "VesselAnalysisParams.h"

VesselAnalysisParams::VesselAnalysisParams()
{
	// Message Box Text Param
	timer_pause_info_message = _T("�̾ �����Ͻðڽ��ϱ�?");
	reset_info_message = _T("������� �۾��� ������ �ʱ�ȭ �˴ϴ�. �ʱ�ȭ �Ͻðڽ��ϱ�?");
	direcotry_error_message = _T("1_fundus ������ �������� �ʽ��ϴ�.");
	server_connection_error_message = _T("���� ���� ���¸� Ȯ���� �ּ���.");
	finish_button_info_message = _T("���� ������ ������ �Ϸ��Ͻðڽ��ϱ�?");
	finished_image_re_edit_info_message = _T("���� ������ ������ �Ϸ�� �����Դϴ�. �ٽ� �����Ͻðڽ��ϱ�?");
	ellipse_fit_error_message = _T("��꿡 ������ �߻��߽��ϴ�. �ٽ� ������ �ּ���.");
	
	// Editing Mode Param
	editing_mode_vec = { _T("MODE : All editing"), _T("MODE : Artery editing"), _T("MODE : Vein editing")};

	// Editing State Param
	editing_state_vec = { L"None", L"Editing", L"Finished"};
	
	// Check Point File Name Param
	Path.work_process_check_point_name = "\\work_process_check_point.txt";
	Path.elapsed_time_check_point_name = "elapsed_time_check_point.csv";
	Path.optic_disc_check_point_name = "optic_disc_information.csv";
	Path.parameter_json_name = "metric.json";
	Path.log_name = "log.txt";
	
	Path.fundus_image_format_vec = { _T("jpg"), _T(".png")};

	// Path Container Param
	Path.fundus_folder_name = "\\1_fundus\\";
	Path.init_av_mask_folder_name = "\\2_AV_mask\\";
	Path.result_folder_name = "\\3_result\\";

	Path.result_mask_folder_name = "mask";
	Path.result_ROI_mask_folder_name = "ROI_mask";
	Path.result_metric_folder_name = "metric";
	Path.result_disc_info_folder_name = "disc_info";
	Path.result_elapsed_time_folder_name = "elapsed_time";

	Path.log_folder_name = "\\4_log\\";
	Path.check_point_folder_name = "\\5_editing_img\\";

	// Set Waiting Command Timer Param
	waiting_time = 30;

	// SERVER IP and Port Number Param
	server_ip = "192.168.10.28";
	server_port_1 = 51300;
	//server_ip = "192.168.10.21";
	//server_port_1 = 9990;
	server_port_2 = 19990;

	// Log Text Param
	account_name_log_cstr = _T("%s start SERVAL Program.");
	closing_serval_log_cstr = _T("Close SERVAL Program.");
	data_loading_log_cstr = _T("Total : %d Fundus images are loaded.");
	prev_image_button_log_cstr = _T("%d / %d %s | Prev Image Button is pressed.");
	next_image_button_log_cstr = _T("%d / %d %s | Next Image Button is pressed.");
	changed_editing_mode_log_cstr = _T("%d / %d %s | Editing mode is changed.");
	selected_fundus_image_log_cstr  = _T("%d th fundus image is selected.");
	finish_fundus_image_log_cstr = _T("%s fundus image editing is finished.");
	finished_image_re_edit_log_cstr = _T("%s : %d th fundus image re-editing start.");
	reset_log_cstr = _T("%s : %d th fudnus image editing reset.");

	// Check Point Param
	center_csv_name = "_center.csv";
}
