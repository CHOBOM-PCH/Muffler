
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
int count_y = 0;
int count_delay = 0;
std::vector<double> x;
std::vector<double> y;
std::vector<double> z;
std::vector<double> x_m;
std::vector<double> y_m;
std::vector<double> z_m;
std::vector<double>* x_z;
std::vector<double>* y_z;
std::vector<double>* z_z;
////////////////////////////

int k = 0;//timer 234에서 카운팅을 위한 변수
ofstream fout;//파일 입출력을 위한 변수

////////////변위센서 변수///////////
HANDLE				hRF60x			= INVALID_HANDLE_VALUE;
USHORT				usMeasured;
RF60xHELLOANSWER	hlans;
CString sensor_port;
const char* sen ;
///////////////////////////////////


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
	DDX_Control(pDX, IDC_SerialPort, m_port);
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
	ON_BN_CLICKED(IDC_Clear, &Cmfc_mufler_1Dlg::OnBnClickedClear)

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
	//O_img = cvLoadImage("image/4.png",-1);
	if (!capture->isOpened()){
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
	m_Graph.SetRanges(-100, 300);            // Y축  0 에서 100 까지 다시그림

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
	m_Graph.SetYUnits(_T("Velocity < mm >"), _T("-100.0"), _T("300.0") ) ;
	m_Graph.SetLegendLabel(_T("X axis"), 0);
	m_Graph.SetLegendLabel(_T("Z axis"), 1);
	//m_Graph.SetLegendLabel(_T("Total"), 2);
	///////////////////////////////////////////////////////////////////////

	//변위 센서////////////////////////////////////////////////////////////
	m_port.AddString(_T("COM1"));
	m_port.AddString(_T("COM2"));
	m_port.AddString(_T("COM3"));
	m_port.AddString(_T("COM4"));
	m_port.AddString(_T("COM5"));
	m_port.SetCurSel(2);//default COM3포트
	m_port.GetLBText(2,sensor_port);
	int select = m_port.GetCurSel();
	switch (select){
	case 0 : sen = "COM1:"; break;
	case 1 : sen = "COM2:"; break;
	case 2 : sen = "COM3:"; break;
	case 3 : sen = "COM4:"; break;
	case 4 : sen = "COM5:"; break;
	default : sen = "ERROR"; break;
	}

	///변위센서 포트 선택후 열기////////////////////////////////////
	memset(&hlans, 0x00, sizeof(RF60xHELLOANSWER));
	RF60x_OpenPort(sen, CBR_9600, &hRF60x);
	height = 0;
	sensor_range = 0;
	mesured_data = 0;

	line_data.Format(_T("포트 : %s \r\n"), sensor_port);
	SetDlgItemText(IDC_Line,line_data);
	//UpdateData(FALSE);
	////////////////////////////////////////////////////////////////
	////////////파일 입출력용//////////////

	fout.open("G_code.pba");


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
		count_delay = count_delay+1;

		if(O_img.empty()){//재생이 끝나고 나가기
			KillTimer(123);
			MessageBox(_T("비디오가 끝남"));
		}

		else{
			//선 검출
			Line_Detect(O_img,H_img,&distance);
			data[0] = (double)(distance);

			//image 화면에 맞게 출력
			ImagePrintInMFC(image_mfc,&Mufler_img,H_img);
		}
		
		////////////시간마다 변위 센서 측정/////
		if (RF60x_HelloCmd( hRF60x, 1, &hlans )){
			RF60x_Measure( hRF60x, 1, &usMeasured);
			sensor_range = hlans.wDeviceRange;
			mesured_data = usMeasured;
			height = (double) (sensor_range*mesured_data/16384.0);
			data[1] = height;
		}
		else {
			height = 0;
			data[1] = height;
			line_data = _T("rs232 error!\r\n");
			UpdateData(FALSE);
		}
		m_Graph.AppendPoints(data);
	}

	//가져오는 방식에 따라 삭제 가능
	if (count_delay == 10){//너무 빨리 가져 오지 않기위한 delay줌
		count_delay = 0;//delay초기화
		if(distance == 0);//거리측정이 안되었을때 쓰레기값 입력 방지
		else {
			count_y = count_y + 10;//x값 변화량임 나중에 실제 촬영하면 속도에따라 변하는 거리값 삽입
			x.push_back(distance);
			y.push_back(count_y);
			z.push_back(height);
			x_z = &x_m;
			y_z = &y_m;
			z_z = &z_m;
	//////////////////////////////////////////////

		}

	}


	else if (nIDEvent == 234){
		pEdit = (CEdit*)GetDlgItem(IDC_Line);
		if (k < (y_m.size())){

			line_data.Format(_T("x = %lf , y = %lf,  z = %lf \r\n"), x_m[k], y_m[k], z_m[k]);
			pEdit->SetSel(0,0);
			pEdit->ReplaceSel(line_data);
			fout<<"x = "<<x_m[k]<<", y = "<<y_m[k]<<", z = "<<z_m[k]<<"\r\n"<<endl;
			data[0] = (double)(x_m[k]);
			data[1] = (double)(z_m[k]);
			m_Graph.AppendPoints(data);

			k = k + 1;
		}
		else {
			KillTimer(234);
			k = 0;
			if (fout.is_open())
				fout.close();
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
	KillTimer(234);
	KillTimer(345);
	if (RF60x_HelloCmd( hRF60x, 1, &hlans )){
		RF60x_ClosePort( hRF60x );
	}
}


void Cmfc_mufler_1Dlg::OnBnClickedOpenbtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	SetTimer(123,20,NULL);


}


