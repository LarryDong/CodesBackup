/*
Hough变换
通过读取每一个点，计算Hough空间的theta和rho，并通过theta和rho的归一化进行画图

董岩 2018/9/19
*/


#include <opencv.hpp>
#include <iostream>
#include <vector>
#include <time.h>
#include <Windows.h>
#include <math.h>

using namespace std;
using namespace cv;


void main(void) {
	VideoCapture cap;
	cap.open(0);
	while (!cap.isOpened()) {
		cout << "No camera..." << endl;
		cap.open(0);
	}
	
	int counter = 0;
	namedWindow("video");
	Mat frame;
	while(1){
		cap >> frame;
		imshow("video", frame);
		int c = waitKey(20);
		if (c == 'b') {
			imwrite("chessboard\\" + to_string(counter++) + ".jpg", frame);
		}
	}


}
