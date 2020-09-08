
/*
代码说明：
1、四叉树抠图，找出斑马线可能区域
2、对可能的区域进行二值化，提取边界，找直线
3、在所有直线中寻找平行线

2018/4/20

修改：
1、修改局部图像黑白图像的人皆有食选取，采用均值而非OTSU以提高计算速度
2、初始化时先进行一次threshold计算，以提高后续计算速度
3、新增计时函数，输出上次调用该函数到本次调用之间时间间隔
4、增加FAST_MODE宏定义，以减少图像存储与输出，提高计算速度。
5、新增判断是否有斑马线。不存在斑马线原则：
没有区域符合双极系数条件，最大ROI区域小于一定像素，区域内不够足够多的直线，区域内平行线数量不够
2018/4/29

修改：
1、批量判断
2、如果有斑马线则标识并保存图片
2018/5/1
*/

#include "main.h"
#include "time.h"

using namespace cv;
using namespace std;

bool isZebraExist = false;
int checkZebraExist(const Mat src, int number);

#define MAX_PICTURE_NUMBER 100
#define PICTURE_PATH "Tsinghua_std\\"

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
		src = imread(PICTURE_PATH + no + ".jpg", 0);

		if (src.empty()) {
			cout << "[Info]: " << i << " founds. Program stopped." << endl;
			totalPictureNumber = i;
			break;
		}
		zebraExistArray[i] = checkZebraExist(src, i);
		if (zebraExistArray[i] == -1) {
			cout << "[Error]. Program stopped." << endl;
			system("pause");
			return;
		}
	}

	cout << "------Result------" << endl;
	int haveZebra = 0, noZebra = 0;
	for (int i = 0; i < totalPictureNumber; ++i) {

		if (zebraExistArray[i]) 
			haveZebra++;
		else
			noZebra++;
	}

	cout <<haveZebra << " pictures have zebra corssing."<< endl;
	cout <<noZebra << " pictures don't." << endl;
	timer = clock() - timer;
	cout << "Total time: " << (timer / 1000) << " s. Average time:" << (timer / totalPictureNumber) << "ms" << endl << endl;

	system("pause");
}


int checkZebraExist(const Mat src, int number) {

	clock_t timer = clock();

	if (src.cols != 320 || src.rows != 240) {
		cout << "[Error] Picture size is not 320x240." << endl;
		system("pause");
		return -1;
	}

	subRegion srcRegion(src);			//将原图作为第一个子区域
	vector<subRegion> regionVector;		//分割区域存储的vector
	recursiveDivide(srcRegion, regionVector);	//递归分割，存储于vector中
												//在分裂图中寻找可能存在斑马线的区域，将区域返回掩码图
	Mat possibleZebraArea = findPossibleROI(src, regionVector);
	//在可能区域中选取斑马线所在区域
	Mat zebraAreaMask = findMaxROI(src, possibleZebraArea);
	//提取在斑马线所在区域的边界线
	Mat boundary = findBoundary(src, zebraAreaMask);
	//在边界线中确定直线，并寻找平行线
	vector<Vec4i> lines;
	lines = findLines(boundary);
	//在直线中寻找平行线
	vector<Vec4i> parallelLines;
	parallelLines = findParallelLines(lines);

	if (isZebraExist) {
		cout << "No. " << number << " : Yes. --" << (clock() - timer) << " ms" << endl;
		Mat tmp;
		drawLines(src, parallelLines, "Tsinghua_std\\mark_" + to_string(number)+".jpg");
		return 1;
	}
	else {
		cout << "No. " << number << " : No. t--" << (clock() - timer) << " ms" << endl;
		return 0;
	}
}
