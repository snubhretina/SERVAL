// VesselImgTransfer.cpp : implementation file
//

#include "stdafx.h"
#include "vesselAnalysis.h"
#include "VesselImgTransfer.h"
#include "afxdialogex.h"
#include "vesselAnalysisDlg.h"
#define DESIRED_WINSOCK_VERSION        0x0101
#define MINIMUM_WINSOCK_VERSION        0x0001


// VesselImgTransfer dialog

IMPLEMENT_DYNAMIC(VesselImgTransfer, CDialog)

VesselImgTransfer::VesselImgTransfer(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_VesselImgTransfer, pParent)
	, m_strBMWa(_T(""))
	, m_strBMWv(_T(""))
	, m_strBSTDWa(_T(""))
	, m_strBSTDWv(_T(""))
	, m_strCMWa(_T(""))
	, m_strCMWv(_T(""))
	, m_strCSTDWa(_T(""))
	, m_strCSTDWv(_T(""))
{
}

VesselImgTransfer::~VesselImgTransfer()
{
}

void VesselImgTransfer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strBMWa);
	DDX_Text(pDX, IDC_EDIT2, m_strBMWv);
	DDX_Text(pDX, IDC_EDIT3, m_strBSTDWa);
	DDX_Text(pDX, IDC_EDIT4, m_strBSTDWv);
	DDX_Text(pDX, IDC_EDIT5, m_strCMWa);
	DDX_Text(pDX, IDC_EDIT6, m_strCMWv);
	DDX_Text(pDX, IDC_EDIT7, m_strCSTDWa);
	DDX_Text(pDX, IDC_EDIT8, m_strCSTDWv);

	DDX_Control(pDX, IDC_EDIT_transfer, m_edit_report);
}

BEGIN_MESSAGE_MAP(VesselImgTransfer, CDialog)
	ON_BN_CLICKED(ID_sendreport, &VesselImgTransfer::OnBnClickedsendreport)
END_MESSAGE_MAP()


// VesselImgTransfer message handlers
BOOL VesselImgTransfer::OnInitDialog()
{
	CDialog::OnInitDialog();	
	
	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;
	Getmetric();

	return TRUE; 
}

// Send realtime log to server //
bool VesselImgTransfer::SendRealTimeLog(CString &log_text)
{
	InitSocket();
	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;

	while (1) {
		if (connect(skt, (SOCKADDR*)&addr, sizeof(addr)) == 0) break;
		else
		{
			if (AfxMessageBox(pDlg->params.server_connection_error_message, MB_OK | MB_APPLMODAL) == IDOK)
			{
				return FALSE;
			}
		}
	}
	while (!WSAGetLastError()) {

		// realtimelog 명령어 전송
		std::string svr_command = "log";
		svr_command = string(16 - svr_command.length(), '0') + svr_command;
		send(skt, svr_command.c_str(), 16, 0);

		// account name 길이 전송
		int account_name_len = m_svr_account_name.length();
		std::string svr_account_name_len_str = std::to_string(account_name_len);

		std::string svr_account_name_len_data = std::string(16 - svr_account_name_len_str.length(), '0') + svr_account_name_len_str;
		send(skt, svr_account_name_len_data.c_str(), 16, 0);

		// account name 전송
		send(skt, m_svr_account_name.c_str(), account_name_len, 0);

		// dataset name 길이 전송
		int dataset_name_len = pDlg->m_VesselImgTransfer.dataset_name.length();
		std::string dataset_name_len_str = std::to_string(dataset_name_len);
		std::string dataset_name_len_data = std::string(16 - dataset_name_len_str.length(), '0') + dataset_name_len_str;
		send(pDlg->m_VesselImgTransfer.skt, dataset_name_len_data.c_str(), 16, 0);

		// dataset name 전송
		send(pDlg->m_VesselImgTransfer.skt, pDlg->m_VesselImgTransfer.dataset_name.c_str(), dataset_name_len, 0);

		// log text 길이 전송
		int log_text_len = log_text.GetLength();
		std::string log_text_len_str = std::to_string(log_text_len);
		std::string svr_log_text_len_data = std::string(16 - log_text_len_str.length(), '0') + log_text_len_str;
		send(skt, svr_log_text_len_data.c_str(), 16, 0);

		// log text 전송
		std::string log_text_str = std::string(CT2CA(log_text.operator LPCWSTR()));
		send(skt, log_text_str.c_str(), log_text_len, 0);

		// 연결 종료
		closesocket(skt);
		if (pDlg->m_VesselImgTransfer.skt == INVALID_SOCKET && pDlg->m_VesselImgTransfer.skt_for_send_result == INVALID_SOCKET) {
			WSACleanup();
		}
		break;
	}
	return TRUE;
}

