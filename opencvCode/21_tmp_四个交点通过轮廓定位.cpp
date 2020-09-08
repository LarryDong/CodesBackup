
/*
存在问题：
接近水平或垂直时，无法准确定位L形两个端点
2018 10 30 
*/

#include <opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void onTrackbar(int, void*);
void process(const Mat src);
Mat drawHoughSapce(Mat binImg, int spaceWidth, int spaceHeight);
Mat drawLines(Mat src);
double calPoint2LineDistance(const Point2f point, const Vec4i lines);

Mat g_src = imread("result\\0.jpg", 0);
int g_min = 25, g_max = 150, g_ratio = 50;
int g_counter = 0;
int g_minLengh = 10, g_minGap = 10, g_rho = 1, g_lineThresh = 15;
double g_theta = CV_PI / 180;

void main() {
	namedWindow("src");
	namedWindow("check");
	namedWindow("所有轮廓");
	namedWindow("二级轮廓");
	namedWindow("lines");

	createTrackbar("minLen", "lines", &g_minLengh, 100, onTrackbar);
	createTrackbar("minGap", "lines", &g_minGap, 100, onTrackbar);
	createTrackbar("th", "lines", &g_lineThresh, 100, onTrackbar);

	createTrackbar("min", "check", &g_min, 100, onTrackbar);
	createTrackbar("max", "check", &g_max, 200, onTrackbar);
	createTrackbar("ratio%", "check", &g_ratio, 100, onTrackbar);

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
		g_counter = i;
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
	//imwrite("result\\所有轮廓\\"+ to_string(g_counter)+".bmp",drawImg);

	//绘制第二层二维码外轮廓
	vector<vector<Point>> contours2;
	vector<Vec4i> hierarchy2;
	//只找第二层轮廓，从每一个顶层轮廓开始，找其作为父轮廓([2]索引)的全部轮廓
	for (int i = 0; i >= 0; i = hierarchy[i][0]) {
		for (int j = hierarchy[i][2]; j >= 0; j = hierarchy[j][0]) {
			Scalar color = Scalar(0, j * 1, 255 - j * 1);
			contours2.push_back(contours[j]);
			hierarchy2.push_back(hierarchy[j]);
		}
	}
	Mat drawImg2 = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < contours2.size(); ++i) {
		Scalar color = Scalar(0, i * 3, 255 - i * 3);
		drawContours(drawImg2, contours2, i, color, 1, 8);
	}
	imshow("二级轮廓", drawImg2);
	//imwrite("result\\二级轮廓\\" + to_string(g_counter) + ".bmp", drawImg2);

	//筛选轮廓长度
	////vector<int> lengthVector;
	vector<vector<Point>> validContours;
	Mat validMat = Mat::zeros(src.size(), CV_8UC3);
	vector<vector<Point>> strangeContours;
	for (int i = 0; i < contours2.size(); ++i) {
		int length = arcLength(contours2[i], true);

		if (length >= g_min * 10 && length <= g_max * 10) {

			Rect rect = boundingRect(contours2[i]);
			int rectWidth = rect.width;
			int rectHeight = rect.height;

			double ratio1 = (rectWidth*1.0f) / rectHeight;
			double ratio2 = (rectHeight*1.0f) / rectWidth;

			if (ratio1 * 100 >= g_ratio && ratio2 * 100 >= g_ratio)
				validContours.push_back(contours2[i]);
			else
				strangeContours.push_back(contours2[i]);
		}
	}
	 //Mat validMat = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < validContours.size(); ++i) {
		drawContours(validMat, validContours, i, Scalar(128,128,128), 1);
	}
	for (int i = 0; i < strangeContours.size(); ++i) {
		drawContours(validMat, strangeContours, i, Scalar(255, 0, 255), 2);
	}
	imshow("check", validMat);
	

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
	imshow("框选", srcCopy);
	//imwrite("result\\框选\\" + to_string(g_counter) + ".bmp", srcCopy);

	Mat LshapeMat= Mat::zeros(src.size(), CV_8UC1);
	drawContours(LshapeMat, validContours, -1, 255, 1);
	imshow("过渡图", LshapeMat);
	//imwrite("result\\有效轮廓\\" + to_string(g_counter) + ".bmp", LshapeMat);


	//获取四个关键点
	vector<Point> keyPoints;
	for (int i = 0; i < validContours.size(); ++i) {
		auto con = validContours[i];
		Point up, down, left, right;
		up.y = 999, down.y = 0, left.x = 999, right.x = 0;
		for (int j = 0; j < con.size(); ++j) {
			Point p = con[j];
			if (p.y < up.y)
				up = p;
			if (p.y > down.y)
				down = p;
			if (p.x < left.x)
				left = p;
			if (p.x > right.x)
				right = p;
		}
		

		keyPoints.push_back(up);
		keyPoints.push_back(down);
		keyPoints.push_back(left);
		keyPoints.push_back(right);

	}
	

	//计算新的完整点
	for (int i = 0; i*4 < keyPoints.size();++i) {
		Point up, down, left, right;
		up = keyPoints[i * 4 + 0];
		down = keyPoints[i * 4 + 1];
		left = keyPoints[i * 4 + 2];
		right = keyPoints[i * 4 + 3];

		circle(srcCopy, up, 3, Scalar(0, 0, 255));
		circle(srcCopy, down, 3, Scalar(0, 255, 0));
		circle(srcCopy, left, 3, Scalar(255, 0, 0));
		circle(srcCopy, right, 3, Scalar(0, 255, 255));

		Point newPoint;
		if (down.y - up.y > right.x - left.x) {
			//上下宽，所以计算左右点的距离
			double d1 = calPoint2LineDistance(left, Vec4i(down.x, down.y, up.x, up.y));
			double d2 = calPoint2LineDistance(right, Vec4i(down.x, down.y, up.x, up.y));

			if (d1 > d2) {
				newPoint = up + (down - left);
				cout << "左 侧点为角点" << endl;
			}
			else {
				newPoint = up + (down - right);
				cout << "右 侧点为角点" << endl;
			}
		}
		else {
			//上下宽，所以计算左右点的距离
			double d1 = calPoint2LineDistance(up, Vec4i(left.x, left.y, right.x, right.y));
			double d2 = calPoint2LineDistance(down, Vec4i(left.x, left.y, right.x, right.y));
			if (d1 > d2) {
				newPoint = left + (right - up);
				cout << "上 侧点为角点" << endl;
			}
			else {
				newPoint = left + (right - down);
				cout << "下 侧点为角点" << endl;
			}
		}
		circle(srcCopy, newPoint, 3, Scalar(255, 0, 255), -1);
	}



	imshow("标点图", srcCopy);
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
	//imwrite("result\\hough\\" + to_string(g_counter) + ".bmp", showImg);
	return houghImg;
}


Mat drawLines(Mat src) {
	vector<Vec4i> lines;
	HoughLinesP(src, lines, g_rho, g_theta, g_lineThresh, g_minLengh, g_minGap);
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

	return lineMat;
}

double calPoint2LineDistance(const Point2f point, const Vec4i lines) {
	double a, b, c, dis;
	Point2f point1 = Point(lines[0], lines[1]);
	Point2f point2 = Point(lines[2], lines[3]);

	// 化简两点式为一般式
	// 两点式公式为(y - y1)/(x - x1) = (y2 - y1)/ (x2 - x1)
	// 化简为一般式为(y2 - y1)x + (x1 - x2)y + (x2y1 - x1y2) = 0
	// A = y2 - y1
	// B = x1 - x2
	// C = x2y1 - x1y2

	a = point2.y - point1.y;
	b = point1.x - point2.x;
	c = point2.x*point1.y - point1.x*point2.y;
	dis = abs(a*point.x + b*point.y + c) / sqrt(a*a + b*b);

	return dis;
}