/*
绘制hue-saturation二维直方图

用到了calcHist的多维功能
之后转化到图片中，0-255灰度图进行显示。
董岩 2018.9.3
*/


#include <opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

void main() {
	Mat src, hsv;
	src = imread("QRCode//0.jpg", 1);
	cvtColor(src, hsv, COLOR_BGR2HSV);

	int hueBinNum = 30;
	int saturationBinNum = 32;
	int histSize[] = { hueBinNum,saturationBinNum };
	float hueRanges[] = { 0,180 };
	float saturationRanges[] = { 0,256 };
	const float* ranges[] = { hueRanges,saturationRanges };
	Mat dstHist;
	int channels[] = { 0,1 };
	calcHist(&hsv, 1, channels, Mat(), dstHist, 2, histSize, ranges, true, false);

	double maxValue = 0;
	minMaxLoc(dstHist, 0, &maxValue, 0, 0);
	int scale = 10;
	Mat histImg = Mat::zeros(saturationBinNum*scale, hueBinNum*scale, CV_8UC3);
	for (int hue = 0; hue < hueBinNum; ++hue) {
		for (int sat = 0; sat < saturationBinNum; ++sat) {
			float binValue = dstHist.at<float>(hue, sat);
			int intensity = cvRound(binValue * 255 / maxValue);
			rectangle(histImg, Point(hue*scale, sat*scale), Point((hue + 1)*scale - 1, (sat + 1)*scale - 1), Scalar::all(intensity), FILLED);
		}
	}
	imshow("原图", src);
	imshow("H-S分布图", histImg);
	waitKey(0);

}