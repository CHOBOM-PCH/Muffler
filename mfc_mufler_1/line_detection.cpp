#include "stdafx.h"
#include "line_detection.h"

void Line_Detect(InputArray _src, OutputArray _dst, int* distance)
{
	using namespace cv;
	Mat input_image = _src.getMat();
	Mat gray_img;
	Mat blur_img;
	Mat thresoutput_image;
	vector<Vec4i>lines;
	int length = 0;
		
	Mat output_image;
	output_image = input_image.clone();
	_dst.getMatRef() = output_image;

	/////pre processing
	cvtColor(input_image,gray_img,CV_BGR2GRAY);
	GaussianBlur(gray_img,blur_img,cv::Size(5,5),10);
	adaptiveThreshold(blur_img, thresoutput_image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 15, 2);
	//line detect
	HoughLinesP(thresoutput_image, lines, 1, PI/270, 10, 200, 0);
	//save line
	Vec4d params,eparams;
	int x1, y1, x2, y2;
	int xe1=0, ye1=0, xe2=0, ye2 =0;
	int e1=0,e2=0;
	for (int k = 0; k < lines.size(); k++){
		params = lines[k];
		x1 = params[0];
		y1 = params[1];
		x2 = params[2];
		y2 = params[3];

		eparams[0] = eparams[0] + params[0];
		eparams[1] = eparams[1] + params[1];
		eparams[2] = eparams[2] + params[2];
		eparams[3] = eparams[3] + params[3];
		xe1 = (eparams[0]) / (k + 1);
		ye1 = (eparams[1]) / (k + 1);
		xe2 = (eparams[2]) / (k + 1);
		ye2 = (eparams[3]) / (k + 1);
		Point pt1(x1,y1),pt2(x2,y2);
	}
	//print line and distance
	if (xe1 == 0){
		xe1 = e1, xe2 = e2;
	}
	else {
		e1 = xe1, e2 = xe2;
		Point ept1(xe1,0),ept2(xe2,700);
		line(output_image,ept1,ept2, Scalar(255,255,0),2);
		Point ct1(((input_image.cols)/2),0),ct2(((input_image.cols)/2),768);
		line(output_image,ct1,ct2, Scalar(0,255,255),2);

		length = ((input_image.cols)/2) - (xe1+xe2)/2 ;
		*distance = length;
		Point dp1((xe1+xe2)/2,300),dp2(((input_image.cols)/2),300);
		//line(output_image,dp1,dp2, Scalar(255,0,255),1);
		string te;
		std::stringstream ste;
		ste<<"distance:"<<*distance<<"pixel"<<std::endl;
		te=ste.str();
		putText(output_image,te,dp1,3,1.2,Scalar(0,255,0));
	}
	//imshow ("Ãâ·Â",output_image);
	return ;
}