// Send editing result to server //
UINT VesselImgTransfer::SendResult(LPVOID _mothod)
{
	// Editing 결과 로드

	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;
	int data_cols, data_rows, data_channel;
	cv::Mat transfer_mask_img, transfer_ROI_mask_img;
	uchar *send_fp_data, *send_ROI_fp_data;

	transfer_mask_img = pDlg->m_calc_param_origin_mask.clone();
	transfer_ROI_mask_img = pDlg->m_calc_param_ROI_mask.clone();

	data_cols = transfer_mask_img.cols;
	data_rows = transfer_mask_img.rows;
	data_channel = transfer_mask_img.channels();

	send_fp_data = transfer_mask_img.data;
	send_ROI_fp_data = transfer_ROI_mask_img.data;
	
	std::string fundus_name = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].fundus_name;

	int pad_start_point = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].pad_start_point;
	int OD_coord_x, OD_coord_y;

	bool is_ellipse_fit = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].is_ellipse_fit;
	int OD_diameter = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].ori_disc_diameter;
	if (pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].pad_axis == 0)
	{
		OD_coord_x = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].ori_disc_center_point.x;
		OD_coord_y = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].ori_disc_center_point.y - pad_start_point;
	}
	else if (pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].pad_axis == 1)
	{
		OD_coord_x = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].ori_disc_center_point.x - pad_start_point;
		OD_coord_y = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].ori_disc_center_point.y;
	}
	else
	{
		OD_coord_x = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].ori_disc_center_point.x;
		OD_coord_y = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].ori_disc_center_point.y;
	}


	float ellipse_inform_x;
	float ellipse_inform_y;
	float ellipse_inform_angle;
	
	char buffer[1024];
	int buffer_result = 0;
	int ellipse_inform_data_len;

	if (is_ellipse_fit)
	{
		ellipse_inform_data_len = 6;
		Eigen::VectorXd cartpool = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].disk_ellipse_inform;
		ellipse_inform_x = cartpool(2);
		ellipse_inform_y = cartpool(3);
		ellipse_inform_angle = cartpool(4);
	}
	else
	{
		ellipse_inform_data_len = 3;
	}

	int send_result;
	CTimeSpan elapsed_time = pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx].elapsed_time;
	CString elapsed_time_cstr;
	elapsed_time_cstr.Format(L"%02d:%02d:%02d", elapsed_time.GetHours(), elapsed_time.GetMinutes(), elapsed_time.GetSeconds());

	pDlg->m_VesselImgTransfer.InitSocketForSendResult();
	char msg[PACKET_SIZE] = { 0 };

	//// 서버 연결
	//while (1) {
	//	if (!connect(pDlg->m_VesselImgTransfer.skt, (SOCKADDR*)&pDlg->m_VesselImgTransfer.addr, sizeof(pDlg->m_VesselImgTransfer.addr))) break;
	//}

	while (1) {
		if (!connect(pDlg->m_VesselImgTransfer.skt_for_send_result, (SOCKADDR*)&pDlg->m_VesselImgTransfer.addr, sizeof(addr))) break;
	}

	while (!WSAGetLastError()) {
		// realtimelog 명령어 전송
		std::string svr_command = "result";
		svr_command = string(16 - svr_command.length(), '0') + svr_command;
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, svr_command.c_str(), 16, 0);

		// account name 길이 전송
		int account_name_len = pDlg->m_VesselImgTransfer.m_svr_account_name.length();
		std::string svr_account_name_len_str = std::to_string(account_name_len);
		std::string svr_account_name_len_data = std::string(16 - svr_account_name_len_str.length(), '0') + svr_account_name_len_str;
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, svr_account_name_len_data.c_str(), 16, 0);

		// account name 전송
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, pDlg->m_VesselImgTransfer.m_svr_account_name.c_str(), account_name_len, 0);

		// dataset name 길이 전송
		int dataset_name_len = pDlg->m_VesselImgTransfer.dataset_name.length();
		std::string dataset_name_len_str = std::to_string(dataset_name_len);
		std::string dataset_name_len_data = std::string(16 - dataset_name_len_str.length(), '0') + dataset_name_len_str;
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, dataset_name_len_data.c_str(), 16, 0);

		// dataset name 전송
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, pDlg->m_VesselImgTransfer.dataset_name.c_str(), dataset_name_len, 0);

		// fundus 파일 이름 길이 전송
		std::string file_name = fundus_name + ".png";
		int file_name_len = file_name.length();
		std::string file_name_len_str = std::to_string(file_name_len);
		std::string file_name_len_data = std::string(16 - file_name_len_str.length(), '0') + file_name_len_str;
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, file_name_len_data.c_str(), 16, 0);

		// fundus 파일 이름 전송
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, file_name.c_str(), file_name_len, 0);

		// elapsed time 길이 전송
		int elapsed_time_len = elapsed_time_cstr.GetLength();
		std::string elapsed_time_len_str = std::to_string(elapsed_time_len);
		std::string elapsed_time_len_data = std::string(16 - elapsed_time_len_str.length(), '0') + elapsed_time_len_str;
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, elapsed_time_len_data.c_str(), 16, 0);

		// elapsed time 전송
		std::string elapsed_time_data((CT2CA) elapsed_time_cstr.operator LPCWSTR());
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, elapsed_time_data.c_str(), elapsed_time_len, 0);

		// Elipse 데이터 개수 전송.
		std::string ellipse_inform_num_str = std::to_string(ellipse_inform_data_len);
		std::string ellipse_inform_num_data = string(16 - ellipse_inform_num_str.length(), '0') + ellipse_inform_num_str;
		char ellipse_inform_num_data_char[16 + 1];
		strcpy(ellipse_inform_num_data_char, ellipse_inform_num_data.c_str());
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, ellipse_inform_num_data_char, 16, 0);

		// Elipsed 데이터 전송.
		std::string OD_coord_y_str = std::to_string(OD_coord_y);
		std::string OD_coord_y_str_data = string(16 - OD_coord_y_str.length(), '0') + OD_coord_y_str;
		char OD_coord_y_data_char[16 + 1];
		strcpy(OD_coord_y_data_char, OD_coord_y_str_data.c_str());
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, OD_coord_y_data_char, 16, 0);

		std::string OD_coord_x_str = std::to_string(OD_coord_x);
		std::string OD_coord_x_str_data = string(16 - OD_coord_x_str.length(), '0') + OD_coord_x_str;
		char OD_coord_x_data_char[16 + 1];
		strcpy(OD_coord_x_data_char, OD_coord_x_str_data.c_str());
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, OD_coord_x_data_char, 16, 0);

		std::string OD_diameter_str = std::to_string(OD_diameter);
		std::string OD_diameter_str_data = string(16 - OD_diameter_str.length(), '0') + OD_diameter_str;
		char OD_diameter_data_char[16 + 1];
		strcpy(OD_diameter_data_char, OD_diameter_str_data.c_str());
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, OD_diameter_data_char, 16, 0);

		// Elipsed 데이터 전송2.
		if (is_ellipse_fit)
		{
			std::string ellipse_inform_x_str = std::to_string(ellipse_inform_x);
			std::string ellipse_inform_x_str_data = string(16 - ellipse_inform_x_str.length(), '0') + ellipse_inform_x_str;
			char ellipse_inform_x_data_char[16 + 1];
			strcpy(ellipse_inform_x_data_char, ellipse_inform_x_str_data.c_str());
			send(pDlg->m_VesselImgTransfer.skt_for_send_result, ellipse_inform_x_data_char, 16, 0);

			std::string ellipse_inform_y_str = std::to_string(ellipse_inform_y);
			std::string ellipse_inform_y_str_data = string(16 - ellipse_inform_y_str.length(), '0') + ellipse_inform_y_str;
			char ellipse_inform_y_data_char[16 + 1];
			strcpy(ellipse_inform_y_data_char, ellipse_inform_y_str_data.c_str());
			send(pDlg->m_VesselImgTransfer.skt_for_send_result, ellipse_inform_y_data_char, 16, 0);

			std::string ellipse_inform_angle_str = std::to_string(ellipse_inform_angle);
			std::string ellipse_inform_angle_str_data = string(16 - ellipse_inform_angle_str.length(), '0') + ellipse_inform_angle_str;
			char ellipse_inform_angle_data_char[16 + 1];
			strcpy(ellipse_inform_angle_data_char, ellipse_inform_angle_str_data.c_str());
			send(pDlg->m_VesselImgTransfer.skt_for_send_result, ellipse_inform_angle_data_char, 16, 0);
		}

		// fundus 파일 세로 길이 전송
		std::string str_height = std::to_string(data_rows);
		str_height = string(16 - str_height.length(), '0') + str_height;
		char strHeight[16 + 1];
		strcpy(strHeight, str_height.c_str());
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, strHeight, 16, 0);

		// fundus 파일 가로 길이 전송
		std::string str_width = std::to_string(data_cols);
		str_width = string(16 - str_width.length(), '0') + str_width;
		char strWidth[16 + 1];
		strcpy(strWidth, str_width.c_str());
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, strWidth, 16, 0);

		// fundus 파일 채널 전송
		std::string str_channel = std::to_string(data_channel);
		str_channel = string(16 - str_channel.length(), '0') + str_channel;
		char strchannel[16 + 1];
		strcpy(strchannel, str_channel.c_str());
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, strchannel, 16, 0);

		// fundus 파일 데이터 전송
		int strLength =data_channel *data_cols *data_rows;

		send_result = send(pDlg->m_VesselImgTransfer.skt_for_send_result, (char *)send_fp_data, strLength, 0);
		char end_str[] = "end";
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, end_str, 4, 0);

		//recv(pDlg->m_VesselImgTransfer.skt_for_send_result, buffer, 16, MSG_WAITALL);
		//buffer_result = atoi(buffer);

		//send image
		int send_result = send(pDlg->m_VesselImgTransfer.skt_for_send_result, (char *)send_ROI_fp_data, strLength, 0);
		//send end token
		send(pDlg->m_VesselImgTransfer.skt_for_send_result, end_str, 4, 0);

		// metric json 결과 전송.
		//std::string str_json_data_size = std::to_string(pDlg->m_json_data.size());
		//str_json_data_size = string(16 - str_json_data_size.length(), '0') + str_json_data_size;
		//char char_json_data_size[16 + 1];
		//strcpy(char_json_data_size, str_json_data_size.c_str());
		//send(pDlg->m_VesselImgTransfer.skt_for_send_result, char_json_data_size, 16, 0);
		//send(pDlg->m_VesselImgTransfer.skt_for_send_result, pDlg->m_json_data.c_str(), pDlg->m_json_data.size(), 0);
		//send(pDlg->m_VesselImgTransfer.skt_for_send_result, end_str, 4, 0);

		closesocket(pDlg->m_VesselImgTransfer.skt_for_send_result);
		
		if (pDlg->m_VesselImgTransfer.skt == INVALID_SOCKET && pDlg->m_VesselImgTransfer.skt_for_send_result == INVALID_SOCKET) {
			WSACleanup();
		}

		break;
	}
	pDlg->m_WaitwindowSendResult.m_is_transferring = false;
	return TRUE;
}

