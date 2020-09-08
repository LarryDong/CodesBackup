
/*
确定二维码定位符位置

依次水平、垂直扫描，检测比例符合1:1:3:1:1的序列。
使用数组，将一行/列像素传递给findSegment函数，通过查找 白-黑 跳变点确定长度，判断比例是否合适，返回符合比例的"3"部分起止点。
最终显示水平与垂直扫描结果的掩码图，得到3个较为完整的区域。

注意：
1. 使用数组进行寻址速度大大快于使用vector：对于200x200左右图片，vector行扫描判断需要150ms，数组为10ms以下；
2. 运行速度依旧不理想。在800x800左右图片，单次扫描用时约80ms

3. BTW，发现抗干扰能力还是挺强的。毕竟1:1:3:1:1比例非常特殊。

董岩 2018.9.5
*/

#include <opencv.hpp>
#include <iostream>
#include <vector>
#include <time.h>
#include <Windows.h>

using namespace std;
using namespace cv;

#define PIXEL_BLACK 0
#define PIXEL_WHITE 255

vector<Point> findSegment(int* pixelArray, int arraySize);
int findNext(int* input, int inputSize, int beginIndex, int value);
void doSomething(Mat);

Mat g_src = imread("QRCode\\6.jpg", 0);
Mat g_srcCopy = Mat::zeros(g_src.size(), CV_8UC1);

void onTrackbar(int, void*);
int g_tolarance = 10;

void main(void) {

	g_src.copyTo(g_srcCopy);
	cvtColor(g_srcCopy, g_srcCopy, CV_GRAY2BGR);
	Mat src = g_src;
	Mat binImg;
	threshold(src, binImg, 255, 255, THRESH_OTSU);
	namedWindow("Binary", WINDOW_FREERATIO);
	namedWindow("horMark", WINDOW_FREERATIO);
	namedWindow("verMark", WINDOW_FREERATIO);
	namedWindow("CommonMask", WINDOW_FREERATIO);

	createTrackbar("tor: ", "CommonMask", &g_tolarance, 100, onTrackbar);
	imshow("Binary", binImg);
	doSomething(binImg);

	waitKey(0);
}

void doSomething(Mat src) {

	LARGE_INTEGER nFreq, t1, t2, t3, t4, t5;
	double p1, p2, p3, p4;
	QueryPerformanceFrequency(&nFreq);

	QueryPerformanceCounter(&t1);

	g_src.copyTo(g_srcCopy);
	Mat horMat = Mat::zeros(src.size(), CV_8UC1);
	Mat verMat = Mat::zeros(src.size(), CV_8UC1);
	Mat commonMask = Mat::zeros(src.size(), CV_8UC1);

	for (int i = 0; i < src.rows; ++i) {
		int horPixels[10000] = { 0 };
		for (int j = 0; j < src.cols; ++j)
			horPixels[j] = src.at<uchar>(i, j);
		vector<Point> vp = findSegment(horPixels, src.cols);
		for (int k = 0; k < vp.size(); ++k) {
			int p1 = vp[k].x, p2 = vp[k].y;
			line(horMat, Point(p1, i), Point(p2, i), 255);
		}
	}
	for (int i = 0; i < src.cols; ++i) {
		int verPixels[10000] = { 0 };
		for (int j = 0; j < src.rows; ++j)
			verPixels[j] = src.at<uchar>(j, i);
		vector<Point> vp = findSegment(verPixels, src.rows);
		for (int k = 0; k < vp.size(); ++k) {
			int p1 = vp[k].x, p2 = vp[k].y;
			line(verMat, Point(i, p1), Point(i, p2), 255);
		}
	}


	commonMask = horMat & verMat;
	imshow("horMark", horMat);
	imshow("verMark", verMat);
	imshow("CommonMask", commonMask);

	QueryPerformanceCounter(&t2);
	p1 = (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000;

	cout << "Time used: " << p1 << " ms." << endl;

}




vector<Point> findSegment(int* pixelVector, int size) {
	vector<Point> lineSegment;
	for (int checkIndex = 0; checkIndex < size;) {	//checkIndex每次从一个白-黑跳变点开始
		int white2BlackPoint = checkIndex;
		for (int i = checkIndex; i < size; ++i) {
			if (pixelVector[i] == PIXEL_WHITE && pixelVector[i + 1] == PIXEL_BLACK) {
				white2BlackPoint = i + 1;
				break;
			}
			//加上这一行以确定本次查询没有找到后续点
			if (i == size - 2)
				//cout << "No PIXEL_WHITE-PIXEL_BLACK jump any more now..." << endl;
				return lineSegment;
		}

		if (white2BlackPoint == 0)	//遍历完均没有找到一个跳变点，则返回-1
			return lineSegment;

		checkIndex = white2BlackPoint;	//记录当前跳变点（跳变后），作为下次搜寻时的起始点
		int compareIndex = white2BlackPoint;	//从跳变点开始比较黑白像素比

		int x[5] = { 0 };
		int lineBegin = 0, lineEnd = 0;
		x[0] = findNext(pixelVector, size, compareIndex, PIXEL_WHITE);
		if (x[0] == -1)
			return lineSegment;
		compareIndex += x[0];

		x[1] = findNext(pixelVector, size, compareIndex, PIXEL_BLACK);
		if (x[1] == -1)
			return lineSegment;
		compareIndex += x[1];
		lineBegin = compareIndex;

		x[2] = findNext(pixelVector, size, compareIndex, PIXEL_WHITE);
		if (x[2] == -1)
			return lineSegment;
		compareIndex += x[2];
		lineEnd = compareIndex - 1;

		x[3] = findNext(pixelVector, size, compareIndex, PIXEL_BLACK);
		if (x[3] == -1)
			return lineSegment;
		compareIndex += x[3];

		x[4] = findNext(pixelVector, size, compareIndex, PIXEL_WHITE);
		if (x[4] == -1)
			return lineSegment;

		float y[5] = { x[0],x[1],x[2],x[3],x[4] };
		float ratio1_lower = 1 - g_tolarance * 0.01;
		float ratio1_upper = 1 + g_tolarance * 0.01;
		float ratio3_lower = 3 - 3 * g_tolarance * 0.01;
		float ratio3_upper = 3 + 3 * g_tolarance * 0.01;

		if ((y[0] / y[1] > ratio1_lower && y[0] / y[1] < ratio1_upper)
			&& (y[0] / y[3] > ratio1_lower && y[0] / y[3] < ratio1_upper)
			&& (y[0] / y[4] > ratio1_lower && y[0] / y[4] < ratio1_upper)
			&& (y[2] / y[0] > ratio3_lower && y[2] / y[0] < ratio3_upper)) {
			//cout << "1: " << ratio1_lower << ", " << ratio1_upper << ". 3:" << ratio3_lower << ", " << ratio3_upper << endl;
			lineSegment.push_back(Point(lineBegin, lineEnd));
		}
	}
	return lineSegment;
}


int findNext(int* input, int size, int beginIndex, int value) {

	if (beginIndex >= size)
		return -1;

	int width = 0;
	int pixelValue = input[beginIndex];

	while (pixelValue != value) {
		width++;
		if (beginIndex + width >= size) 
			return -1;
		pixelValue = input[beginIndex + width];
	}
	return width;
}


void onTrackbar(int, void*) {
	imshow("SRC", g_src);
	Mat binImg;
	Mat src;
	g_src.copyTo(src);
	threshold(src, binImg, 255, 255, THRESH_OTSU);
	imshow("Binary", binImg);
	//cout << "G_TORLARNCE: " << g_tolarance;
	doSomething(binImg);
}
