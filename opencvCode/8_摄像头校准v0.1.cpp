/*
摄像头校准v0.1
由于手头摄像头不存在明显畸变，故无法确定代码是否正确。

董岩 2018/9/11
*/

#include <opencv.hpp>
#include <iostream>
//#include <highgui.hpp>
#include <Windows.h>

using namespace std;
using namespace cv;

#define FILE_PATH "chessboard\\"

void doSomething(Mat src);
void calculateRealPoint(vector<vector<Point3f>>& objectPoints, int boradWidth, int boardHeight, int imgNumber, int squareSize);


void main() {
	// 
	Size boardSize = Size(9, 6);
	vector<Point2f> corner;
	vector<vector<Point2f>> imagePoints;

	int validChessboardNumber = 0;
	int pictureIndex = 0;

	while (validChessboardNumber  < 15) {

		Mat src = imread("chessboard\\" + to_string(pictureIndex) + ".jpg", 1);
		pictureIndex++;
		if (src.empty()) {
			cout << "No picture found..." << endl;
			return;
		}
		imshow("src", src);
		int isChessboardFound = findChessboardCorners(src, boardSize, corner, CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE);
		if (isChessboardFound == true) {
			Mat grayImg;
			cvtColor(src, grayImg, CV_BGR2GRAY);
			cornerSubPix(grayImg, corner, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
			drawChessboardCorners(src, boardSize, corner, isChessboardFound);

			imshow("chessboard", src);
			imagePoints.push_back(corner);
			validChessboardNumber++;
			int c = waitKey(50);
			if (c == 'q') {
				cout << "Quit loading pictures. Valid picture number: " << validChessboardNumber << endl;
				break;
			}
		}
		else {
			cout << "No chessboard found in picture NO: " << pictureIndex << endl;
			continue;
		}
	}


	vector<vector<Point3f>> objectPoints;
	calculateRealPoint(objectPoints, 9, 6, validChessboardNumber, 27);

	Mat intrinsicMatrix, distortionCoefficient;
	vector<Mat> rotateMatrix, translationMatrix;

	calibrateCamera(objectPoints, imagePoints, Size(640, 480), intrinsicMatrix, distortionCoefficient, rotateMatrix, translationMatrix, 0);

	cout << "Intrinsic: " << endl;
	cout << intrinsicMatrix;
	cout << "Distortion: " << endl;
	cout << distortionCoefficient;


	//以下为视频流获取图像校准情况
	VideoCapture capture;
	capture.open(0);
	while (!capture.isOpened()) {
		cout << "Waiting for camera...";
		Sleep(1000);
		capture.open(0);
	}

	int c = waitKey(1);
	namedWindow("video");
	namedWindow("校准");
	Mat frame;
	while (c != 'q') {
		capture >> frame;
		imshow("video", frame);
		Mat cImg;
		undistort(frame, cImg, intrinsicMatrix, distortionCoefficient);
		imshow("校准", cImg);
		waitKey(1000);
	}

}

//计算实际点的坐标。规定Z=0，将通过正方形边长计算后的x与y放到目标点中，用于后续计算
void calculateRealPoint(vector<vector<Point3f>>& objectPoints, int boardWidth, int boardHeight, int imgNumber, int squareSize) {
	vector<Point3f> imagePoints;
	for (int i = 0; i < boardHeight; ++i) {
		for (int j = 0; j < boardWidth; ++j) {
			imagePoints.push_back(Point3f(i*squareSize, j*squareSize, 0));
		}
	}
	for (int i = 0; i < imgNumber; ++i) {
		objectPoints.push_back(imagePoints);
	}
}