// Send error report //`
void VesselImgTransfer::SendError()
{
	InitSocket();
	FILE* file = NULL;
	while (1) {
		if (!connect(skt, (SOCKADDR*)&addr, sizeof(addr))) break;
	}
	char buffer[1024], buffer_buffer[1024];
	std::string report_str = std::string(CT2CA(m_cstr_errorreport));
	while (!WSAGetLastError()) {
		/* 전송할 파일 이름을 작성합니다 */
		//client send image size
		std::string strdataorder = "report";
		strdataorder = string(16 - strdataorder.length(), '0') + strdataorder;
		send(skt, strdataorder.c_str(), 16, 0);

		// account name 길이 전송
		int account_name_len = m_svr_account_name.length();
		std::string svr_account_name_len_str = std::to_string(account_name_len);
		std::string svr_account_name_len_data = std::string(16 - svr_account_name_len_str.length(), '0') + svr_account_name_len_str;
		send(skt, svr_account_name_len_data.c_str(), 16, 0);

		// account name 전송
		send(skt, m_svr_account_name.c_str(), account_name_len, 0);

		std::string fn_length_padding = std::string(16 - std::to_string(report_str.length()).length(), '0') + std::to_string(report_str.length());
		send(skt, fn_length_padding.c_str(), 16, 0);
		recv(skt, buffer, 16, 0);
		send(skt, report_str.c_str(), report_str.length(), 0);

		closesocket(skt);
		WSACleanup();
	}
}