void Cmfc_mufler_1Dlg::OnBnClickedSpline()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//스플라인을 검출하기위한 버튼

	if (y.size() > 2){//x배열의 개수가 2개 이상일때 그래프를 찾음
		//vector에 값 중첩을 막기위해 초기화 필요
		y_m.clear();
		x_m.clear();
		cubic_spline(y,x,y_z,x_z);
		//y를 두번 클리어 하는것도 위에서 검사한 y값이 들어있기에 중복 방지
		y_m.clear();
		z_m.clear();
		cubic_spline(y,z,y_z,z_z);

	}
	KillTimer(123);
	SetTimer(234,100,NULL);

}


void Cmfc_mufler_1Dlg::OnBnClickedSensorbtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetTimer(345,100,NULL);

	if (RF60x_HelloCmd( hRF60x, 1, &hlans )){

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
		/*pEdit->SetSel(0,0);
		pEdit->ReplaceSel(line_data);*/
		UpdateData(FALSE);//data의 값을 올린다.

	}
	else {
		line_data = _T("rs232 error!\r\n");
		UpdateData(FALSE);
	}


}


void Cmfc_mufler_1Dlg::OnBnClickedClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	////////진행하던 타이머 끄기////
	KillTimer(123);
	KillTimer(234);
	SetTimer(345,100,NULL);
	fout.open("G_code.pba");
	///////////로그창 지우기+포트 선택////////
	int select = m_port.GetCurSel();


	if (select == -1){

		line_data.Format(_T("clear \r\n"));
		UpdateData(FALSE);
	}
	else {
		m_port.GetLBText(select,sensor_port);
		line_data.Format(_T("clear \r\n Select Port : %s \r\n"), sensor_port);
		UpdateData(FALSE);
		switch (select){
		case 0 : sen = "COM1:"; break;
		case 1 : sen = "COM2:"; break;
		case 2 : sen = "COM3:"; break;
		case 3 : sen = "COM4:"; break;
		case 4 : sen = "COM5:"; break;
		default : sen = "ERROR"; break;
		}
		if (RF60x_HelloCmd( hRF60x, 1, &hlans )){
			RF60x_ClosePort( hRF60x );
		}
		memset(&hlans, 0x00, sizeof(RF60xHELLOANSWER));
		RF60x_OpenPort(sen, CBR_9600, &hRF60x);

	}
	////////////////////////////////

}



