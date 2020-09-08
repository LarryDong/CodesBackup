
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

void main() {
	int counter = 0;
	namedWindow("src");
	namedWindow("canny");
	namedWindow("bin");

	int c = 0;
	while (c != 'q') {
		Mat src = imread(FILE_PATH + to_string(counter++) + ".jpg", 0);
		if (src.empty()) {
			cout << "end of photos..." << endl;
			counter = 0;
			continue;
		}
		imshow("src", src);
		Mat binImg;
		threshold(src, binImg, 255, 255, THRESH_OTSU);
		imshow("bin", binImg);
		Mat cannyImg;
		Canny(binImg, cannyImg, 3, 9, 3);
		imshow("canny", cannyImg);
		drawHoughSapce(cannyImg, 600, 400);
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

