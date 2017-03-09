
// mfc_mufler_1Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "mfc_mufler_1.h"
#include "mfc_mufler_1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////스플라인 변수///
int z = 0;
int ke = 0;
std::vector<double> x;
std::vector<double> y;
std::vector<double> x_m;
std::vector<double> y_m;
std::vector<double>* x_z;
std::vector<double>* y_z;

////////////변위센서 변수
	HANDLE				hRF60x			= INVALID_HANDLE_VALUE;
	USHORT				usMeasured;
	RF60xHELLOANSWER	hlans;



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
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


// Cmfc_mufler_1Dlg 대화 상자




Cmfc_mufler_1Dlg::Cmfc_mufler_1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cmfc_mufler_1Dlg::IDD, pParent)
	, line_data(_T(""))
	, m_Graph(2)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmfc_mufler_1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Mufler_IMG, Mufler_img);
	DDX_Text(pDX, IDC_Line, line_data);
}

BEGIN_MESSAGE_MAP(Cmfc_mufler_1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_Open_btn, &Cmfc_mufler_1Dlg::OnBnClickedOpenbtn)
	ON_BN_CLICKED(IDC_Spline, &Cmfc_mufler_1Dlg::OnBnClickedSpline)
	ON_BN_CLICKED(IDC_Sensor_btn, &Cmfc_mufler_1Dlg::OnBnClickedSensorbtn)
END_MESSAGE_MAP()


// Cmfc_mufler_1Dlg 메시지 처리기

