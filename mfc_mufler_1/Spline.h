#include <math.h>
#include <vector>
//#include <iostream>
#define eps 1*(10^(-10))

bool mono_spline(int time_limit,//monotone 스플라인 보간법//토크 계산을 위한 시간 제한
	const std::vector<double> x_mrc,//x 좌표값
	const std::vector<double> y_mrc,//y 좌표값
	std::vector<double>* destX,
	std::vector<double>* destY);

bool cubic_spline(std::vector<double>x_meries,//cubic 스플라인 보간법
	std::vector<double>y_meries,
	std::vector<double> *destX,
	std::vector<double>* destY);

