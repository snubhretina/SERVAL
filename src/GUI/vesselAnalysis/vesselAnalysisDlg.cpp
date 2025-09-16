#pragma once
#include "stdafx.h"

#include "vesselAnalysis.h"
#include "vesselAnalysisDlg.h"
#include "afxdialogex.h"
#include "VesselImgTransfer.h"
#include "EllipseFit.h"
#include "vascularity.hpp"
#include "branch_vectorization.hpp"

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

protected:
	DECLARE_MESSAGE_MAP()
public:

};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()

CVesselAnalysisDlg::CVesselAnalysisDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVesselAnalysisDlg::IDD, pParent)
	, m_artery_vein_mouse_cursor_change(0)
	, m_AV_enable(0)
	, m_editing_mode(0)
	, m_FE_control(TRUE)
	, m_radio_control_value(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVesselAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_WHOLE_IMG_VIEW, m_editing_board);

	DDX_Control(pDX, IDC_CHECK_Overlay, m_check_Overlay);
	DDX_Control(pDX, IDC_CHECK_Diameter, m_check_Diameter);
	DDX_Control(pDX, IDC_CHECK_Centerline, m_check_centerline);

	DDX_Radio(pDX, IDC_RADIO_Aanlasys1, (int &)m_radio_control_value);
	DDX_Control(pDX, IDC_ELAPSED_TIMER_TEXT, ELAPSED_TIMER_TEXT);
	DDX_Control(pDX, IDC_PAUSE_TIMER_TEXT, PAUSE_TIMER_TEXT);
	DDX_Control(pDX, IDC_UNDO_REDO_NUM_TEXT, UNDO_REDO_TEXT);
	DDX_Control(pDX, IDC_RESET_BUTTON, m_reset_button);
	DDX_Control(pDX, IDC_FUNDUS_IMAGE_LIST_BOX, m_fundus_image_list_box);

	DDX_Control(pDX, IDC_EDITING_STATE_TEXT, m_editing_state_text);
	DDX_Control(pDX, IDC_EDITING_MODE_TEXT2, editing_mode_text);
	DDX_Control(pDX, IDC_CURRENT_DATA_COUNT_TEXT, cur_data_count_text);
}

BEGIN_MESSAGE_MAP(CVesselAnalysisDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BUTTON_LOAD_DIR, &CVesselAnalysisDlg::OnClickedButtonLoadDir)
	ON_BN_CLICKED(IDC_BUTTON_PREV_IMAGE, &CVesselAnalysisDlg::OnBnClickedButtonPrevImage)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_IMAGE, &CVesselAnalysisDlg::OnBnClickedButtonNextImage)

	ON_LBN_SELCHANGE(IDC_FUNDUS_IMAGE_LIST_BOX, &CVesselAnalysisDlg::OnLbnSelchangeFundusImageListBox)

	ON_BN_CLICKED(IDC_INIT_MASK_AND_DISC, &CVesselAnalysisDlg::OnBnClickedInitMaskAndDisc)

	ON_BN_CLICKED(IDC_CHECK_Overlay, &CVesselAnalysisDlg::OnBnClickedCheckOverlay)
	ON_BN_CLICKED(IDC_CHECK_Diameter, &CVesselAnalysisDlg::OnBnClickedCheckDiameter)
	ON_BN_CLICKED(IDC_CHECK_Centerline, &CVesselAnalysisDlg::OnBnClickedCheckCenterline)

	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_Aanlasys1, IDC_RADIO_Aanlasys3, RadioCtrl)

	ON_BN_CLICKED(IDC_RESET_BUTTON, &CVesselAnalysisDlg::OnBnClickedResetButton)
	ON_BN_CLICKED(IDC_FINISH_BUTTON, &CVesselAnalysisDlg::OnBnClickedFinishButton)
	ON_BN_CLICKED(IDC_ProgramErrorReport, &CVesselAnalysisDlg::OnBnClickedErrorReport)

	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

// CvesselAnalysisDlg �޽��� ó����
BOOL CVesselAnalysisDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ���� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// ���� ���� �� �α��� ������ //
	if (create_account_window.DoModal() == IDOK) {
		m_account_name_cstr = create_account_window.m_account_name;
	}
	else {
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
	}
	m_test_mode = TRUE;
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);	
	
	m_radio_and_check_button_vec = { IDC_RADIO_Aanlasys1, IDC_RADIO_Aanlasys2, IDC_RADIO_Aanlasys3, IDC_CHECK_Overlay, IDC_CHECK_Diameter, IDC_RESET_BUTTON, IDC_INIT_MASK_AND_DISC };
	EnableCheckButtonAndRadioButton(&m_radio_and_check_button_vec, FALSE);
	GetDlgItem(IDC_FINISH_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_Centerline)->EnableWindow(FALSE);
	
	m_mouse_point_in_editing_board = CPoint(0, 0);
	m_is_loaded_cur_img = FALSE;
	m_cur_img_idx = 0;
	m_ROI_zoom_ratio = 1000;
	
	m_is_checked_overlay = FALSE;
	m_is_checked_diameter = FALSE;
	m_is_modified = FALSE;
	
	m_is_pressed_ctrl = FALSE;
	m_is_pressed_mouse_L = FALSE;
	m_is_pressed_mouse_R = FALSE;
	
	/*m_waiting_time_thread = TRUE;*/
	m_waiting_write_check_point = TRUE;
	
	// ���� �ʿ�
	m_b_edtied = FALSE;
	utils.work_process_idx = 0;

	m_drawing_EB_radius_size = 10;

	m_is_checked_centerline = FALSE;
	
	m_edit_option = 0;
	m_is_zoom = TRUE;

	cur_log_flag = true;
	m_cur_image_brightness = 100;

	m_editing_mode_text = CString("None");
	m_cur_data_count_text = CString("0 / 0");

	m_mouse_point = cv::Point(0, 0);
	utils.work_process_num = 21;

	m_editing_board_width_prev = 0;
	m_editing_board_height_prev = 0;

	CRect cur_editing_board_size;
	m_editing_board.GetClientRect(&cur_editing_board_size);
	utils.editing_board_size = cv::Size(cur_editing_board_size.Width(), cur_editing_board_size.Height());
	utils.pad_size = cv::Size(cur_editing_board_size.Width(), cur_editing_board_size.Height());

	m_time_format = _T("%02d:%02d:%02d");
	m_undo_redo_num_format = _T("%02d / %02d");
	logger.time_format_cstr = _T("%04d-%02d-%02d %02d:%02d:%02d ");
	
	m_zero_time_cstr = ChangeTimeFormat();

	m_VesselImgTransfer.server_ip = params.server_ip;
	m_VesselImgTransfer.server_port_1 = params.server_port_1;
	m_VesselImgTransfer.server_port_2 = params.server_port_2;
	m_account_name = std::string(CT2CA(m_account_name_cstr.operator LPCWSTR()));
	m_VesselImgTransfer.m_svr_account_name = m_account_name;

	m_command_check_time = CTimeSpan(0, 0, 0, params.waiting_time);
	m_time_thread_sleep = TRUE;
	
	logger.CurTime();
	logger.log_cstr.Format(params.account_name_log_cstr, m_account_name_cstr);	

	if (!m_VesselImgTransfer.SendRealTimeLog(logger.cur_time_cstr +  logger.log_cstr))
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
	}

	UpdateData(FALSE);
	return TRUE;
}

// OnSysCommand that runs on SERVER termination.
void CVesselAnalysisDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == SC_CLOSE)
	{
		if (m_is_loaded_cur_img)
		{
			if (!m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing)
			{
				if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
				{
					utils.is_modified = TRUE;
					m_time_thread_kill = TRUE;

					//while (m_waiting_time_thread)
					//{
					//	Sleep(100);
					//}
					utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
					utils.WriteElapsedTime(params.Path, m_fundus_img_data_vec[m_cur_img_idx]);
				}
			}
		}

		logger.log_cstr.Format(params.closing_serval_log_cstr);
		logger.WriteLog();
		m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);
		CDialogEx::OnSysCommand(nID, lParam);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// CalculateZoomRatio function that runs on calculating zoom ratio and ratio between Editing board and image.
void CVesselAnalysisDlg::CalculateZoomRatio(cv::Mat &cur_view) {
	CRect cur_editing_board_size;
	m_editing_board.GetClientRect(&cur_editing_board_size);

	m_editing_board_zoom_ratio = m_ROI_zoom_ratio / (1000.f);

	m_editing_board_width_prev = cur_editing_board_size.Width();
	m_editing_board_height_prev = cur_editing_board_size.Height();

	m_editing_board_width = m_editing_board_width_prev;
	m_editing_board_height = m_editing_board_height_prev;

	m_editing_board_ratio_x = m_editing_board_width / float(cur_view.cols);
	m_editing_board_ratio_y = m_editing_board_height / float(cur_view.rows);
}

// CalculateZoomRatio function that runs on calculating only zoom ratio.
void CVesselAnalysisDlg::CalculateZoomRatio() {
	m_editing_board_zoom_ratio = m_ROI_zoom_ratio / (1000.f);
}

 //CalculateEllipseMask function that get m_origin_diameter_mask and m_origin_circle_mask.
 //m_origin_diameter_mask -> Vessel zone mask (circle region mask)
 //m_origin_circle_mask -> Zone circle mask (only circle boundary mask)
void CVesselAnalysisDlg::CalculateEllipseMask(Eigen::VectorXd &disk_ellipse_inform, int &disk_diameter)
{
	m_origin_diameter_mask = cv::Mat::zeros(m_origin_img.rows, m_origin_img.cols, CV_8UC1);
	m_origin_circle_mask = cv::Mat::zeros(m_origin_img.rows, m_origin_img.cols, CV_8UC1);

	cv::RotatedRect rotatedRect(cv::Point2d(disk_ellipse_inform(1), disk_ellipse_inform(0)), cv::Size2d(disk_ellipse_inform(3) * 2, disk_ellipse_inform(2) * 2), disk_ellipse_inform(4));

	cv::circle(m_origin_circle_mask, cv::Point(disk_ellipse_inform(1), disk_ellipse_inform(0)), disk_diameter * 5, cv::Scalar(255), 2);
	cv::circle(m_origin_circle_mask, cv::Point(disk_ellipse_inform(1), disk_ellipse_inform(0)), 1, cv::Scalar(255), 2);

	cv::circle(m_origin_diameter_mask, cv::Point(disk_ellipse_inform(1), disk_ellipse_inform(0)), disk_diameter * 5, cv::Scalar(255), -1);
	
	// Visualize Zone B/C - 231108 SJ.Go
	if (m_is_checked_diameter == TRUE)
	{
		cv::circle(m_origin_circle_mask, cv::Point(disk_ellipse_inform(1), disk_ellipse_inform(0)), disk_diameter * 2, cv::Scalar(255), 2);
		cv::circle(m_origin_circle_mask, cv::Point(disk_ellipse_inform(1), disk_ellipse_inform(0)), disk_diameter * 3, cv::Scalar(255), 2);
	}

	if (disk_ellipse_inform(2) == disk_ellipse_inform(3))
	{
		cv::ellipse(m_origin_circle_mask, rotatedRect, cv::Scalar(255), 2);
	}
	else
	{
		cv::ellipse(m_origin_circle_mask, rotatedRect, cv::Scalar(128), 2);
	}
	
	cv::ellipse(m_origin_diameter_mask, rotatedRect, cv::Scalar(0), cv::FILLED);

	m_origin_diameter_mask = 255 - m_origin_diameter_mask;
}

//OnPaint function that combine masks to one image and draw image to Editing board.
void CVesselAnalysisDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);

		CalculateZoomRatio();

		int x = (m_editing_board_width - cxIcon + 1) / 2;
		int y = (m_editing_board_height - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (m_is_loaded_cur_img)
		{
			int cropped_EB_radius_size = 1.0;
			cv::Mat cur_cropped_view, cur_view;
			m_cropped_img.copyTo(cur_view);

			if (!m_is_mouse_moving)
			{
				if (m_is_checked_overlay)
				{
					std::vector<cv::Mat> bgr, bg_bgr, endp_fat_viz_bgr, mask_bg_bgr, temp_mask_bg_bgr;
					cv::Mat mask_bgr;

					if (m_is_checked_centerline == TRUE) {
						cv::split(m_centerline_img, bgr);
					}
					else {
						m_cur_cropped_mask.copyTo(mask_bgr);
					}

					// Draw Optic Disc and Zone C - SJ.Go 231109
					// for visual inspection.
					// if possess Ellipse information.
					if (m_is_modified == TRUE) {
						if (m_fundus_img_data_vec[m_cur_img_idx].is_ellipse_fit == TRUE) {
							mask_bgr.setTo(cv::Vec3b(0, 0, 0), m_cropped_diameter_mask);
						}

						// Draw manual edit result to cur_view.
						cv::split(mask_bgr, mask_bg_bgr);

						if (m_editing_mode == 0)
						{
							cur_view.setTo(cv::Vec3b(255, 0, 0), mask_bg_bgr[0]);
							cur_view.setTo(cv::Vec3b(0, 0, 255), mask_bg_bgr[2]);
							cv::Mat crossing = mask_bg_bgr[0] & mask_bg_bgr[2];

							cur_view.setTo(cv::Vec3b(255, 0, 255), crossing);
						}
						else if (m_editing_mode == 1)
						{
							cur_view.setTo(cv::Vec3b(0, 0, 255), mask_bg_bgr[2]);

						}
						else if (m_editing_mode == 2)
						{
							cur_view.setTo(cv::Vec3b(255, 0, 0), mask_bg_bgr[0]);
						}
					}

					// darw zone circle boundary. Yellow is circle and Cyan is ellipse circle
					if (m_fundus_img_data_vec[m_cur_img_idx].is_ellipse_fit == true)
					{
						cur_view.setTo(cv::Vec3b(0, 255, 255), m_cropped_circle_mask == 255);
						cur_view.setTo(cv::Vec3b(255, 255, 0), m_cropped_circle_mask == 128);
					}
				}
					if (m_edit_option == 3) {
						for (int x = 0; x < origin_viusalize_vessel_map.cols; x++)
							for (int y = 0; y < origin_viusalize_vessel_map.rows; y++) {
								if (origin_viusalize_vessel_map.at<uchar>(y, x) != 0) {
									cur_view.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 255, 0);
								}
							}
					}

					// Draw ellipse boundary mouse points. (m_edit_option = 0)
					if (m_edit_option == 0 && pt_OD_bondary_Xlist.size() > 0)
					{
						for (int i = 0; i < pt_OD_bondary_Xlist.size(); i++)
						{
							cv::circle(cur_view, cv::Point(pt_OD_bondary_Xlist[i] - m_zoom_img_offset.x, pt_OD_bondary_Ylist[i] - m_zoom_img_offset.y), 2, cv::Scalar(255, 255, 0), -1);
						}
					}

					// Draw interactive edit mouse points. (m_edit_option = 2 and push 'i' key)
					if (m_is_connect_pts == true && m_edit_option == 2 && pt_connect_Xlist.size() > 0)
					{
						for (int i = 0; i < pt_connect_Xlist.size(); i++)
						{
							cv::circle(cur_view, cv::Point(pt_connect_Xlist[i], pt_connect_Ylist[i]), 2, cv::Scalar(255, 255, 0), -1);
						}
					}/*
					m_is_modified = FALSE;*/
				}

			// Resize cropped image to fit Editing Board size and Draw image to Editing Board
			cv::resize(cur_view, m_drawing_view, cv::Size(m_editing_board_width, m_editing_board_height), 0, 0, cv::InterpolationFlags::INTER_LINEAR);

			cv::Mat drawing_view;
			m_drawing_view.copyTo(drawing_view);
			cropped_EB_radius_size = m_drawing_EB_radius_size * std::round(m_editing_board_zoom_ratio);

			DrawLine(drawing_view, m_mouse_point, cropped_EB_radius_size);
			DrawPicture(drawing_view);
			m_is_mouse_moving = FALSE;
		}
		CDialogEx::OnPaint();
	}
}

