/*
Hough变换
通过读取每一个点，计算Hough空间的theta和rho，并通过theta和rho的归一化进行画图

董岩 2018/9/19
*/


#include <opencv.hpp>
#include <iostream>
#include <vector>
#include <time.h>
#include <Windows.h>
#include <math.h>

using namespace std;
using namespace cv;

// Define the size of Hough Space


Mat drawHoughSapce(Mat binImg, int spaceWidth, int spaceHeight);

void main(void) {
	Mat src = imread("pic.jpg", 0);
	namedWindow("src", WINDOW_FREERATIO);
	namedWindow("hough");

	Mat binImg;
	threshold(src, binImg, 255, 255, THRESH_OTSU);
	drawHoughSapce(binImg, 600, 400);
	waitKey(0);
}

Mat drawHoughSapce(Mat src, int spaceWidth, int spaceHeight) {

	vector<Point> pixels;
	for (int i = 0; i < src.rows; ++i) {
		for (int j = 0; j < src.cols; ++j) {
			if (src.data[i*src.cols + j] > 128)
				pixels.push_back(Point(j, i));	//record while pixels
		}
	}

	Mat houghImg = Mat::zeros(Size(spaceWidth, spaceHeight), CV_16UC1);	//16U以避免累加时超过255
	float rhoMax = sqrt((src.rows*src.rows) + (src.cols*src.cols));	//对角线长为最大值，用于归一化
	for (int i = 0; i < pixels.size(); ++i) {
		int x = pixels[i].x;
		int y = pixels[i].y;
		// 遍历Hough空间进行绘图
		for (int thetaUnit = 0; thetaUnit < spaceWidth; ++thetaUnit) {
			float theta = thetaUnit * 2 * CV_PI / spaceWidth;	//实际角度，0-2pi
			float rho = y*sin(theta) + x*cos(theta);		//图像空间的rho值
			float rhoNorm = rho / rhoMax / 2;			//归一化为(-0.5,0.5)
			int rhoUnit = (round)(rhoNorm*spaceHeight);		//使用round近似，如果默认(int)则在(-1,1)之间均成0
			rhoUnit += spaceHeight / 2;					//转成Hough空间坐标轴
			if (rhoUnit >= spaceHeight || rhoUnit < 0)	//如果超出边界则无法绘制点
				continue;
			//houghImg.at<ushort>(rhoUnit, thetaUnit)++;
			(*((ushort*)(houghImg.data) + rhoUnit * spaceWidth + thetaUnit))++;
		}
	}

	//归一化需保证图像深度不变，所以归一化到255，再通过convertTo函数改变深度
	normalize(houghImg, houghImg, 255, 0, NORM_INF);
	Mat showImg = Mat::zeros(Size(spaceWidth, spaceHeight), CV_8UC1);
	houghImg.convertTo(showImg, CV_8U);

	imshow("hough", showImg);
	return houghImg;
}

