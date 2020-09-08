/*
SUSAN算子
通过SUSAN算子提取图像边界与角点。g_t表示中心点与模板内点差值，g_g为边缘阈值。

2018/9/18
*/


#include <opencv.hpp>
#include <iostream>
#include <vector>
#include <time.h>
#include <Windows.h>
#include <math.h>
using namespace std;
using namespace cv;

#define PIXEL_BLACK 0
#define PIXEL_WHITE 255


void doSomething(Mat src);
void onTrackbar(int, void*);

int g_t = 10, g_g = 10;

int g_xOffset[37] = {
	-1,0,1,
	-2,-1,0,1,2,
	-3,-2,-1,0,1,2,3,
	-3,-2,-1,0,1,2,3,
	-3,-2,-1,0,1,2,3,
	-2,-1,0,1,2,
	-1,0,1
};
int g_yOffset[37] = {
	-3,-3,-3,
	-2,-2,-2,-2,-2,
	-1,-1,-1,-1,-1,-1,-1,
	0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2,
	3, 3, 3
};

Mat g_src = imread("susan.jpg", 0);
void main(void) {
	
	namedWindow("Src");
	namedWindow("SUSAN",WINDOW_FREERATIO);
	namedWindow("Corner", WINDOW_FREERATIO);
	namedWindow("Value", WINDOW_FREERATIO);

	createTrackbar("t:", "SUSAN", &g_t, 100, onTrackbar);
	createTrackbar("g:", "SUSAN", &g_g, 37, onTrackbar);
	onTrackbar(0, 0);
	while (1) {
		doSomething(g_src);
		waitKey(0);
	}
}

void doSomething(Mat src) {
	auto t = clock();
	imshow("Src", src);
	Mat valueImg = Mat::zeros(src.size(), CV_8UC1);
	Mat cornerImg = Mat::zeros(src.size(), CV_8UC1);
	Mat binImg = Mat::zeros(src.size(), CV_8UC1);

	uchar *p = src.data;
	uchar *pValueImg = valueImg.data;
	uchar *pCornerImg = cornerImg.data;

	for (int i = 3; i < src.rows - 3; ++i) {
		for (int j = 3; j < src.cols - 3; ++j) {
			int value = 0;
			int centerValue = p[i*src.cols + j];
			for (int k = 0; k < 37; ++k) {
				int circleValue = p[(i + g_yOffset[k])*src.cols + (j + g_xOffset[k])];
				if (abs(centerValue - circleValue) < g_t) {
					pValueImg[i*valueImg.cols + j]++;
				}
			}
			if (pValueImg[i*valueImg.cols + j]<g_g)
				pCornerImg[i*valueImg.cols + j]=PIXEL_WHITE;
		}
	}

	imshow("Corner", cornerImg);
	imshow("Value", valueImg);
}

void onTrackbar(int, void*) {
	doSomething(g_src);
}
