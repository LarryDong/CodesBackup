

#include <opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void eqThreeChannel(Mat src, Mat& output);
void eqOneChannel(Mat src, Mat& output);
void showHist(Mat hist, int histSize, const string& windowName, int color);

#define HISTGRAM_X_SCALE 2
#define HISTGRAM_X_SIZE (256*HISTGRAM_X_SCALE)
#define HISTGRAM_Y_SIZE 400

int main() {

	Mat srcImg = imread("cctv.jpg", 1);
	Mat eqImg;

	vector<Mat> src_channels(3);
	vector<Mat> dst_channels(3);

	split(srcImg, src_channels);

	int nimages = 1;//图像的个数
	int channels = 0;//需要统计通道的索引
	Mat mask = Mat();
	Mat histArray;//存放输出的直方图
	int dims = 1;//需要计算的直方图的维度
	int histSize = 256;//计算的直方图的分组数
	float range[] = { 0,256 };//表示直方图每一维度的取值范围
	const float* ranges[] = { range };//参数形式需要,表示每一维度数值的取值范围

	calcHist(&src_channels.at(0), nimages, &channels, mask, histArray, dims, &histSize, ranges);
	showHist(histArray, histSize, "蓝色分布直方图", 0);

	calcHist(&src_channels.at(1), nimages, &channels, mask, histArray, dims, &histSize, ranges);
	showHist(histArray, histSize, "绿色分布直方图", 1);

	calcHist(&src_channels.at(2), nimages, &channels, mask, histArray, dims, &histSize, ranges);
	showHist(histArray, histSize, "红色分布直方图", 2);


	equalizeHist(src_channels.at(0), dst_channels.at(0));
	calcHist(&dst_channels.at(0), nimages, &channels, mask, histArray, dims, &histSize, ranges);
	showHist(histArray, histSize, "均衡化蓝色直方图分布", 0);

	equalizeHist(src_channels.at(1), dst_channels.at(1));
	calcHist(&dst_channels.at(1), nimages, &channels, mask, histArray, dims, &histSize, ranges);
	showHist(histArray, histSize, "均衡化绿色直方图分布", 1);

	equalizeHist(src_channels.at(2), dst_channels.at(2));
	calcHist(&dst_channels.at(2), nimages, &channels, mask, histArray, dims, &histSize, ranges);
	showHist(histArray, histSize, "均衡化红色直方图分布", 2);

	merge(dst_channels, eqImg);
	imshow("原图", srcImg);
	imshow("3通道均衡", eqImg);

	waitKey(0);
	return 0;
}

void showHist(Mat hist, int histSize, const string& windowName, int color) {

	Mat histDrawing = Mat::zeros(Size(HISTGRAM_X_SIZE, HISTGRAM_Y_SIZE), CV_8UC3);
	double minValue = 0;
	double maxValue = 0;
	minMaxLoc(hist, &minValue, &maxValue);//得到计算出的直方图中的最小值和最大值
	
	Scalar channelColor;
	channelColor = (color == 0 ? Scalar(255, 0, 0) : (color == 1 ? Scalar(0, 255, 0) : Scalar(0, 0, 255)));


	for (int i = 0; i < histSize; i++) {
		//遍历histImg
		float binValue = hist.at<float>(i);//得到histImg中每一分组的值
		float realValue = (binValue / maxValue)*HISTGRAM_Y_SIZE;//归一化数据，缩放到图像的height之内
		rectangle(histDrawing, Point(HISTGRAM_X_SCALE*i, HISTGRAM_Y_SIZE - 1 - realValue),
			Point(HISTGRAM_X_SCALE * i + HISTGRAM_X_SCALE / 2, HISTGRAM_Y_SIZE - 1), channelColor, -1);
	}

	imshow(windowName, histDrawing);
}
