
#include <opencv.hpp>
#include <iostream>
#include <math.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//可用滑动条调整，但存在bug。一段时间后雾霾区域扩大

using namespace std;
using namespace cv;


void on_Trackbar(int, void*);
void SSR(Mat, Mat&, int[]);


Mat src = imread("car.jpg", 1);
vector<Mat> src_channels;
vector<Mat> dst_channels(3);
int g_gaussSigma[3];//滑动条对应的变量

void main() {
	imshow("原图", src);
	split(src, src_channels);
	g_gaussSigma[0] = 15;
	g_gaussSigma[1] = 80;
	g_gaussSigma[2] = 200;

	namedWindow("颜色融合图");
	namedWindow("toolbar", CV_WINDOW_AUTOSIZE);

	createTrackbar("scale1", "toolbar", &g_gaussSigma[0], 100, on_Trackbar);
	createTrackbar("scale2", "toolbar", &g_gaussSigma[1], 200, on_Trackbar);
	createTrackbar("scale3", "toolbar", &g_gaussSigma[2], 300, on_Trackbar);

	on_Trackbar(g_gaussSigma[0], 0);
	on_Trackbar(g_gaussSigma[1], 0);
	on_Trackbar(g_gaussSigma[2], 0);
	waitKey(0);
}

void on_Trackbar(int, void*) {
	SSR(src_channels.at(0), dst_channels.at(0), g_gaussSigma);		//目前不懂这两个参数是怎么来的
	SSR(src_channels.at(1), dst_channels.at(1), g_gaussSigma);
	SSR(src_channels.at(2), dst_channels.at(2), g_gaussSigma);
	Mat dst;
	merge(dst_channels, dst);
	imshow("颜色融合图", dst);
}


void SSR(Mat src, Mat& dst, int *gaussSigma) {

	Mat src_log;
	//mat.converTo代替了旧版的convertScale(src,dst...)函数，方便了很多
	src.convertTo(src, CV_8UC1, 1.0, 1);		//所有像素点+1，避免计算log(0)
	src.convertTo(src, CV_32FC1, 1.0 / 255, 0);	//将1-256的点映射到 0-1的float

	log(src, src_log);	//计算src的log值。此函数代替了旧版的cvLog

	Mat L[3], L_log[3], R_log[3];		//入射光线强度，和其对数

	for (int i = 0; i < 3; ++i) {
		GaussianBlur(src, L[i], Size(0, 0), gaussSigma[i]);	//Size(0,0)根据sigma自动计算尺寸
		log(L[i], L_log[i]);
		subtract(src_log, L_log[i], R_log[i]);	//整个图像相减，每个像素相减
	}


	Mat R;
	R = (R_log[0] + R_log[1] + R_log[2])/3;

	Mat mean, std;
	meanStdDev(R, mean, std);		//计算图像均值与方差
	double min = mean.at<double>(0) - 2 * std.at<double>(0);	//可以认为所有点基本都在正负2sigma之间，进行平衡化
	double max = mean.at<double>(0) + 2 * std.at<double>(0);

	//将图像拉伸映射到0-255
	for (int i = 0; i < R.rows; ++i) {
		for (int j = 0; j < R.cols; ++j) {
			R.at<float>(i*R.cols + j) = (R.at<float>(i*R.cols + j) - min) / (max - min) * 255;
		}
	}

	R.convertTo(dst, CV_8UC1);
}


