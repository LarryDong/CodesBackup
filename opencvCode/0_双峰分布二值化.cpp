
/*
基于谷底最小值的阈值分割
通过邻近3点平滑直方图分布，形成双峰分布，然后选取谷底最小值，进行二值化分割

董岩 2018/9/2
*/


#include <opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;


#define HISTGRAM_X_SCALE 2
#define HISTGRAM_X_SIZE (256*HISTGRAM_X_SCALE)
#define HISTGRAM_Y_SIZE 400


void doSomething(Mat src);
void showHistgram(int *grayLevel, string windowName);
int smoothHistgram(int *oldHist, double *newHist);
bool isTwoPolar(double *hist);
uchar findLocalMinimum(double *hist);


#define MAX_PICTURE_NUMBER 100
string FILE_SOURCE_PATH = "nearBall\\";

void main() {
	Mat src;
	int i = 0;
	string no;

	for (int i = 0; i < MAX_PICTURE_NUMBER; ++i) {

		if (i < 0)
			i = 0;
		no = to_string(i);
		src = imread(FILE_SOURCE_PATH + no + ".jpg", 1);
		if (src.empty())
			cout << endl << "ERROR" << endl;

		imshow("原图", src);
		doSomething(src);
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


void doSomething(Mat src) {

	Mat grayImg;
	int grayLevel[256] = { 0 };
	if (src.type() != CV_8UC1)
		cvtColor(src, grayImg, CV_BGR2GRAY);
	imshow("灰度图", grayImg);

	// 统计灰度分布
	for (int i = 0; i < src.rows; ++i) {
		for (int j = 0; j < src.cols; ++j) {
			uchar value = src.at<uchar>(i, j);
			grayLevel[value]++;
		}
	}
	showHistgram(grayLevel, "Original");

	double newHist[256] = { 0 };
	if (smoothHistgram(grayLevel, newHist) != -1) {
		uchar p1, p2;
		int intHist[256] = { 0 };
		for (int i = 0; i < 256; ++i)
			intHist[i] = static_cast<int>(newHist[i]);
		showHistgram(intHist, "New");
		int thresh = findLocalMinimum(newHist);
		Mat binImg;
		threshold(grayImg, binImg, thresh, 255, THRESH_BINARY);
		imshow("二值化图", binImg);
	}
	else
		;
	// 绘制平滑直方图

}


void showHistgram(int *grayLevel, string windowName = "Default") {

	double minValue = *min_element(grayLevel, grayLevel + 256);
	double maxValue = *max_element(grayLevel, grayLevel + 256);
	//cout << "Min: " << minValue << ", Max: " << maxValue << endl;

	Mat drawImg = Mat::zeros(Size(HISTGRAM_X_SIZE, HISTGRAM_Y_SIZE), CV_8UC3);
	for (int i = 0; i < 256; i++) {
		//遍历histImg
		int realValue = static_cast<int>(grayLevel[i] * HISTGRAM_Y_SIZE / maxValue);//归一化数据，缩放到图像的height之内
		rectangle(drawImg, Point(HISTGRAM_X_SCALE*i, HISTGRAM_Y_SIZE - 1 - realValue),
			Point(HISTGRAM_X_SCALE * i, HISTGRAM_Y_SIZE - 1), Scalar(255, 255, 255), -1);
	}
	imshow(windowName, drawImg);
}

// 通过3个相邻点进行平滑
int smoothHistgram(int *oldHist, double *newHist) {

	double histC[256] = { 0 }, histCC[256] = { 0 };
	for (int i = 0; i < 256; ++i) {
		histC[i] = oldHist[i];
		histCC[i] = oldHist[i];
	}

	int iter = 0;
	while (isTwoPolar(histCC) == false) {
		histCC[0] = (histC[0] + histC[0] + histC[1]) / 3;
		for (int i = 1; i < 255; ++i) {
			histCC[i] = (histC[i - 1] + histC[i] + histC[i + 1]) / 3;
		}
		histCC[255] = (histC[254] + histC[255] + histC[255]) / 3;

		for (int i = 0; i < 256; ++i)
			histC[i] = histCC[i];

		iter++;
		if (iter > 1000) {
			cout << "Cannot change to two polar.";
			return -1;
		}
	}

	for (int i = 0; i < 256; ++i)
		newHist[i] = static_cast<double>(histCC[i]);
	return 0;
}

bool isTwoPolar(double *hist) {
	int peakCount = 0;
	int cnt = 0;
	
	vector<int> peakValue;	//用于判断0这个点是否为峰值
	for (int i = 1; i < 256 - 1; ++i) {
		if (hist[i - 1] < hist[i] && hist[i + 1] < hist[i]) {	//注意这里是double类型所以不会出现相等的情况
			peakCount++;
			peakValue.push_back(hist[i]);
		}
	}

	int minValue = *min_element(peakValue.begin(), peakValue.end());
	if (hist[0] > minValue)
		peakCount++;

	if (peakCount == 2)
		return true;
	else
		return false;
}


uchar findLocalMinimum(double *hist) {
	//在两个峰值之间寻找最小值，从而对应灰度等级
	int p1 = 0, p2 = 0;
	for (int i = 1; i < 256 - 1; ++i) {
		if (hist[i - 1] < hist[i] && hist[i + 1] < hist[i]) {
			p1 = p2;
			p2 = i;
		}
	}
	cout << "Peak at: " << p1 << ", " << p2 << endl;
	auto minPosition = min_element(hist + p1, hist + p2);
	cout << "Min position at: " << (minPosition - hist) << endl;
	//int minValue = *minPosition;
	return minPosition - hist;
}