
/*
瞎写了一个角点检测的代码
当图像发生旋转时，检测效果相差较大。

2018/10/15
*/
#include <opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define FILE_PATH "result\\"

void doSomething(Mat src);
void onTrackbar(int, void*);

int g_k = 1, g_blockSize = 1, g_kSize = 1,g_thresh=0;
Mat g_src = imread("result\\0.jpg", 0);

int g_counter = 0;

//int tmp_counter = 0;

void main() {
	
	namedWindow("src");
	namedWindow("binary");
	namedWindow("harris");
	namedWindow("selected");

	createTrackbar("k%", "harris", &g_k, 10, onTrackbar);
	createTrackbar("block", "harris", &g_blockSize, 10, onTrackbar);
	createTrackbar("kSize", "harris", &g_kSize, 10, onTrackbar);

	createTrackbar("thresh", "selected", &g_thresh, 255, onTrackbar);

	int c = 0;
	while (c != 'q') {
		g_src = imread(FILE_PATH + to_string(g_counter++) + ".jpg", 0);
		Mat src = g_src;

		if (src.empty()) {
			cout << "end of photos..." << endl;
			g_counter = 0;
			continue;
		}
		doSomething(src);

		c = waitKey(0);
	}

}

void onTrackbar(int, void*) {
	doSomething(g_src);
}

void doSomething(Mat src) {
	imshow("src", src);
	//Mat binImg;
	//threshold(src, binImg, 0, 255, THRESH_OTSU);
	//imwrite("result\\binary\\"+to_string(g_counter)+".jpg", binImg);
	//imshow("binary", binImg);

	Mat cornersImg = Mat::zeros(src.size(), CV_32FC1);
	cornerHarris(src, cornersImg, g_blockSize * 2 + 1, g_kSize * 2 + 1, (double)g_k*0.01f);

	Mat normImg;
	normalize(cornersImg, normImg, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

	imshow("harris", normImg);
	Mat selectImg = Mat::zeros(src.size(), CV_8UC1);

	selectImg.convertTo(selectImg, CV_8UC3);
	for (int i = 0; i < normImg.rows; ++i) {
		for (int j = 0; j < normImg.cols; ++j) {
			int value = normImg.at<float>(i, j);
			if (value > g_thresh) {
				circle(selectImg, Point(j, i), 1, Scalar(255, 0, 0));
			}
		}
	}
	imshow("harris", selectImg);
}