HCURSOR CVesselAnalysisDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	if (uMsg == BFFM_INITIALIZED) SendMessage(hwnd, BFFM_SETSELECTION, TRUE, pData);
	return 0;

}

void CVesselAnalysisDlg::OnClickedButtonLoadDir()
{
	// load dialog to set dir
	ITEMIDLIST *pidlBrowse = NULL;
	BROWSEINFO brInfo;
	brInfo.hwndOwner = GetSafeHwnd();
	memset(&brInfo, 0, sizeof(brInfo));
	brInfo.lpfn = BrowseCallbackProc;
	brInfo.pszDisplayName = m_selected_path;
	brInfo.lpszTitle = _T("Select Directory");
	brInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	CString strInitPath = _T("C:\\");
	brInfo.lParam = (LPARAM)strInitPath.GetString();
	pidlBrowse = ::SHBrowseForFolder(&brInfo);
	
	if (pidlBrowse != NULL)
	{
		SHGetPathFromIDList(pidlBrowse, m_selected_path);

		std::wstring selected_path_wstr(m_selected_path);
		params.Path.SetDirectoryPath(std::string(selected_path_wstr.begin(), selected_path_wstr.end()), m_account_name);
		
		m_VesselImgTransfer.dataset_name = params.Path.dataset_name;

		if (utils.IsPathExist(params.Path.fundus_img_path.c_str()))
		{
			if (!utils.IsPathExist(params.Path.init_mask_img_path.c_str())) 
			{
				utils.MakeDirecotry(params.Path.init_mask_img_path);
			}

			if (!utils.IsPathExist(params.Path.result_path.c_str()))
			{
				utils.MakeDirecotry(params.Path.result_path);

				utils.MakeDirecotry(params.Path.result_mask_path);
				utils.MakeDirecotry(params.Path.result_ROI_mask_path);
				utils.MakeDirecotry(params.Path.result_metric_path);
				utils.MakeDirecotry(params.Path.result_elapsed_time_path);
				utils.MakeDirecotry(params.Path.result_disc_info_path);
			}

			if (!utils.IsPathExist(params.Path.editing_img_path.c_str()))
			{
				utils.MakeDirecotry(params.Path.editing_img_path);
			}

			if (!utils.IsPathExist(params.Path.log_path.c_str()))
			{
				utils.MakeDirecotry(params.Path.log_path);
			}

			m_fundus_img_data_vec.clear();
			m_cur_img_idx = utils.LoadDataList(params.Path, m_fundus_img_data_vec, m_fundus_image_list_box);
			
			params.Path.SetCheckPointPath(m_fundus_img_data_vec[m_cur_img_idx].fundus_name);

			m_fundus_img_num = m_fundus_img_data_vec.size();
			
			logger.log_path = params.Path.log_path + params.Path.log_name;
			logger.log_cstr.Format(params.data_loading_log_cstr, m_fundus_img_num);
			logger.WriteLog();
			m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);
			
			m_is_loaded_cur_img = true;
			
			EnableFundusListBoxHorizontalScroll();
			m_fundus_image_list_box.SetCurSel(m_cur_img_idx);
			SetData();
			OnPaint();
		}
		else
		{
			Invalidate();
			m_is_loaded_cur_img = false;
			MessageBox(_T("1_fundus ������ �������� �ʽ��ϴ�."), _T("Directory Error"), MB_OK | MB_ICONINFORMATION);
		}
	}
	else
		return;
}

void CVesselAnalysisDlg::EnableCheckButtonAndRadioButton(std::vector<int> *m_radio_and_check_button_vec, bool radio_and_check_button_enable)
{
	for (int button = 0; button != m_radio_and_check_button_vec->size(); button++) {
		GetDlgItem((*m_radio_and_check_button_vec)[button])->EnableWindow(radio_and_check_button_enable);
	}
}

void CVesselAnalysisDlg::EnableFundusListBoxHorizontalScroll()
{
	TEXTMETRIC   tm;
	CDC *image_list_box_cdc = m_fundus_image_list_box.GetDC();
	CFont* image_list_box_cfont = m_fundus_image_list_box.GetFont();
	CFont* image_list_box_old_cfont = image_list_box_cdc->SelectObject(image_list_box_cfont);
	CSize text_width;
	CString fundus_name;

	int dx = 0;
	image_list_box_cdc->GetTextMetrics(&tm);

	for (int idx = 0; idx < m_fundus_image_list_box.GetCount(); idx++)
	{
		m_fundus_image_list_box.GetText(idx, fundus_name);
		text_width = image_list_box_cdc->GetTextExtent(fundus_name);
		text_width.cx += tm.tmAveCharWidth;

		if (text_width.cx > dx)
			dx = text_width.cx;
	}
	image_list_box_cdc->SelectObject(image_list_box_old_cfont);
	m_fundus_image_list_box.ReleaseDC(image_list_box_cdc);

	m_fundus_image_list_box.SetHorizontalExtent(dx);
}

void CVesselAnalysisDlg::ApplyEllipseMask(cv::Mat &img, cv::Mat &masked_img)
{
	std::vector<cv::Mat> mask_bgr;
	cv::split(img, mask_bgr);
	if (m_fundus_img_data_vec[m_cur_img_idx].is_ellipse_fit == TRUE) {
		cv::Mat EB_back_artery, EB_back_vein;
		cv::bitwise_and(mask_bgr[0], m_cropped_diameter_mask, EB_back_artery);
		cv::bitwise_and(mask_bgr[2], m_cropped_diameter_mask, EB_back_vein);

		mask_bgr[0].setTo(cv::Scalar(0), m_cropped_diameter_mask);
		mask_bgr[1].setTo(cv::Scalar(0), m_cropped_diameter_mask);
		mask_bgr[2].setTo(cv::Scalar(0), m_cropped_diameter_mask);
	}
}

// Load fundus image, vessel mask, ellipse info
//
// m_origin_img -> padded fundus image.
// m_cropped_img -> cropped fundus image by zoom ratio.

void CVesselAnalysisDlg::SetData()
{
	m_editing_board.GetClientRect(&m_editing_board_size);
	m_editing_board_width = m_editing_board_size.Width();
	m_editing_board_height = m_editing_board_size.Height();

	utils.LoadFundusAndPreprocess(m_fundus_img_data_vec[m_cur_img_idx], m_origin_img);
	
	m_cur_data_count_text.Format(_T("%d / %d"), m_cur_img_idx + 1, m_fundus_img_data_vec.size());
	SetDlgItemText(IDC_CURRENT_DATA_COUNT_TEXT, m_cur_data_count_text);

	m_origin_offset_pt = cv::Point(m_origin_img.cols / 2, m_origin_img.rows / 2);

	// Calculate zoom ratio and offset for cropping image.
	CalculateZoomRatio(m_origin_img);
	m_zoom_img_offset = CalculateOffset(m_origin_img, m_origin_offset_pt);
	
	m_zoom_ROI_rect = cv::Rect(
		m_zoom_img_offset.x,
		m_zoom_img_offset.y,
		int(m_origin_img.cols / m_editing_board_zoom_ratio),
		int(m_origin_img.rows / m_editing_board_zoom_ratio)
	);

	// Crop image using m_zoom_ROI_rect.
	CropZoomImg(m_origin_img, m_zoom_ROI_rect, m_cropped_img);
	m_origin_img.copyTo(m_remain_origin_img);

	if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing)
	{
		utils.ReadCheckPoint(params.Path, m_fundus_img_data_vec[m_cur_img_idx], m_origin_mask, m_work_process_vec);

		m_fundus_img_data_vec[m_cur_img_idx].ConvertOriEllipseInfoToEB();

		//m_cur_restricted_bn_mask = TheshodMask(m_cur_origin_mask, m_cur_editing_mask);
		TheshodMask(m_origin_mask, m_cur_origin_mask, m_cur_restricted_origin_mask);

		m_is_checked_overlay = TRUE;
		m_is_checked_diameter = TRUE;

		CalculateEllipseMask(m_fundus_img_data_vec[m_cur_img_idx].ori_disk_ellipse_inform, m_fundus_img_data_vec[m_cur_img_idx].ori_disc_diameter);

		// Crop masks by m_zoom_ROI_rect.
		CropZoomImg(m_origin_circle_mask, m_zoom_ROI_rect, m_cropped_circle_mask);
		CropZoomImg(m_origin_diameter_mask, m_zoom_ROI_rect, m_cropped_diameter_mask);
		CropZoomImg(m_cur_origin_mask, m_zoom_ROI_rect, m_cur_cropped_mask);
		CropZoomImg(m_cur_restricted_origin_mask, m_zoom_ROI_rect, m_cur_cropped_restricted_mask);

		m_is_modified = TRUE;

		utils.work_process_idx = 0;
		threshold_radius = 40;

		EnableCheckButtonAndRadioButton(&m_radio_and_check_button_vec, FALSE);
		GetDlgItem(IDC_FINISH_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_INIT_MASK_AND_DISC)->EnableWindow(FALSE);

		m_check_Overlay.SetCheck(1);
		m_check_Diameter.SetCheck(1);
		m_check_centerline.SetCheck(0);

		m_is_checked_centerline = FALSE;
		
		SetDlgItemText(IDC_EDITING_STATE_TEXT, params.editing_state_vec[2]);

		CString elapsed_time_cstr;
		ChangeTimeFormat(m_fundus_img_data_vec[m_cur_img_idx].elapsed_time, elapsed_time_cstr);
		SetDlgItemText(IDC_ELAPSED_TIMER_TEXT, elapsed_time_cstr);
		SetDlgItemText(IDC_PAUSE_TIMER_TEXT, m_zero_time_cstr);
		m_undo_redo_num_text.Format(m_undo_redo_num_format, 00, 00);
	}
	else
	{
		if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess) {
			utils.ReadCheckPoint(params.Path, m_fundus_img_data_vec[m_cur_img_idx], m_origin_mask, m_work_process_vec);

			TheshodMask(m_origin_mask, m_cur_origin_mask, m_cur_restricted_origin_mask);

			m_is_checked_overlay = TRUE;
			m_is_checked_diameter = TRUE;

			CalculateEllipseMask(m_fundus_img_data_vec[m_cur_img_idx].ori_disk_ellipse_inform, m_fundus_img_data_vec[m_cur_img_idx].ori_disc_diameter);

			// Crop masks by m_zoom_ROI_rect.
			CropZoomImg(m_origin_circle_mask, m_zoom_ROI_rect, m_cropped_circle_mask);
			CropZoomImg(m_origin_diameter_mask, m_zoom_ROI_rect, m_cropped_diameter_mask);
			CropZoomImg(m_cur_origin_mask, m_zoom_ROI_rect, m_cur_cropped_mask);
			CropZoomImg(m_cur_restricted_origin_mask, m_zoom_ROI_rect, m_cur_cropped_restricted_mask);

			utils.work_process_idx = 0;
			threshold_radius = 40;

			EnableCheckButtonAndRadioButton(&m_radio_and_check_button_vec, true);
			GetDlgItem(IDC_FINISH_BUTTON)->EnableWindow(TRUE);
			GetDlgItem(IDC_INIT_MASK_AND_DISC)->EnableWindow(TRUE);

			m_check_Overlay.SetCheck(1);
			m_check_Diameter.SetCheck(1);

			m_radio_control_value = 1;

			//if (m_is_checked_centerline)
			//{
			//	ThinArteryAndVein();
			//}

			m_is_modified = TRUE;
			m_edit_option = 1;

			switch (m_editing_mode)
			{
			case 0:
				m_editing_mode_text = CString("All");
				SetDlgItemText(IDC_EDITING_MODE_TEXT2, m_editing_mode_text);
				break;

			case 1:
				m_editing_mode_text = CString("Artery");
				SetDlgItemText(IDC_EDITING_MODE_TEXT2, m_editing_mode_text);
				break;

			case 2:
				m_editing_mode_text = CString("Vein");
				SetDlgItemText(IDC_EDITING_MODE_TEXT2, m_editing_mode_text);
			}

			m_time_thread_sleep = FALSE;
			/*m_waiting_time_thread = TRUE;*/

			if (m_time_thread == NULL)
				m_time_thread = AfxBeginThread(TimeThread, this);
			else
				m_time_thread->ResumeThread();
			
			SetDlgItemText(IDC_EDITING_STATE_TEXT, params.editing_state_vec[1]);
			ChangeUndoRedoNumText();
		}
		else {
			EnableCheckButtonAndRadioButton(&m_radio_and_check_button_vec, false);
			GetDlgItem(IDC_FINISH_BUTTON)->EnableWindow(FALSE);
			GetDlgItem(IDC_INIT_MASK_AND_DISC)->EnableWindow(TRUE);
			
			m_work_process_vec.clear();

			m_check_Overlay.SetCheck(0);
			m_check_Diameter.SetCheck(0);
			m_check_centerline.SetCheck(0);

			m_is_checked_overlay = false;
			m_is_checked_diameter = false;
			m_is_checked_centerline = false;

			m_zero_time_cstr = ChangeTimeFormat();

			SetDlgItemText(IDC_EDITING_STATE_TEXT, params.editing_state_vec[0]);
			SetDlgItemText(IDC_ELAPSED_TIMER_TEXT, m_zero_time_cstr);
			SetDlgItemText(IDC_PAUSE_TIMER_TEXT, m_zero_time_cstr);

			m_undo_redo_num_text.Format(m_undo_redo_num_format, 00, 00);
			ChangeUndoRedoNumText(m_undo_redo_num_text);
		}
	}
	UpdateData(false);
}

cv::Mat CVesselAnalysisDlg::TheshodMask(cv::Mat &origin_mask, cv::Mat &editing_mask)
{
	std::vector<cv::Mat> bgr;
	cv::Mat restricted_bn_mask;
	origin_mask = (origin_mask > 150);
	editing_mask = origin_mask.clone();

	cv::split(editing_mask, bgr);
	restricted_bn_mask = (bgr[0] > 200 | bgr[2] > 200);

	return restricted_bn_mask;
}

void CVesselAnalysisDlg::TheshodMask(cv::Mat &origin_mask, cv::Mat &editing_mask, cv::Mat &restricted_bn_mask)
{
	std::vector<cv::Mat> bgr;
	cv::Mat new_restricted_bn_mask;
	origin_mask = (origin_mask > 150);
	editing_mask = origin_mask.clone();

	cv::split(editing_mask, bgr);
	new_restricted_bn_mask = (bgr[0] > 200 | bgr[2] > 200);
	new_restricted_bn_mask.copyTo(restricted_bn_mask);
}

cv::Mat CVesselAnalysisDlg::TheshodMask(cv::Mat &origin_mask)
{
	std::vector<cv::Mat> bgr;
	cv::Mat restricted_bn_mask;

	cv::split(origin_mask, bgr);
	restricted_bn_mask = (bgr[0] > 200 | bgr[2] > 200);

	return restricted_bn_mask;
}

