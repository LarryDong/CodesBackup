
/*
自己写着玩的，测试图像各种二值化操作的程序。
包括：固定阈值法（可调整）、OTSU法、自适应法（Mean和Gaussian，均可调整尺寸和常数值）

						董岩 2018/3/20
*/

#include <opencv.hpp>  
#include <iostream>

using namespace cv;
using namespace std;

#define BINARY_PROCESS_CONST_THRESHOD 0
#define BINARY_PROCESS_OTSU 1
#define BINARY_PROCESS_ADAPTIVE_MEAN 2
#define BINARY_PROCESS_ADAPTIVE_GAUSSIAN 3


void on_Trackbar(int, void*);

Mat srcImg = imread("picture.jpg", 0);
int g_blockSize = 1;
int g_constValue = 0;
int g_adaptiveGaussian = 0;
int g_binary_type = 0;
int g_threshold = 0;

int main(void) {
	cout << "This demo is to show different types of threshold settings." << endl;
	cout << "                              By Larry Dong,     2018/3/20" << endl << endl;

	if(!srcImg.data){
		cout << "Cannot find \"picture.jpg\", please check the filename." << endl << endl;
		system("pause");
		return -1;
	}

	imshow("原图", srcImg);

	namedWindow("toolbar", CV_WINDOW_AUTOSIZE);
	createTrackbar("Type", "toolbar", &g_binary_type, 3, on_Trackbar);
	createTrackbar("Thresh", "toolbar", &g_threshold, 255, on_Trackbar);
	createTrackbar("adSize:", "toolbar", &g_blockSize, 100, on_Trackbar);
	createTrackbar("adConst", "toolbar", &g_constValue, 255, on_Trackbar);
	on_Trackbar(0,0);	//启动鼠标点击调用

	waitKey(0);
	return 0;
}


void on_Trackbar(int, void*) {
	Mat dstImg;
	switch (g_binary_type) {
	case BINARY_PROCESS_CONST_THRESHOD:
		cout << "Constant threshold mesthod. \"Thresh\" can be changed." << endl;
		threshold(srcImg, dstImg, g_threshold, 255, THRESH_BINARY);
		break;
	case BINARY_PROCESS_OTSU:
		cout << "OTSU mesthod. Nothing can be changed." << endl;
		threshold(srcImg, dstImg, 0, 255, THRESH_OTSU);
		break;
	case BINARY_PROCESS_ADAPTIVE_MEAN:
		cout << "Adaptive threshold (mean) mesthod. \"Size\" and \"Const\" can be changed." << endl;
		adaptiveThreshold(srcImg, dstImg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, g_blockSize * 2 + 3, g_constValue);
		break;
	case BINARY_PROCESS_ADAPTIVE_GAUSSIAN:
		cout << "Adaptive threshold (guassian) mesthod. \"Size\" and \"Const\" can be changed." << endl;
		adaptiveThreshold(srcImg, dstImg, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, g_blockSize * 2 + 3, g_constValue);
		break;
	}
	imshow("二值化图", dstImg);
}
