

#include <opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void onTrackbar(int, void*);
void process(const Mat src);
Mat drawHoughSapce(Mat binImg, int spaceWidth, int spaceHeight);
Mat drawLines(Mat src);


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

		int keyValue = waitKey(10);
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

	//drawHoughSapce(LshapeMat, 600, 400);
	//drawLines(LshapeMat);
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

