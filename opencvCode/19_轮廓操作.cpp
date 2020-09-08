/*
一些轮廓的计算

包括：轮廓提取、轮廓继承关系、轮廓周长、轮廓近似、正向矩形、最小矩形、最小圆

		董岩 2018/10/29
*/

#include <opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void onTrackbar(int, void*);
void process(const Mat src);

Mat g_src = imread("result\\0.jpg", 0);
int g_min = 30, g_max = 150;
int g_epsilon = 0;

void main() {
	namedWindow("src");
	namedWindow("check");
	namedWindow("所有轮廓");
	namedWindow("二级轮廓");
	namedWindow("approx");
		
	createTrackbar("min", "check", &g_min, 100, onTrackbar);
	createTrackbar("max", "check", &g_max, 200, onTrackbar);
	createTrackbar("ep", "approx", &g_epsilon, 100, onTrackbar);

	for (int i = 0; i < 1000;) {
		if (i < 0)
			i = 0;
		g_src = imread("result\\" + to_string(i) + ".jpg", 0);
		if (g_src.empty()) {
			cout << endl << "---------------------------------" << endl;
			cout << "共找到" << i << "张图片。" << endl << endl;
			i = 0;
			continue;
		}

		cout << endl << "Picture No. " << i << endl;
		process(g_src);

		int keyValue = waitKey(0);
		switch (keyValue) {
		case 'w': case 'a':
			cout << "返回上一张图片..." << endl;
			i--;
			break;
		case 'b':
			cout << "回到第0张图..." << endl;
			break;
		case 'q':
			return;
		default:
			i++;
			break;
		}
	}
	//Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	//morphologyEx(binImg, binImg, MORPH_CLOSE, element);
	//morphologyEx(binImg, binImg, MORPH_OPEN, element);

	
}

void onTrackbar(int, void*) {
	process(g_src);
}

void process(const Mat src) {
	imshow("src", src);
	Mat binImg;
	threshold(src, binImg, 0, 255, THRESH_OTSU);

	//Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(binImg, binImg, 3);

	//绘制所有轮廓
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(binImg, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	Mat drawImg = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); ++i) {
		Scalar color = Scalar(0, i * 5, 255 - i * 5);
		drawContours(drawImg, contours, i, color, 1, 8, hierarchy, 0, Point());
	}
	imshow("所有轮廓", drawImg);

	//绘制第二层二维码外轮廓
	vector<vector<Point>> contours2;
	vector<Vec4i> hierarchy2;
	//只找第二层轮廓，从每一个顶层轮廓开始，找其作为父轮廓([2]索引)的全部轮廓
	for (int i = 0; i >= 0; i = hierarchy[i][0]) {
		for (int j = hierarchy[i][2]; j >= 0; j = hierarchy[j][0]) {
			Scalar color = Scalar(0, j * 5, 255 - j * 5);
			contours2.push_back(contours[j]);
			hierarchy2.push_back(hierarchy[j]);
		}
	}
	Mat drawImg2 = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < contours2.size(); ++i) {
		Scalar color = Scalar(0, i * 5, 255 - i * 5);
		drawContours(drawImg2, contours2, i, color, 1, 8);
	}
	imshow("二级轮廓", drawImg2);

	//筛选轮廓长度
	vector<int> lengthVector;
	vector<vector<Point>> validContours;
	for (int i = 0; i < contours2.size(); ++i) {
		int length = arcLength(contours2[i],true);
		if (length >= g_min*10 && length <= g_max*10) {
			lengthVector.push_back(length);
			validContours.push_back(contours2[i]);
		}
		//cout << "Length No." << i << ": " << length << endl;
		//if(length>=g_min*10)
		//lengthVector.push_back(arcLength(contours2[i], true));
	}
	Mat validMat = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < validContours.size(); ++i) {
		drawContours(validMat, validContours, i, Scalar(128,128,128), 1);
	}
	imshow("check", validMat);

	
	//epsilon近似
	vector<vector<Point>> approxContours;
	for (int i = 0; i < lengthVector.size(); ++i) {
		vector<Point> tmpContour;
		approxPolyDP(validContours[i], tmpContour, g_epsilon*0.1f, true);
		approxContours.push_back(tmpContour);
	}
	Mat approxMat = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < approxContours.size(); ++i) {
		drawContours(approxMat, approxContours, i, Scalar(255, 255, 255), 1);
	}
	imshow("approx", approxMat);
	

	Mat srcCopy;
	src.copyTo(srcCopy);
	cvtColor(srcCopy, srcCopy, CV_GRAY2BGR);

	vector<Rect> normalRect;
	vector<RotatedRect> rotatedRect;
	vector<Point2f> minCircleCenter;
	vector<float> minCircleRadius;
	for (int i = 0; i < validContours.size(); ++i) {
		auto con = validContours[i];
		normalRect.push_back(boundingRect(con));
		rotatedRect.push_back(minAreaRect(con));

		Point2f center;
		float radius;
		minEnclosingCircle(con, center, radius);
		minCircleCenter.push_back(center);
		minCircleRadius.push_back(radius);
	}

	Mat rectMat = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < validContours.size(); ++i) {
		Rect rect = normalRect[i];
		rectangle(srcCopy, rect, Scalar(0, 0, 255), 1);
	}

	Mat rotatedRectMat = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < validContours.size(); ++i) {
		Point2f vertices[4];
		rotatedRect[i].points(vertices);
		for (int j = 0; j < 4; j++) {
			line(srcCopy, vertices[j], vertices[(j + 1) % 4], Scalar(0, 255, 0), 1);
		}
	}

	Mat circleMat = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < validContours.size(); ++i) {
		circle(srcCopy, minCircleCenter[i], minCircleRadius[i], Scalar(255, 0, 0), 1);
	}

	imshow("result", srcCopy);
}