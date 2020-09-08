
#include <opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define FILE_PATH "video\\"


void doSomething(Mat src);
void onTrackbar(int, void*);

Mat g_src, g_template;
string g_filePath = "video\\";
Mat const g_oriTemplate = imread(g_filePath+"template.jpg", 0);


int g_rotateAngle = 0;

void main() {
	int counter = 0;
	g_oriTemplate.copyTo(g_template);
	namedWindow("src");
	namedWindow("canny");
	namedWindow("bin");
	namedWindow("result");
	namedWindow("template", WINDOW_FREERATIO);
	createTrackbar("angle", "template", &g_rotateAngle, 360, onTrackbar);
	onTrackbar(0, 0);

	int c = 0;
	while (c != 'q') {
		g_src = imread(g_filePath + to_string(counter) + ".jpg", 0);
		cout << "Picture No. " << counter << endl;
		if (g_src.empty()) {
			cout << "end of photos..." << endl;
			counter = 0;
			continue;
		}
		doSomething(g_src);
		c = waitKey(0);
		if (c == 'a' || c == 'w')
			counter--;
		else
			counter++;
	}
}

void doSomething(Mat src) {
	imshow("src", src);
	Mat binImg;
	threshold(src, binImg, 255, 255, THRESH_OTSU);
	threshold(binImg, binImg, 128, 255, THRESH_BINARY_INV);
	//adaptiveThreshold(src, binImg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, g_adaptiveSize * 10 + 1, g_const);
	imshow("bin", binImg);
	Mat cannyImg;
	Canny(binImg, cannyImg, 3, 9, 3);
	imshow("canny", cannyImg);

	imwrite(g_filePath + "bin.jpg", binImg);
	imwrite(g_filePath + "canny.jpg", cannyImg);

	Mat dstImg;
	matchTemplate(binImg, g_template, dstImg, CV_TM_CCOEFF_NORMED);	//模板匹配结果存放于dstImg
	Mat grayImg;
	dstImg.convertTo(grayImg, CV_8UC3, 255, 0);		//转换结果，用于绘制0-255灰度图
	imshow("result", dstImg);

}

void onTrackbar(int, void*) {

	Point center = Point(g_oriTemplate.cols / 2, g_oriTemplate.rows / 2);
	Mat rotImg = getRotationMatrix2D(center, g_rotateAngle, 1);
	warpAffine(g_oriTemplate, g_template, rotImg, g_template.size());
	imshow("template", g_template);
}