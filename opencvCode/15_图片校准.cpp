
#include <opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define FILE_PATH "video\\"
void main() {

	FileStorage fs("camera.xml", FileStorage::READ);

	if (!fs.isOpened()) {
		cout << "Cannot open cameral parameter..." << endl;
		system("pause");
		return;
	}

	Mat instrinsicMatrix, distortionMatrix;
	fs["Intrinsic"] >> instrinsicMatrix;
	fs["Distortion"] >> distortionMatrix;

	Mat frame;
	int c = 0;
	namedWindow("video");
	namedWindow("calib");
	int counter = 0;

	while (c != 'q') {
		frame = imread("video\\src\\" + to_string(counter) + ".jpg", 0);
		if(frame.empty()) {
			cout << "No more pictures;" << endl;
			system("pause");
		}
		imshow("video", frame);
		Mat cImg;
		undistort(frame, cImg, instrinsicMatrix, distortionMatrix);
		imshow("calib", cImg);
		c = waitKey(0);
		if (c == 's')
			imwrite(FILE_PATH + to_string(counter++) + ".jpg", cImg);
	}

}