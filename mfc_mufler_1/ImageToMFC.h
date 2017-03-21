//#include "afxwin.h"
#include <conio.h>
#include <windows.h>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <atlimage.h>
using namespace cv;

void ImagePrintInMFC(CImage *image_mfc, CStatic* Print_window, InputArray _src);