
#include "main.h"
#include "time.h"

using namespace cv;
using namespace std;

bool isZebraExist = false;

#define MAX_PICTURE_NUMBER 100

void main() {

	cout << "Start looking for zebra corssing..." << endl;
	clock_t timer = clock();

	Mat mmp = imread("mmp.jpg", 0);	//mmp空图像用于先进行一次threshold计算
	threshold(mmp, mmp, 0, 0, 0);

	Mat src, dst;
	string no;

	int zebraExistArray[MAX_PICTURE_NUMBER] = { 0 };
	int totalPictureNumber = 0;
	for (int i = 0; i < MAX_PICTURE_NUMBER; ++i) {
		no = to_string(i);
		src = imread("picture\\" + no + ".jpg", 1);

		if (src.empty()) {
			cout << "[Info]: " << i << " founds. Program stopped." << endl;
			totalPictureNumber = i;
			break;
		}

		Mat dst;
		
		int mx = src.cols / 2, my = src.rows / 2;
		int width, height;

		width = src.cols*0.75 / 2;
		height = src.rows*0.75 / 2;
		int offset = 300;

		dst = src(Rect(offset, 0, offset + 2*width, height*2));
		imwrite("picture\\new\\" + no + ".jpg",dst);

	}

	system("pause");
}

