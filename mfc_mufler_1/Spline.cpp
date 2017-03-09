#include "stdafx.h"
#include "Spline.h"



double h00(double t)
{
	return 2*t*t*t - 3*t*t +1;
}
double h10(double t)
{
	return t*(1-t)*(1-t);
}
double h01(double t)
{
	return t*t*(3-2*t);
}
double h11(double t)
{
	return t*t*(t-1);
}

bool mono_spline(int time_limit,//monotone 스플라인 보간법//토크 계산을 위한 시간 제한
	const std::vector<double> x_mrc,//x 좌표값
	const std::vector<double> y_mrc,//y 좌표값
	std::vector<double>* destX,
	std::vector<double>* destY)
{
	// 0-based index 사용.
	int n = (int)x_mrc.size();//x좌표의 개수 n개의 x좌표면 마지막은 x_(n-1)이됨
	int k = 0;//n의 변수?
	double *m = new double[n];//기울기 m
	m[0] = (y_mrc[1] - y_mrc[0])/(x_mrc[1] - x_mrc[0]);//처음 x와 관련된 기울기
	m[n-1] = (y_mrc[n-1] - y_mrc[n-2])/(x_mrc[n-1]-x_mrc[n-2]);//마지막 x기울기

	for (k = 1; k<n-1; k++){//각x좌표에 따른 y값과의 기울기
		m[k] = (y_mrc[k] - y_mrc[k-1])/(2*(x_mrc[k] - x_mrc[k-1])) + (y_mrc[k+1] - y_mrc[k])/(2*(x_mrc[k+1]-x_mrc[k]));
	}
	for (k = 0; k<n-1; k++){
		double delta_k = (y_mrc[k+1]-y_mrc[k])/(x_mrc[k+1]-x_mrc[k]);

		if (fabs(delta_k) <= eps){
			m[k] = m[k+1] = 0;
		}
		else{
			double ak = m[k]/delta_k;
			double bk = m[k+1]/delta_k;
			if(ak*ak + bk*bk > 9){//오버슈트 판별: 9보다 크면 토크k의 값이 1보다 작다는말
				m[k] = 3/(sqrt(ak*ak+bk*bk))*ak*delta_k;//m_k와 m_k+1을 새롭게 계산
				m[k+1] = 3/(sqrt(ak*ak+bk*bk))*bk*delta_k;//이는 벡터 a와 b크기를 반지름 3인 원의 크기로 제한
			}
		}
	}

	for (k = 0; k<n-1; k++){//cubic 보간법
		double cur_x = (double)((int)(0.5 + x_mrc[k]));//x_lower current
		double next_x = (double)((int)(x_mrc[k+1]));//x_upper
		double cur_y = y_mrc[k];
		double next_y = y_mrc[k+1];
		double h = next_x - cur_x;
		double x = 0;
		double inc = (next_x - cur_x)*0.1;//다음 x값과 지금 x값사이에서 10분할해서 증가값정함
		for (x = cur_x; x<next_x; x+=inc){
			if (x > time_limit) break;
			double t = (x-cur_x)/h;
			if (destX != NULL){
				destX->push_back(x);//destX값뒤에 현재 x값 집어넣음
			}
			double y = cur_y*h00(t) + h*m[k]*h10(t) + next_y*h01(t) + h*m[k+1]*h11(t);
			destY->push_back(y);
		}
	}

	delete m;

	return true;
}



bool cubic_spline(std::vector<double>x_meries,//cubic 스플라인 보간법
	std::vector<double>y_meries,
	std::vector<double> *destX,
	std::vector<double>* destY)
{   
	int n = x_meries.size() -1;
	// Step 1.
	double *h = new double[n+1];
	double *alpha = new double[n+1];
	int i = 0;
	for(i = 0; i<=n-1; i++){
		h[i] = (x_meries)[i+1] - (x_meries)[i];
	}

	// Step 2.
	for(i = 1; i<=n-1;i++){
		alpha[i]= 3*((y_meries)[i+1]-(y_meries)[i])/h[i]-3*((y_meries)[i]-(y_meries)[i-1])/h[i-1];
	}

	// Step 3.
	double *l = new double[n+1];
	double *u = new double[n+1];
	double *z = new double[n+1];
	double *c = new double[n+1];
	double *b = new double[n+1];
	double *d = new double[n+1];

	l[0] = 1; u[0] = 0; z[0] = 0;

	// Step 4.
	for(i = 1; i<=n-1; i++){
		l[i] = 2*((x_meries)[i+1] - (x_meries)[i-1]) - h[i-1]*u[i-1];
		u[i] = h[i]/l[i];
		z[i] = (alpha[i] - h[i-1]*z[i-1]) / l[i];
	}

	// Step 5.
	l[n] = 1;     z[n] = 0;     c[n] = 0;

	// Step 6.
	for(i = n-1; i>=0; i--){
		c[i] = z[i] - u[i]*c[i+1];
		b[i] = ((y_meries)[i+1] - (y_meries)[i])/h[i] - h[i]*(c[i+1] + 2*c[i])/3;
		d[i] = (c[i+1] - c[i]) / (3*h[i]);
	}

	for(i = 0; i<=n-2;i++){
		double x = (x_meries)[i];
		double inc = ((x_meries)[i+1] - (x_meries)[i])*0.1;//1/10단위로 계산
		for(; x < (x_meries)[i+1]; x+=inc){
			double x_offset = x - (x_meries)[i];
			double Sx = (y_meries)[i] + b[i]*x_offset + c[i]*x_offset*x_offset + d[i]*x_offset*x_offset*x_offset;
			if(destX != NULL){
				destX->push_back(x);
			}
			if(destY != NULL){
				destY->push_back(Sx);
			}
		}           
	}
	delete [] h;
	delete [] alpha;
	delete [] l;
	delete [] u;
	delete [] z;
	delete [] c;
	delete [] b;
	delete [] d;


	return true;
}