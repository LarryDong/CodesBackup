

#include <opencv.hpp>  
#include <iostream>

using namespace cv;
using namespace std;

int main(void) {

	Mat srcImg = imread("hsv.jpg", 1);
	imshow("原图", srcImg);
	Mat bgr32Img, hsv32Img;
	srcImg.convertTo(bgr32Img, CV_32FC3, 1.0 / 255, 0);	//先将图像转化成32F
	cvtColor(bgr32Img, hsv32Img, COLOR_BGR2HSV);

	vector<Mat> hsvChannels(3);
	split(hsv32Img, hsvChannels);

	Mat grayLevel;
	Mat Hgray, Sgray, Vgray;
	
	//H通道是0-360，S和V是0-1
	hsvChannels.at(0).convertTo(Hgray, CV_8UC1, 255.0 / 360);
	hsvChannels.at(1).convertTo(Sgray, CV_8UC1, 255);
	hsvChannels.at(2).convertTo(Vgray, CV_8UC1, 255);

	imshow("H分量灰度图", Hgray);
	imshow("S分量灰度图", Sgray);
	imshow("V分量灰度图", Vgray);

	waitKey(0);

	return 0;
}


