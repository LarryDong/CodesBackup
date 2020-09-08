
/*
一些思路：
1、斑马线区域二值化时，仅针对斑马线区域进行二值化，而非全部整张图像。
2、canny计算前，先将图像二值化，已获得高对比度区域，这样对Canny的阈值选择要求较低
*/

#include <opencv.hpp>  
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;
int OTSU(const Mat srcImg);
int OTSU_mask(const Mat srcImg, Mat mask);
void drawBoundary(const Mat src, Mat boundary);




void main() {

	Mat src = imread("zebra.jpg", 0);
	GaussianBlur(src, src, Size(5,5), 0);

	Mat mask = imread("zebra-mask.jpg", 0);
	int thresh;
	Mat binImg,boundaryImg,boundaryROI;
	thresh = OTSU_mask(src,mask);
	threshold(src, binImg, thresh, 255, THRESH_BINARY);

	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(binImg, binImg, MORPH_OPEN, element);
	morphologyEx(binImg, binImg, MORPH_CLOSE, element);
	Canny(binImg, boundaryImg, 3, 9, 3);
	
	boundaryImg.copyTo(boundaryROI,mask);
	drawBoundary(src, boundaryROI);
	
	waitKey(0);
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

//掩码区域计算阈值
int OTSU_mask(const Mat srcImg,Mat mask) {

	if (srcImg.type() != CV_8UC1)
		while (1)
			cerr << "请输入8位灰度图像！" << endl;

	int T = 0;

	//int totalNumber = srcImg.rows*srcImg.cols;
	int totalNumber = 0;
	double varValue = 0;
	double w0 = 0, w1 = 0, u0 = 0, u1 = 0;
	vector<int> grayLevel(256);

	//累加灰度等级
	for (int i = 0; i < srcImg.rows; ++i) {
		for (int j = 0; j < srcImg.cols; ++j) {
			if (mask.at<uchar>(i, j) != 0) {		//仅计算Mask区域
				grayLevel[srcImg.at<uchar>(i, j)]++;
				totalNumber++;
			}
		}
	}

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

void drawBoundary(const Mat src, Mat boundary) {

	//放宽边界
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(boundary, boundary, element);
	Scalar color = Scalar(0, 0, 255);

	Mat srcCopy(src);
	if (srcCopy.type() != CV_8UC3)	//转为彩色
		cvtColor(srcCopy,srcCopy, CV_GRAY2BGR);

	//如果图像对应边界点，则染成红色
	for (int i = 0; i < src.rows; ++i) {
		for (int j = 0; j < src.cols; ++j) {
			if (boundary.at<uchar>(i, j)!= 0) {
				srcCopy.at<Vec3b>(i, j)[0] = 0;
				srcCopy.at<Vec3b>(i, j)[1] = 0;
				srcCopy.at<Vec3b>(i, j)[2] = 255;
			}
		}
	}
	imshow("斑马线标注图", srcCopy);

}

