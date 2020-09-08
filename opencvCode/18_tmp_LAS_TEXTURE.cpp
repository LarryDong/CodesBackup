
/*
Law's texture energy 
load mask from .xml file.

2018/10/16

*/

#include <opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define FILE_PATH "result\\"

void doSomething(Mat src);
void onTrackbar(int, void*);

Mat g_src = imread("result\\0.jpg", 0);

int g_counter = 0;

void main() {
	/*
	Mat L5 = (Mat_<char>(1, 5) << 1, 4, 6, 4, 1);
	Mat S5 = (Mat_<char>(1, 5) << -1, 0, 2, 0, -1);
	Mat R5 = (Mat_<char>(1, 5) << 1, -4, 6, -4, 1);
	Mat E5 = (Mat_<char>(1, 5) << -1, -2, 0, 2, 1);
	Mat W5 = (Mat_<char>(1, 5) << -1, 2, 0, -2, 1);
	*/

	FileStorage fs("texture.xml", FileStorage::READ);
	if (!fs.isOpened()) {
		cout << "Cannot open cameral parameter..." << endl;
		system("pause");
		return;
	}
	Mat ls, lr, le, lw, sr, se, sw, re, rw, ew;
	fs["l5s5"] >> ls;
	fs["l5r5"] >> lr;
	fs["l5e5"] >> le;
	fs["l5w5"] >> lw;
	fs["s5r5"] >> sr;
	fs["s5e5"] >> se;
	fs["s5w5"] >> sw;
	fs["r5e5"] >> re;
	fs["r5w5"] >> rw;
	fs["e5w5"] >> ew;

	int c = 0;
	while (c != 'q') {
		g_src = imread(FILE_PATH + to_string(g_counter++) + ".jpg", 0);
		Mat src = g_src;
		//src = imread("texture.jpg", 0);

		if (src.empty()) {
			cout << "end of photos..." << endl;
			g_counter = 0;
			continue;
		}
		
		imshow("src", src);

		auto t = clock();

		Mat dstImg;
		filter2D(src, dstImg, -1, ls);
		imshow("ls", dstImg);

		filter2D(src, dstImg, -1, lr);
		imshow("lr", dstImg);

		filter2D(src, dstImg, -1, le);
		imshow("le", dstImg);

		filter2D(src, dstImg, -1, lw);
		imshow("lw", dstImg);


		filter2D(src, dstImg, -1, sr);
		imshow("sr", dstImg);

		filter2D(src, dstImg, -1, se);
		imshow("se", dstImg);

		filter2D(src, dstImg, -1, sw);
		imshow("sw", dstImg);


		filter2D(src, dstImg, -1, re);
		imshow("re", dstImg);

		filter2D(src, dstImg, -1, rw);
		imshow("le", dstImg);

		filter2D(src, dstImg, -1, ew);
		imshow("ew", dstImg);

		cout << "Time used: " << clock() - t << " ms." << endl;
		c = waitKey(0);
	}
}
