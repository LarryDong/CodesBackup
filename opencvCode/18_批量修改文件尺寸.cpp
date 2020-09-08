
#include "main.h"
#include "time.h"

using namespace cv;
using namespace std;

bool isZebraExist = false;


#define MAX_PICTURE_NUMBER 500

#define FILE_SOURCE_PATH "Tsinghua_src\\"
#define FILE_SAVE_PATH "Tsinghua_std\\"

void main() {

	clock_t timer = clock();

	cout << "开始转化图片为 320x240 分辨率..." << endl;

	Mat src, dst;
	string no;
	for (int i = 0; i < MAX_PICTURE_NUMBER; ++i) {
		no = to_string(i);
		src = imread(FILE_SOURCE_PATH + no + ".jpg", 1);

		if (!src.empty()) {
			if (src.rows > src.cols)	//图片是细长的，则旋转90°
				transpose(src, src);
			resize(src, dst, Size(320, 240));
			imwrite(FILE_SAVE_PATH + no + ".jpg", dst);
		}
		else {
			cout << "只能找到 " << i << " 张图片。转化结束。" << endl;
			break;
		}
	}

	cout << "已转化完图片，用时 " << (clock() - timer) << " ms." << endl << endl;
	system("pause");
}