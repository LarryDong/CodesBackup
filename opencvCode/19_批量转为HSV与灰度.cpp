
#include "main.h"
#include "time.h"

using namespace cv;
using namespace std;

#define MAX_PICTURE_NUMBER 500

#define FILE_SOURCE_PATH "Tsinghua_src\\"
#define FILE_SAVE_PATH "Tsinghua_std\\"


int toHSV(const Mat src, Mat& H, Mat& S, Mat& V);
	
void main() {

	cout << "开始转化..." << endl;
	clock_t timer,beginTimer=clock();

	Mat src, H, S, V, gray;
	string no;
	for (int i = 0; i < MAX_PICTURE_NUMBER; ++i) {

		timer = clock();
		no = to_string(i);

		src = imread(FILE_SOURCE_PATH + no + ".jpg", 1);

		if (src.empty()) {
			cout << "只能找到 " << i << " 张图片。转化结束。" << endl;
			break;
		}
		
		toHSV(src, H, S, V);
		
		imwrite("Tsinghua_std\\H" + no + ".jpg", H);
		imwrite("Tsinghua_std\\S" + no + ".jpg", S);
		imwrite("Tsinghua_std\\V" + no + ".jpg", V);
		cvtColor(src, gray, COLOR_BGR2GRAY);
		imwrite("Tsinghua_std\\gray" + no + ".jpg", gray);
		cout << "第 " << no << "图转化完成，用时 " << (clock() - timer) << " ms" << endl;

	}
	cout << endl << "全部结束，用时 " << (clock() - beginTimer) << " ms" << endl << endl;
	system("pause");
}

int toHSV(const Mat src, Mat& H, Mat& S,Mat& V) {

	Mat bgr32Img, hsv32Img;
	src.convertTo(bgr32Img, CV_32FC3, 1.0 / 255, 0);	//先将图像转化成32F
	cvtColor(bgr32Img, hsv32Img, COLOR_BGR2HSV);
	
	vector<Mat> hsvChannels(3);
	split(hsv32Img, hsvChannels);

	//H通道是0-360，S和V是0-1
	hsvChannels.at(0).convertTo(H, CV_8UC1, 255.0 / 360);
	hsvChannels.at(1).convertTo(S, CV_8UC1, 255);
	hsvChannels.at(2).convertTo(V, CV_8UC1, 255);
	return 0;
}
