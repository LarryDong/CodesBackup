
/*
两种局部二值化方法比较
1. 局部均值二值化。采用cv自带adaptiveThreshold函数进行实现，两参数可调
2. niblack方法局部二值化。

存在问题：niblack方法计算速度较慢

董岩 2018/9/2
*/


#include <opencv.hpp>
#include <iostream>
#include <vector>
#include <time.h>


using namespace cv;
using namespace std;


#define HISTGRAM_X_SCALE 2
#define HISTGRAM_X_SIZE (256*HISTGRAM_X_SCALE)
#define HISTGRAM_Y_SIZE 400


void doSomething(Mat src);
void onTrackbar_mean(int, void*);
void onTrackbar_niblack(int, void*);
Mat NiBlack(Mat src);


Mat g_src = imread("QRCode\\0.jpg", 1);

int g_mean_blockSize = 10, g_mean_const = 5, g_niblack_blockSize=10, g_niblack_const=1;
#define MAX_PICTURE_NUMBER 100
string FILE_SOURCE_PATH = "nearBall\\";

void main() {
	Mat src;
	int i = 0;
	string no;
	namedWindow("mean");
	namedWindow("niblack");

	createTrackbar("size", "mean", &g_mean_blockSize, 100, onTrackbar_mean);
	createTrackbar("const", "mean", &g_mean_const, 100, onTrackbar_mean);

	createTrackbar("size", "niblack", &g_niblack_blockSize, 100, onTrackbar_niblack);
	createTrackbar("k(%)", "niblack", &g_niblack_const, 100, onTrackbar_niblack);

	for (int i = 0; i < MAX_PICTURE_NUMBER; ++i) {

		if (i < 0)
			i = 0;
		no = to_string(i);
		g_src = imread(FILE_SOURCE_PATH + no + ".jpg", 1);
		if (g_src.empty())
			cout << endl << "ERROR" << endl;

		imshow("原图", g_src);
		doSomething(g_src);
		int keyValue = waitKey(0);

		switch (keyValue) {
		case 'w': case 'a':
			i--;
			break;
		case 'b':
			i = 0;
			break;
		case 'q':
			return;
		default:
			i++;
			break;
		}
	}
	return;

}

void onTrackbar_mean(int, void*){
	Mat grayImg;
	cvtColor(g_src, grayImg, CV_BGR2GRAY);
	Mat binImg;
	adaptiveThreshold(grayImg, binImg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, g_mean_blockSize * 2 + 3, g_mean_const);
	imshow("mean", binImg);
}

//单击滚动条时重新绘制图像
void onTrackbar_niblack(int, void*) {
	Mat grayImg;
	cvtColor(g_src, grayImg, CV_BGR2GRAY);
	Mat src;
	cvtColor(g_src, src, CV_BGR2GRAY);
	int halfBlockSize = (g_niblack_blockSize * 2 + 3) / 2;	//虽然可以直接采用g_niblack_blockSize，但为与mean方法统一尺寸，也进行了 *2+3
	Mat threshImg = Mat::zeros(src.size(), CV_8UC1);

	//auto start = clock();
	//此方法另一个问题是，存在外围区域无法产生阈值
	for (int i = halfBlockSize; i <= src.rows - halfBlockSize - 1; ++i) {
		for (int j = halfBlockSize; j <= src.cols - halfBlockSize - 1; ++j) {
			uchar value = src.at<uchar>(i, j);
			Point leftUp = Point(j - halfBlockSize, i - halfBlockSize);
			Point rightDown = Point(j + halfBlockSize, i + halfBlockSize);
			Mat roiImg = src(Rect(leftUp, rightDown));

			Mat srcMean, srcSigma;
			meanStdDev(roiImg, srcMean, srcSigma);
			double mean = srcMean.at<double>(0);
			double sigma = srcSigma.at<double>(0);
			threshImg.at<uchar>(i, j) = mean + g_niblack_const*sigma / 100;
			//imshow("ROI", roiImg);
		}
	}
	//cout << "找阈值用时：" << (clock() - start) << " ms." << endl;
	imshow("niblack阈值", threshImg);

	Mat dstImg = Mat::zeros(src.size(), CV_8UC1);
	for (int i = halfBlockSize; i < src.rows - halfBlockSize - 1; ++i) {
		for (int j = halfBlockSize; j < src.cols - halfBlockSize - 1; ++j) {
			uchar value = src.at<uchar>(i, j);
			if (value < threshImg.at<uchar>(i, j))
				dstImg.at<uchar>(i, j) = 0;
			else
				dstImg.at<uchar>(i, j) = 255;
		}
	}
	imshow("niblack", dstImg);

}

void doSomething(Mat src) {
	Mat grayImg;
	int grayLevel[256] = { 0 };
	if (src.type() != CV_8UC1)
		cvtColor(src, grayImg, CV_BGR2GRAY);
	imshow("灰度图", grayImg);
	onTrackbar_mean(0, 0);
	onTrackbar_niblack(0, 0);
}