BOOL Cmfc_mufler_1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// 비디오 캡쳐를 위한 변수 선언
	capture = new VideoCapture("mufler.mp4");
	//capture = new VideoCapture(0);
	if (!capture->isOpened())
	{
		MessageBox(_T("비디오파일이 없습니다."));

	}
	image_mfc = NULL;
	//그래프를 위한 선언///////////////////////////////////////////////////
	data[0] = data[1] = 0.0;
	wchar_t   buf[250];   // 그래프에서 사용
	CRect rect;    
	GetDlgItem(IDC_Graph)->GetWindowRect(rect);                      // (3)
	ScreenToClient(rect);

	m_Graph.Create(WS_VISIBLE|WS_CHILD, rect, this);             // (241) / 150 -> 350       (327) / 150 -> 500
	m_Graph.SetRanges(-600, 600);            // Y축  0 에서 100 까지 다시그림

	m_Graph.SetBackgroundColor(RGB(0,0,0));
	m_Graph.SetGridColor(RGB(255,255,255));
	m_Graph.SetPlotColor(RGB(255,128,0), 0);//선1
	m_Graph.SetPlotColor(RGB(0,255,0), 1);// 선2
	//m_Graph.SetPlotColor(RGB(255,0,0), 2);// 선3
	//m_Graph.GetXPoints(&z);
	//m_Graph.SetXUnits("Time < 10[ms] sampling >", "0", "1000") ;      // 100ms->100     10ms->1000
	//sprintf(buf, "                      Time < 10[ms] sampling - %d points >", num);
	wsprintf(buf,_T( "Time < 10[ms] sampling >"));
	m_Graph.SetXUnits(buf, _T("0") ) ;
	m_Graph.SetYUnits(_T("Velocity < mm >"), _T("-600.0"), _T("600.0") ) ;
	m_Graph.SetLegendLabel(_T("X axis"), 0);
	m_Graph.SetLegendLabel(_T("Y axis"), 1);
	m_Graph.SetLegendLabel(_T("Total"), 2);
	///////////////////////////////////////////////////////////////////////

	//변위 센서////////////////////////////////////////////////////////////


	memset(&hlans, 0x00, sizeof(RF60xHELLOANSWER));
	RF60x_OpenPort("COM3:", CBR_9600, &hRF60x);
	height = 0;
	sensor_range = 0;
	mesured_data = 0;



	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void Cmfc_mufler_1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void Cmfc_mufler_1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR Cmfc_mufler_1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cmfc_mufler_1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//캡쳐한 이미지를 한프레임씩 가져와서 image파일에 넣음

	int distance = 0; //거리측정용 변수선언

	if(nIDEvent == 123){
		capture->read(O_img);
		ke = ke+1;

		if(O_img.empty()){//재생이 끝나고 나가기
			KillTimer(123);
			MessageBox(_T("비디오가 끝남"));

		}

		//bit info에서 bitcount 정의 bpp 8bit 24bit 32bit 이미지
		//int bpp = 8 * O_img.elemSize();
		//   assert((bpp == 8 || bpp == 24 || bpp == 32));
		//
		//   int padding = 0;
		//   //32 bit image is always DWORD aligned because each pixel requires 4 bytes
		//   if (bpp < 32)
		//       padding = 4 - (O_img.cols % 4);
		//
		//   if (padding == 4)
		//       padding = 0;
		//
		//   int border = 0;
		//   //32 bit image is always DWORD aligned because each pixel requires 4 bytes
		//   if (bpp < 32)
		//   {
		//       border = 4 - (O_img.cols % 4);
		//   }
		//
		//
		//
		//   Mat mat_temp;
		//   if (border > 0 || mat_frame.isContinuous() == false)
		//   {
		//       // Adding needed columns on the right (max 3 px)
		//       cv::copyMakeBorder(mat_frame, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
		//   }
		//   else
		//   {
		//       mat_temp = mat_frame;
		//}

		else{
			RECT r;//출력할 윈도우의 크기 측정
			Mufler_img.GetClientRect(&r);
			cv::Size winSize(r.right, r.bottom);



			//머플러 선 검출

			cvtColor(O_img,gray_img,CV_BGR2GRAY);

			GaussianBlur(gray_img,blur_img,cv::Size(5,5),10);

			adaptiveThreshold(blur_img,           // input image //지역가변 쓰레쉬 홀드사용
				thresh_img,                              // output image
				255,                                    // make pixels that pass the threshold full white
				ADAPTIVE_THRESH_GAUSSIAN_C,         // use gaussian rather than mean, seems to give better imgs
				THRESH_BINARY_INV,                  // invert so foreground will be white, background will be black
				15,                                     // size of a pixel neighborhood used to calculate threshold value
				2);                                     // constant subtracted from the mean or weighted mean

			vector<Vec4i>lines;//lines 선언
			HoughLinesP(thresh_img,//허프추출을 위해 참조할 이미지
				lines,//참조된 선이 저장될 장소
				1,//원점으로부터 거리간격rho
				PI/270,//x축과의 각도 라디안 간격
				10,//직선을 검출하기위한 accumulator의 임계값
				200,//최소측정 라인길이
				0);//최대 점의 간격
			Mat H_img = O_img.clone();
			Vec4d params,eparams;//vector형 행렬변수 선언
			int x1, y1, x2, y2;
			int xe1=0, ye1=0, xe2=0, ye2 =0;//변수들의 좌표값저장
			int e1=0,e2=0;
			for (int k = 0; k < lines.size(); k++){
				params = lines[k];
				x1 = params[0];//라인들의 시작점
				y1 = params[1];
				x2 = params[2];//라인들의 끝점
				y2 = params[3];
				//eparams = eparams + params;//라인들 각 좌표값의 합
				eparams[0]=eparams[0]+params[0];
				eparams[1]=eparams[1]+params[1];
				eparams[2]=eparams[2]+params[2];
				eparams[3]=eparams[3]+params[3];
				xe1 = (eparams[0])/(k+1);//평균 라인의 시작점
				ye1 = (eparams[1])/(k+1);
				xe2 = (eparams[2])/(k+1);//평균라인의 끝점
				ye2 = (eparams[3])/(k+1);
				Point pt1(x1,y1),pt2(x2,y2);
				//line(H_img,pt1,pt2, Scalar(0,0,255),2);//각라인 출력

			}

			if (xe1 == 0){
				xe1 = e1, xe2 = e2;
			}
			else {
				e1 = xe1, e2 = xe2;
				Point ept1(xe1,0),ept2(xe2,700);//평균라인 출력
				line(H_img,ept1,ept2, Scalar(255,255,0),2);//그리기
				Point ct1(512,0),ct2(512,768);//중심선 시작점 끝점
				line(H_img,ct1,ct2, Scalar(0,255,255),1);//중심선 그리기

				distance = (xe1+xe2)/2 - 512;//용접과 중심의 거리측정
				Point dp1((xe1+xe2)/2,300),dp2(512,300);//떨어진 거리 표현선
				line(H_img,dp1,dp2, Scalar(255,0,255),1);
				string te;//string 에 가변 숫자 변수 입력
				stringstream ste;
				ste<<"distance:"<<distance<<"pixel"<<endl;
				te=ste.str();
				putText(H_img,te,dp1,3,1.2,Scalar(0,255,0));//이미지에 거리표시
				//data[0] = (double)(distance);
			}
			//mfc_img 즉 CImage의 초기 설정

			if (image_mfc)
			{
				image_mfc->ReleaseDC();
				delete image_mfc;
				image_mfc = nullptr;
			}

			image_mfc = new CImage();
			image_mfc->Create(winSize.width, winSize.height, 24);


			BITMAPINFO bitInfo;
			bitInfo.bmiHeader.biBitCount = 24;
			bitInfo.bmiHeader.biWidth = H_img.cols;
			bitInfo.bmiHeader.biHeight =- H_img.rows;
			bitInfo.bmiHeader.biPlanes = 1;
			bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitInfo.bmiHeader.biCompression = BI_RGB;
			bitInfo.bmiHeader.biClrImportant = 0;
			bitInfo.bmiHeader.biClrUsed = 0;
			bitInfo.bmiHeader.biSizeImage = 0;
			bitInfo.bmiHeader.biXPelsPerMeter = 0;
			bitInfo.bmiHeader.biYPelsPerMeter = 0;
			///////////////
			if (H_img.cols == winSize.width  && H_img.rows == winSize.height)
			{
				// source and destination have same size
				// transfer memory block
				// NOTE: the padding border will be shown here. Anyway it will be max 3px width

				SetDIBitsToDevice(image_mfc->GetDC(),
					//destination rectangle
					0, 0, winSize.width, winSize.height,
					0, 0, 0, H_img.rows,
					H_img.data, &bitInfo, DIB_RGB_COLORS);
			}
			else
			{
				// destination rectangle
				int destx = 0, desty = 0;
				int destw = winSize.width;
				int desth = winSize.height;

				// rectangle defined on source bitmap
				// using imgWidth instead of mat_temp.cols will ignore the padding border
				int imgx = 0, imgy = 0;
				int imgWidth = H_img.cols;//- border;
				int imgHeight = H_img.rows;

				StretchDIBits(image_mfc->GetDC(),
					destx, desty, destw, desth,
					imgx, imgy, imgWidth, imgHeight,
					H_img.data, &bitInfo, DIB_RGB_COLORS, SRCCOPY);
			}
			image_mfc->BitBlt(::GetDC(Mufler_img.m_hWnd), 0, 0);


			if (image_mfc)
			{
				image_mfc->ReleaseDC();
				delete image_mfc; 
				image_mfc = nullptr;
			}
		}
		////////////시간마다 변위 센서 측정/////
		RF60x_OpenPort("COM3:", CBR_9600, &hRF60x);
			if (RF60x_HelloCmd( hRF60x, 1, &hlans ))
			{
				RF60x_Measure( hRF60x, 1, &usMeasured);
				sensor_range = hlans.wDeviceRange;
				mesured_data = usMeasured;
				height = (double) (sensor_range*mesured_data/16384.0);
				data[1] = height;
			}
			m_Graph.AppendPoints(data);
	}


	if (ke == 10){//너무 빨리 가져 오지 않기위한 delay줌
		ke = 0;//delay초기화
		if(distance == 0);//거리측정이 안되었을때 쓰레기값 입력 방지
		else{
			//int time=1000;//토크 측정을 위한 시간
			z = z + 10;//x값 변화량임 나중에 실제 촬영하면 속도에따라 변하는 거리값 삽입
			x.push_back(z);
			y.push_back(distance);
			x_z = &x_m;
			y_z = &y_m;
			
			}
		
		}

		CDialogEx::OnTimer(nIDEvent);
	}


