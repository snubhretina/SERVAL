
// FIREFLY_Inspection_dialogDlg.h : 헤더 파일
//

#pragma once

#include "FrangiFilter.h"
#include "afxwin.h"
#include "C:\opencv\build\include\opencv2\core\mat.hpp"
#include "VesselImgTransfer.h"
#include "utils.h"
#include "Logger.h"

#include "Waitwindow.h"
#include "ErrorReport.h"
#include "VesselAnalysisParams.h"
#include "CreateAccountWindow.h"
#include "WaitWindowSendResult.h"
#include "WaitWindowCalcParams.h"
#include "afxdialogex.h"
//#include <GdiPlus.h>
//#pragma comment(lib, "gdiplus.lib")

#include <d2d1.h>
#include <opencv2/opencv.hpp>
#pragma comment(lib, "d2d1.lib")



class CVesselAnalysisDlg : public CDialogEx
{
public:
	CVesselAnalysisDlg(CWnd* pParent = NULL);	
	VesselAnalysisParams params;
	VesselImgTransfer m_VesselImgTransfer;
	CUtils utils;
	CLogger logger;
	CErrorReport m_ErrorReportWnd;
	CWaitwindow m_Waitwindow;
	WaitWindowSendResult m_WaitwindowSendResult;
	WaitWindowCalcParams m_WaitWindowCalcParams;
	CreateAccountWindow create_account_window;

	enum { IDD = IDD_VESSELANALYSIS_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	

protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	void CalculateEllipseMask(Eigen::VectorXd &disk_ellipse_inform, int &disk_diameter);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	// ----- Editing Board ----- //
	CStatic m_editing_board;

	// Update Editing Board 
	void DrawPicture(cv::Mat &cur_view);
	//void DrawPicture(HWND hwnd, cv::Mat &zoom_img);
	void CropZoomImg(cv::Mat &img, cv::Rect zoom_ROI_rect, cv::Mat &zoom_img);
	cv::Point CalculateOffsetInZoomImg(cv::Mat &cur_view, cv::Point &disp_offset);
	cv::Point CalculateOffsetInZoomOriginImg(cv::Mat &cur_view, cv::Point &disp_offset);
	void DrawLine(cv::Mat& zoom_img, const cv::Point &pt, int drawing_radius_size);

	
	void ManualEditUpdate();
	
	void InitDrawnAreaMask();
	void InteractEditUpdate();
	bool m_interactive_connect_mode;
	void CalculateZoomRatio(cv::Mat &cur_view);
	void CalculateZoomRatio();	
	void DrawingRadiusUpdate(int updown);
	void ROIUpdate(int updown);

	cv::Mat m_zoom_img;
	cv::Mat m_zoom_mask;

	// Editing Board State Variable
	int m_editing_board_width_prev, m_editing_board_height_prev;
	int m_editing_board_width, m_editing_board_height;
	float m_editing_board_ratio_x, m_editing_board_ratio_y, m_editing_board_origin_ratio_x, m_editing_board_origin_ratio_y;
	float m_editing_board_zoom_ratio;
	int m_ROI_zoom_ratio;

	// Editing Board Image Variable
	bool m_is_loaded_cur_img;
	cv::Mat m_remain_origin_img;
	cv::Mat m_cur_origin_mask;
	cv::Mat m_unpadded_origin_mask;
	cv::Mat m_unpadded_ROI_mask;

	cv::Mat m_calc_param_origin_mask;
	cv::Mat m_calc_param_ROI_mask;

	cv::Mat m_cur_EB_mask;
	cv::Mat m_cur_editing_img;

	cv::Mat m_cur_editing_mask;
	int m_cur_img_idx;
	int m_calc_param_img_idx;

	cv::Mat m_padded_origin_img;

	cv::Mat m_origin_img;
	cv::Mat m_origin_mask;

	cv::Mat m_origin_diameter_mask;
	
	// Editing Board Text Variable
	char m_cur_image_index_text[200], m_cur_image_brightness_text[100], m_cur_editing_mode_text[100];

	// Editing Board Image Brightness Variable
	int m_cur_image_brightness;
	cv::Point m_brightness_control_start_point, m_brightness_control_end_point;

	// ----- Account -----//
	CString m_account_name_cstr;	
	std::string m_account_name;

	// ----- Check Point ----- //
	void Undo();
	void Redo();
	bool m_waiting_write_check_point;
	
	
	// Change Undo Redo Text in "Current State"
	void ChangeUndoRedoNumText();
	void ChangeUndoRedoNumText(CString zero_num);
	wchar_t *m_undo_redo_num_format;
	CString m_undo_redo_num_text;
	CStatic UNDO_REDO_TEXT;

