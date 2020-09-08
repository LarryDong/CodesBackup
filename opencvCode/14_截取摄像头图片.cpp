
#include <opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define FILE_PATH "chessboard\\"
void main() {

	VideoCapture cap(0);
	while (!cap.isOpened()) {
		cout << "wait for camera..." << endl;
		cap.open(0);
	}
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	Mat frame;
	int c = 0;
	namedWindow("video");
	int counter = 0;
	while (c != 'q') {
		cap >> frame;
		imshow("video", frame);
		c = waitKey(10);
		if (c == 's')
			imwrite("video\\src\\" + to_string(counter++) + ".jpg", frame);
	}
}