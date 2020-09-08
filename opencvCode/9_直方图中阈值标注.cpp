
/*
绘制分割阈值到直方图中
*/
#include <opencv.hpp>  
#include <iostream>

using namespace cv;
using namespace std;



int OTSU(const Mat);
//绘制直方图函数，我很机智的使用了重载，可以改名字，也可以加入阈值高亮显示分割值
void showHist(const Mat, const string windowName="直方图", const int thresh=-1);

int main(void) {

	Mat srcImg = imread("盲道1.jpg", 0);

	imshow("原图", srcImg);
	int thresh = OTSU(srcImg);

	Mat dst1, dst2;
	threshold(srcImg, dst1, thresh, 255, THRESH_BINARY);
	threshold(srcImg, dst2, thresh, 255, THRESH_OTSU);

	cout << "使用OTSU法计算的分割阈值为：" << thresh << endl;
	imshow("手写函数", dst1);
	imshow("OTSU函数", dst2);

	namedWindow("Histgram");
	showHist(srcImg,"Histgram",thresh);

	waitKey(0);
	return 0;
}

int OTSU(const Mat srcImg) {
	
	if (srcImg.type() != CV_8UC1) 
		while(1)
			cerr << "请输入8位灰度图像！" << endl;

	int T = 0;
	int totalNumber = srcImg.rows*srcImg.cols;
	double varValue = 0;
	double w0 = 0, w1 = 0, u0 = 0, u1 = 0;
	vector<int> grayLevel(256);

	//累加灰度等级
	for (int i = 0; i<srcImg.rows; i++)
		for (int j = 0; j<srcImg.cols; j++)
			grayLevel[srcImg.at<uchar>(i*srcImg.cols + j)]++;

	for (int i = 0; i < 255; ++i) {
		w1 = 0; w0 = 0; u0 = 0; u1 = 0;

		//计算背景相关
		for (int j = 0; j <= i; ++j) {
			w1 += grayLevel[j];
			u1 += j*grayLevel[j];
		}
		if (w1 == 0)
			continue;		//避免除数为0，但还要继续进行
		u1 = u1 / w1;
		w1 = w1 / totalNumber;

		//计算前景相关
		for (int j = i + 1; j <= 255; ++j) {
			w0 += grayLevel[j];
			u0 += j*grayLevel[j];
		}
		if (w0 == 0)
			continue;
		u0 = u0 / w0;
		w0 = w0 / totalNumber;

		double varValueNew = w0*w1*(u1 - u0)*(u1 - u0);
		if (varValueNew > varValue) {
			varValue = varValueNew;
			T = i;
		}
	}

	return T;
}

#define HISTGRAM_X_SCALE 2
#define HISTGRAM_X_SIZE (256*HISTGRAM_X_SCALE)
#define HISTGRAM_Y_SIZE 400

void showHist(const Mat src, const string windowName ,const int thresh) {

	if (src.type() != CV_8UC1)
		while (1)
			cerr << "请输入8位灰度图像！" << endl;

	int nimages = 1;//图像的个数
	int channels = 0;//需要统计通道的索引
	Mat mask = Mat();
	Mat histArray;//存放输出的直方图
	int dims = 1;//需要计算的直方图的维度
	int histSize = 256;//计算的直方图的分组数
	float range[] = { 0,256 };//表示直方图每一维度的取值范围
	const float* ranges[] = { range };//参数形式需要,表示每一维度数值的取值范围

	calcHist(&src, nimages, &channels, mask, histArray, dims, &histSize, ranges);

	double minValue = 0;
	double maxValue = 0;
	minMaxLoc(histArray, &minValue, &maxValue);//得到计算出的直方图中的最小值和最大值
	

	//namedWindow(windowName);
	Mat histDrawing = Mat::zeros(Size(HISTGRAM_X_SIZE, HISTGRAM_Y_SIZE), CV_8UC3);
	for (int i = 0; i < histSize; i++) {
		//遍历histImg
		float binValue = histArray.at<float>(i);//得到histImg中每一分组的值
		float realValue = (binValue / maxValue)*HISTGRAM_Y_SIZE;//归一化数据，缩放到图像的height之内
		rectangle(histDrawing, Point(HISTGRAM_X_SCALE*i, HISTGRAM_Y_SIZE  - realValue),
			Point(HISTGRAM_X_SCALE * i, HISTGRAM_Y_SIZE ), Scalar(255, 255, 255), -1);
	}

	if (thresh != -1)	//当输入阈值时，绘制阈值线
		rectangle(histDrawing, Point(HISTGRAM_X_SCALE*thresh, HISTGRAM_Y_SIZE ),
			Point(HISTGRAM_X_SCALE * thresh, 1), Scalar(0, 0, 255), -1);

	imshow(windowName, histDrawing);
}