std::string VesselImgTransfer::GetMyIP()
{
	WSADATA wsadata;
	std::string strIP; // 이 변수에 IP주소가 저장된다.
	strIP = "";


	if (!WSAStartup(0x0101, &wsadata))
	{
		if (wsadata.wVersion >= 0x0001)
		{
			HOSTENT *p_host_info;
			IN_ADDR in;
			char host_name[128] = { 0, };
			gethostname(host_name, 128);
			p_host_info = gethostbyname(host_name);

			if (p_host_info != NULL)
			{
				for (int i = 0; p_host_info->h_addr_list[i]; i++)
				{
					memcpy(&in, p_host_info->h_addr_list[i], 4);
					strIP = inet_ntoa(in);
				}
			}
		}
		WSACleanup();
	}
	return strIP;
}

// Init mask and disc
UINT VesselImgTransfer::VesselExtractThread(LPVOID _mothod)
{
	// TODO: Add your control notification handler code here
	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;

	cv::Mat image = pDlg->m_origin_img.clone();

	pDlg->m_VesselImgTransfer.data_channel = image.elemSize();
	pDlg->m_VesselImgTransfer.data_cols = image.cols;
	pDlg->m_VesselImgTransfer.data_rows = image.rows;
	
	pDlg->m_VesselImgTransfer.send_fp_data = image.data;
	size_t data_size = image.total();
	
	pDlg->m_VesselImgTransfer.InitSocket();
	while (1) {
		if (!connect(pDlg->m_VesselImgTransfer.skt, (SOCKADDR*)&pDlg->m_VesselImgTransfer.addr, sizeof(pDlg->m_VesselImgTransfer.addr))) break;
	}

	char msg[PACKET_SIZE] = { 0 };
	char buffer[1024], buffer_buffer[1024];
	while (!WSAGetLastError()) {
		//order send
		std::string strdataorder = "extract";
		strdataorder = string(16 - strdataorder.length(), '0') + strdataorder;
		char strOrder[16 + 1];
		strcpy(strOrder, strdataorder.c_str());
		send(pDlg->m_VesselImgTransfer.skt, strOrder, 16, 0);

		// account name 길이 전송
		int account_name_len = pDlg->m_VesselImgTransfer.m_svr_account_name.length();
		std::string svr_account_name_len_str = std::to_string(account_name_len);
		std::string svr_account_name_len_data = std::string(16 - svr_account_name_len_str.length(), '0') + svr_account_name_len_str;
		send(pDlg->m_VesselImgTransfer.skt, svr_account_name_len_data.c_str(), 16, 0);

		// account name 전송
		send(pDlg->m_VesselImgTransfer.skt, pDlg->m_VesselImgTransfer.m_svr_account_name.c_str(), account_name_len, 0);

		// dataset name 길이 전송
		int dataset_name_len = pDlg->m_VesselImgTransfer.dataset_name.length();
		std::string dataset_name_len_str = std::to_string(dataset_name_len);
		std::string dataset_name_len_data = std::string(16 - dataset_name_len_str.length(), '0') + dataset_name_len_str;
		send(pDlg->m_VesselImgTransfer.skt, dataset_name_len_data.c_str(), 16, 0);

		// dataset name 전송
		send(pDlg->m_VesselImgTransfer.skt, pDlg->m_VesselImgTransfer.dataset_name.c_str(), dataset_name_len, 0);

		//file length send
		int strLength = pDlg->m_VesselImgTransfer.data_channel * pDlg->m_VesselImgTransfer.data_cols * pDlg->m_VesselImgTransfer.data_rows;
		std::string strSizeTmp = std::to_string(strLength);
		strSizeTmp = string(16 - strSizeTmp.length(), '0') + strSizeTmp;
		char strSize[16 + 1];
		strcpy(strSize, strSizeTmp.c_str());
		send(pDlg->m_VesselImgTransfer.skt, strSize, 16, 0);

		//filename length send
		std::string strimagename = pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].fundus_name + pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].fundus_format;
		std::string fn_length_padding = std::string(16 - std::to_string(strimagename.length()).length(), '0') + std::to_string(strimagename.length());
		send(pDlg->m_VesselImgTransfer.skt, fn_length_padding.c_str(), fn_length_padding.length(), 0);
		//filename length receive
		recv(pDlg->m_VesselImgTransfer.skt, buffer, 16, 0);
		//filename send
		send(pDlg->m_VesselImgTransfer.skt, strimagename.c_str(), strimagename.length(), 0);
		recv(pDlg->m_VesselImgTransfer.skt, buffer, 16, 0);

		std::string str_height = std::to_string(pDlg->m_VesselImgTransfer.data_rows);
		str_height = string(16 - str_height.length(), '0') + str_height;
		char strHeight[16 + 1];
		strcpy(strHeight, str_height.c_str());
		send(pDlg->m_VesselImgTransfer.skt, strHeight, 16, 0);

		std::string str_width = std::to_string(pDlg->m_VesselImgTransfer.data_cols);
		str_width = string(16 - str_width.length(), '0') + str_width;
		char strWidth[16 + 1];
		strcpy(strWidth, str_width.c_str());
		send(pDlg->m_VesselImgTransfer.skt, strWidth, 16, 0);

		//send image
		int send_result = send(pDlg->m_VesselImgTransfer.skt, (char *)image.data, strLength, 0);
		char end_str[] = "end";
		//send end token
		send(pDlg->m_VesselImgTransfer.skt, end_str, 4, 0);

		//vector<uchar> buffer_str;
		recv(pDlg->m_VesselImgTransfer.skt, buffer_buffer, 16, 0);
		pDlg->m_VesselImgTransfer.fovea_x = atoi(buffer_buffer);
		recv(pDlg->m_VesselImgTransfer.skt, buffer_buffer, 16, 0);
		pDlg->m_VesselImgTransfer.fovea_y = atoi(buffer_buffer);
		recv(pDlg->m_VesselImgTransfer.skt, buffer, 16, 0);
		pDlg->m_VesselImgTransfer.disc_center_x = atoi(buffer);
		recv(pDlg->m_VesselImgTransfer.skt, buffer, 16, 0);
		pDlg->m_VesselImgTransfer.disc_center_y = atoi(buffer);

		//receive classified image
		vector<uchar> buffer_img(strLength);
		int file_length = recv(pDlg->m_VesselImgTransfer.skt, buffer, 16, 0);
		int result_length = atoi(buffer);
		int bytes = 0;
		for (int i = 0; i < result_length; i += bytes) {
			if ((bytes = recv(pDlg->m_VesselImgTransfer.skt, (char *)buffer_img.data() + i, result_length - i, 0)) == -1) {
				break;
			}
		}

		// Assign pixel value to img
		int ptr = 0;
		pDlg->m_VesselImgTransfer.classification_img = cv::Mat::zeros(pDlg->m_VesselImgTransfer.data_rows, pDlg->m_VesselImgTransfer.data_cols, CV_8UC3);
		//i heigth j width
		for (int i = 0; i < pDlg->m_VesselImgTransfer.classification_img.rows; i++) {
			for (int j = 0; j < pDlg->m_VesselImgTransfer.classification_img.cols; j++) {
				pDlg->m_VesselImgTransfer.classification_img.at<cv::Vec3b>(i, j) = cv::Vec3b(buffer_img[ptr + 0], buffer_img[ptr + 1], buffer_img[ptr + 2]);
				ptr = ptr + 3;
			}
		}

		//receive seg image
		file_length = recv(pDlg->m_VesselImgTransfer.skt, buffer, 16, 0);
		result_length = atoi(buffer);
		vector<uchar> buffer_seg_img(result_length);
		bytes = 0;
		for (int i = 0; i < result_length; i += bytes) {
			if ((bytes = recv(pDlg->m_VesselImgTransfer.skt, (char *)buffer_seg_img.data() + i, result_length - i, 0)) == -1) {
				break;
			}
		}

		pDlg->m_VesselImgTransfer.seg_img = cv::Mat::zeros(pDlg->m_VesselImgTransfer.data_rows, pDlg->m_VesselImgTransfer.data_cols, CV_8UC1);
		ptr = 0;
		//i heigth j width
		for (int i = 0; i <pDlg->m_VesselImgTransfer.seg_img.rows; i++) {
			for (int j = 0; j < pDlg->m_VesselImgTransfer.seg_img.cols; j++) {
				pDlg->m_VesselImgTransfer.seg_img.at<uchar>(i, j) = buffer_seg_img[ptr];
				ptr++;
			}
		}

		cv::cvtColor(pDlg->m_VesselImgTransfer.classification_img, pDlg->m_VesselImgTransfer.classification_img, cv::COLOR_RGB2BGR);
		send(pDlg->m_VesselImgTransfer.skt, msg, strlen(msg), 0);

		closesocket(pDlg->m_VesselImgTransfer.skt);
		
		if (pDlg->m_VesselImgTransfer.skt == INVALID_SOCKET && pDlg->m_VesselImgTransfer.skt_for_send_result == INVALID_SOCKET) {
			WSACleanup();
		}

		break;
	}
	pDlg->m_Waitwindow.m_is_transferring = false;
	return 300;
}

