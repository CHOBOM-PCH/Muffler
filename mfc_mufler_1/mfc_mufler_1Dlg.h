
// mfc_mufler_1Dlg.h : 헤더 파일
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

// Cmfc_mufler_1Dlg 대화 상자
class Cmfc_mufler_1Dlg : public CDialogEx
{
// 생성입니다.
public:
	Cmfc_mufler_1Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MFC_MUFLER_1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//비젼 변수 선언
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

	//그래프 변수 선언
	COScopeCtrl		m_Graph;
	double data[2];
	//변위 센서 변수 선언
	double height;
	double sensor_range ;
	int mesured_data;


	afx_msg void OnBnClickedSensorbtn();
};
