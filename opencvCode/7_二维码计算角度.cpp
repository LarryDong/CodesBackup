
/*
确定二维码定位符位置

依次水平、垂直扫描，检测比例符合1:1:3:1:1的序列。
使用数组，将一行/列像素传递给findSegment函数，通过查找 白-黑 跳变点确定长度，判断比例是否合适，返回符合比例的"3"部分起止点。
最终显示水平与垂直扫描结果的掩码图，得到3个较为完整的区域。
通过开闭运算，消除小的相交区域，连接断开的定位区域
搜寻轮廓，获取每个轮廓的中点，再从中点出发向上下左右精准的找定位边界，获得新的精确的定位区域坐标
保证只有3个定位区域坐标，传给函数计算旋转角度。

注意：
1. 使用数组进行寻址速度大大快于使用vector：对于200x200左右图片，vector行扫描判断需要150ms，数组为10ms以下；
2. 运行时间：320x240: 20ms, 640x480: 80ms. 基本与图像面积成正比
3. 未考虑不是3个区域的情况。需根据实际情况进行分析，否则考虑问题太多


董岩 2018.9.6
*/

#include <opencv.hpp>
#include <iostream>
#include <vector>
#include <time.h>
#include <Windows.h>
#include <math.h>
using namespace std;
using namespace cv;

#define PIXEL_BLACK 0
#define PIXEL_WHITE 255

vector<Point> findSegment(int* pixelArray, int arraySize);
int findNext(int* input, int inputSize, int beginIndex, int value);
void QRCode_Process(Mat);
double calculateRotateAngle(Point, Point, Point);

Mat g_src = imread("QRCode\\6.jpg", 0);
Mat g_srcCopy = Mat::zeros(g_src.size(), CV_8UC1);

void onTrackbar(int, void*);
int g_tolarance = 50, g_openCloseSize = 2;

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
	namedWindow("openClose", WINDOW_FREERATIO);
	namedWindow("Result", WINDOW_FREERATIO);

	createTrackbar("tor: ", "CommonMask", &g_tolarance, 100, onTrackbar);
	createTrackbar("size: ", "openClose", &g_openCloseSize, 100, onTrackbar);
	onTrackbar(0, 0);

	imshow("Binary", binImg);
	QRCode_Process(binImg);

	waitKey(0);
}

