
// mfc_mufler_1Dlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "use_opencv.h"
#include <vector>
#include <iostream>
#include "Spline.h"
#include <conio.h>
#include <windows.h>
#include "RF60x.h"
#include "OScopeCtrl.h"

using namespace cv;
using namespace std;
#define PI 3.1415926

// Cmfc_mufler_1Dlg ��ȭ ����
class Cmfc_mufler_1Dlg : public CDialogEx
{
// �����Դϴ�.
public:
	Cmfc_mufler_1Dlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MFC_MUFLER_1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//���� ���� ����
	VideoCapture *capture;
	Mat O_img;
	Mat gray_img;
	Mat blur_img;
	Mat thresh_img;
	Mat H_img;
	CImage *image_mfc;

	CStatic Mufler_img;



	afx_msg void OnBnClickedButton1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOpenbtn();
	afx_msg void OnBnClickedSpline();
	
	CString line_data;

	//�׷��� ���� ����
	COScopeCtrl		m_Graph;
	double data[2];
	//���� ���� ���� ����
	double height;
	double sensor_range ;
	int mesured_data;


	afx_msg void OnBnClickedSensorbtn();
};
