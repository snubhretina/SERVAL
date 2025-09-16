#pragma once
#include "afxwin.h"
#include <iostream>
#include <winsock2.h>
#include <thread>

#define PACKET_SIZE 4096
using namespace std;

// VesselImgTransfer dialog

class VesselImgTransfer : public CDialog
{
	DECLARE_DYNAMIC(VesselImgTransfer)

public:
	VesselImgTransfer(CWnd* pParent = NULL);   // standard constructor
	virtual ~VesselImgTransfer();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VesselImgTransfer };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	//CStatic mTransferStatus;
	virtual BOOL OnInitDialog();
	uchar *send_fp_data;
	uchar *send_ROI_fp_data;
	cv::Mat transfer_mask_img, transfer_ROI_mask_img;
	uchar *send_seg_data;
	int data_cols, data_rows, data_channel;
	CString elapsed_time;
	int disc_center_x, disc_center_y, fovea_x, fovea_y;
	cv::Mat classification_img, seg_img;

	SOCKET skt;
	SOCKET skt_for_send_result;
	WSADATA wsa;
	SOCKADDR_IN addr = {};
	char *m_ip_address;
	std::string m_svr_account_name;

	int recv_len = 0;

	CString cur_status, cur_status1, cur_status2, cur_status3, cur_status4, cur_status5, cur_status6, cur_status7;
	std::string GetMyIP();
	bool SendRealTimeLog(CString &log_text);

	double BMWa, BMWv, BSTDWa, BSTDWv, CMWa, CMWv, CSTDWa, CSTDWv;
	CString str_BMWa, str_BMWv, str_BSTDWa, str_BSTDWv, str_CMWa, str_CMWv, str_CSTDWa, str_CSTDWv;
	CString m_strBMWa;
	CString m_strBMWv;
	CString m_strBSTDWa;
	CString m_strBSTDWv;
	CString m_strCMWa;
	CString m_strCMWv;
	CString m_strCSTDWa;
	CString m_strCSTDWv;
	
	CWinThread *p2, *p3;
	CEdit m_edit_example;
	static UINT VesselExtractThread(LPVOID _mothod);
	static UINT LoopReport(LPVOID _mothod);
	char* server_ip;
	int server_port_1;
	int server_port_2;
	string dataset_name;
	
	void InitSocket();
	void InitSocketForSendResult();
	void Getmetric();
	CEdit m_edit_report;
	bool send_message;
	afx_msg void OnBnClickedsendreport();

	std::string image_comment;
	std::vector<double> metric;
	CString m_cstr_errorreport, report;
	void SendError();
	static UINT SendResult(LPVOID _mothod);
};