	// Workprocess Recording Vector
	std::vector<WorkProcessStruct> m_work_process_vec;
	
	// ----- Data ----- //
	// Change Current Data Function
	void SetData();

	// Data Saving Vector
	std::vector<FundusImageData> m_fundus_img_data_vec;
	std::string m_fundus_img_num_str;
	int m_fundus_img_num;

	// Data Root Path
	WCHAR m_selected_path[2000];
	
	// ----- Mouse ----- //
	// Mouse Function
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void ConvertVesselType(cv::Mat &editing_mask, cv::Mat diameter_mask, cv::Point &mouse_point);

	// Calculate Mouse Pointer in Editing Board
	void ComputeCurePoint();

	cv::Mat TheshodMask(cv::Mat &origin_mask, cv::Mat &editing_mask);
	cv::Mat TheshodMask(cv::Mat &origin_mask);
	void TheshodMask(cv::Mat &origin_mask, cv::Mat &editing_mask, cv::Mat &restricted_bn_mask);

	// Mouse Pointer Coordinate in Editing Board
	CPoint m_mouse_point_in_editing_board;

	// Mouse button State
	bool m_is_pressed_mouse_L;
	bool m_is_pressed_mouse_R;

	bool m_is_zoom;

	CString m_editing_mode_text;

	// ----- Check Box Function ----- //
	afx_msg void OnBnClickedCheckOverlay();
	afx_msg void OnBnClickedCheckDiameter();
	afx_msg void OnBnClickedCheckCenterline();
	CButton m_check_Overlay;
	CButton m_check_Diameter;
	CButton m_check_centerline;
	bool m_is_checked_overlay;
	bool m_is_checked_diameter;
	bool m_is_checked_centerline;

	// ----- Button Function ----- //
	afx_msg void OnClickedButtonLoadDir();
	afx_msg void OnBnClickedButtonPrevImage();
	afx_msg void OnBnClickedButtonNextImage();

	
	afx_msg void OnBnClickedInitMaskAndDisc();
	afx_msg void OnBnClickedErrorReport();
	afx_msg void OnBnClickedResetButton();
	afx_msg void OnLbnSelchangeFundusImageListBox();
	afx_msg void OnBnClickedFinishButton();

	CButton m_reset_button;
	CButton m_all_reset_button;
	CListBox m_fundus_image_list_box;
	CBrush m_list_box_brush;
	CStatic m_editing_state_text;
	CString m_cur_data_count_text;

	// ----- Key Board ----- //
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	bool m_is_pressed_ctrl;
	bool m_b_press_shift;

	bool m_check_mode;
	// 수정 필요 //
	void ConnectedComponent(cv::Mat pos, cv::Mat &image);
	void ConnectedComponent(cv::Mat pos, cv::Mat &A, cv::Mat &V);
	void EnableFundusListBoxHorizontalScroll();


	cv::Mat m_cur_restricted_bn_EB_mask;
	cv::Mat m_cur_restricted_bn_origin_mask;
	bool m_b_edtied;

	int m_artery_vein_mouse_cursor_change;
	cv::Mat m_drawn_area_mask;
	cv::Mat m_drawn_area_origin_mask;
	int m_drawing_radius_size;
	int m_drawing_EB_radius_size;
	int m_drawing_radius_size_in_EB;
	int m_prev_drawing_radius_size;

	std::vector<cv::Point> m_all_pts;
	cv::Point m_pt_disp_offset;
	cv::Point m_pt_ori_disp_offset;

	cv::Point m_origin_offset_pt;
	cv::Point m_EB_offset_pt;

	cv::Point m_cur_offset;
	
	cv::Point m_pt_old;
	cv::Point m_pt_new;
	
	cv::Point m_ori_pt_old;
	cv::Point m_ori_pt_new;

	cv::Point m_pt_draw_old;
	cv::Point m_pt_draw_cur;

	cv::Point m_ori_pt_draw_old;
	cv::Point m_ori_pt_draw_cur;

	cv::Point m_start_pt_in_cropped_img;
	cv::Point m_end_pt_in_cropped_img;

	cv::Point m_start_pt_in_origin_img;

	cv::Mat m_drawing_origin_mask;
	cv::Mat m_drawing_EB_mask;

	cv::Mat m_drawing_view;
	cv::Mat m_before_mask;

	cv::Mat m_cur_view;
	bool m_is_mouse_moving;
	int m_AV_enable;

	cv::Mat m_cur_Vview;
	cv::Mat m_cur_Aview;

	std::vector<cv::Point> pair_AendPoint;
	std::vector<cv::Point> pair_VendPoint;
	std::vector<cv::Point> tmp_endPoint;

	bool m_FE_control;

	std::string cur_log_time;
	bool cur_log_flag;
	int m_editing_mode;
	int seg_ratio;

