#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#define PI 3.1415926
using namespace cv;


void Line_Detect(InputArray _src, OutputArray _dst,
	int* distance);//distance of center to line