
/*
区域分割例子

遍历所有像素点，满足阈值接近时合并，之后每个区域随机赋予灰度值。

			董岩 2018/3/21
*/

#include <opencv.hpp>  
#include <iostream>
#include <stack>

using namespace cv;
using namespace std;

void on_Trackbar(int, void*);
int g_grownThreshold = 0;

//定义一个点周围8邻域
const Point pointAround8[8] = {
	Point(1,0),
	Point(1,-1),
	Point(0,-1),
	Point(-1,-1),
	Point(-1,0),
	Point(-1,1),
	Point(0,1),
	Point(1,1)
};

Mat src = imread("picture.jpg", 0);

void main(void) {
	if (!src.data) {
		cout << "请在.exe所在文件夹中放入名为\"picture.jpg\"的图像。像素点不要过大(建议小于200*200)" << endl;
		system("pause");
		return ;
	}

	cout << endl << "区域生长分割演示。" << endl << endl;
	cout << "遍历图像每一个点，将其附近颜色相近（相近程度由阈值决定）的点合并到一起，并随机着色。" << endl;
	cout << "可以看出，纯色区域只要有一点点阈值就立刻被分离；但颜色变化区域会随着阈值上升而逐渐合并。" << endl;
	cout <<endl<< "          董岩 2018/3/21" << endl;
	imshow("原图", src);
	//GaussianBlur(src, src, Size(3, 3), 0);	//开启高斯模糊可以去除小范围噪点，但是对纯色分割存在影响
	namedWindow("分割图",WINDOW_AUTOSIZE);
	createTrackbar("生长阈值：", "分割图", &g_grownThreshold, 255, on_Trackbar);
	on_Trackbar(0, 0);
	waitKey(0);

}

void on_Trackbar(int, void*) {
	//此Map用于记录某个点是否属于一个区域。如果不属于则为0，属于的话则随机赋予了一个区域标号（同时也是灰度值）
	Mat grownMap = Mat::zeros(Size(src.cols, src.rows), CV_8UC1);	
	stack<Point> pointStack;
	RNG rng;		//随机数生成器 Random Number Generator

	for (int i = 0; i < src.cols; ++i) {	//遍历所有像素点，耗时非常长
		for (int j = 0; j < src.rows; ++j) {

			int pointIndex = i*src.rows + j;
			int pointValue = src.at<uchar>(pointIndex);

			if (grownMap.at<uchar>(pointIndex) == 0) {//找到第一个没有归类的点

				int grownMarker = rng.uniform(1, 256);	//随机给定一个标号
				grownMap.at<uchar>(pointIndex) = grownMarker;

				Point seed = Point(j, i);

				pointStack.push(seed);	//存下第一个生长点

				while (!pointStack.empty()) {		//当仍有的生长点未经过遍历时
					seed = pointStack.top();
					pointStack.pop();

					for (int k = 0; k < 8; ++k) {
						Point searchPoint = seed + pointAround8[k];
						//边界约束
						if (searchPoint.x < 0 || searchPoint.y < 0 
							|| searchPoint.x >= src.cols || searchPoint.y >= src.rows)
							continue;
						int searchPointIndex = searchPoint.y*src.cols + searchPoint.x;
						int searchPointValue = src.at<uchar>(searchPoint.y*src.cols + searchPoint.x);
						//当差值小于设定的阈值时，且该点不属于其他已生长的区域时，将该点归入现在的生长区域
						if (abs(searchPointValue - pointValue) < g_grownThreshold	
							&& grownMap.at<uchar>(searchPointIndex) == 0) {
							grownMap.at<uchar>(searchPointIndex) = grownMarker;
							pointStack.push(searchPoint);
						}
					}
				}
			}
		}
	}

	imshow("分割图", grownMap);
}

