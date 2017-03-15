
// mfc_mufler_1Dlg.cpp : ���� ����
//

#include "stdafx.h"
#include "mfc_mufler_1.h"
#include "mfc_mufler_1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////���ö��� ����///
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

int k = 0;//timer 234���� ī������ ���� ����
ofstream fout;//���� ������� ���� ����

////////////�������� ����///////////
HANDLE				hRF60x			= INVALID_HANDLE_VALUE;
USHORT				usMeasured;
RF60xHELLOANSWER	hlans;
CString sensor_port;
const char* sen ;
///////////////////////////////////


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	// �����Դϴ�.
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


// Cmfc_mufler_1Dlg ��ȭ ����




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


// Cmfc_mufler_1Dlg �޽��� ó����

BOOL Cmfc_mufler_1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	// ���� ĸ�ĸ� ���� ���� ����
	capture = new VideoCapture("mufler.mp4");
	//capture = new VideoCapture(0);
	if (!capture->isOpened()){
		MessageBox(_T("���������� �����ϴ�."));

	}
	image_mfc = NULL;
	//�׷����� ���� ����///////////////////////////////////////////////////
	data[0] = data[1] = 0.0;
	wchar_t   buf[250];   // �׷������� ���
	CRect rect;    
	GetDlgItem(IDC_Graph)->GetWindowRect(rect);                      // (3)
	ScreenToClient(rect);

	m_Graph.Create(WS_VISIBLE|WS_CHILD, rect, this);             // (241) / 150 -> 350       (327) / 150 -> 500
	m_Graph.SetRanges(-100, 300);            // Y��  0 ���� 100 ���� �ٽñ׸�

	m_Graph.SetBackgroundColor(RGB(0,0,0));
	m_Graph.SetGridColor(RGB(255,255,255));
	m_Graph.SetPlotColor(RGB(255,128,0), 0);//��1
	m_Graph.SetPlotColor(RGB(0,255,0), 1);// ��2
	//m_Graph.SetPlotColor(RGB(255,0,0), 2);// ��3
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

	//���� ����////////////////////////////////////////////////////////////
	m_port.AddString(_T("COM1"));
	m_port.AddString(_T("COM2"));
	m_port.AddString(_T("COM3"));
	m_port.AddString(_T("COM4"));
	m_port.AddString(_T("COM5"));
	m_port.SetCurSel(2);//default COM3��Ʈ
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
	
	///�������� ��Ʈ ������ ����////////////////////////////////////
	memset(&hlans, 0x00, sizeof(RF60xHELLOANSWER));
	RF60x_OpenPort(sen, CBR_9600, &hRF60x);
	height = 0;
	sensor_range = 0;
	mesured_data = 0;

	line_data.Format(_T("��Ʈ : %s \r\n"), sensor_port);
	SetDlgItemText(IDC_Line,line_data);
	//UpdateData(FALSE);
	////////////////////////////////////////////////////////////////
	////////////���� ����¿�//////////////
	
	fout.open("G_code.pba");

	
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void Cmfc_mufler_1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR Cmfc_mufler_1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cmfc_mufler_1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	//ĸ���� �̹����� �������Ӿ� �����ͼ� image���Ͽ� ����

	int distance = 0; //�Ÿ������� ��������


	if(nIDEvent == 123){
		capture->read(O_img);
		count_delay = count_delay+1;

		if(O_img.empty()){//����� ������ ������
			KillTimer(123);
			MessageBox(_T("������ ����"));

		}


		else{
			RECT r;//����� �������� ũ�� ����
			Mufler_img.GetClientRect(&r);
			cv::Size winSize(r.right, r.bottom);



			//���÷� �� ����

			cvtColor(O_img,gray_img,CV_BGR2GRAY);

			GaussianBlur(gray_img,blur_img,cv::Size(5,5),10);

			adaptiveThreshold(blur_img,           // input image //�������� ������ Ȧ����
				thresh_img,                              // output image
				255,                                    // make pixels that pass the threshold full white
				ADAPTIVE_THRESH_GAUSSIAN_C,         // use gaussian rather than mean, seems to give better imgs
				THRESH_BINARY_INV,                  // invert so foreground will be white, background will be black
				15,                                     // size of a pixel neighborhood used to calculate threshold value
				2);                                     // constant subtracted from the mean or weighted mean

			vector<Vec4i>lines;//lines ����
			HoughLinesP(thresh_img,//���������� ���� ������ �̹���
				lines,//������ ���� ����� ���
				1,//�������κ��� �Ÿ�����rho
				PI/270,//x����� ���� ���� ����
				10,//������ �����ϱ����� accumulator�� �Ӱ谪
				200,//�ּ����� ���α���
				0);//�ִ� ���� ����
			Mat H_img = O_img.clone();
			Vec4d params,eparams;//vector�� ��ĺ��� ����
			int x1, y1, x2, y2;
			int xe1=0, ye1=0, xe2=0, ye2 =0;//�������� ��ǥ������
			int e1=0,e2=0;
			for (int k = 0; k < lines.size(); k++){
				params = lines[k];
				x1 = params[0];//���ε��� ������
				y1 = params[1];
				x2 = params[2];//���ε��� ����
				y2 = params[3];
				//eparams = eparams + params;//���ε� �� ��ǥ���� ��
				eparams[0]=eparams[0]+params[0];
				eparams[1]=eparams[1]+params[1];
				eparams[2]=eparams[2]+params[2];
				eparams[3]=eparams[3]+params[3];
				xe1 = (eparams[0])/(k+1);//��� ������ ������
				ye1 = (eparams[1])/(k+1);
				xe2 = (eparams[2])/(k+1);//��ն����� ����
				ye2 = (eparams[3])/(k+1);
				Point pt1(x1,y1),pt2(x2,y2);
				line(H_img,pt1,pt2, Scalar(0,0,255),1);//������ ���

			}

			if (xe1 == 0){
				xe1 = e1, xe2 = e2;
			}
			else {
				e1 = xe1, e2 = xe2;
				Point ept1(xe1,0),ept2(xe2,700);//��ն��� ���
				line(H_img,ept1,ept2, Scalar(255,255,0),2);//�׸���
				Point ct1(((O_img.cols)/2),0),ct2(((O_img.cols)/2),768);//�߽ɼ� ������ ����
				line(H_img,ct1,ct2, Scalar(0,255,255),2);//�߽ɼ� �׸���

				distance = ((O_img.cols)/2) - (xe1+xe2)/2 ;//������ �߽��� �Ÿ�����
				Point dp1((xe1+xe2)/2,300),dp2(((O_img.cols)/2),300);//������ �Ÿ� ǥ����
				//line(H_img,dp1,dp2, Scalar(255,0,255),1);
				string te;//string �� ���� ���� ���� �Է�
				stringstream ste;
				ste<<"distance:"<<distance<<"pixel"<<endl;
				te=ste.str();
				putText(H_img,te,dp1,3,1.2,Scalar(0,255,0));//�̹����� �Ÿ�ǥ��

				data[0] = (double)(distance);
			}
			//mfc_img �� CImage�� �ʱ� ����

			if (image_mfc){
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
			if (H_img.cols == winSize.width  && H_img.rows == winSize.height){
				// source and destination have same size
				// transfer memory block
				// NOTE: the padding border will be shown here. Anyway it will be max 3px width

				SetDIBitsToDevice(image_mfc->GetDC(),
					//destination rectangle
					0, 0, winSize.width, winSize.height,
					0, 0, 0, H_img.rows,
					H_img.data, &bitInfo, DIB_RGB_COLORS);
			}
			else {
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


			if (image_mfc){
				image_mfc->ReleaseDC();
				delete image_mfc; 
				image_mfc = nullptr;
			}
		}
		////////////�ð����� ���� ���� ����/////
		//RF60x_OpenPort("COM3:", CBR_9600, &hRF60x);

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


	if (count_delay == 10){//�ʹ� ���� ���� ���� �ʱ����� delay��
		count_delay = 0;//delay�ʱ�ȭ
		if(distance == 0);//�Ÿ������� �ȵǾ����� �����Ⱚ �Է� ����
		else {
			//int time=1000;//��ũ ������ ���� �ð�
			count_y = count_y + 10;//x�� ��ȭ���� ���߿� ���� �Կ��ϸ� �ӵ������� ���ϴ� �Ÿ��� ����
			x.push_back(distance);
			y.push_back(count_y);
			z.push_back(height);
			x_z = &x_m;
			y_z = &y_m;
			z_z = &z_m;


		}

	}


	else if (nIDEvent == 234){
		pEdit = (CEdit*)GetDlgItem(IDC_Line);
		//for (int k =0;k<y_m.size();k++){//mono spline�� �׷���
		if (k < (y_m.size())){

			//string te;//string �� ���� ���� ���� �Է�
			//stringstream ste;
			//ste<<"x = "<<x_m[k]<<", y = "<<y_m[k]<<", z = "<<z_m[k]<<"\r\n"<<endl;
			//te=ste.str();
			//line_data = (te.c_str());//string->CString


			line_data.Format(_T("x = %lf , y = %lf,  z = %lf \r\n"), x_m[k], y_m[k], z_m[k]);
			pEdit->SetSel(0,0);
			pEdit->ReplaceSel(line_data);
			fout<<"x = "<<x_m[k]<<", y = "<<y_m[k]<<", z = "<<z_m[k]<<"\r\n"<<endl;
			//UpdateData(FALSE);//data�� ���� 
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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	SetTimer(123,20,NULL);


}


void Cmfc_mufler_1Dlg::OnBnClickedSpline()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//���ö����� �����ϱ����� ��ư

	if (y.size() > 2){//x�迭�� ������ 2�� �̻��϶� �׷����� ã��
		//vector�� �� ��ø�� �������� �ʱ�ȭ �ʿ�
		y_m.clear();
		x_m.clear();
		cubic_spline(y,x,y_z,x_z);
		//y�� �ι� Ŭ���� �ϴ°͵� ������ �˻��� y���� ����ֱ⿡ �ߺ� ����
		y_m.clear();
		z_m.clear();
		cubic_spline(y,z,y_z,z_z);

	}
	KillTimer(123);
	SetTimer(234,100,NULL);

}


void Cmfc_mufler_1Dlg::OnBnClickedSensorbtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetTimer(345,100,NULL);
		
	if (RF60x_HelloCmd( hRF60x, 1, &hlans )){

		RF60x_Measure( hRF60x, 1, &usMeasured);
		mesured_data = usMeasured;
		sensor_range = hlans.wDeviceRange;
		height = (double) (sensor_range*mesured_data/16384.0);

		string te;//string �� ���� ���� ���� �Է�

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
		UpdateData(FALSE);//data�� ���� �ø���.
		
	}
	else {
		line_data = _T("rs232 error!\r\n");
		UpdateData(FALSE);
	}


}


void Cmfc_mufler_1Dlg::OnBnClickedClear()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	////////�����ϴ� Ÿ�̸� ����////
	KillTimer(123);
	KillTimer(234);
	SetTimer(345,100,NULL);
	fout.open("G_code.txt");
	///////////�α�â �����+��Ʈ ����////////
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