void Cmfc_mufler_1Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	capture->release();
	KillTimer(123);
	RF60x_ClosePort( hRF60x );
}


void Cmfc_mufler_1Dlg::OnBnClickedOpenbtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	SetTimer(123,200,NULL);


}


void Cmfc_mufler_1Dlg::OnBnClickedSpline()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//SetTimer(234,1000,NULL);



	if (x.size() > 2){//x배열의 개수가 2개 이상일때 그래프를 찾음
		cubic_spline(x,y,x_z,y_z);
	}
	//for (int k = 0;k<x.size();k++){//입력된 값에 의한 그래프
	//	//printf("입력된 값 x = %lf , y = %lf \n" ,x[k],y[k]);
	//	string te;//string 에 가변 숫자 변수 입력

	//	stringstream ste;
	//	ste<<"x = "<<x[k]<<", y = "<<y[k]<<"\r\n"<<endl;

	//	te=ste.str();
	//	line_data = (te.c_str());//string->CString
	//	//str.Format("%d", i); 
	//	SetDlgItemText(IDC_Line, line_data); 
	//	GetDlgItem(IDC_Line)->UpdateWindow();
	//	//그래프 실행
	//	//data[0] = y[k];


	//	//UpdateData(FALSE);//data의 값을 올린다.
	//	//	if (k == 0);
	//	//	else
	//	//		line(plot_img,Point(x[k-1],y[k-1]),Point(x[k],y[k]),Scalar(0,255,255),2,8,0);

	//}


	for (int k =0;k<x_m.size();k++){//mono spline후 그래프
		string te;//string 에 가변 숫자 변수 입력
		stringstream ste;
		ste<<"x = "<<x_m[k]<<", y = "<<y_m[k]<<"\r\n"<<endl;

		te=ste.str();
		line_data = (te.c_str());//string->CString
		SetDlgItemText(IDC_Line, line_data); 
		GetDlgItem(IDC_Line)->UpdateWindow();
		//UpdateData(FALSE);//data의 값을 
		//data[0] = y_m[k];
	}




	}


