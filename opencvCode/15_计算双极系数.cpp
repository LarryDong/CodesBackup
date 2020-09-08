
/*
可以优化的工作：
1、beginPoint与endPoint这两个点没有存在的意义，可以用 图像起始点+图像尺寸Size代替。
2、分割的过程中实时通过this->originPoint和this->size确定新分割图像的相应值，同时保留索引。
3、搜索时可以按照索引值（连续）或者图像起始点（格式统一）寻找，但还未确定用何种形式
4、不会合并……

*/


#include <opencv.hpp>  
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

int calculatePolarity(const Mat src, Mat &dst);
void showHist(Mat hist, const string& windowName="默认直方图",Mat mask=Mat());
int OTSU(const Mat src);

void main(void) {
	Mat srcImg = imread("zebra3.jpg", 0);
	imshow("原图", srcImg);

	Mat tmpImg;
	calculatePolarity(srcImg, tmpImg);

	showHist(srcImg);


	//waitKey(0);
}

int calculatePolarity(const Mat src, Mat &dst) {

	int thresh = OTSU(src);	//获得分割阈值

	//亮和暗图像，以及相应掩码图
	Mat whitePart, whiteMask, blackPart, blackMask;

	threshold(src, whiteMask, thresh, 1, THRESH_BINARY);
	threshold(src, blackMask, thresh, 1, THRESH_BINARY_INV);
	src.copyTo(whitePart, whiteMask);
	src.copyTo(blackPart, blackMask);

	imshow("白像素图", whitePart);
	imshow("黑像素图", blackPart);
	
	/*//显示亮部和暗部的直方图
	showHist(src);
	showHist(whitePart,"白色直方图", whiteMask);
	showHist(blackPart, "黑色直方图",blackMask);
	*/
	
	//以下为双极系数gamma的计算
	Mat srcMean, srcSigma, whiteMean, whiteSigma, blackMean, blackSigma;
	double u0, u1, u2, sigma0, sigma1, sigma2, alpha, gamma;

	meanStdDev(src, srcMean, srcSigma, Mat());
	meanStdDev(whitePart, whiteMean, whiteSigma, whiteMask);
	meanStdDev(blackPart, blackMean, blackSigma, blackMask);

	sigma0 = srcSigma.at<double>(0);
	sigma1 = whiteSigma.at<double>(0);
	sigma2 = blackSigma.at<double>(0);
	u1 = whiteMean.at<double>(0);
	u2 = blackMean.at<double>(0);
	
	double n1 = 0, n2 = 0;
	for (int i = 0; i < src.rows; ++i) {
		for (int j = 0; j < src.cols; ++j) {
			if (src.at<uchar>(i, j) <= thresh)
				n1++;
			else
				n2++;
		}
	}
	alpha = n1 / (n1 + n2);
	gamma = 1 / (sigma0*sigma0)*alpha*(1 - alpha)*(u1 - u2)*(u1 - u2);

	cout << "该区域中双极系数gamma为：" << gamma << endl;
	
	return gamma;
}


#define HISTGRAM_X_SCALE 2
#define HISTGRAM_X_SIZE (256*HISTGRAM_X_SCALE)
#define HISTGRAM_Y_SIZE 400
void showHist(Mat src,const string& windowName,Mat mask) {
	int nimages = 1;//图像的个数
	int channels = 0;//需要统计通道的索引
	//Mat mask = Mat();		//因为新增掩码图所以去掉了这一段
	Mat histArray;//存放输出的直方图
	int dims = 1;//需要计算的直方图的维度
	int histSize = 256;//计算的直方图的分组数
	float range[] = { 0,256 };//表示直方图每一维度的取值范围
	const float* ranges[] = { range };//参数形式需要,表示每一维度数值的取值范围

	Mat histDrawing = Mat::zeros(Size(HISTGRAM_X_SIZE, HISTGRAM_Y_SIZE), CV_8UC1);
	calcHist(&src, nimages, &channels, mask, histArray, dims, &histSize, ranges);
	
	double minValue = 0;
	double maxValue = 0;
	minMaxLoc(histArray, &minValue, &maxValue);//得到计算出的直方图中的最小值和最大值

	for (int i = 0; i < histSize; i++) {
		//遍历histImg
		float binValue = histArray.at<float>(i);//得到histImg中每一分组的值
		float realValue = (binValue / maxValue)*HISTGRAM_Y_SIZE;//归一化数据，缩放到图像的height之内
		rectangle(histDrawing, Point(HISTGRAM_X_SCALE*i, HISTGRAM_Y_SIZE - 1 - realValue),
			Point(HISTGRAM_X_SCALE * i, HISTGRAM_Y_SIZE - 1), Scalar(255, 255, 255), -1);
	}

	imshow(windowName, histDrawing);
}

int OTSU(const Mat srcImg) {

	if (srcImg.type() != CV_8UC1)
		while (1)
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