
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
int z = 0;
int ke = 0;
std::vector<double> x;
std::vector<double> y;
std::vector<double> x_m;
std::vector<double> y_m;
std::vector<double>* x_z;
std::vector<double>* y_z;

////////////�������� ����
	HANDLE				hRF60x			= INVALID_HANDLE_VALUE;
	USHORT				usMeasured;
	RF60xHELLOANSWER	hlans;



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
	if (!capture->isOpened())
	{
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
	m_Graph.SetRanges(-600, 600);            // Y��  0 ���� 100 ���� �ٽñ׸�

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
	m_Graph.SetYUnits(_T("Velocity < mm >"), _T("-600.0"), _T("600.0") ) ;
	m_Graph.SetLegendLabel(_T("X axis"), 0);
	m_Graph.SetLegendLabel(_T("Y axis"), 1);
	m_Graph.SetLegendLabel(_T("Total"), 2);
	///////////////////////////////////////////////////////////////////////

	//���� ����////////////////////////////////////////////////////////////


	memset(&hlans, 0x00, sizeof(RF60xHELLOANSWER));
	RF60x_OpenPort("COM3:", CBR_9600, &hRF60x);
	height = 0;
	sensor_range = 0;
	mesured_data = 0;



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
		ke = ke+1;

		if(O_img.empty()){//����� ������ ������
			KillTimer(123);
			MessageBox(_T("������ ����"));

		}

		//bit info���� bitcount ���� bpp 8bit 24bit 32bit �̹���
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
				//line(H_img,pt1,pt2, Scalar(0,0,255),2);//������ ���

			}

			if (xe1 == 0){
				xe1 = e1, xe2 = e2;
			}
			else {
				e1 = xe1, e2 = xe2;
				Point ept1(xe1,0),ept2(xe2,700);//��ն��� ���
				line(H_img,ept1,ept2, Scalar(255,255,0),2);//�׸���
				Point ct1(512,0),ct2(512,768);//�߽ɼ� ������ ����
				line(H_img,ct1,ct2, Scalar(0,255,255),1);//�߽ɼ� �׸���

				distance = (xe1+xe2)/2 - 512;//������ �߽��� �Ÿ�����
				Point dp1((xe1+xe2)/2,300),dp2(512,300);//������ �Ÿ� ǥ����
				line(H_img,dp1,dp2, Scalar(255,0,255),1);
				string te;//string �� ���� ���� ���� �Է�
				stringstream ste;
				ste<<"distance:"<<distance<<"pixel"<<endl;
				te=ste.str();
				putText(H_img,te,dp1,3,1.2,Scalar(0,255,0));//�̹����� �Ÿ�ǥ��
				//data[0] = (double)(distance);
			}
			//mfc_img �� CImage�� �ʱ� ����

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
		////////////�ð����� ���� ���� ����/////
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


	if (ke == 10){//�ʹ� ���� ���� ���� �ʱ����� delay��
		ke = 0;//delay�ʱ�ȭ
		if(distance == 0);//�Ÿ������� �ȵǾ����� �����Ⱚ �Է� ����
		else{
			//int time=1000;//��ũ ������ ���� �ð�
			z = z + 10;//x�� ��ȭ���� ���߿� ���� �Կ��ϸ� �ӵ������� ���ϴ� �Ÿ��� ����
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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	capture->release();
	KillTimer(123);
	RF60x_ClosePort( hRF60x );
}


void Cmfc_mufler_1Dlg::OnBnClickedOpenbtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	SetTimer(123,200,NULL);


}


void Cmfc_mufler_1Dlg::OnBnClickedSpline()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	//SetTimer(234,1000,NULL);



	if (x.size() > 2){//x�迭�� ������ 2�� �̻��϶� �׷����� ã��
		cubic_spline(x,y,x_z,y_z);
	}
	//for (int k = 0;k<x.size();k++){//�Էµ� ���� ���� �׷���
	//	//printf("�Էµ� �� x = %lf , y = %lf \n" ,x[k],y[k]);
	//	string te;//string �� ���� ���� ���� �Է�

	//	stringstream ste;
	//	ste<<"x = "<<x[k]<<", y = "<<y[k]<<"\r\n"<<endl;

	//	te=ste.str();
	//	line_data = (te.c_str());//string->CString
	//	//str.Format("%d", i); 
	//	SetDlgItemText(IDC_Line, line_data); 
	//	GetDlgItem(IDC_Line)->UpdateWindow();
	//	//�׷��� ����
	//	//data[0] = y[k];


	//	//UpdateData(FALSE);//data�� ���� �ø���.
	//	//	if (k == 0);
	//	//	else
	//	//		line(plot_img,Point(x[k-1],y[k-1]),Point(x[k],y[k]),Scalar(0,255,255),2,8,0);

	//}


	for (int k =0;k<x_m.size();k++){//mono spline�� �׷���
		string te;//string �� ���� ���� ���� �Է�
		stringstream ste;
		ste<<"x = "<<x_m[k]<<", y = "<<y_m[k]<<"\r\n"<<endl;

		te=ste.str();
		line_data = (te.c_str());//string->CString
		SetDlgItemText(IDC_Line, line_data); 
		GetDlgItem(IDC_Line)->UpdateWindow();
		//UpdateData(FALSE);//data�� ���� 
		//data[0] = y_m[k];
	}




	}


void Cmfc_mufler_1Dlg::OnBnClickedSensorbtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	memset(&hlans, 0x00, sizeof(RF60xHELLOANSWER));

	RF60x_OpenPort("COM3:", CBR_9600, &hRF60x);

	if (RF60x_HelloCmd( hRF60x, 1, &hlans ))
	{

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

		UpdateData(FALSE);//data�� ���� �ø���.


	} 

	RF60x_ClosePort( hRF60x );
}
