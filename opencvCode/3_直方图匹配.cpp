
/*
直方图匹配4法。
通过 compareHist()进行匹配两个灰度图的直方图相关性。

董岩 2018.9.4
*/

#include <opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;


int g_compareType = 1;

void onTrackbar(int, void*);
void doSomething(void);

void main() {
	namedWindow("src1");
	createTrackbar("Type:", "src1", &g_compareType, 3, onTrackbar);
	onTrackbar(0, 0);
	waitKey(0);
}

void onTrackbar(int, void*) {
	doSomething();
}

void doSomething(void) {
	Mat src1 = imread("0.jpg",0);
	Mat src2 = imread("1.jpg",0);

	imshow("src1", src1);
	imshow("src2", src2);

	int nimages = 1;//图像的个数
	int channels = 0;//需要统计通道的索引
	Mat mask = Mat();
	Mat hist1, hist2;//存放输出的直方图
	int dims = 1;//需要计算的直方图的维度
	int histSize = 256;//计算的直方图的分组数
	float range[] = { 0,256 };//表示直方图每一维度的取值范围
	const float* ranges[] = { range };//参数形式需要,表示每一维度数值的取值范围

	calcHist(&src1, nimages, &channels, mask, hist1, dims, &histSize, ranges);
	calcHist(&src2, nimages, &channels, mask, hist2, dims, &histSize, ranges);

	double distance = 0;
	int compareType = g_compareType + 1;
	
	if (compareType == 1) {
		distance = compareHist(hist1, hist2, CV_COMP_CORREL);
		cout << "Use 'Correlation' type, distance: " << distance << endl;
	}
	else if (compareType == 2) {
		distance = compareHist(hist1, hist2, CV_COMP_CHISQR);
		cout << "Use 'Chi-Square' type, distance: " << distance << endl;
	}
	else if (compareType == 3) {
		distance = compareHist(hist1, hist2, CV_COMP_INTERSECT);
		cout << "Use 'Intersection' type, distance: " << distance << endl;
	}
	else if (compareType == 4) {
		distance = compareHist(hist1, hist2, CV_COMP_BHATTACHARYYA);
		cout << "Use 'Bhattacharyya' type, distance: " << distance << endl;
	}
	else {
		cout << "ERROR INPUT TYPE!" << endl;
		system("pause");
	}
}