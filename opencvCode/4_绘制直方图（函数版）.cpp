

#include <opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void eqThreeChannel(Mat src, Mat& output);
void eqOneChannel(Mat src, Mat& output);
void showHist(Mat hist, int histSize, Mat& histgram, const string& windowName);

#define HISTGRAM_X_SCALE 2
#define HISTGRAM_X_SIZE (256*HISTGRAM_X_SCALE)
#define HISTGRAM_Y_SIZE 400

int main() {

	Mat srcImg = imread("baboon.jpg", 0);
	Mat eqImg;

	int nimages = 1;//图像的个数
	int channels = 0;//需要统计通道的索引
	Mat mask = Mat();
	Mat histArray;//存放输出的直方图
	int dims = 1;//需要计算的直方图的维度
	int histSize = 256;//计算的直方图的分组数
	float range[] = { 0,256 };//表示直方图每一维度的取值范围
	const float* ranges[] = { range };//参数形式需要,表示每一维度数值的取值范围

	Mat histDrawing = Mat::zeros(Size(HISTGRAM_X_SIZE, HISTGRAM_Y_SIZE), CV_8UC1);

	calcHist(&srcImg, nimages, &channels, mask, histArray, dims, &histSize, ranges);
	showHist(histArray, histSize, histDrawing, "原图直方图分布");


	equalizeHist(srcImg, eqImg);
	calcHist(&eqImg, nimages, &channels, mask, histArray, dims, &histSize, ranges);
	showHist(histArray, histSize, histDrawing, "均衡化直方图分布");

	waitKey(0);
	return 0;
}

void showHist(Mat hist, int histSize, Mat& histgram, const string& windowName) {

	double minValue = 0;
	double maxValue = 0;
	minMaxLoc(hist, &minValue, &maxValue);//得到计算出的直方图中的最小值和最大值

	for (int i = 0; i < histSize; i++) {
		//遍历histImg
		float binValue = hist.at<float>(i);//得到histImg中每一分组的值
		float realValue = (binValue / maxValue)*HISTGRAM_Y_SIZE;//归一化数据，缩放到图像的height之内
		rectangle(histgram, Point(HISTGRAM_X_SCALE*i, HISTGRAM_Y_SIZE - 1 - realValue),
			Point(HISTGRAM_X_SCALE * i, HISTGRAM_Y_SIZE - 1), Scalar(255, 255, 255), -1);
	}

	imshow(windowName, histgram);
}
