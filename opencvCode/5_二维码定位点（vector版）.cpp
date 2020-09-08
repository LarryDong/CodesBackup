
#include <opencv.hpp>
#include <iostream>
#include <vector>
#include <time.h>
#include <Windows.h>

using namespace std;
using namespace cv;

int count = 0;

LARGE_INTEGER nFreq, moment1, moment2, moment3, moment4;
double period1 = 0, period2 = 0, period3 = 0;

#define PIXEL_BLACK 0
#define PIXEL_WHITE 255

#define SCAN_HOR 0
#define SCAN_VER 1

vector<Point> findSegment(vector<int> pixelVector);
int findNext(vector<int> input, int beginIndex, int value);
void doSomething(Mat);


//Mat g_src = imread("QRCode\\code_rotate1.jpg", 0);
Mat g_src = imread("QRCode\\code_rotate2.jpg", 0);
Mat g_srcCopy = Mat::zeros(g_src.size(), CV_8UC1);

void onTrackbar(int, void*);
int g_rows = 0;
int g_tolarance = 10;

void main(void) {
/*
	LARGE_INTEGER nFreq, t1, t2;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&t1);
	Sleep(1005);
	QueryPerformanceCounter(&t2);
	cout << (t2.QuadPart - t1.QuadPart)/(double)nFreq.QuadPart << "+++++++" << endl;*/

	QueryPerformanceFrequency(&nFreq);

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

	g_src.copyTo(g_srcCopy);
	Mat horMat = Mat::zeros(src.size(), CV_8UC1);
	Mat verMat = Mat::zeros(src.size(), CV_8UC1);
	Mat commonMask = Mat::zeros(src.size(), CV_8UC1);

	QueryPerformanceFrequency(&nFreq);
	
	for (int i = 0; i < src.rows; ++i) {
		vector<int> horPixels;

		//QueryPerformanceCounter(&t1);

		for (int j = 0; j < src.cols; ++j) {
			horPixels.push_back(src.at<uchar>(i, j));
		}

		QueryPerformanceCounter(&moment1);
		vector<Point> vp = findSegment(horPixels);
		QueryPerformanceCounter(&moment2);

		period1 += (moment2.QuadPart - moment1.QuadPart) / (double)nFreq.QuadPart * 1000000;
		//period2 += (moment3.QuadPart - moment2.QuadPart) / (double)nFreq.QuadPart * 1000000;
		//cout << "Time used once: " << period1 / 1000 << " ms." << endl;

		//QueryPerformanceCounter(&t2);
		//time1 += (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000000;

		for (int k = 0; k < vp.size(); ++k) {
			int p1 = vp[k].x, p2 = vp[k].y;
			line(verMat, Point(p1, i), Point(p2, i), 255);
		}

		//QueryPerformanceCounter(&t3);
		//time2 += (t3.QuadPart - t2.QuadPart) / (double)nFreq.QuadPart * 1000000;
	}
	cout << "Time used 1: " << period1 / 1000 << " ms." << endl;
	//cout << "Part1: " << time1/1000 << ", Part2: " << time2/1000 << endl;

	for (int i = 0; i < src.cols; ++i) {
		vector<int> verPixels;
		for (int j = 0; j < src.rows; ++j)
			verPixels.push_back(src.at<uchar>(j, i));
		vector<Point> vp = findSegment(verPixels);
		for (int k = 0; k < vp.size(); ++k) {
			int p1 = vp[k].x, p2 = vp[k].y;
			line(horMat, Point(i, p1), Point(i, p2), 255);
		}
	}

	commonMask = horMat & verMat;
	imshow("horMark", horMat);
	imshow("verMark", verMat);
	imshow("CommonMask", commonMask);
	//imwrite("Mark.jpg", g_srcCopy);

}




vector<Point> findSegment(vector<int> pixelVector) {
	vector<Point> lineSegment;
	for (int checkIndex = 0; checkIndex < pixelVector.size();) {	//checkIndex每次从一个白-黑跳变点开始
		int white2BlackPoint = checkIndex;
		for (int i = checkIndex; i < pixelVector.size() - 1; ++i) {
			if (pixelVector[i] == PIXEL_WHITE && pixelVector[i + 1] == PIXEL_BLACK) {
				white2BlackPoint = i + 1;
				break;
			}
			//加上这一行以确定本次查询没有找到后续点
			if (i == pixelVector.size() - 2)
				//cout << "No PIXEL_WHITE-PIXEL_BLACK jump any more now..." << endl;
				return lineSegment;
		}

		if (white2BlackPoint == 0)	//遍历完均没有找到一个跳变点，则返回-1
			return lineSegment;

		checkIndex = white2BlackPoint;	//记录当前跳变点（跳变后），作为下次搜寻时的起始点
		int compareIndex = white2BlackPoint;	//从跳变点开始比较黑白像素比

		int x[5] = { 0 };
		int lineBegin = 0, lineEnd = 0;
		x[0] = findNext(pixelVector, compareIndex, PIXEL_WHITE);
		if (x[0] == -1) 
			return lineSegment;
		compareIndex += x[0];

		x[1] = findNext(pixelVector, compareIndex, PIXEL_BLACK);
		if (x[1] == -1)
			return lineSegment;
		compareIndex += x[1];
		lineBegin = compareIndex;

		x[2] = findNext(pixelVector, compareIndex, PIXEL_WHITE);
		if (x[2] == -1)
			return lineSegment;
		compareIndex += x[2];
		lineEnd = compareIndex - 1;

		x[3] = findNext(pixelVector, compareIndex, PIXEL_BLACK);
		if (x[3] == -1)
			return lineSegment;
		compareIndex += x[3];

		x[4] = findNext(pixelVector, compareIndex, PIXEL_WHITE);
		if (x[4] == -1)
			return lineSegment;

		float y[5] = { x[0],x[1],x[2],x[3],x[4] };
		float ratio1_lower = 1 - g_tolarance * 0.01;
		float ratio1_upper = 1 + g_tolarance * 0.01;
		float ratio3_lower = 3 - 3 * g_tolarance * 0.01;
		float ratio3_upper = 3 + 3 * g_tolarance * 0.01;
		//
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


int findNext(vector<int> input, int beginIndex, int value){

	if (beginIndex >= input.size())
		return -1;

	int width = 0;
	int pixelValue = input[beginIndex];
	
	while (pixelValue != value) {
		width++;
		if (beginIndex + width >= input.size()) {
			return -1;
		}
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