void VesselImgTransfer::OnBnClickedsendreport()
{
	GetDlgItemText(IDC_EDIT_transfer, report);
	send_message = true;
	EndDialog(IDOK);
}

UINT VesselImgTransfer::LoopReport(LPVOID _mothod)
{// TODO: Add your control notification handler code here
	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;
	char buffer[1024];
	std::string report_str;
	while(!WSAGetLastError()){
		if (pDlg->m_VesselImgTransfer.send_message) {
			report_str = std::string(CT2CA(pDlg->m_VesselImgTransfer.report));
			std::string fn_length_padding = std::string(16 - std::to_string(report_str.length()).length(), '0') + std::to_string(report_str.length());
			send(pDlg->m_VesselImgTransfer.skt, fn_length_padding.c_str(), 16, 0);
			recv(pDlg->m_VesselImgTransfer.skt, buffer, 16, 0);
			send(pDlg->m_VesselImgTransfer.skt, report_str.c_str(), report_str.length(), 0);
			pDlg->m_VesselImgTransfer.image_comment = report_str;
			pDlg->m_VesselImgTransfer.send_message = false;
			closesocket(pDlg->m_VesselImgTransfer.skt);
			WSACleanup();
			break;
		}
	}
	return 0;
}

void VesselImgTransfer::InitSocket()
{
	WSAStartup(MAKEWORD(2, 2), &wsa);
	skt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(server_ip);

	addr.sin_port = htons(server_port_1);
	

	//	// `recv` 함수의 타임아웃 설정
	//struct timeval timeout;
	//timeout.tv_sec = 50000;  // 타임아웃 시간(초)
	//timeout.tv_usec = 0; // 타임아웃 시간(마이크로초)

	//if (setsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
	//	std::cerr << "타임아웃 설정 오류\n";
	//	closesocket(skt);
	//}

	//struct timeval timeout2;
	//timeout2.tv_sec = 0;  // 타임아웃 시간(초)
	//timeout2.tv_usec = 0; // 타임아웃 시간(마이크로초)
	//
	//int optlen = sizeof(timeout);

	//// 수신 타임아웃 값 가져오기
	//if (getsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout2, &optlen) == 0) {
	//	std::cout << "현재 수신 타임아웃: " << timeout2.tv_sec << "초, "
	//		<< timeout2.tv_usec << "마이크로초\n";
	//}
	//else {
	//	std::cerr << "수신 타임아웃 가져오기 오류\n";
	//}
}


