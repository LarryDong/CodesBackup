
/*
模板匹配例子
所使用的方法最亮的地方为最匹配的区域左上角

董岩 2018.9.4
*/

#include <opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void main(void) {
	Mat src = imread("2.jpg");
	Mat tmp = imread("2_template.jpg");
	Mat srcCopy;
	src.copyTo(srcCopy);

	Mat dstImg;
	matchTemplate(src, tmp, dstImg, CV_TM_CCOEFF_NORMED);	//模板匹配结果存放于dstImg
	
	Mat grayImg;
	dstImg.convertTo(grayImg, CV_8UC3, 255, 0);		//转换结果，用于绘制0-255灰度图

	Point maxLocation;
	minMaxLoc(dstImg, 0, 0, 0, &maxLocation,Mat());

	rectangle(srcCopy, maxLocation, Point(maxLocation.x + tmp.cols, maxLocation.y + tmp.rows), Scalar(128, 0, 255), 2, 8, 0);
	imshow("Template", tmp);
	imshow("Source", src);
	imshow("Result", srcCopy);
	imshow("Feature", dstImg);
	waitKey(0);
}