// Covert mouse points based on Edting board to origin image base (padded image) and cropped image base (padded and cropped image).
void CVesselAnalysisDlg::ComputeCurePoint()
{
	CalculateZoomRatio(m_origin_img);

	bool tmp_flag = false;
	int mouse_pt_in_cropped_img_x = int(((m_mouse_point_in_editing_board.x / m_editing_board_ratio_x) / m_editing_board_zoom_ratio));
	int mouse_pt_in_cropped_img_y = int(((m_mouse_point_in_editing_board.y / m_editing_board_ratio_y) / m_editing_board_zoom_ratio));

	int mouse_pt_in_origin_img_x = int(m_zoom_img_offset.x + mouse_pt_in_cropped_img_x);
	int mouse_pt_in_origin_img_y = int(m_zoom_img_offset.y + mouse_pt_in_cropped_img_y);

	m_start_pt_in_cropped_img = cv::Point (mouse_pt_in_cropped_img_x, mouse_pt_in_cropped_img_y);
	m_start_pt_in_origin_img = cv::Point(mouse_pt_in_origin_img_x, mouse_pt_in_origin_img_y);
}

cv::Point CVesselAnalysisDlg::CalculateOffset(cv::Mat &cur_view, cv::Point &offset)
{
	if (m_is_pressed_ctrl & m_is_pressed_mouse_L)
	{
		int dx = int(((m_end_pt_in_cropped_img.x - m_start_pt_in_cropped_img.x)) / m_editing_board_zoom_ratio);
		int dy = int(((m_end_pt_in_cropped_img.y - m_start_pt_in_cropped_img.y)) / m_editing_board_zoom_ratio);

		cv::Point tmp_old = cv::Point(offset);
		offset.x += dx;
		offset.y += dy;

		int cur_offset_x = offset.x - int(cur_view.cols / m_editing_board_zoom_ratio / 2.);
		int cur_offset_y = offset.y - int(cur_view.rows / m_editing_board_zoom_ratio / 2.);

		if (cur_offset_x < 0 | cur_offset_x + int(cur_view.cols / m_editing_board_zoom_ratio) >= cur_view.cols |
			cur_offset_y < 0 | cur_offset_y + int(cur_view.rows / m_editing_board_zoom_ratio) >= cur_view.rows)
			offset = tmp_old;
	}

	int cur_offset_x = offset.x - int(cur_view.cols / m_editing_board_zoom_ratio / 2.);
	int cur_offset_y = offset.y - int(cur_view.rows / m_editing_board_zoom_ratio / 2.);

	if (cur_offset_x < 0)
	{
		offset.x -= cur_offset_x;
		cur_offset_x = 0;
	}
	else if (cur_offset_x + int(cur_view.cols / m_editing_board_zoom_ratio) > cur_view.cols)
	{
		offset.x -= cur_offset_x - (cur_view.cols - int(cur_view.cols / m_editing_board_zoom_ratio));
		cur_offset_x = cur_view.cols - int(cur_view.cols / m_editing_board_zoom_ratio);
	}

	if (cur_offset_y < 0)
	{
		offset.y -= cur_offset_y;
		cur_offset_y = 0;
	}
	else if (cur_offset_y + int(cur_view.rows / m_editing_board_zoom_ratio) > cur_view.rows)
	{
		offset.y -= cur_offset_y - (cur_view.rows - int(cur_view.rows / m_editing_board_zoom_ratio));
		cur_offset_y = cur_view.rows - int(cur_view.rows / m_editing_board_zoom_ratio);
	}
	return cv::Point(cur_offset_x, cur_offset_y);
}

cv::Point CVesselAnalysisDlg::CalculateOffsetInZoomOriginImg(cv::Mat &cur_view, cv::Point &disp_offset)
{
	if (m_is_pressed_ctrl & m_is_pressed_mouse_L)
	{
		int dx = int(((m_end_pt_in_cropped_img.x - m_start_pt_in_cropped_img.x)) / m_editing_board_zoom_ratio);
		int dy = int(((m_end_pt_in_cropped_img.y - m_start_pt_in_cropped_img.y)) / m_editing_board_zoom_ratio);
		cv::Point tmp_old = cv::Point(disp_offset);
		disp_offset.x += dx;
		disp_offset.y += dy;

		int cur_offset_x = disp_offset.x - int(cur_view.cols / m_editing_board_zoom_ratio / 2.);
		int cur_offset_y = disp_offset.y - int(cur_view.rows / m_editing_board_zoom_ratio / 2.);

		if (cur_offset_x < 0 | cur_offset_x + int(cur_view.cols / m_editing_board_zoom_ratio) >= cur_view.cols |
			cur_offset_y < 0 | cur_offset_y + int(cur_view.rows / m_editing_board_zoom_ratio) >= cur_view.rows)
			disp_offset = tmp_old;
	}

	int cur_offset_x = disp_offset.x - int(cur_view.cols / m_editing_board_zoom_ratio / 2.);
	int cur_offset_y = disp_offset.y - int(cur_view.rows / m_editing_board_zoom_ratio / 2.);

	if (cur_offset_x < 0)
	{
		disp_offset.x -= cur_offset_x;
		cur_offset_x = 0;
	}
	else if (cur_offset_x + int(cur_view.cols / m_editing_board_zoom_ratio) > cur_view.cols)
	{
		disp_offset.x -= cur_offset_x - (cur_view.cols - int(cur_view.cols / m_editing_board_zoom_ratio));
		cur_offset_x = cur_view.cols - int(cur_view.cols / m_editing_board_zoom_ratio);
	}

	if (cur_offset_y < 0)
	{
		disp_offset.y -= cur_offset_y;
		cur_offset_y = 0;
	}
	else if (cur_offset_y + int(cur_view.rows / m_editing_board_zoom_ratio) > cur_view.rows)
	{
		disp_offset.y -= cur_offset_y - (cur_view.rows - int(cur_view.rows / m_editing_board_zoom_ratio));
		cur_offset_y = cur_view.rows - int(cur_view.rows / m_editing_board_zoom_ratio);
	}
	return cv::Point(cur_offset_x, cur_offset_y);
}

cv::Point CVesselAnalysisDlg::CalculateOffsetInZoomImg(cv::Mat &cur_view, cv::Point &disp_offset)
{
	if (m_is_pressed_ctrl & m_is_pressed_mouse_L)
	{
		//int dx = int((((m_end_pt_in_cropped_img.x - m_start_pt_in_cropped_img.x) / m_editing_board_ratio_x)) / m_editing_board_zoom_ratio);
		//int dy = int((((m_end_pt_in_cropped_img.y - m_start_pt_in_cropped_img.y) / m_editing_board_ratio_y)) / m_editing_board_zoom_ratio);

		int dx = int(((m_end_pt_in_cropped_img.x - m_start_pt_in_cropped_img.x)) / m_editing_board_zoom_ratio);
		int dy = int(((m_end_pt_in_cropped_img.y - m_start_pt_in_cropped_img.y)) / m_editing_board_zoom_ratio);

		cv::Point tmp_old = cv::Point(disp_offset);
		disp_offset.x += dx;
		disp_offset.y += dy;
		
		int cur_offset_x = disp_offset.x - int(cur_view.cols / m_editing_board_zoom_ratio / 2.);
		int cur_offset_y = disp_offset.y - int(cur_view.rows / m_editing_board_zoom_ratio / 2.);

		if (cur_offset_x < 0 | cur_offset_x + int(cur_view.cols / m_editing_board_zoom_ratio) >= cur_view.cols |
			cur_offset_y < 0 | cur_offset_y + int(cur_view.rows / m_editing_board_zoom_ratio) >= cur_view.rows)
			disp_offset = tmp_old;
	}

	int cur_offset_x = disp_offset.x - int(cur_view.cols / m_editing_board_zoom_ratio / 2.);
	int cur_offset_y = disp_offset.y - int(cur_view.rows / m_editing_board_zoom_ratio / 2.);

	if (cur_offset_x < 0)
	{
		disp_offset.x -= cur_offset_x;
		cur_offset_x = 0;
	}
	else if (cur_offset_x + int(cur_view.cols / m_editing_board_zoom_ratio) > cur_view.cols)
	{
		disp_offset.x -= cur_offset_x - (cur_view.cols - int(cur_view.cols / m_editing_board_zoom_ratio));
		cur_offset_x = cur_view.cols - int(cur_view.cols / m_editing_board_zoom_ratio);
	}

	if (cur_offset_y < 0)
	{
		disp_offset.y -= cur_offset_y;
		cur_offset_y = 0;
	}
	else if (cur_offset_y + int(cur_view.rows / m_editing_board_zoom_ratio) > cur_view.rows)
	{
		disp_offset.y -= cur_offset_y - (cur_view.rows - int(cur_view.rows / m_editing_board_zoom_ratio));
		cur_offset_y = cur_view.rows - int(cur_view.rows / m_editing_board_zoom_ratio);
	}
	return cv::Point(cur_offset_x, cur_offset_y);
}

void CVesselAnalysisDlg::DrawLine(cv::Mat& zoom_img, const cv::Point &pt, int drawing_radius_size)
{
	if (!m_check_mode)
	{
		if (m_edit_option == 1)
		{
			if (m_editing_mode == 0)
			{
				if (m_artery_vein_mouse_cursor_change)
				{
					cv::line(zoom_img, pt, pt, CV_RGB(0, 0, 255), drawing_radius_size);
				}
				else
				{
					cv::line(zoom_img, pt, pt, CV_RGB(255, 0, 0), drawing_radius_size);
				}
			}
			else if (m_editing_mode == 1)
			{
				cv::line(zoom_img, pt, pt, CV_RGB(255, 0, 0), drawing_radius_size);
			}
			else if (m_editing_mode == 2)
			{
				cv::line(zoom_img, pt, pt, CV_RGB(0, 0, 255), drawing_radius_size);
			}
		}
		else if (m_edit_option == 2)
		{
			if (m_is_connect_pts == true)
			{
				cv::line(zoom_img, pt, pt, CV_RGB(0, 255, 255), 2);
			}
			else
			{
				if (m_FE_control)
				{
					if (m_editing_mode == 0)
					{
						if (m_artery_vein_mouse_cursor_change)
						{
							cv::line(zoom_img, pt, pt, CV_RGB(0, 0, 255), drawing_radius_size);
						}
						else
						{
							cv::line(zoom_img, pt, pt, CV_RGB(255, 0, 0), drawing_radius_size);
						}
					}
					else if (m_editing_mode == 1)
					{
						cv::line(zoom_img, pt, pt, CV_RGB(255, 0, 0), drawing_radius_size);
					}
					else if (m_editing_mode == 2)
					{
						cv::line(zoom_img, pt, pt, CV_RGB(0, 0, 255), drawing_radius_size);
					}
				}
				else
				{
					cv::line(zoom_img, pt, pt, CV_RGB(255, 255, 255), drawing_radius_size);
				}
			}
		}
		else if (m_edit_option == 3)
		{
			cv::line(zoom_img, pt, pt, CV_RGB(0, 255, 0), drawing_radius_size);
		}
	}
	else
	{
		cv::line(zoom_img, pt, pt, CV_RGB(0, 255, 0), drawing_radius_size);
	}
}

void CVesselAnalysisDlg::CropZoomImg(cv::Mat &img, cv::Rect zoom_ROI_rect, cv::Mat &zoom_img)
{
	img(zoom_ROI_rect).copyTo(zoom_img);
}

// Render image to Editing board using direct2d lib,
void CVesselAnalysisDlg::DrawPicture(cv::Mat &zoom_img)
{
	// CStatic 컨트롤의 HWND 가져오기
	HWND hwnd = m_editing_board.GetSafeHwnd();

	// CStatic의 크기 얻기
	CRect rect;
	m_editing_board.GetClientRect(&rect);

	static bool firstTime = true;
	if (firstTime) {
		// Direct2D 팩토리 생성
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);

		// 렌더 타겟 생성
		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rect.Width(), rect.Height())),
			&pRenderTarget
		);

		firstTime = false;
	}

	cv::Mat matBGRA;
	cv::cvtColor(zoom_img, matBGRA, cv::COLOR_BGR2BGRA);

	// 비트맵 속성 설정
	D2D1_BITMAP_PROPERTIES bitmapProperties = D2D1::BitmapProperties(
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
	);

	// Direct2D Bitmap 생성
	pRenderTarget->CreateBitmap(
		D2D1::SizeU(matBGRA.cols, matBGRA.rows),
		matBGRA.data,
		matBGRA.step[0],
		&bitmapProperties,
		&pBitmap
	);

	// 그리기 시작
	pRenderTarget->BeginDraw();

	// 배경을 흰색으로 지우기
	//pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// 비트맵을 화면에 그리기
	pRenderTarget->DrawBitmap(
		pBitmap,
		D2D1::RectF(0, 0, rect.Width(), rect.Height())
	);

	// 그리기 완료
	HRESULT hr = pRenderTarget->EndDraw();
	if (FAILED(hr)) {
		// 에러 처리
	}

	if (pBitmap) pBitmap->Release();
}

void CVesselAnalysisDlg::OnBnClickedButtonPrevImage()
{
	if (!m_is_loaded_cur_img)
		return;

	if (m_cur_img_idx > 0)
	{	
		if (!m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing)
		{
			if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
			{
				m_time_thread_sleep = TRUE;
				//while (m_waiting_time_thread)
				//{
				//	Sleep(100);
				//};

				utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
				utils.WriteElapsedTime(params.Path, m_fundus_img_data_vec[m_cur_img_idx]);
				ChangeUndoRedoNumText();
			}
		}

		m_cur_img_idx -= 1;
		params.Path.SetCheckPointPath(m_fundus_img_data_vec[m_cur_img_idx].fundus_name);
		m_fundus_image_list_box.SetCurSel(m_cur_img_idx);

		pair_AendPoint.clear();
		pair_VendPoint.clear();

		m_cur_data_count_text.Format(_T("%d / %d"), m_cur_img_idx + 1, m_fundus_img_data_vec.size());
		SetDlgItemText(IDC_CURRENT_DATA_COUNT_TEXT, m_cur_data_count_text);

		//logger.log_cstr.Format(params.prev_image_button_log_cstr, m_cur_img_idx + 1, m_fundus_img_num, params.editing_mode_vec[m_editing_mode]);
		//logger.WriteLog();
		//m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);

		SetData();

		UpdateData(false);
		OnPaint();
	}
}

void CVesselAnalysisDlg::OnBnClickedButtonNextImage()
{
	if (!m_is_loaded_cur_img)
		return;

	if (m_cur_img_idx < m_fundus_img_num - 1)
	{
		if (!m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing)
		{
			if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
			{
				m_time_thread_sleep = TRUE;
				//while (m_waiting_time_thread)
				//{
				//	Sleep(100);
				//}

				utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
				utils.WriteElapsedTime(params.Path, m_fundus_img_data_vec[m_cur_img_idx]);
				ChangeUndoRedoNumText();
			}
		}

		m_cur_img_idx += 1;
		params.Path.SetCheckPointPath(m_fundus_img_data_vec[m_cur_img_idx].fundus_name);
		m_fundus_image_list_box.SetCurSel(m_cur_img_idx);
		m_AV_enable = 0;

		m_cur_data_count_text.Format(_T("%d / %d"), m_cur_img_idx + 1, m_fundus_img_data_vec.size());
		SetDlgItemText(IDC_CURRENT_DATA_COUNT_TEXT, m_cur_data_count_text);

		//logger.log_cstr.Format(params.next_image_button_log_cstr, m_cur_img_idx + 1, m_fundus_img_num, params.editing_mode_vec[m_editing_mode]);
		//logger.WriteLog();
		//m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);

		pair_AendPoint.clear();
		pair_VendPoint.clear();
		SetData();
		
		UpdateData(false);
		OnPaint();
	}
}