	CString m_logger_time_format;

	cv::Mat endp_fat, branch_fat, endp_fat_viz, endp_fat_viz_tmp, lowhyst, highhyst, endp_artery, endp_vein, endp_vein_python, endp_artery_python, branchp_artery_python, branchp_vein_python;
	cv::Point m_pt_endp_highlight;
	std::vector<cv::Mat> bgr;

	int threshold_radius, tmp_segment_label_num, cur_segment_label_num;
	cv::Mat broken_prob_map, broken_mask_stats, broken_mask_label_map, viusalize_vessel_map, origin_viusalize_vessel_map;
	void threshold_update(int updown);
	
	bool drawing_mode;

	// ----- Optical Disc ----- //
	int FindOpticDisc(cv::Mat &fundus, cv::Point &opt_center);
	cv::Point pt_opt;
	cv::Point m_optical_disc_dragging_start_point, m_optical_disc_dragging_end_point;
	cv::Mat m_cur_optic_disc_circle;
	cv::Mat m_cur_origin_optic_disc_circle;
	int opt_distance;
	BOOL m_is_dragging_disc_diameter;
	//Eigen::VectorXd m_cartpool;

	BOOL m_is_connect_pts;

	std::vector<double> pt_OD_bondary_Xlist;
	std::vector<double> pt_OD_bondary_Ylist;

	std::vector<double> pt_connect_Xlist;
	std::vector<double> pt_connect_Ylist;

	std::vector<double> pt_connect_origin_Xlist;
	std::vector<double> pt_connect_origin_Ylist;


	// ----- Centerline ----- //
	void ThinArteryAndVein();
	cv::Mat m_centerline_img;


	// ----- Timer ----- //
	bool m_waiting_time_thread;
	static UINT TimeThread(LPVOID _mothod);
	CWinThread *m_time_thread, *m_checking_mouse_movement_thread;
	bool m_reset_pause_timer;
	bool m_time_thread_kill;
	bool m_not_update_time;
	bool m_elapsed_timer_reset;
	CTimeSpan m_command_check_time;
	void ChangeTimeFormat(CTimeSpan &elapsed_time, CString &elapsed_time_cstr);
	CString ChangeTimeFormat();
	CStatic ELAPSED_TIMER_TEXT;
	CStatic PAUSE_TIMER_TEXT;
	CString m_zero_time_cstr;
	bool m_time_thread_sleep;
	
	wchar_t *m_time_format;

	// ----- Radio Function ----- //
	UINT m_radio_control_value;
	void RadioCtrl(UINT ID);
	int m_edit_option;

	// ----- Diasble Radio Button And Check Bokx ----- //
	std::vector<int> m_radio_and_check_button_vec;
	void EnableCheckButtonAndRadioButton(std::vector<int> *radio_and_check_button_vec, bool radio_and_check_button_enable);

	// ----- Make Message Box ----- //
	int MakeInfoMessageBox(CString &info_message);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	bool m_test_mode;

	// ----- Data Sending Mode (0 - None, 1 - Send metric, 2 - Send result) ----- //
	int m_data_sending_mode;

	CRect m_rc;

	std::vector<cv::Point> m_pt_vec_in_origin_img;

	void ManualEditOriginRes();
	void InteractEditOriginRes();

	cv::Mat m_cur_restricted_origin_mask;
	

	cv::Mat m_cropped_img;
	cv::Mat m_cropped_diameter_mask;
	cv::Mat m_cropped_mask;

	cv::Mat m_cur_cropped_mask;
	cv::Mat m_cur_cropped_restricted_mask;

	CRect m_editing_board_size;
	cv::Point m_zoom_img_offset;

	cv::Mat m_ellipse_circle_mask;

	cv::Point CalculateOffset(cv::Mat &cur_view, cv::Point &offset);

	cv::Rect m_zoom_ROI_rect;
	cv::Point m_mouse_point;

	cv::Mat m_applied_ellipse_mask_img;
	cv::Mat m_origin_circle_mask;
	cv::Mat m_cropped_circle_mask;

	cv::Mat m_drawing_cropped_mask;

	bool m_is_modified;

	void ApplyEllipseMask(cv::Mat &img, cv::Mat &masked_img);

	std::string m_json_data;

	// Direct2D 리소스
	ID2D1Factory* pFactory = nullptr;
	ID2D1HwndRenderTarget* pRenderTarget = nullptr;
	ID2D1Bitmap* pBitmap = nullptr;

	CStatic editing_mode_text;
	CStatic cur_data_count_text;

	cv::Mat m_ROI_mask;
	
	vascularity *example;
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	static UINT CVesselAnalysisDlg::CalcParams(LPVOID _mothod);
	CWinThread *m_calc_params_pthread;
};