void QRCode_Process(Mat src) {

	LARGE_INTEGER nFreq, t1, t2;
	double p1;
	//QueryPerformanceFrequency(&nFreq);
	//QueryPerformanceCounter(&t1);

	g_src.copyTo(g_srcCopy);
	Mat horMat = Mat::zeros(src.size(), CV_8UC1);
	Mat verMat = Mat::zeros(src.size(), CV_8UC1);
	Mat commonMask = Mat::zeros(src.size(), CV_8UC1);

	//水平扫描，寻找比例点
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
	//垂直扫描，寻找比例点。
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

	//开运算消除小白点，闭运算消除被断开的定位区域
	int size = g_openCloseSize * 2 + 1;
	Mat element = getStructuringElement(MORPH_RECT, Size(size, size));
	morphologyEx(commonMask, commonMask, MORPH_CLOSE, element);
	morphologyEx(commonMask, commonMask, MORPH_OPEN, element);
	imshow("openClose", commonMask);

	//寻找剩下的轮廓区域
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	Mat contourImg = Mat::zeros(commonMask.size(), CV_8UC3);
	findContours(commonMask, contours, hierarchy, CV_RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < contours.size(); ++i) 
		drawContours(contourImg, contours, i, Scalar(0, 0, 255));

	imshow("Contours", contourImg);

	//轮廓的“中心点”
	vector<Point> contourMiddle;
	for (int i = 0; i < contours.size(); ++i) {
		int up = 9999, down = 0, left = 9999, right = 0;
		for (int j = 0; j < contours[i].size(); ++j) {
			Point p = contours[i][j];
			if (p.y < up)
				up = p.y;
			if (p.y > down)
				down = p.y;
			if (p.x < left)
				left = p.x;
			if (p.x > right)
				right = p.x;
		}
		contourMiddle.push_back(Point((left + right) / 2, (up + down) / 2));
	}

	Mat srcCopy = Mat::zeros(src.size(), CV_8UC1);
	src.copyTo(srcCopy);
	cvtColor(srcCopy, srcCopy, CV_GRAY2BGR);
	for (int i = 0; i < contourMiddle.size(); ++i)
		circle(srcCopy, contourMiddle[i], 1, Scalar(255, 0, 0), CV_FILLED);

	//根据中心点，向四个方向搜寻精准的边界线，重新找中心点
	vector<Point> accurateMiddlePoint;
	for (int i = 0; i < contourMiddle.size(); ++i) {
		Point p = contourMiddle[i];
		int up = 0, down = 0, left = 0, right = 0;
		for (int j = p.y; j >= 0; --j) {
			if (src.at<uchar>(j, p.x) == PIXEL_WHITE) {
				up = j;
				circle(srcCopy, Point(p.x, j), 1, Scalar(0, 255, 0), CV_FILLED);	//画一下找到的定位边界
				break;
			}
		}
		for (int j = p.y; j < src.rows; ++j) {
			if (src.at<uchar>(j, p.x) == PIXEL_WHITE) {
				down = j;
				circle(srcCopy, Point(p.x, j), 1, Scalar(0, 255, 0), CV_FILLED);
				break;
			}
		}
		for (int j = p.x; j > 0; --j) {
			if (src.at<uchar>(p.y, j) == PIXEL_WHITE) {
				left = j;
				circle(srcCopy, Point(j, p.y), 1, Scalar(0, 255, 0), CV_FILLED);
				break;
			}
		}

		for (int j = p.x; j < src.cols; ++j) {
			if (src.at<uchar>(p.y, j) == PIXEL_WHITE) {
				right = j;
				circle(srcCopy, Point(j, p.y), 1, Scalar(0, 255, 0), CV_FILLED);
				break;
			}
		}
		accurateMiddlePoint.push_back(Point((left + right) / 2, (up + down) / 2));
	}

	//画一下新的精准定位坐标点
	for (int i = 0; i < accurateMiddlePoint.size(); ++i) 
		circle(srcCopy, accurateMiddlePoint[i], 3, Scalar(0, 0, 255), CV_FILLED);

	imshow("Result", srcCopy);

	double angle = 0;

	if (accurateMiddlePoint.size() != 3) {
		// 未考虑非3情况
		cout << "No QR code found." << endl;
	}
	else {
		angle = calculateRotateAngle(accurateMiddlePoint[0], accurateMiddlePoint[1], accurateMiddlePoint[2]);
	}

	//QueryPerformanceCounter(&t2);
	//p1 = (t2.QuadPart - t1.QuadPart) / (double)nFreq.QuadPart * 1000;
	//cout << "Time used: " << p1 << " ms." << endl;
	//cout << "对于第 " << g_number << "张图，求得旋转角度为：" << angle << "，用时 " << p1 << " ms." << endl;
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

double calculateRotateAngle(Point p1, Point p2, Point p3) {

	//// 由于图像坐标系左上角为原点，为便于笛卡尔坐标系计算旋转，直接将y取反，坐标虽不同但不影响向量
	//p1.y *= -1;
	//p2.y *= -1;
	//p3.y *= -1;

	// 通过模长，确定等腰直角三角形的直角。最长的模长对应的两个点即锐角
	double mod12 = sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
	double mod23 = sqrt((p3.x - p2.x)*(p3.x - p2.x) + (p3.y - p2.y)*(p3.y - p2.y));
	double mod31 = sqrt((p1.x - p3.x)*(p1.x - p3.x) + (p1.y - p3.y)*(p1.y - p3.y));

	Point rightAngle;
	Point2f featureVector;		//“特征向量”为直角点到两条边构成的平行四边形的对角线。如此定义可保证角度

	if (mod12 > mod23 && mod12 > mod31) {
		rightAngle = p3;
		featureVector.x = (p2.x - p3.x) + (p1.x - p3.x);
		featureVector.y = (p2.y - p3.y) + (p1.y - p3.y);
	}
	if (mod23 > mod31 && mod23 > mod12) {
		rightAngle = p1;
		featureVector.x = (p2.x - p1.x) + (p3.x - p1.x);
		featureVector.y = (p2.y - p1.y) + (p3.y - p1.y);
	}

	if (mod31 > mod12 && mod31 > mod23) {
		rightAngle = p2;
		featureVector.x = (p1.x - p2.x) + (p3.x - p2.x);
		featureVector.y = (p1.y - p2.y) + (p3.y - p2.y);
	}

	//由于图像坐标系定义问题，根据实际情况求得旋转角度
	double featureAngle = atan2(featureVector.y, featureVector.x) * 180 / CV_PI;
	double rotateAngle = - featureAngle + 45;

	return rotateAngle;
}

void onTrackbar(int, void*) {
	imshow("SRC", g_src);
	Mat binImg;
	Mat src;
	g_src.copyTo(src);
	threshold(src, binImg, 255, 255, THRESH_OTSU);
	imshow("Binary", binImg);
	//cout << "G_TORLARNCE: " << g_tolarance;
	QRCode_Process(binImg);
}