BOOL CVesselAnalysisDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) & m_is_loaded_cur_img)
	{
		m_reset_pause_timer = TRUE;
		if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing)
		{
			if ((pMsg->wParam == VK_LEFT) | (pMsg->wParam == 'a') | (pMsg->wParam == 'A') | (pMsg->wParam == VK_RIGHT) | (pMsg->wParam == 'd') | (pMsg->wParam == 'D'))
			{
			}
			else
				return TRUE;
		}

		if (pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}

		if ((pMsg->wParam == '1' && !m_is_pressed_ctrl))
		{
			m_editing_mode = 0;

			m_editing_mode_text = CString("All");
			SetDlgItemText(IDC_EDITING_MODE_TEXT2, m_editing_mode_text);
			
			//logger.log_cstr.Format(params.changed_editing_mode_log_cstr, m_cur_img_idx + 1, m_fundus_img_num, params.editing_mode_vec[m_editing_mode]);
			//logger.WriteLog();
			//m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);
			
			UpdateData(false);
			OnPaint();
			return true;
		}
		else if ((pMsg->wParam == '2' && !m_is_pressed_ctrl))
		{
			m_editing_mode = 1;

			m_editing_mode_text = CString("Artery");
			SetDlgItemText(IDC_EDITING_MODE_TEXT2, m_editing_mode_text);
			//logger.log_cstr.Format(params.changed_editing_mode_log_cstr, m_cur_img_idx + 1, m_fundus_img_num, params.editing_mode_vec[m_editing_mode]);
			//logger.WriteLog();
			//m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);

			UpdateData(false);
			OnPaint();
			return true;
		}
		else if ((pMsg->wParam == '3' && !m_is_pressed_ctrl))
		{
			m_editing_mode = 2;

			m_editing_mode_text = CString("Vein");
			SetDlgItemText(IDC_EDITING_MODE_TEXT2, m_editing_mode_text);

			//logger.log_cstr.Format(params.changed_editing_mode_log_cstr, m_cur_img_idx + 1, m_fundus_img_num, params.editing_mode_vec[m_editing_mode]);
			//logger.WriteLog();
			//m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);

			UpdateData(false);
			OnPaint();
			return true;
		}

		if ((pMsg->wParam == '1') & m_is_pressed_ctrl)
		{
			if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess) {
				m_edit_option = 0;
				m_radio_control_value = 0;
				m_is_dragging_disc_diameter = false;

				m_cur_origin_optic_disc_circle = cv::Mat::zeros(m_origin_img.rows, m_origin_img.cols, CV_8UC1);
				cv::circle(m_cur_origin_optic_disc_circle, m_fundus_img_data_vec[m_cur_img_idx].ori_disc_center_point, m_fundus_img_data_vec[m_cur_img_idx].ori_disc_diameter, 255, 10);

				OnPaint();
				UpdateData(false);
			}
			return true;
		}
		else if ((pMsg->wParam == '2') & m_is_pressed_ctrl)
		{
			if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess) {
				m_edit_option = 1;
				m_radio_control_value = 1;
				OnPaint();
				UpdateData(false);
				return true;
			}
		}
		else if ((pMsg->wParam == '3')  & m_is_pressed_ctrl)
		{
			if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess) {
				m_edit_option = 2;
				m_radio_control_value = 2;
				OnPaint();
				UpdateData(false);
				return true;
			}
		}

		else if ((pMsg->wParam == VK_RIGHT) | (pMsg->wParam == 'd') | (pMsg->wParam == 'D'))
		{
			if (m_is_pressed_ctrl)
			{	
				utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
				utils.WriteElapsedTime(params.Path, m_fundus_img_data_vec[m_cur_img_idx]);
				ChangeUndoRedoNumText();

				m_cur_img_idx += 20;

				if (m_cur_img_idx >= m_fundus_img_data_vec.size())
				{
					m_cur_img_idx = m_fundus_img_data_vec.size() - 1;
				}
				params.Path.SetCheckPointPath(m_fundus_img_data_vec[m_cur_img_idx].fundus_name);
				
				SetData();
				UpdateData(false);
				OnPaint();
			}
			else
				OnBnClickedButtonNextImage();
			return true;
		}
		else if ((pMsg->wParam == VK_LEFT) | (pMsg->wParam == 'a') | (pMsg->wParam == 'A'))
		{
			if (m_is_pressed_ctrl)
			{
				utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
				utils.WriteElapsedTime(params.Path, m_fundus_img_data_vec[m_cur_img_idx]);
				ChangeUndoRedoNumText();

				m_cur_img_idx -= 20;
				if (m_cur_img_idx < 0)
					m_cur_img_idx = 0;

				params.Path.SetCheckPointPath(m_fundus_img_data_vec[m_cur_img_idx].fundus_name);
				SetData();

				UpdateData(false);
				OnPaint();

			}
			else
				OnBnClickedButtonPrevImage();
			return true;
		}
		else if ((pMsg->wParam == 'c') | (pMsg->wParam == 'C'))
		{
			if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess) {
				if (m_check_mode)
					m_check_mode = FALSE;
				else
					m_check_mode = TRUE;
				return true;
			}
			else {
				return true;
			}
		}
		else if ((pMsg->wParam == 'q') | (pMsg->wParam == 'Q'))
		{
			if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess) {
				m_check_Overlay.SetCheck(!m_check_Overlay.GetCheck());
				OnBnClickedCheckOverlay();
				return true;
			}
			else{
				return true;
			}
		}

		else if ((pMsg->wParam == 'w') | (pMsg->wParam == 'W'))
		{
			if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess) {
				m_check_Diameter.SetCheck(!m_check_Diameter.GetCheck());
				OnBnClickedCheckDiameter();
				return true;
			}
			else {
				return true;
			}

		}

		else if ((pMsg->wParam == 'e') | (pMsg->wParam == 'E'))
		{
			if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess) {
				//m_check_centerline.SetCheck(!m_check_centerline.GetCheck());
				//OnBnClickedCheckCenterline();
				//OnPaint();
				return true;
			}
			else {
				return true;
			}
		}
		else if ((pMsg->wParam == 'r') | (pMsg->wParam == 'R'))
		{
			m_FE_control = !m_FE_control;
			OnPaint();
			return true;
		}
		// if Press 'i' -> Interactive Connect mode On - 231110 SJ.Go
		else if ((pMsg->wParam == 'i') | (pMsg->wParam == 'I'))
		{
			if (m_edit_option == 2)
			{
				if (m_is_connect_pts)
				{
					m_is_connect_pts = false;
					m_drawing_EB_radius_size = m_prev_drawing_radius_size;
					pt_connect_Xlist.clear();
					pt_connect_Ylist.clear();
				}
				else
				{
					m_is_connect_pts = true;
					m_prev_drawing_radius_size = m_drawing_EB_radius_size;
					m_drawing_EB_radius_size = 1;
					pt_connect_Xlist.clear();
					pt_connect_Ylist.clear();
				}				
			}
			else
				m_is_connect_pts = false;
			OnPaint();
		return true;
		}
		else if (pMsg->wParam == VK_CONTROL)
		{
			m_is_pressed_ctrl = true;
			return true;
		}
		if (pMsg->wParam == VK_SHIFT)
		{
			m_b_press_shift = true;
			return true;
		}
		else if (((pMsg->wParam == 'z') | (pMsg->wParam == 'Z')) & m_is_pressed_ctrl)
		{
			if (m_AV_enable == 0) {
				if (m_b_press_shift)
				{
					Redo();
					m_is_modified = TRUE;
					OnPaint();
				}
				else
				{
					Undo();
					m_is_modified = TRUE;
					OnPaint();
				}
			}
			return true;
		}
		else if (pMsg->wParam == VK_SPACE)
		{
			if (m_edit_option == 0)
			{
				CRect cur_editing_board_size;
				m_editing_board.GetClientRect(&cur_editing_board_size);

				// if complete click several points on optic disc bondary -> Press SpaceBar -> Fit Ellipse - (SJ.Go 231109)
				Eigen::VectorXd coeffs;
				Eigen::VectorXd m_cartpool;
				if (pt_OD_bondary_Xlist.size() > 0)
				{
					// caculate Ellipse fit.
					try
					{
						coeffs = fitEllipse(pt_OD_bondary_Ylist, pt_OD_bondary_Xlist);
						m_cartpool = ellipseParameters(coeffs);

						int height_boundary, width_boundary;

						if (m_fundus_img_data_vec[m_cur_img_idx].pad_axis == 0)
						{
							height_boundary = int(m_fundus_img_data_vec[m_cur_img_idx].pad_length);
							width_boundary = int(m_fundus_img_data_vec[m_cur_img_idx].ori_fundus_cols);
						}
						else
						{
							height_boundary = int(m_fundus_img_data_vec[m_cur_img_idx].ori_fundus_rows);
							width_boundary = int(m_fundus_img_data_vec[m_cur_img_idx].pad_length);
						}

						if (m_cartpool(0) < 0 | m_cartpool(0) > height_boundary | m_cartpool(1) < 0 | m_cartpool(1) > width_boundary)
						{
							int OK = AfxMessageBox(params.ellipse_fit_error_message, MB_OK | MB_APPLMODAL);
							pt_OD_bondary_Xlist.clear();
							pt_OD_bondary_Ylist.clear();
							return true;
						}

						if (std::isnan(m_cartpool(0)) | std::isnan(m_cartpool(1)) | std::isnan(m_cartpool(2)))
						{
							int OK = AfxMessageBox(params.ellipse_fit_error_message, MB_OK | MB_APPLMODAL);
							pt_OD_bondary_Xlist.clear();
							pt_OD_bondary_Ylist.clear();
							return true;
						}
					}
					catch (std::exception & e)
					{
						int OK = AfxMessageBox(params.ellipse_fit_error_message, MB_OK | MB_APPLMODAL);
						pt_OD_bondary_Xlist.clear();
						pt_OD_bondary_Ylist.clear();
						return true;
					}
					pt_OD_bondary_Xlist.clear();
					pt_OD_bondary_Ylist.clear();

					// m_cartpool - ellipse information  (0-1 : center point, 2-3 : axis, 4 : phi)
					m_fundus_img_data_vec[m_cur_img_idx].SetOriDiscInfo(m_cartpool);
					
					m_fundus_img_data_vec[m_cur_img_idx].SetEBSize(cur_editing_board_size);
					m_fundus_img_data_vec[m_cur_img_idx].ConvertOriEllipseInfoToEB(m_cartpool);

					CalculateEllipseMask(m_fundus_img_data_vec[m_cur_img_idx].ori_disk_ellipse_inform, m_fundus_img_data_vec[m_cur_img_idx].ori_disc_diameter);

					CropZoomImg(m_origin_circle_mask, m_zoom_ROI_rect, m_cropped_circle_mask);
					CropZoomImg(m_origin_diameter_mask, m_zoom_ROI_rect, m_cropped_diameter_mask);

					m_is_checked_diameter = TRUE;
					m_radio_control_value = 1;
					m_edit_option = 1;

					std::string check_point_root_path = params.Path.editing_img_path + m_fundus_img_data_vec[m_cur_img_idx].fundus_name + "\\";
					utils.WriteCSV(check_point_root_path + params.Path.optic_disc_check_point_name, m_fundus_img_data_vec[m_cur_img_idx]);
				}
			}
			else if (m_edit_option == 1)
			{
				if (m_is_connect_pts)
				{
					// add function InteractEditUpdate() - 231201 SJ.Go
					if (pt_connect_Xlist.size() == 0)
					{
						int OK = AfxMessageBox(L"i�� ���� ��, ���� ������ ��, Space�� �����ּ���.", MB_OK | MB_APPLMODAL);
						m_is_connect_pts = false;
						OnPaint();
					}
					else
					{
						InteractEditUpdate();
						InteractEditOriginRes();

						utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
						ChangeUndoRedoNumText();
					}
					UpdateData(false);
					return true;
				}
				else
				{
					if (m_editing_mode == 0)
					{
						if (m_artery_vein_mouse_cursor_change)
							m_artery_vein_mouse_cursor_change = 0;
						else
							m_artery_vein_mouse_cursor_change = 1;
					}
				}
			}
			else if (m_edit_option == 2)
			{
				if (m_is_connect_pts)
				{
					// add function InteractEditUpdate() - 231201 SJ.Go
					if (pt_connect_Xlist.size() == 0)
					{
						int OK = AfxMessageBox(L"i�� ���� ��, ���� ������ ��, Space�� �����ּ���.", MB_OK | MB_APPLMODAL);
						m_is_connect_pts = false;
						OnPaint();
					}
					else
					{
						InteractEditUpdate();
						InteractEditOriginRes();

						utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
						ChangeUndoRedoNumText();
					}
					UpdateData(false);
					return true;
				}
				else
				{
					if (m_editing_mode == 0)
					{
						if (m_artery_vein_mouse_cursor_change)
							m_artery_vein_mouse_cursor_change = 0;
						else
							m_artery_vein_mouse_cursor_change = 1;
					}
				}
			}
			OnPaint();
			UpdateData(false);
			return true;
		}
		else if (pMsg->wParam == VK_UP)
		{
			if (m_is_pressed_ctrl)
			{
				DrawingRadiusUpdate(1);
				OnPaint();
				return true;
			}
			else
			{
				if (GetFocus()->GetDlgCtrlID() == IDC_FUNDUS_IMAGE_LIST_BOX)
				{
					OnBnClickedButtonPrevImage();
				}
				else
				{
					ROIUpdate(1);
					OnPaint();
				}
				return true;
			}
		}
		else if (pMsg->wParam == VK_DOWN)
		{
			if (m_is_pressed_ctrl)
			{
				DrawingRadiusUpdate(-1);
				OnPaint();
				return true;
			}
			else
			{
				if (GetFocus()->GetDlgCtrlID() == IDC_FUNDUS_IMAGE_LIST_BOX)
				{
					OnBnClickedButtonNextImage();
				}
				else
				{
					ROIUpdate(-1);
					OnPaint();
				}
				return true;
			}
		}
	}
	else if ((pMsg->message == WM_KEYUP) & m_is_loaded_cur_img)
	{
		if (pMsg->wParam == VK_CONTROL)
		{
			m_is_pressed_ctrl = false;
			return true;
		}
		else if (pMsg->wParam == VK_SHIFT)
		{
			m_b_press_shift = false;
			return true;
		}

	}
	else if (pMsg->message == WM_SYSKEYDOWN) {
		return true;
	}
	
	
	return CDialogEx::PreTranslateMessage(pMsg);
}

