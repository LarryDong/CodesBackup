
#include <opencv.hpp>
#include <iostream>
#include <math.h>
#include <opencv2/core.hpp>
using namespace std;
using namespace cv;


void SSR(Mat src) {

	Mat src_log;

	//mat.converTo代替了旧版的convertScale(src,dst...)函数，方便了很多
	src.convertTo(src, CV_8U, 1.0, 1);		//所有像素点+1，避免计算log(0)
	src.convertTo(src, CV_32F, 1.0/255, 0);	//将1-256的点映射到 0-1的float
	
	log(src, src_log);	//计算src的log值。此函数代替了旧版的cvLog

	Mat L,L_log;		//入射光线强度，和其对数
	
	int sigma_of_gauss = 30;
	GaussianBlur(src, L, Size(0, 0), sigma_of_gauss);	//高斯模糊参数30不知道是哪里来的。Size(0,0)使用自动尺寸
	log(L, L_log);
	
	Mat R;
	subtract(src_log, L_log, R);	//整个图像相减，每个像素相减

	Mat mean, std;
	meanStdDev(R, mean, std);		//计算图像均值与方差
	double min = mean.at<double>(0) - 2*std.at<double>(0);	//可以认为所有点基本都在正负2sigma之间，进行平衡化
	double max = mean.at<double>(0) + 2*std.at<double>(0);

	//将图像拉伸映射到0-255
	for (int i = 0; i < R.rows; ++i) {
		for (int j = 0; j < R.cols; ++j) {
			R.at<float>(i*R.cols + j) = (R.at<float>(i*R.cols + j) - min) / (max - min) * 255;
		}
	}

	Mat dst;
	//转化为灰度图，作用于//convertScaleAbs(R, dst, 1, 0);相同
	R.convertTo(dst, CV_8U);
	imshow("灰度SSR效果图",dst);
}

void main() {

	Mat src = imread("car.jpg", 0);
	imshow("原图", src);
	SSR(src);		//目前不懂这两个参数是怎么来的
	waitKey(0);
}

