
#include <opencv.hpp>
#include <iostream>
//#include <highgui.hpp>
#include <Windows.h>

using namespace std;
using namespace cv;

#define FILE_PATH "result\\"

void doSomething(Mat src);
void calculateRealPoint(vector<vector<Point3f>>& objectPoints, int boradWidth, int boardHeight, int imgNumber, int squareSize);
Mat drawHoughSapce(Mat binImg, int spaceWidth, int spaceHeight);
Mat drawLines(Mat binImg);
void onTrackbar(int, void*);

int g_minLengh = 10, g_minGap = 10, g_rho = 1, g_lineThresh = 15;
double g_theta = CV_PI / 180;
int g_adaptiveSize = 5,g_const=5;
Mat g_src;

void main() {
	int counter = 0;
	namedWindow("src");
	namedWindow("canny");
	namedWindow("bin");
	namedWindow("lines");
	createTrackbar("minLen", "lines", &g_minLengh, 100, onTrackbar);
	createTrackbar("minGap", "lines", &g_minGap, 100, onTrackbar);
	createTrackbar("th*10", "lines", &g_lineThresh, 100, onTrackbar);
	//createTrackbar("minLen", "lines", &g_minLengh, 100, onTrackbar);
	createTrackbar("size*10", "bin", &g_adaptiveSize, 100, onTrackbar);
	createTrackbar("const", "bin", &g_const, 100, onTrackbar);
	int c = 0;
	while (c != 'q') {
		g_src = imread(FILE_PATH + to_string(counter++) + ".jpg", 0);
		if (g_src.empty()) {
			cout << "end of photos..." << endl;
			counter = 0;
			continue;
		}

		doSomething(g_src);
		c = waitKey(0);
	}
	
}

//计算实际点的坐标。规定Z=0，将通过正方形边长计算后的x与y放到目标点中，用于后续计算
void calculateRealPoint(vector<vector<Point3f>>& objectPoints, int boardWidth, int boardHeight, int imgNumber, int squareSize) {
	vector<Point3f> imagePoints;
	for (int i = 0; i < boardHeight; ++i) {
		for (int j = 0; j < boardWidth; ++j) {
			imagePoints.push_back(Point3f(i*squareSize, j*squareSize, 0));
		}
	}
	for (int i = 0; i < imgNumber; ++i) {
		objectPoints.push_back(imagePoints);
	}
}


Mat drawHoughSapce(Mat src, int spaceWidth, int spaceHeight) {

	vector<Point> pixels;
	for (int i = 0; i < src.rows; ++i) {
		for (int j = 0; j < src.cols; ++j) {
			if (src.data[i*src.cols + j] > 128)
				pixels.push_back(Point(j, i));	//record while pixels
		}
	}

	Mat houghImg = Mat::zeros(Size(spaceWidth, spaceHeight), CV_16UC1);	//16U以避免累加时超过255
	float rhoMax = sqrt((src.rows*src.rows) + (src.cols*src.cols));	//对角线长为最大值，用于归一化
	for (int i = 0; i < pixels.size(); ++i) {
		int x = pixels[i].x;
		int y = pixels[i].y;
		// 遍历Hough空间进行绘图
		for (int thetaUnit = 0; thetaUnit < spaceWidth; ++thetaUnit) {
			float theta = thetaUnit * CV_PI / spaceWidth;	//实际角度，0-2pi
			float rho = y*sin(theta) + x*cos(theta);		//图像空间的rho值
			float rhoNorm = rho / rhoMax / 2;			//归一化为(-0.5,0.5)
			int rhoUnit = (round)(rhoNorm*spaceHeight);		//使用round近似，如果默认(int)则在(-1,1)之间均成0
			rhoUnit += spaceHeight / 2;					//转成Hough空间坐标轴
			if (rhoUnit >= spaceHeight || rhoUnit < 0)	//如果超出边界则无法绘制点
				continue;
			//houghImg.at<ushort>(rhoUnit, thetaUnit)++;
			(*((ushort*)(houghImg.data) + rhoUnit * spaceWidth + thetaUnit))++;
		}
	}

	//归一化需保证图像深度不变，所以归一化到255，再通过convertTo函数改变深度
	normalize(houghImg, houghImg, 255, 0, NORM_INF);
	Mat showImg = Mat::zeros(Size(spaceWidth, spaceHeight), CV_8UC1);
	houghImg.convertTo(showImg, CV_8U);

	imshow("hough", showImg);
	return houghImg;
}

int g_cnt = 0;
Mat drawLines(Mat src) {
	vector<Vec4i> lines;
	HoughLinesP(src, lines, g_rho, g_theta, g_lineThresh * 10, g_minLengh, g_minGap);
	Mat lineMat = Mat::zeros(src.size(), src.type());
	for (int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		line(lineMat, Point(l[0], l[1]), Point(l[2], l[3]), 255, 1, LINE_AA);
	}
	imshow("lines", lineMat);

	int lineAngle[190] = { 0 };
	for (int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		Point lineBegin = Point(l[0], l[1]);
		Point lineEnd = Point(l[2], l[3]);
		double line_x = l[2] - l[0];
		double line_y = l[3] - l[1];
		double distance = sqrt(line_x*line_x + line_y*line_y);
		//double angle = acos((line_x * 1 + line_y * 0) / sqrt(line_x*line_x + line_y*line_y)) * 180 / CV_PI;
		//cout << angle << "  ";
		double angle = (atan2(line_y, line_x) * 180 / CV_PI);
		if (angle < 0)
			angle += 180;
		//cout << angle << "  ";
		lineAngle[(int)angle]++;
	}

	//
	//for (int i = 0; i < 180; ++i) {
	//	cout << i << ": " << lineAngle[i] << endl;
	//}
	cout << "Line numbers:" << lines.size() << endl;
	for (int i = 0; i < 180; ++i) {
		if (lineAngle[i] != 0)
			cout << i << ": " << lineAngle[i] << endl;
	}

	cout << "counter:" << g_cnt++ << endl;
	return lineMat;
}

void onTrackbar(int, void*) {
	doSomething(g_src);
}

void doSomething(Mat src) {
	imshow("src", src);
	Mat binImg;
	//threshold(src, binImg, 255, 255, THRESH_OTSU);
	adaptiveThreshold(src, binImg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, g_adaptiveSize*10+1, g_const);
	imshow("bin", binImg);
	Mat cannyImg;
	Canny(binImg, cannyImg, 3, 9, 3);
	imshow("canny", cannyImg);
	drawLines(cannyImg);
}