// Get new offset and re calculate m_zoom_ROI_rect.
BOOL CVesselAnalysisDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_is_pressed_ctrl)
	{
		m_is_zoom = TRUE;
		ROIUpdate(zDelta);
		CalculateZoomRatio();

		m_zoom_img_offset = CalculateOffset(m_origin_img, m_origin_offset_pt);

		m_zoom_ROI_rect = cv::Rect(
			m_zoom_img_offset.x,
			m_zoom_img_offset.y,
			int(m_origin_img.cols / m_editing_board_zoom_ratio),
			int(m_origin_img.rows / m_editing_board_zoom_ratio)
		);

		CropZoomImg(m_origin_img, m_zoom_ROI_rect, m_cropped_img);
		if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
		{
			CropZoomImg(m_origin_circle_mask, m_zoom_ROI_rect, m_cropped_circle_mask);
			CropZoomImg(m_origin_diameter_mask, m_zoom_ROI_rect, m_cropped_diameter_mask);
			CropZoomImg(m_cur_origin_mask, m_zoom_ROI_rect, m_cur_cropped_mask);
			CropZoomImg(m_cur_restricted_origin_mask, m_zoom_ROI_rect, m_cur_cropped_restricted_mask);
		}
		m_is_modified = TRUE;
	}
	else
	{
		if (m_edit_option != 3) {
			DrawingRadiusUpdate(zDelta);
		}
	}
	UpdateData(false);
	OnPaint();
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void CVesselAnalysisDlg::DrawingRadiusUpdate(int updown)
{
	if (updown < 0)
	{
		m_drawing_EB_radius_size -= 1;
		if (m_drawing_EB_radius_size <= 0)
			m_drawing_EB_radius_size = 1;
		
	}
	else if (updown > 0)
	{
		m_drawing_EB_radius_size += 1;
		if (m_drawing_EB_radius_size > 300)
			m_drawing_EB_radius_size = 300;
	}
	UpdateData(false);
}


void CVesselAnalysisDlg::threshold_update(int updown)
{
	if (updown < 0)
	{
		threshold_radius -= 1;
		if (threshold_radius < 0)
			threshold_radius = 0;
	}
	else if (updown > 0)
	{
		threshold_radius += 1;
		if (threshold_radius > 255)
			threshold_radius = 255;
	}
	UpdateData(false);
}
void CVesselAnalysisDlg::ROIUpdate(int updown)
{
	if (updown < 0)
	{
		m_ROI_zoom_ratio -= 100;
		if (m_ROI_zoom_ratio < 1000)
			m_ROI_zoom_ratio = 1000;
	}
	else if (updown > 0)
	{
		m_ROI_zoom_ratio += 100;
		if (m_ROI_zoom_ratio > 6000)
			m_ROI_zoom_ratio = 6000;
	}
	UpdateData(false);
}

void CVesselAnalysisDlg::InitDrawnAreaMask()
{
	m_drawing_cropped_mask = cv::Mat::zeros(m_cur_cropped_mask.size(), CV_8UC1);
}

// Get mouse points and draw mouse points to vessel mask.
void CVesselAnalysisDlg::ManualEditUpdate()
{
	if (!m_is_loaded_cur_img)
		return;

	m_b_edtied = true;

	auto cropped_img_radius_size = m_drawing_EB_radius_size  / m_editing_board_ratio_x;
	cv::line(m_drawing_cropped_mask, m_start_pt_in_cropped_img, m_end_pt_in_cropped_img, 255, int(cropped_img_radius_size));
	cv::threshold(m_drawing_cropped_mask, m_drawing_cropped_mask, 0, 255, cv::ThresholdTypes::THRESH_BINARY);

	std::vector<cv::Mat> splited_cur_EB_editing_mask_bgr;
	cv::split(m_cur_cropped_mask, splited_cur_EB_editing_mask_bgr);
	
	if (!m_check_mode)
	{
		if (m_edit_option == 1) {
			//m_drawn_area_mask.setTo(0, m_cur_restricted_bn_mask == 0);
			m_drawing_cropped_mask.setTo(0, m_cur_cropped_restricted_mask == 0);

			if (m_editing_mode == 0) {

				if (m_artery_vein_mouse_cursor_change)
				{
					//m_cur_editing_mask.setTo(cv::Scalar(255, 0, 0), m_drawn_area_mask);
					m_cur_cropped_mask.setTo(cv::Scalar(255, 0, 0), m_drawing_cropped_mask);
				}
				else
				{
					//m_cur_editing_mask.setTo(cv::Scalar(0, 0, 255), m_drawn_area_mask);
					m_cur_cropped_mask.setTo(cv::Scalar(0, 0, 255), m_drawing_cropped_mask);
				}

			}
			else if (m_editing_mode == 1)
			{
				//m_cur_editing_mask.setTo(cv::Scalar(0, 0, 255), m_drawn_area_mask);
				m_cur_cropped_mask.setTo(cv::Scalar(0, 0, 255), m_drawing_cropped_mask);
			}
				
			else if (m_editing_mode == 2)
			{
				//m_cur_editing_mask.setTo(cv::Scalar(255, 0, 0), m_drawn_area_mask);
				m_cur_cropped_mask.setTo(cv::Scalar(255, 0, 0), m_drawing_cropped_mask);
			}
				
		}
		else if (m_edit_option == 2) {

			if (m_FE_control)
			{
				if (!m_is_connect_pts)
				{
					if (m_editing_mode == 0) {
						if (m_artery_vein_mouse_cursor_change)
						{
							//splited_cur_editing_mask_bgr[0].setTo(cv::Scalar(255), m_drawn_area_mask);
							splited_cur_EB_editing_mask_bgr[0].setTo(cv::Scalar(255), m_drawing_cropped_mask);
						}
						else
						{
							//splited_cur_editing_mask_bgr[2].setTo(cv::Scalar(255), m_drawn_area_mask);
							splited_cur_EB_editing_mask_bgr[2].setTo(cv::Scalar(255), m_drawing_cropped_mask);
						}
					}
					else if (m_editing_mode == 1) 
					{
						//splited_cur_editing_mask_bgr[2].setTo(cv::Scalar(255), m_drawn_area_mask);
						splited_cur_EB_editing_mask_bgr[2].setTo(cv::Scalar(255), m_drawing_cropped_mask);
					}
					else if (m_editing_mode == 2) 
					{
						//splited_cur_editing_mask_bgr[0].setTo(cv::Scalar(255), m_drawn_area_mask);
						splited_cur_EB_editing_mask_bgr[0].setTo(cv::Scalar(255), m_drawing_cropped_mask);
					}
					//cv::merge(splited_cur_editing_mask_bgr, m_cur_editing_mask);
					cv::merge(splited_cur_EB_editing_mask_bgr, m_cur_cropped_mask);
				}
			}
			else
			{
				if (!m_is_connect_pts)
				{
					if (m_editing_mode == 0) {
						//m_cur_editing_mask.setTo(cv::Scalar(0, 0, 0), m_drawn_area_mask);
						m_cur_cropped_mask.setTo(cv::Scalar(0, 0, 0), m_drawing_cropped_mask);
					}
					else if (m_editing_mode == 1)
					{
						//splited_cur_editing_mask_bgr[2].setTo(cv::Scalar(0), m_drawn_area_mask);
						splited_cur_EB_editing_mask_bgr[2].setTo(cv::Scalar(0), m_drawing_cropped_mask);
						//cv::merge(splited_cur_editing_mask_bgr, m_cur_editing_mask);
						cv::merge(splited_cur_EB_editing_mask_bgr, m_cur_cropped_mask);
					}
					else if (m_editing_mode == 2)
					{
						//splited_cur_editing_mask_bgr[0].setTo(cv::Scalar(0), m_drawn_area_mask);
						splited_cur_EB_editing_mask_bgr[0].setTo(cv::Scalar(0), m_drawing_cropped_mask);
						//cv::merge(splited_cur_editing_mask_bgr, m_cur_editing_mask);
						cv::merge(splited_cur_EB_editing_mask_bgr, m_cur_cropped_mask);
					}
				}
			}
			//m_cur_restricted_bn_mask = (splited_cur_editing_mask_bgr[0] > 0 | splited_cur_editing_mask_bgr[2] > 0);
			m_cur_cropped_restricted_mask = (splited_cur_EB_editing_mask_bgr[0] > 0 | splited_cur_EB_editing_mask_bgr[2] > 0);
		}
	}
	else
	{
		//m_cur_editing_mask.setTo(cv::Scalar(0, 255, 0), m_drawn_area_mask);
		m_cur_cropped_mask.setTo(cv::Scalar(0, 255, 0), m_drawing_cropped_mask);
		//splited_cur_editing_mask_bgr[1].setTo(cv::Scalar(255), m_drawn_area_mask);
		splited_cur_EB_editing_mask_bgr[1].setTo(cv::Scalar(255), m_drawing_cropped_mask);
		//cv::merge(splited_cur_editing_mask_bgr, m_cur_editing_mask);
		cv::merge(splited_cur_EB_editing_mask_bgr, m_cur_cropped_mask);

		//m_cur_restricted_bn_mask = (splited_cur_editing_mask_bgr[0] > 0 | splited_cur_editing_mask_bgr[1] > 0 | splited_cur_editing_mask_bgr[2] > 0);
		m_cur_cropped_restricted_mask = (splited_cur_EB_editing_mask_bgr[0] > 0 | splited_cur_EB_editing_mask_bgr[1] > 0 | splited_cur_EB_editing_mask_bgr[2] > 0);
	}
	m_is_modified = TRUE;
	OnPaint();
}

// Draw line by two mouse points.
void CVesselAnalysisDlg::InteractEditUpdate()
{
	if (!m_is_loaded_cur_img)
		return;

	m_b_edtied = true;
	std::vector<cv::Mat> splited_cropped_mask;
	
	cv::split(m_cur_cropped_mask, splited_cropped_mask);
	cv::Mat drawn_connect_mask = cv::Mat::zeros(m_cur_cropped_mask.size(), CV_8UC1);
	
	if (m_edit_option == 2)
	{
		m_is_modified = TRUE;
		utils.is_modified = TRUE;
		for (int i = 0; i < pt_connect_Xlist.size() - 1; i++)
		{
			int cur_x = pt_connect_Xlist[i];
			int cur_y = pt_connect_Ylist[i];

			int next_x = pt_connect_Xlist[i + 1];
			int next_y = pt_connect_Ylist[i + 1];

			cv::line(drawn_connect_mask, cv::Point(cur_x, cur_y), cv::Point(next_x, next_y), cv::Scalar(255), 2);
		}
		if (m_editing_mode == 0)
		{
			if (m_artery_vein_mouse_cursor_change)
			{
				splited_cropped_mask[0].setTo(cv::Scalar(255), drawn_connect_mask);
			}
			else
			{
				splited_cropped_mask[2].setTo(cv::Scalar(255), drawn_connect_mask);
			}
		}
		else if (m_editing_mode == 1)
		{
			splited_cropped_mask[2].setTo(cv::Scalar(255), drawn_connect_mask);
		}
		else if (m_editing_mode == 2)
		{
			splited_cropped_mask[0].setTo(cv::Scalar(255), drawn_connect_mask);
		}
		cv::merge(splited_cropped_mask, m_cur_cropped_mask);
		pt_connect_Xlist.clear();
		pt_connect_Ylist.clear();
		m_drawing_EB_radius_size = m_prev_drawing_radius_size;
	}
	
	OnPaint();
}

void CVesselAnalysisDlg::ChangeUndoRedoNumText()
{
	m_undo_redo_num_text.Format(m_undo_redo_num_format, m_work_process_vec.size() - 1 - utils.work_process_idx, m_work_process_vec.size()-1);
	SetDlgItemText(IDC_UNDO_REDO_NUM_TEXT, m_undo_redo_num_text);
}

void CVesselAnalysisDlg::ChangeUndoRedoNumText(CString zero_num)
{
	SetDlgItemText(IDC_UNDO_REDO_NUM_TEXT, zero_num);
}


void CVesselAnalysisDlg::Undo()
{
	if (utils.work_process_idx >= m_work_process_vec.size() - 1)
		return;

	utils.work_process_idx += 1;

	std::string check_point_path;
	check_point_path = params.Path.check_point_path + m_work_process_vec[utils.work_process_idx].prev_editied_mask_path;
	utils.LoadMaskAndPreprocess(check_point_path, m_fundus_img_data_vec[utils.work_process_idx], m_origin_mask);
	
	m_cur_restricted_origin_mask = TheshodMask(m_origin_mask, m_cur_origin_mask);

	CropZoomImg(m_cur_origin_mask, m_zoom_ROI_rect, m_cur_cropped_mask);
	CropZoomImg(m_cur_restricted_origin_mask, m_zoom_ROI_rect, m_cur_cropped_restricted_mask);

	ChangeUndoRedoNumText();
	UpdateData(false);
}

void CVesselAnalysisDlg::Redo()
{
	if (utils.work_process_idx <= 0)
		return;

	utils.work_process_idx -= 1;

	std::string check_point_path;
	check_point_path = params.Path.check_point_path + m_work_process_vec[utils.work_process_idx].prev_editied_mask_path;
	utils.LoadMaskAndPreprocess(check_point_path, m_fundus_img_data_vec[utils.work_process_idx], m_origin_mask);

	m_cur_restricted_origin_mask = TheshodMask(m_origin_mask, m_cur_origin_mask);

	CropZoomImg(m_cur_origin_mask, m_zoom_ROI_rect, m_cur_cropped_mask);
	CropZoomImg(m_cur_restricted_origin_mask, m_zoom_ROI_rect, m_cur_cropped_restricted_mask);

	ChangeUndoRedoNumText();
	UpdateData(false);
}

void CVesselAnalysisDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_is_loaded_cur_img)
		return;
	
	CRect rc;
	m_editing_board.GetWindowRect(&rc);
	ScreenToClient(rc);

	m_reset_pause_timer = TRUE;

	utils.is_modified = FALSE;
	if ((point.x >= rc.left + 5) & (point.y >= rc.top + 5) & (point.x < rc.right - 5) & (point.y < rc.bottom - 5))
	{
		auto start1 = std::chrono::high_resolution_clock::now();
		m_mouse_point_in_editing_board = CPoint(int((point.x - rc.left)), int((point.y - rc.top)));
		m_mouse_point = cv::Point(m_mouse_point_in_editing_board.x, m_mouse_point_in_editing_board.y);


		if (!m_is_connect_pts)
		{
			utils.is_modified = TRUE;
			if (m_edit_option != 3) 
			{
				//drawing on the entire map(seg + background)
				if (m_is_pressed_mouse_L & !m_is_pressed_ctrl & m_b_edtied &(m_AV_enable == 0))
				{
					ComputeCurePoint();
					m_pt_vec_in_origin_img.push_back(m_start_pt_in_origin_img);

					ManualEditUpdate();
					m_end_pt_in_cropped_img = m_start_pt_in_cropped_img;
				}
				//drawing on the segmentation image
				else if (m_is_pressed_mouse_L & m_is_pressed_ctrl)
				{
					m_start_pt_in_cropped_img = cv::Point(m_mouse_point_in_editing_board.x, m_mouse_point_in_editing_board.y);

					m_zoom_img_offset = CalculateOffset(m_origin_img, m_origin_offset_pt);

					m_zoom_ROI_rect = cv::Rect(
						m_zoom_img_offset.x,
						m_zoom_img_offset.y,
						int(m_origin_img.cols / m_editing_board_zoom_ratio),
						int(m_origin_img.rows / m_editing_board_zoom_ratio)
					);

					CropZoomImg(m_origin_img, m_zoom_ROI_rect, m_cropped_img);
					 if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
					 {
						 CropZoomImg(m_origin_circle_mask, m_zoom_ROI_rect, m_cropped_circle_mask);
						 CropZoomImg(m_origin_diameter_mask, m_zoom_ROI_rect, m_cropped_diameter_mask);
						 CropZoomImg(m_cur_origin_mask, m_zoom_ROI_rect, m_cur_cropped_mask);
						 CropZoomImg(m_cur_restricted_origin_mask, m_zoom_ROI_rect, m_cur_cropped_restricted_mask);
					 }
					
					m_end_pt_in_cropped_img = m_mouse_point;
					m_is_modified = TRUE;
				}

				else if (m_is_pressed_mouse_R & m_is_pressed_ctrl)
				{
					ComputeCurePoint();
					m_brightness_control_end_point = m_start_pt_in_cropped_img;
					
					/*m_brightness_control_end_point = ComputeCurePoint();*/
					m_cur_image_brightness = 100 + (m_brightness_control_end_point.x - m_brightness_control_start_point.x);

					m_origin_img = m_remain_origin_img * ((m_cur_image_brightness)*0.001 + 1);
					CropZoomImg(m_origin_img, m_zoom_ROI_rect, m_cropped_img);
				}
			}
			else
			{
				 m_mouse_point_in_editing_board = CPoint(int((point.x - rc.left)), int((point.y - rc.top)));
				 ComputeCurePoint();

				 cv::Vec3b & p1 = m_cur_origin_mask.at<cv::Vec3b>(m_start_pt_in_cropped_img);
				if (m_editing_mode > 0) {
					if (m_cur_origin_mask.at<cv::Vec3b>(m_start_pt_in_cropped_img)[(m_editing_mode % 2) * 2] != 0) {
						tmp_segment_label_num = broken_mask_label_map.at<int>(m_start_pt_in_cropped_img);
						if (tmp_segment_label_num > 0) {
							cur_segment_label_num = tmp_segment_label_num;
							origin_viusalize_vessel_map = (broken_mask_label_map == cur_segment_label_num) * 255;
						}
					}
				}
			}
		}
		else
		{
			m_is_mouse_moving = FALSE;
			utils.is_modified = FALSE;
		}
		auto end1 = std::chrono::high_resolution_clock::now();
		auto start2 = std::chrono::high_resolution_clock::now();

		OnPaint();
		auto end2 = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> duration1 = end1 - start1;
		std::chrono::duration<double> duration2 = end2 - start2;
		
	}
	else
	{
		m_is_pressed_mouse_L = false;
	}
}

// Convert vessel type to another type (artery -> vein, vein -> artery).
void CVesselAnalysisDlg::ConvertVesselType(cv::Mat &editing_mask, cv::Mat diameter_mask, cv::Point &mouse_point)
{
	cv::Mat vessel_converting_mask = cv::Mat::zeros(editing_mask.size(), CV_8UC1);
	cv::line(vessel_converting_mask, mouse_point, mouse_point, 255, m_drawing_EB_radius_size);

	std::vector<cv::Mat> bgr, remain_bgr;
	cv::split(editing_mask, bgr);
	cv::split(editing_mask, remain_bgr);

	cv::Mat back_artery, back_vein;
	cv::bitwise_and(bgr[0], diameter_mask, back_artery);
	cv::bitwise_and(bgr[2], diameter_mask, back_vein);

	bgr[0].setTo(cv::Scalar(0), diameter_mask);
	bgr[1].setTo(cv::Scalar(0), diameter_mask);
	bgr[2].setTo(cv::Scalar(0), diameter_mask);

	remain_bgr[0].setTo(cv::Scalar(0), 255 - diameter_mask);
	remain_bgr[1].setTo(cv::Scalar(0), 255 - diameter_mask);
	remain_bgr[2].setTo(cv::Scalar(0), 255 - diameter_mask);

	ConnectedComponent(vessel_converting_mask, bgr[2], bgr[0]);

	bgr[0] = bgr[0] + remain_bgr[0];
	bgr[1] = bgr[1] + remain_bgr[1];
	bgr[2] = bgr[2] + remain_bgr[2];

	cv::merge(bgr, editing_mask);
}

void CVesselAnalysisDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (!m_is_loaded_cur_img)
		return;

	if (!m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
		return;

	if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing)
		return;

	m_is_pressed_mouse_R = true;
	
	if (!m_is_pressed_ctrl) {
		ComputeCurePoint();
		
		CRect rc;
		m_editing_board.GetWindowRect(&rc);
		ScreenToClient(rc);

		if ((point.x >= rc.left + 5) & (point.y >= rc.top + 5) & (point.x < rc.right - 5) & (point.y < rc.bottom - 5))
		{
			ConvertVesselType(m_cur_origin_mask, m_origin_diameter_mask, m_start_pt_in_origin_img);

			CropZoomImg(m_origin_img, m_zoom_ROI_rect, m_cropped_img);
			CropZoomImg(m_origin_circle_mask, m_zoom_ROI_rect, m_cropped_circle_mask);
			CropZoomImg(m_origin_diameter_mask, m_zoom_ROI_rect, m_cropped_diameter_mask);
			CropZoomImg(m_cur_origin_mask, m_zoom_ROI_rect, m_cur_cropped_mask);
			CropZoomImg(m_cur_restricted_origin_mask, m_zoom_ROI_rect, m_cur_cropped_restricted_mask);

			utils.is_modified = TRUE;

			utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
			ChangeUndoRedoNumText();
			OnPaint();
		}
	}
	else {
		ComputeCurePoint();
		m_brightness_control_start_point = m_start_pt_in_cropped_img;
		/*m_brightness_control_start_point = ComputeCurePoint();*/
	}
	CDialogEx::OnRButtonDown(nFlags, point);
}

void CVesselAnalysisDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_is_loaded_cur_img)
		return;
	if (!m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
		return;

	if (m_is_pressed_ctrl) {
		m_remain_origin_img.copyTo(m_origin_img);
		CropZoomImg(m_origin_img, m_zoom_ROI_rect, m_cropped_img);

		m_cur_image_brightness = 100;
		OnPaint();
	}

	CDialogEx::OnRButtonDblClk(nFlags, point);
}

void CVesselAnalysisDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (!m_is_loaded_cur_img)
		return;

	if (!m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
		return;

	m_is_pressed_mouse_R = false;
	CDialogEx::OnRButtonUp(nFlags, point);
}

void CVesselAnalysisDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_is_pressed_mouse_L = true;

	if (!m_is_loaded_cur_img)
		return;

	CRect rc;
	m_editing_board.GetWindowRect(&rc);
	ScreenToClient(rc);

	if ((point.x >= rc.left + 5) & (point.y >= rc.top + 5) & (point.x < rc.right - 5) & (point.y < rc.bottom - 5))
	{
		m_end_pt_in_cropped_img = cv::Point(int(point.x - rc.left), int(point.y - rc.top));
	}

	if (!m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
		return;

	if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing)
	{
		if ((point.x >= rc.left + 5) & (point.y >= rc.top + 5) & (point.x < rc.right - 5) & (point.y < rc.bottom - 5))
		{
			int OK_Cancel = AfxMessageBox(params.finished_image_re_edit_info_message, MB_OKCANCEL | MB_SYSTEMMODAL);
			if (OK_Cancel == IDOK)
			{
				CString fundus_name_cstr(m_fundus_img_data_vec[m_cur_img_idx].fundus_name.c_str());

				logger.log_cstr.Format(params.finished_image_re_edit_log_cstr, fundus_name_cstr, m_cur_img_idx + 1);
				logger.WriteLog();
				m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);
				
				utils.RemoveResult(params.Path, m_fundus_img_data_vec[m_cur_img_idx]);
				
				m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing = FALSE;
				m_fundus_image_list_box.Invalidate();
				SetData();
			}
		}

		m_is_pressed_mouse_L = false;
		return;
	}
	
	if (m_edit_option == 0) 
	{
		if ((point.x >= rc.left + 5) & (point.y >= rc.top + 5) & (point.x < rc.right - 5) & (point.y < rc.bottom - 5))
		{
			GetDlgItem(IDC_STATIC_WHOLE_IMG_VIEW)->SetFocus();
			ComputeCurePoint();

			pt_OD_bondary_Ylist.push_back(m_start_pt_in_origin_img.y);
			pt_OD_bondary_Xlist.push_back(m_start_pt_in_origin_img.x);
			return;
		}
	}
	else if (m_edit_option == 2)
	{
		if ((point.x >= rc.left + 5) & (point.y >= rc.top + 5) & (point.x < rc.right - 5) & (point.y < rc.bottom - 5))
		{
			// Interactive connecting - click and save several center points  (SJ.Go 231108)
			if (m_is_connect_pts)
			{
				ComputeCurePoint();

				pt_connect_Ylist.push_back(m_start_pt_in_cropped_img.y);
				pt_connect_Xlist.push_back(m_start_pt_in_cropped_img.x);
				
				pt_connect_origin_Ylist.push_back(m_start_pt_in_origin_img.y);
				pt_connect_origin_Xlist.push_back(m_start_pt_in_origin_img.x);

			}
			else
			{
				pt_connect_Ylist.clear();
				pt_connect_Xlist.clear();

				pt_connect_origin_Ylist.clear();
				pt_connect_origin_Xlist.clear();

			}
		}
	}

	if (!m_is_pressed_ctrl)
	{
		if (m_AV_enable == 0) {
			InitDrawnAreaMask();
			m_b_edtied = true;
			ComputeCurePoint();
			m_end_pt_in_cropped_img = m_start_pt_in_cropped_img;

			m_pt_vec_in_origin_img.clear();
			m_pt_vec_in_origin_img.push_back(m_start_pt_in_origin_img);
			m_pt_vec_in_origin_img.push_back(m_start_pt_in_origin_img);

			ManualEditUpdate();
		
		}
		else {
			InitDrawnAreaMask();
			m_b_edtied = true;
			ComputeCurePoint();

			m_end_pt_in_cropped_img = m_start_pt_in_cropped_img;
			ManualEditUpdate();
		}
	}
	else
	{
		
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CVesselAnalysisDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_is_pressed_mouse_L = false;

	if (!m_is_loaded_cur_img)
		return;

	if (!m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
		return;
	
	if (m_edit_option == 0) {
		// visualize optic disc boundary points - SJ.Go
		OnPaint();
		return;
	}

	if (!m_is_pressed_ctrl)
	{
		ManualEditOriginRes();

		utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
		ChangeUndoRedoNumText();
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CVesselAnalysisDlg::ConnectedComponent(cv::Mat pos, cv::Mat &image)
{
	cv::Mat label;
	int num = cv::connectedComponents(~image, label, 8);

	std::vector<cv::Point> vec_pts;
	cv::findNonZero(pos, vec_pts);

	for (int i = 0; i < vec_pts.size(); i++)
	{
		int a = label.at<int>(vec_pts[i].y, vec_pts[i].x);
		cv::Mat cur_label = (label == label.at<int>(vec_pts[i].y, vec_pts[i].x));
		cur_label = cur_label.clone();
		cv::bitwise_or(image, cur_label, image);
	}
}

void CVesselAnalysisDlg::ConnectedComponent(cv::Mat pos, cv::Mat &A, cv::Mat &V)
{
	cv::Mat label_A;
	cv::Mat label_V;
	cv::Mat zero_img = V;
	int num_A = cv::connectedComponents(A, label_A, 8);
	int num_V = cv::connectedComponents(V, label_V, 8);
	
	std::vector<cv::Point> vec_pts, not_zero_label;
	cv::findNonZero(pos, vec_pts);
	bool is_A = false;
	bool is_V = false;
	for (int i = 0; i < vec_pts.size(); i++)
	{
		if (label_A.at<int>(vec_pts[i]) != 0) {
			is_A = true;
			not_zero_label.push_back(vec_pts[i]);
		}
		else if (label_V.at<int>(vec_pts[i]) != 0) {
			is_V = true;

			not_zero_label.push_back(vec_pts[i]);
		}

	}
	if (not_zero_label.size() != 0)
	{
		if (is_A != is_V)
		{
			// artery vessel map
			if (is_A)
			{
				cv::Mat cur_label = cv::Mat(label_A == label_A.at<int>(not_zero_label[0])).clone();
				A = A - cur_label;
				cv::bitwise_or(V, cur_label, V);

			}
			// vein vessel map
			else if (is_V)
			{
				cv::Mat cur_label = cv::Mat(label_V == label_V.at<int>(not_zero_label[0])).clone();
				V = V - cur_label;
				cv::bitwise_or(A, cur_label, A);
				
			}
			utils.is_modified = TRUE;
		}
		else
		{
			utils.is_modified = FALSE;
		}
	}
	else
	{
		utils.is_modified = FALSE;
	}
	return;
}


void CVesselAnalysisDlg::OnBnClickedCheckOverlay()
{
	if (!m_is_loaded_cur_img && m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
		return;


	// TODO: Add your control notification handler code here
	if (m_check_Overlay.GetCheck() == 0)
	{
		m_is_checked_overlay = false;
		m_check_Overlay.SetCheck(0);
	}
	else {
		m_is_checked_overlay = true;
		m_check_Overlay.SetCheck(1);
	}
	OnPaint();
	return;
}

void CVesselAnalysisDlg::OnBnClickedCheckDiameter()
{
	// TODO: Add your control notification handler code here
	if (!m_is_loaded_cur_img && m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
		return;
	
	if (m_check_Diameter.GetCheck() == 0)
	{
		m_is_checked_diameter = false;
		m_check_Diameter.SetCheck(0);
	}
	else {
		m_is_checked_diameter = true;
		m_check_Diameter.SetCheck(1);
	}

	CalculateEllipseMask(m_fundus_img_data_vec[m_cur_img_idx].ori_disk_ellipse_inform, m_fundus_img_data_vec[m_cur_img_idx].ori_disc_diameter);
	CropZoomImg(m_origin_circle_mask, m_zoom_ROI_rect, m_cropped_circle_mask);
	OnPaint();
}

void CVesselAnalysisDlg::ChangeTimeFormat(CTimeSpan &elapsed_time, CString &elapsed_time_cstr) 
{
	elapsed_time_cstr.Format(m_time_format, elapsed_time.GetHours(), elapsed_time.GetMinutes(), elapsed_time.GetSeconds());
}

CString CVesselAnalysisDlg::ChangeTimeFormat()
{
	CTimeSpan zero_time = CTimeSpan(0, 0, 0, 0);
	CString zero_time_cstr;
	zero_time_cstr.Format(m_time_format, zero_time.GetHours(), zero_time.GetMinutes(), zero_time.GetSeconds());
	return zero_time_cstr;
}

int CVesselAnalysisDlg::MakeInfoMessageBox(CString &info_message)
{
	int message_result = AfxMessageBox(info_message, MB_OK | MB_APPLMODAL);
	return message_result;
}


UINT CVesselAnalysisDlg::TimeThread(LPVOID _mothod)
{
	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;
	
	CTimeSpan zero_time = CTimeSpan(0, 0, 0, 0);
	CTimeSpan prev_elapsed_time = zero_time;
	CTimeSpan cur_elapsed_time;
	CTimeSpan total_elapsed_time = zero_time;
	CTimeSpan command_check_elapsed_time = zero_time;
		
	CString total_elapsed_time_cstr;
	CString command_check_elapsed_time_cstr;
	CString zero_time_cstr;
	
	CTime cur_time;
	CTime start_time = CTime::GetCurrentTime();
	CTime command_check_start_time = start_time;
	bool save_cur_time = TRUE;
	bool start_time_reset = FALSE;
	int info_message;

	while (1)
	{
		if (!pDlg->m_time_thread_kill)
		{

			if (pDlg->m_reset_pause_timer)
			{
				command_check_start_time = CTime::GetCurrentTime();;
				pDlg->m_reset_pause_timer = FALSE;
			}

			if (pDlg->m_elapsed_timer_reset)
			{
				pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time = zero_time;
				prev_elapsed_time = zero_time;

				pDlg->m_elapsed_timer_reset = FALSE;
				start_time_reset = TRUE;
			}

			if (command_check_elapsed_time == pDlg->m_command_check_time)
			{
				info_message = pDlg->MakeInfoMessageBox(pDlg->params.timer_pause_info_message);

				if (info_message == IDOK) {
					prev_elapsed_time += (cur_elapsed_time - pDlg->m_command_check_time);

					total_elapsed_time = prev_elapsed_time + pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time;
					command_check_elapsed_time = CTimeSpan(0, 0, 0, 0);

					pDlg->ChangeTimeFormat(total_elapsed_time, total_elapsed_time_cstr);
					pDlg->ChangeTimeFormat(command_check_elapsed_time, command_check_elapsed_time_cstr);

					pDlg->SetDlgItemText(IDC_ELAPSED_TIMER_TEXT, total_elapsed_time_cstr);
					pDlg->SetDlgItemText(IDC_PAUSE_TIMER_TEXT, command_check_elapsed_time_cstr);

					start_time_reset = TRUE;
				}
			}

			if (start_time_reset)
			{
				start_time = CTime::GetCurrentTime();
				command_check_start_time = start_time;
				start_time_reset = FALSE;
			}

			if (pDlg->m_time_thread_sleep)
			{
				if (save_cur_time)
				{
					total_elapsed_time = cur_elapsed_time + prev_elapsed_time + pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time;
					pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time = total_elapsed_time;
					prev_elapsed_time = zero_time;

					save_cur_time = FALSE;
				}
			}
			else
			{
				if (!save_cur_time)
				{
					start_time = CTime::GetCurrentTime();
					command_check_start_time = start_time;
					save_cur_time = TRUE;
				}

				cur_time = CTime::GetCurrentTime();
				cur_elapsed_time = cur_time - start_time;
				command_check_elapsed_time = cur_time - command_check_start_time;

				total_elapsed_time = cur_elapsed_time + prev_elapsed_time + pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time;

				pDlg->ChangeTimeFormat(total_elapsed_time, total_elapsed_time_cstr);
				pDlg->ChangeTimeFormat(command_check_elapsed_time, command_check_elapsed_time_cstr);

				pDlg->SetDlgItemText(IDC_ELAPSED_TIMER_TEXT, total_elapsed_time_cstr);
				pDlg->SetDlgItemText(IDC_PAUSE_TIMER_TEXT, command_check_elapsed_time_cstr);
			}
			Sleep(1000);
		}
		else
		{
			pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time = total_elapsed_time;
			/*pDlg->m_waiting_time_thread = FALSE;*/
			break;
		}
	}

	//while (1)
	//{
	//	if (!pDlg->m_time_thread_kill)
	//	{
	//		if (pDlg->m_time_thread_sleep) 
	//		{
	//			pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time = total_elapsed_time;
	//			prev_elapsed_time = zero_time;
	//			pDlg->m_waiting_time_thread = FALSE;
	//			pDlg->m_time_thread->SuspendThread();
	//			start_time_reset = TRUE;
	//		}

	//		if (pDlg->m_elapsed_timer_reset)
	//		{
	//			pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time = zero_time;
	//			prev_elapsed_time = zero_time;
	//			pDlg->m_elapsed_timer_reset = FALSE;
	//			start_time_reset = TRUE;
	//		}

	//		if (start_time_reset) 
	//		{
	//			start_time = CTime::GetCurrentTime();
	//			command_check_start_time = start_time;
	//			start_time_reset = FALSE;
	//		}

	//		cur_time = CTime::GetCurrentTime();

	//		if (pDlg->m_reset_pause_timer) 
	//		{
	//			command_check_start_time = cur_time;
	//			pDlg ->m_reset_pause_timer = FALSE;
	//		}
	//					
	//		cur_elapsed_time = cur_time - start_time;

	//		total_elapsed_time = cur_elapsed_time + prev_elapsed_time + pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time;
	//		command_check_elapsed_time = cur_time - command_check_start_time;
	//		
	//		pDlg->ChangeTimeFormat(total_elapsed_time, total_elapsed_time_cstr);
	//		pDlg->ChangeTimeFormat(command_check_elapsed_time, command_check_elapsed_time_cstr);

	//		pDlg->SetDlgItemText(IDC_ELAPSED_TIMER_TEXT, total_elapsed_time_cstr);
	//		pDlg->SetDlgItemText(IDC_PAUSE_TIMER_TEXT, command_check_elapsed_time_cstr);

	//		if (command_check_elapsed_time == pDlg->m_command_check_time) {
	//			info_message = pDlg->MakeInfoMessageBox(pDlg->params.timer_pause_info_message);

	//			if (info_message == IDOK) {
	//				prev_elapsed_time += (cur_elapsed_time - pDlg->m_command_check_time);
	//				
	//				total_elapsed_time = prev_elapsed_time + pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time;
	//				command_check_elapsed_time = CTimeSpan(0, 0, 0, 0);

	//				pDlg->ChangeTimeFormat(total_elapsed_time, total_elapsed_time_cstr);
	//				pDlg->ChangeTimeFormat(command_check_elapsed_time, command_check_elapsed_time_cstr);

	//				pDlg->SetDlgItemText(IDC_ELAPSED_TIMER_TEXT, total_elapsed_time_cstr);
	//				pDlg->SetDlgItemText(IDC_PAUSE_TIMER_TEXT, command_check_elapsed_time_cstr);

	//				start_time_reset = TRUE;
	//			}
	//		}
	//		Sleep(1000);
	//	}
	//	else {
	//		pDlg->m_fundus_img_data_vec[pDlg->m_cur_img_idx].elapsed_time = total_elapsed_time;
	//		pDlg->m_waiting_time_thread = FALSE;
	//		break;
	//	}
	//}
	return 0;
}

// Send padded image to Server and get init vessel mask and optic disc info.
void CVesselAnalysisDlg::OnBnClickedInitMaskAndDisc()
{
	if (!cur_log_flag) {
		AfxMessageBox(L"current connection is lost");
		return;
	}
	m_time_thread_sleep = TRUE;

	m_Waitwindow.m_VesselImgTransfer = &m_VesselImgTransfer;
	int wait_window_result = m_Waitwindow.DoModal();
	if (IDOK == wait_window_result) {
	}
	else if (IDCANCEL == wait_window_result) {
	}

	if (m_VesselImgTransfer.classification_img.empty() == false) {
		cv::Mat bgr[3], origin_bgr[3];
		cv::Mat origin_classification_img;
		m_VesselImgTransfer.classification_img.copyTo(origin_classification_img);
		
		utils.CollapsedFlip(&origin_classification_img);
		origin_classification_img = (origin_classification_img > 200);
		origin_classification_img.copyTo(m_origin_mask);
		
		cv::Mat unpadded_mask = utils.UnpadImage(m_fundus_img_data_vec[m_cur_img_idx], m_origin_mask);
		cv::imwrite(m_fundus_img_data_vec[m_cur_img_idx].AV_mask_path, unpadded_mask);

		TheshodMask(m_origin_mask, m_cur_origin_mask, m_cur_restricted_origin_mask);

		// Set opic disc info.
		m_fundus_img_data_vec[m_cur_img_idx].ori_disc_center_point.y = int(m_VesselImgTransfer.disc_center_y);
		m_fundus_img_data_vec[m_cur_img_idx].ori_disc_center_point.x = int(m_VesselImgTransfer.disc_center_x);
		m_fundus_img_data_vec[m_cur_img_idx].ori_disc_diameter = FindOpticDisc(m_origin_img, m_fundus_img_data_vec[m_cur_img_idx].ori_disc_center_point);

		// Convert optic disc info to eigen vector.
		m_fundus_img_data_vec[m_cur_img_idx].SetInitOriDiscInfo();

		CalculateEllipseMask(m_fundus_img_data_vec[m_cur_img_idx].ori_disk_ellipse_inform, m_fundus_img_data_vec[m_cur_img_idx].ori_disc_diameter);

		CropZoomImg(m_origin_circle_mask, m_zoom_ROI_rect, m_cropped_circle_mask);
		CropZoomImg(m_origin_diameter_mask, m_zoom_ROI_rect, m_cropped_diameter_mask);
		CropZoomImg(m_cur_origin_mask, m_zoom_ROI_rect, m_cur_cropped_mask);
		CropZoomImg(m_cur_restricted_origin_mask, m_zoom_ROI_rect, m_cur_cropped_restricted_mask);
		
		m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess = TRUE;
		m_fundus_image_list_box.Invalidate();
		
		std::string check_point_path = params.Path.editing_img_path + m_fundus_img_data_vec[m_cur_img_idx].fundus_name + "\\" + params.Path.optic_disc_check_point_name;
		utils.WriteCSV(check_point_path, m_fundus_img_data_vec[m_cur_img_idx]);

		std::string center_csv_path = params.Path.init_mask_img_path + m_fundus_img_data_vec[m_cur_img_idx].fundus_name + params.center_csv_name;
		utils.WriteCSV(center_csv_path, m_fundus_img_data_vec[m_cur_img_idx]);

		m_fundus_img_data_vec[m_cur_img_idx].is_ellipse_fit = TRUE;
		utils.is_modified = TRUE;

		m_is_modified = TRUE;
		utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
		ChangeUndoRedoNumText();

		m_check_Overlay.SetCheck(1);
		OnBnClickedCheckOverlay();

		m_check_Diameter.SetCheck(1);
		OnBnClickedCheckDiameter();

		m_edit_option = 1;
		m_radio_control_value = 1;
		
		EnableCheckButtonAndRadioButton(&m_radio_and_check_button_vec, TRUE);
		GetDlgItem(IDC_FINISH_BUTTON)->EnableWindow(TRUE);
		SetDlgItemText(IDC_EDITING_STATE_TEXT, params.editing_state_vec[1]);

		if (m_time_thread == NULL)
		{
			m_time_thread_sleep = FALSE;
			m_time_thread = AfxBeginThread(TimeThread, this);

			if (m_time_thread == NULL)
			{
				AfxMessageBox(_T("Ÿ�̸� Thread ���� ����"));
			}
		}
		else
		{
			m_time_thread_sleep = FALSE;
			m_waiting_time_thread = TRUE;

			m_time_thread->ResumeThread();
		}
	}
	OnPaint();
}

void CVesselAnalysisDlg::ThinArteryAndVein() {
	vector<cv::Mat> splited_mask_bgr;

	cv::split(m_cur_editing_mask, splited_mask_bgr);
	utils.ThinVessel(splited_mask_bgr[0], splited_mask_bgr[0]);
	utils.ThinVessel(splited_mask_bgr[2], splited_mask_bgr[2]);
	cv::merge(splited_mask_bgr, m_centerline_img);
}

void CVesselAnalysisDlg::OnBnClickedCheckCenterline()
{
	if (!m_is_loaded_cur_img && m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
		return;

	if (m_check_centerline.GetCheck() == 0)
	{
		m_is_checked_centerline = false;
		m_check_centerline.SetCheck(0);
	}

	else if (m_check_centerline.GetCheck() == 1) {
		ThinArteryAndVein();
		m_is_checked_centerline = true;
		m_check_centerline.SetCheck(1);
	}

	OnPaint();
	return;
}

void CVesselAnalysisDlg::RadioCtrl(UINT ID)
{	
	UpdateData(true);
	switch (m_radio_control_value)
	{
	case 0:
		m_edit_option = 0;
		break;
	case 1:
		m_edit_option = 1;
		break;
	case 2:
		m_edit_option = 2 ;
		break;
	}
	UpdateData(false);
}

void CVesselAnalysisDlg::OnBnClickedErrorReport()
{
	if (!cur_log_flag) {
		AfxMessageBox(L"current connection is lost");
		return;
	}
	m_time_thread_sleep = TRUE;
	//while (m_waiting_time_thread)
	//{
	//	Sleep(100);
	//};

	int error_report_result = m_ErrorReportWnd.DoModal();
	if (IDOK == error_report_result) {
		m_time_thread_sleep = FALSE;
		//m_waiting_time_thread = TRUE;

		if (m_time_thread == NULL)
			m_time_thread = AfxBeginThread(TimeThread, this);
		else
			m_time_thread->ResumeThread();
	}
	else if (IDCANCEL == error_report_result) {
		m_time_thread_sleep = FALSE;
		//m_waiting_time_thread = TRUE;

		if (m_time_thread == NULL)
			m_time_thread = AfxBeginThread(TimeThread, this);
		else
			m_time_thread->ResumeThread();
	}
}

int CVesselAnalysisDlg::FindOpticDisc(cv::Mat &fundus, cv::Point &opt_center)
{
	float max_disc_pixel_sum_rate = 0;
	int max_disc_diameter = 0;
	float big_disc_mask_pixel_sum_rate, small_disc_mask_pixel_sum_rate;
	
	cv::Mat bit_mask;
	cv::Mat center_mask = cv::Mat::zeros(fundus.size(), CV_8UC1);

	int diameter = int(fundus.rows / 15);
	int max_diameter = 300;
	int step = 10;

	for (int start_diameter = diameter; start_diameter <= max_diameter; start_diameter = start_diameter + step) {
		center_mask = 0;
		cv::circle(center_mask, opt_center, start_diameter, 255, -1);
		cv::bitwise_and(fundus, fundus, bit_mask, center_mask);
		
		// debuging code
		//std::string diameter_str = std::to_string(start_diameter);
		//std::string small_file_name = diameter_str + "_small_center_mask.png";
		//cv::imwrite(small_file_name, bit_mask);

		int blue = cv::sum(bit_mask)[0];
		int green = cv::sum(bit_mask)[1];
		int red = cv::sum(bit_mask)[2];

		int bit_mask_sum = std::round(((blue + green + red) / 3) / 255);
		int cente_mask_sum = std::round(cv::sum(center_mask)[0] / 255);
		small_disc_mask_pixel_sum_rate = float (bit_mask_sum) / float (cente_mask_sum);
		
		center_mask = 0;
		cv::circle(center_mask, opt_center, start_diameter + 10, 255, -1);
		cv::bitwise_and(fundus, fundus, bit_mask, center_mask);

		blue = cv::sum(bit_mask)[0];
		green = cv::sum(bit_mask)[1];
		red = cv::sum(bit_mask)[2];

		bit_mask_sum = std::round(((blue + green + red) / 3) / 255);
		cente_mask_sum = std::round(cv::sum(center_mask)[0] / 255);
		big_disc_mask_pixel_sum_rate = float(bit_mask_sum) / float(cente_mask_sum);

		// for debuging code
		//diameter_str = std::to_string(start_diameter);
		//std::string large_file_name = diameter_str + "_large_center_mask.png";
		//cv::imwrite(large_file_name, bit_mask);

		if (small_disc_mask_pixel_sum_rate - big_disc_mask_pixel_sum_rate > max_disc_pixel_sum_rate) {
			max_disc_pixel_sum_rate = small_disc_mask_pixel_sum_rate - big_disc_mask_pixel_sum_rate;
			max_disc_diameter = start_diameter;
		}
	}
	return max_disc_diameter;
}

void CVesselAnalysisDlg::OnBnClickedResetButton()
{
	m_time_thread_sleep = TRUE;
	//while (m_waiting_time_thread)
	//{
	//	Sleep(100);
	//};

	if (AfxMessageBox(params.reset_info_message, MB_OKCANCEL | MB_SYSTEMMODAL) == IDOK)
	{
		m_elapsed_timer_reset = TRUE;
		utils.ResetCheckPoint(params.Path, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
		
		CString fundus_name_cstr(m_fundus_img_data_vec[m_cur_img_idx].fundus_name.c_str());

		logger.log_cstr.Format(params.reset_log_cstr, fundus_name_cstr, m_cur_img_idx + 1);
		logger.WriteLog();
		m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);

		SetData();
		OnPaint();
	}
	else
	{
		m_time_thread_sleep = FALSE;
		//m_waiting_time_thread = TRUE;

		if (m_time_thread == NULL)
			m_time_thread = AfxBeginThread(TimeThread, this);
		else
			m_time_thread->ResumeThread();
	}


	return;
}

void CVesselAnalysisDlg::OnLbnSelchangeFundusImageListBox()
{
	GetDlgItem(IDC_FUNDUS_IMAGE_LIST_BOX)->SetFocus();
	

	if (m_fundus_image_list_box.GetCurSel() != m_cur_img_idx)
	{
		if (!m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing)
		{
			if (m_fundus_img_data_vec[m_cur_img_idx].is_finished_preprocess)
			{
				m_time_thread_sleep = TRUE;
				//while (m_waiting_time_thread)
				//{
				//	Sleep(100);
				//}
				utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
				utils.WriteElapsedTime(params.Path, m_fundus_img_data_vec[m_cur_img_idx]);
				ChangeUndoRedoNumText();
			}
		}

		m_cur_img_idx = m_fundus_image_list_box.GetCurSel();
		params.Path.SetCheckPointPath(m_fundus_img_data_vec[m_cur_img_idx].fundus_name);

		pair_AendPoint.clear();
		pair_VendPoint.clear();

		//logger.log_cstr.Format(params.selected_fundus_image_log_cstr, m_cur_img_idx + 1);
		//logger.WriteLog();
		//m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);

		SetData();

		UpdateData(false);
		OnPaint();
	}
	else
		return;
}

UINT CVesselAnalysisDlg::CalcParams(LPVOID _mothod)
{
	CVesselAnalysisDlg* pDlg = (CVesselAnalysisDlg*)AfxGetApp()->m_pMainWnd;

	/*vascularity* example = new vascularity(pDlg->m_calc_param_ROI_mask, pDlg->m_calc_param_ROI_mask);*/

	pDlg->utils.SaveResult(pDlg->params.Path, pDlg->m_calc_param_origin_mask, pDlg->m_calc_param_ROI_mask, pDlg->m_fundus_img_data_vec[pDlg->m_calc_param_img_idx]);
	
	/*pDlg->m_json_data = example->jsonString;*/
	
	
	LPVOID method = nullptr;
	UINT result = pDlg->m_VesselImgTransfer.SendResult(method);

	pDlg->m_fundus_image_list_box.Invalidate();

	/*delete example;*/

	return TRUE;
}

void CVesselAnalysisDlg::OnBnClickedFinishButton()
{
	m_time_thread_sleep = TRUE;
	/*while (m_waiting_time_thread)
	{
		Sleep(100);
	};*/
	int OK_Cancel = AfxMessageBox(params.finish_button_info_message, MB_OKCANCEL | MB_SYSTEMMODAL);
	
	if (OK_Cancel == IDOK)
	{	
		utils.is_modified = TRUE;
		//CString fundus_name_cstr(m_fundus_img_data_vec[m_cur_img_idx].fundus_name.c_str());
		//logger.log_cstr.Format(params.finish_fundus_image_log_cstr, fundus_name_cstr);
		//logger.WriteLog();
		//m_VesselImgTransfer.SendRealTimeLog(logger.log_cstr);

		m_ROI_mask = utils.GetROIFromMask(m_cur_origin_mask, m_origin_diameter_mask);

		m_unpadded_origin_mask = utils.UnpadImage(m_fundus_img_data_vec[m_cur_img_idx], m_cur_origin_mask);
		m_unpadded_ROI_mask = utils.UnpadImage(m_fundus_img_data_vec[m_cur_img_idx], m_ROI_mask);
		ChangeUndoRedoNumText();

		// parameterization
		// output : .json file
		//cv::imwrite("zone_mask.png", unpadded_ROI_mask);
		//vascularity example(unpadded_ROI_mask, unpadded_ROI_mask);
		
		utils.WriteCheckPoint(params.Path, m_cur_origin_mask, m_fundus_img_data_vec[m_cur_img_idx], m_work_process_vec);
		utils.WriteElapsedTime(params.Path, m_fundus_img_data_vec[m_cur_img_idx]);
		
		m_calc_param_img_idx = m_cur_img_idx;
		m_calc_param_origin_mask = m_unpadded_origin_mask.clone();
		m_calc_param_ROI_mask = m_unpadded_ROI_mask.clone();

		m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing = TRUE;

		m_calc_params_pthread = NULL;
		m_calc_params_pthread = AfxBeginThread(CalcParams, this);

		utils.work_process_idx = 0;
		threshold_radius = 40;

		EnableCheckButtonAndRadioButton(&m_radio_and_check_button_vec, FALSE);
		GetDlgItem(IDC_FINISH_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_INIT_MASK_AND_DISC)->EnableWindow(FALSE);

		m_check_Overlay.SetCheck(1);
		m_check_Diameter.SetCheck(1);
		m_check_centerline.SetCheck(0);

		m_is_checked_centerline = FALSE;

		SetDlgItemText(IDC_EDITING_STATE_TEXT, params.editing_state_vec[2]);

		CString elapsed_time_cstr;
		ChangeTimeFormat(m_fundus_img_data_vec[m_cur_img_idx].elapsed_time, elapsed_time_cstr);
		SetDlgItemText(IDC_ELAPSED_TIMER_TEXT, elapsed_time_cstr);
		SetDlgItemText(IDC_PAUSE_TIMER_TEXT, m_zero_time_cstr);
		m_undo_redo_num_text.Format(m_undo_redo_num_format, 00, 00);


		//int clac_params_wait_window_result = m_WaitWindowCalcParams.DoModal();
		//if (IDOK == clac_params_wait_window_result) {
		//}
		//else if (IDCANCEL == clac_params_wait_window_result) {
		//}

		//utils.SaveResult(params.Path, m_unpadded_origin_mask, m_unpadded_ROI_mask, m_fundus_img_data_vec[m_cur_img_idx], example);
		//ChangeUndoRedoNumText();

		//m_json_data = example->jsonString;

		//m_WaitwindowSendResult.VesselImgTransfer = &m_VesselImgTransfer;
		//int wait_window_result = m_WaitwindowSendResult.DoModal();
		//if (IDOK == wait_window_result) {
		//}
		//else if (IDCANCEL == wait_window_result) {
		//}

		
		//SetData();
	}
	else if (OK_Cancel == IDCANCEL)
	{
		m_time_thread_sleep = FALSE;
		/*m_waiting_time_thread = TRUE;*/

		if (m_time_thread == NULL)
			m_time_thread = AfxBeginThread(TimeThread, this);
		else
			m_time_thread->ResumeThread();
		return;
	}
	else
	{
		m_time_thread_sleep = FALSE;
		/*m_waiting_time_thread = TRUE;*/

		if (m_time_thread == NULL)
			m_time_thread = AfxBeginThread(TimeThread, this);
		else
			m_time_thread->ResumeThread();
		return;
	}
}


HBRUSH CVesselAnalysisDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC && pWnd->GetDlgCtrlID() == IDC_EDITING_STATE_TEXT)
	{
		COLORREF normal = RGB(0, 0, 0);
		COLORREF red = RGB(255, 0, 0);

		if (m_is_loaded_cur_img)
		{
			pDC->SetTextColor(m_fundus_img_data_vec[m_cur_img_idx].is_finished_editing ? red : normal);
		}
	}
	return hbr;
}
 
// Calculate ManulEdit result in origin resolution image (not padded and not cropped image).
// This function is runned when the mouseLbuttonup function is triggered.
void CVesselAnalysisDlg::ManualEditOriginRes()
{
	cv::Mat drawing_origin_mask = cv::Mat::zeros(m_origin_img.size(), CV_8UC1);
	m_drawing_radius_size = m_drawing_EB_radius_size * int(m_fundus_img_data_vec[m_cur_img_idx].to_ori_ratio_x);

	for (size_t i = 0; i < m_pt_vec_in_origin_img.size() - 1; ++i)
	{
		cv::line(drawing_origin_mask, m_pt_vec_in_origin_img[i], m_pt_vec_in_origin_img[i + 1], 255, m_drawing_radius_size);
	}

	cv::threshold(drawing_origin_mask, drawing_origin_mask, 0, 255, cv::ThresholdTypes::THRESH_BINARY);
	std::vector<cv::Mat> splited_origin_editing_mask_bgr;
	cv::split(m_cur_origin_mask, splited_origin_editing_mask_bgr);

	if (m_edit_option == 1) {
		drawing_origin_mask.setTo(0, m_cur_restricted_origin_mask == 0);

		if (m_editing_mode == 0) {
			if (m_artery_vein_mouse_cursor_change)
			{
				m_cur_origin_mask.setTo(cv::Scalar(255, 0, 0), drawing_origin_mask);
			}
			else
			{
				m_cur_origin_mask.setTo(cv::Scalar(0, 0, 255), drawing_origin_mask);
			}

		}
		else if (m_editing_mode == 1)
		{
			m_cur_origin_mask.setTo(cv::Scalar(0, 0, 255), drawing_origin_mask);
		}

		else if (m_editing_mode == 2)
		{
			m_cur_origin_mask.setTo(cv::Scalar(255, 0, 0), drawing_origin_mask);
		}

	}
	else if (m_edit_option == 2) {

		if (m_FE_control)
		{
			if (!m_is_connect_pts)
			{
				if (m_editing_mode == 0) {
					if (m_artery_vein_mouse_cursor_change)
					{
						splited_origin_editing_mask_bgr[0].setTo(cv::Scalar(255), drawing_origin_mask);
					}
					else
					{
						splited_origin_editing_mask_bgr[2].setTo(cv::Scalar(255), drawing_origin_mask);
					}
				}
				else if (m_editing_mode == 1)
				{
					splited_origin_editing_mask_bgr[2].setTo(cv::Scalar(255), drawing_origin_mask);
				}
				else if (m_editing_mode == 2)
				{
					splited_origin_editing_mask_bgr[0].setTo(cv::Scalar(255), drawing_origin_mask);
				}
				cv::merge(splited_origin_editing_mask_bgr, m_cur_origin_mask);
			}
		}
		else
		{
			if (!m_is_connect_pts)
			{
				if (m_editing_mode == 0) {
					m_cur_origin_mask.setTo(cv::Scalar(0, 0, 0), drawing_origin_mask);
				}
				else if (m_editing_mode == 1)
				{
					splited_origin_editing_mask_bgr[2].setTo(cv::Scalar(0), drawing_origin_mask);
					cv::merge(splited_origin_editing_mask_bgr, m_cur_origin_mask);
				}
				else if (m_editing_mode == 2)
				{
					splited_origin_editing_mask_bgr[0].setTo(cv::Scalar(0), drawing_origin_mask);
					cv::merge(splited_origin_editing_mask_bgr, m_cur_origin_mask);
				}
			}
		}
		m_cur_restricted_origin_mask = (splited_origin_editing_mask_bgr[0] > 0 | splited_origin_editing_mask_bgr[2] > 0);
	}
}

// Draw line by two mouse points.
void CVesselAnalysisDlg::InteractEditOriginRes()
{
	if (!m_is_loaded_cur_img)
		return;

	m_b_edtied = true;
	std::vector<cv::Mat> splited_origin_mask;

	cv::split(m_cur_origin_mask, splited_origin_mask);
	cv::Mat drawn_connect_mask = cv::Mat::zeros(m_cur_origin_mask.size(), CV_8UC1);

	if (m_edit_option == 2)
	{
		m_is_modified = TRUE;
		utils.is_modified = TRUE;
		for (int i = 0; i < pt_connect_origin_Xlist.size() - 1; i++)
		{
			int cur_y = pt_connect_origin_Ylist[i];
			int cur_x = pt_connect_origin_Xlist[i];
			
			int next_y = pt_connect_origin_Ylist[i + 1];
			int next_x = pt_connect_origin_Xlist[i + 1];
			
			cv::line(drawn_connect_mask, cv::Point(cur_x, cur_y), cv::Point(next_x, next_y), cv::Scalar(255), 2);
		}
		
		if (m_editing_mode == 0) 
		{
			if (m_artery_vein_mouse_cursor_change)
			{
				splited_origin_mask[0].setTo(cv::Scalar(255), drawn_connect_mask);
			}
			else
			{
				splited_origin_mask[2].setTo(cv::Scalar(255), drawn_connect_mask);
			}
		}
		else if (m_editing_mode == 1)
		{
			splited_origin_mask[2].setTo(cv::Scalar(255), drawn_connect_mask);
		}
		else if (m_editing_mode == 2)
		{
			splited_origin_mask[0].setTo(cv::Scalar(255), drawn_connect_mask);
		}
		cv::merge(splited_origin_mask, m_cur_origin_mask);
		
		pt_connect_origin_Ylist.clear();
		pt_connect_origin_Xlist.clear();
		
		m_drawing_EB_radius_size = m_prev_drawing_radius_size;
	}
	m_is_connect_pts = false;
}



void CVesselAnalysisDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDCtl == IDC_FUNDUS_IMAGE_LIST_BOX)
	{
		CListBox* pListBox = (CListBox*)GetDlgItem(IDC_FUNDUS_IMAGE_LIST_BOX);
		// 항목 텍스트를 가져옵니다.
		CString strText;
		pListBox->GetText(lpDrawItemStruct->itemID, strText);

		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		BOOL bSelected = (lpDrawItemStruct->itemState & ODS_FOCUS) || (lpDrawItemStruct->itemState & ODS_SELECTED);

		if (m_fundus_img_data_vec[lpDrawItemStruct->itemID].is_finished_editing)
		{
			pDC->SetBkColor(RGB(144, 238, 144));
			pDC->SetTextColor(RGB(0, 0, 0));
		}
		else if (m_fundus_img_data_vec[lpDrawItemStruct->itemID].is_finished_preprocess)
		{
			pDC->SetBkColor(RGB(238, 144, 144));
			pDC->SetTextColor(RGB(0, 0, 0));
		}
		else
		{
			pDC->SetBkColor(RGB(255, 255, 255));
			pDC->SetTextColor(RGB(0, 0, 0));
		}

		pDC->FillSolidRect(&lpDrawItemStruct->rcItem, pDC->GetBkColor());
		pDC->DrawText(strText, &lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_VCENTER);
		if (bSelected)
		{
			pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
		}

	
	}
	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