void Cmfc_mufler_1Dlg::OnBnClickedSensorbtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	memset(&hlans, 0x00, sizeof(RF60xHELLOANSWER));

	RF60x_OpenPort("COM3:", CBR_9600, &hRF60x);

	if (RF60x_HelloCmd( hRF60x, 1, &hlans ))
	{

		RF60x_Measure( hRF60x, 1, &usMeasured);
		mesured_data = usMeasured;
		sensor_range = hlans.wDeviceRange;
		height = (double) (sensor_range*mesured_data/16384.0);

		string te;//string 에 가변 숫자 변수 입력

		stringstream ste;
		ste<<hlans.bDeviceModificaton<<"\r\n"
			//<<"Dev type\t: "<<hlans.bDeviceType<<"\r\n"
			<<"Dev max dist\t:  "<<hlans.wDeviceMaxDistance<<"\r\n"
			<<"Dev range\t: "<<hlans.wDeviceRange<<"\r\n"
			<<"Dev serial\t: "<<hlans.wDeviceSerial<<"\r\n"
			<<"Measure \t: "<<usMeasured<<"\r\n"
			<<"Length \t: "<<height<<" mm \r\n"
			<<endl;

		te=ste.str();
		line_data = (te.c_str());//string->CString

		UpdateData(FALSE);//data의 값을 올린다.


	} 

	RF60x_ClosePort( hRF60x );
}
