

#include <opencv.hpp>  
#include <iostream>

using namespace cv;
using namespace std;

void rgb2hsv_3channels(Mat src, Mat &H, Mat &S, Mat &V);

void SSR_MultipleChannels(Mat &);
void SSR_SingleChannels(Mat &);
void on_Trackbar(int, void*);

Mat src = imread("mangdao3.jpg", 1);
Mat src_copy=imread("mangdao3.jpg", 1);
Mat fianl_binary;

int g_gaussSigma = 60;
int g_gaussSize = 200;

int main(void) {

	
	//src.copyTo(src_copy);
	
	namedWindow("SSR效果图");
	namedWindow("SSR后H通道灰度图");
	namedWindow("SSR后H通道二值化图");

	namedWindow("原图");
	namedWindow("H通道灰度图");
	namedWindow("H通道二值化图");
	
	imshow("未经SSR原图", src_copy);
	Mat dst, Hgray, Sgray, Vgray;
	rgb2hsv_3channels(src_copy, Hgray, Sgray, Vgray);
	imshow("H通道灰度图", Hgray);
	threshold(Hgray, dst, 0, 255, THRESH_OTSU);
	imshow("H通道二值化图", dst);

	createTrackbar("高斯sigma", "SSR效果图", &g_gaussSigma, 255, on_Trackbar);
	createTrackbar("高斯size", "SSR效果图", &g_gaussSize, 255, on_Trackbar);
	on_Trackbar(0,0);
	
	waitKey(0);

	destroyWindow("原图");
	destroyWindow("SSR效果图");
	destroyWindow("SSR后H通道灰度图");
	destroyWindow("H通道灰度图");

	namedWindow("开闭运算图");

	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(fianl_binary, dst, MORPH_OPEN, element);
	morphologyEx(dst, dst, MORPH_CLOSE, element);
	morphologyEx(dst, dst, MORPH_OPEN, element);


	imshow("开闭运算图", dst);

	waitKey(0);

	return 0;
}

void rgb2hsv_3channels(Mat src, Mat &H, Mat &S, Mat &V) {
	
	Mat hsv;
	src.convertTo(src, CV_32FC3, 1.0 / 255, 0);	//先将图像转化成32F
	cvtColor(src, hsv, COLOR_BGR2HSV);

	vector<Mat> hsvChannels(3);
	split(hsv, hsvChannels);
	//H通道是0-360，S和V是0-1
	hsvChannels.at(0).convertTo(H, CV_8UC1, 255.0 / 360);
	hsvChannels.at(1).convertTo(S, CV_8UC1, 255);
	hsvChannels.at(2).convertTo(V, CV_8UC1, 255);
}


void SSR_MultipleChannels(Mat &src) {

	if (src.type() != CV_8UC3) {
		cout << "ERROR" << endl;
		system("pause");
		return;
	}

	vector<Mat> singleChannels(3);
	split(src, singleChannels);

	SSR_SingleChannels(singleChannels.at(0));
	SSR_SingleChannels(singleChannels.at(1));
	SSR_SingleChannels(singleChannels.at(2));
	merge(singleChannels, src);

}

void SSR_SingleChannels(Mat& src) {

	Mat src_log;
	//mat.converTo代替了旧版的convertScale(src,dst...)函数，方便了很多
	src.convertTo(src, CV_8UC1, 1.0, 1);		//所有像素点+1，避免计算log(0)
	src.convertTo(src, CV_32FC1, 1.0 / 255, 0);	//将1-256的点映射到 0-1的float

	log(src, src_log);	//计算src的log值。此函数代替了旧版的cvLog

	Mat L, L_log;		//入射光线强度，和其对数

	int gaussSigma = 100;
	GaussianBlur(src, L, Size(g_gaussSize *2+1, g_gaussSize * 2 + 1), g_gaussSigma);	//高斯模糊参数30不知道是哪里来的。Size(0,0)使用自动尺寸
	log(L, L_log);

	Mat R;
	subtract(src_log, L_log, R);	//整个图像相减，每个像素相减

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
	R.convertTo(src, CV_8UC1);
	
}

void on_Trackbar(int, void*) {

	Mat src = imread("mangdao3.jpg", 1);
	Mat src_copy = imread("mangdao3.jpg", 1);

	
	SSR_MultipleChannels(src);
	imshow("SSR效果图", src);

	Mat dst, Hgray, Sgray, Vgray;
	rgb2hsv_3channels(src, Hgray, Sgray, Vgray);
	imshow("SSR后H通道灰度图", Hgray);

	threshold(Hgray, dst, 0, 255, THRESH_OTSU);
	imshow("SSR后H通道二值化图", dst);
	dst.copyTo(fianl_binary);

}