void VesselImgTransfer::InitSocketForSendResult()
{
	WSAStartup(MAKEWORD(2, 2), &wsa);
	skt_for_send_result = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(server_ip);

	addr.sin_port = htons(server_port_1);


	//	// `recv` 함수의 타임아웃 설정
	//struct timeval timeout;
	//timeout.tv_sec = 50000;  // 타임아웃 시간(초)
	//timeout.tv_usec = 0; // 타임아웃 시간(마이크로초)

	//if (setsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
	//	std::cerr << "타임아웃 설정 오류\n";
	//	closesocket(skt);
	//}

	//struct timeval timeout2;
	//timeout2.tv_sec = 0;  // 타임아웃 시간(초)
	//timeout2.tv_usec = 0; // 타임아웃 시간(마이크로초)
	//
	//int optlen = sizeof(timeout);

	//// 수신 타임아웃 값 가져오기
	//if (getsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout2, &optlen) == 0) {
	//	std::cout << "현재 수신 타임아웃: " << timeout2.tv_sec << "초, "
	//		<< timeout2.tv_usec << "마이크로초\n";
	//}
	//else {
	//	std::cerr << "수신 타임아웃 가져오기 오류\n";
	//}
}

void VesselImgTransfer::Getmetric()
{
	InitSocket();
	// TODO: Add your control notification handler code here
	
	while (1) {
		if (!connect(skt, (SOCKADDR*)&addr, sizeof(addr))) break;
	}

	char msg[PACKET_SIZE] = { 0 };
	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;

	data_channel = pDlg->m_cur_editing_mask.elemSize();
	data_cols = pDlg->m_cur_editing_mask.cols;
	data_rows = pDlg->m_cur_editing_mask.rows;
	send_seg_data = pDlg->m_cur_editing_mask.data;
	pDlg->ChangeTimeFormat(pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time, elapsed_time);

	int disc_Y_location = pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].disc_center_point.y;
	int disc_X_location = pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].disc_center_point.x;
	int opt_distance = pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].disc_diameter;
	char buffer[1024], buffer_buffer[1024];
	while (!WSAGetLastError()) {
		//client send image size
		std::string strdataorder = "metric";
		strdataorder = string(16 - strdataorder.length(), '0') + strdataorder;
		char strOrder[16 + 1];
		strcpy(strOrder, strdataorder.c_str());
		send(skt, strOrder, 16, 0);

		// account name 길이 전송
		int account_name_len = pDlg->m_VesselImgTransfer.m_svr_account_name.length();
		std::string svr_account_name_len_str = std::to_string(account_name_len);
		std::string svr_account_name_len_data = std::string(16 - svr_account_name_len_str.length(), '0') + svr_account_name_len_str;
		send(pDlg->m_VesselImgTransfer.skt, svr_account_name_len_data.c_str(), 16, 0);

		// account name 전송
		send(pDlg->m_VesselImgTransfer.skt, pDlg->m_VesselImgTransfer.m_svr_account_name.c_str(), account_name_len, 0);

		// dataset name 길이 전송
		int dataset_name_len = pDlg->m_VesselImgTransfer.dataset_name.length();
		std::string dataset_name_len_str = std::to_string(dataset_name_len);
		std::string dataset_name_len_data = std::string(16 - dataset_name_len_str.length(), '0') + dataset_name_len_str;
		send(pDlg->m_VesselImgTransfer.skt, dataset_name_len_data.c_str(), 16, 0);

		// dataset name 전송
		send(pDlg->m_VesselImgTransfer.skt, pDlg->m_VesselImgTransfer.dataset_name.c_str(), dataset_name_len, 0);

		//client send image size
		int strLength = data_channel * data_cols * data_rows;
		std::string strSizeTmp = std::to_string(strLength);
		strSizeTmp = string(16 - strSizeTmp.length(), '0') + strSizeTmp;
		char strSize[16 + 1];
		strcpy(strSize, strSizeTmp.c_str());
		send(skt, strSize, 16, 0);

		//client send filename size
		std::string strimagename = pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].fundus_name;
		std::string fn_length_padding = std::string(16 - std::to_string(strimagename.length()).length(), '0') + std::to_string(strimagename.length());
		send(skt, fn_length_padding.c_str(), fn_length_padding.length(), 0);
		//client recieve
		recv(skt, buffer, 16, 0);
		//client send filename
		send(skt, strimagename.c_str(), strimagename.length(), 0);

		std::string strdisc_Y = std::to_string(disc_Y_location);
		strdisc_Y = string(16 - strdisc_Y.length(), '0') + strdisc_Y;
		char disc_Y[16 + 1];
		strcpy(disc_Y, strdisc_Y.c_str());
		send(skt, disc_Y, 16, 0);

		std::string strdisc_X = std::to_string(disc_X_location);
		strdisc_X = string(16 - strdisc_X.length(), '0') + strdisc_X;
		char disc_X[16 + 1];
		strcpy(disc_X, strdisc_X.c_str());
		send(skt, disc_X, 16, 0);
		
		std::string strdisc_dist = std::to_string(opt_distance);
		strdisc_dist = string(16 - strdisc_dist.length(), '0') + strdisc_dist;
		char disc_dist[16 + 1];
		strcpy(disc_dist, strdisc_dist.c_str());
		send(skt, disc_dist, 16, 0);

		std::string strelapsed_time = std::string(CT2CA(elapsed_time));
		strelapsed_time = string(16 - strelapsed_time.length(), '0') + strelapsed_time;
		char elapsed_time_str[16 + 1];
		strcpy(elapsed_time_str, strelapsed_time.c_str());
		send(skt, elapsed_time_str, 16, 0);

		std::string str_height = std::to_string(data_rows);
		str_height = string(16 - str_height.length(), '0') + str_height;
		char strHeight[16 + 1];
		strcpy(strHeight, str_height.c_str());
		send(skt, strHeight, 16, 0);

		std::string str_width = std::to_string(data_cols);
		str_width = string(16 - str_width.length(), '0') + str_width;
		char strWidth[16 + 1];
		strcpy(strWidth, str_width.c_str());
		send(skt, strWidth, 16, 0);
		recv(skt, buffer, 16, 0);

		//send image
		int send_result = send(skt, (char *)send_seg_data, strLength, 0);
		char end_str[] = "end";
		send(skt, end_str, 4, 0);
		//send end token
		recv(skt, buffer, 8, 0);
		BMWa = atof(buffer);
		recv(skt, buffer, 8, 0);
		BMWv = atof(buffer);
		recv(skt, buffer, 8, 0);
		BSTDWa = atof(buffer);
		recv(skt, buffer, 8, 0);
		BSTDWv = atof(buffer);
		recv(skt, buffer, 8, 0);
		CMWa = atof(buffer);
		recv(skt, buffer, 8, 0);
		CMWv = atof(buffer);
		recv(skt, buffer, 8, 0);
		CSTDWa = atof(buffer);
		recv(skt, buffer, 8, 0);
		CSTDWv = atof(buffer);

		//char end_str_metric[] = "end";
		////send end token
		//send(skt, end_str_metric, 4, 0);
		p3 = AfxBeginThread(LoopReport, this);
		if (p3 == NULL)
		{
			AfxMessageBox(L"Error VesselExtractThread");
		}

		break;
	}
	str_BMWa.Format(_T("%f"), BMWa);
	m_strBMWa = str_BMWa;
	str_BMWv.Format(_T("%f"), BMWv);
	m_strBMWv = str_BMWv;
	str_BSTDWa.Format(_T("%f"), BSTDWa);
	m_strBSTDWa = str_BSTDWa;
	str_BSTDWv.Format(_T("%f"), BSTDWv);
	m_strBSTDWv = str_BSTDWv;
	str_CMWa.Format(_T("%f"), CMWa);
	m_strCMWa = str_CMWa;
	str_CMWv.Format(_T("%f"), CMWv);
	m_strCMWv = str_CMWv;
	str_CSTDWa.Format(_T("%f"), CSTDWa);
	m_strCSTDWa = str_CSTDWa;
	str_CSTDWv.Format(_T("%f"), CSTDWv);
	m_strCSTDWv = str_CSTDWv;


	UpdateData(FALSE);
	//proc1.join();
}






