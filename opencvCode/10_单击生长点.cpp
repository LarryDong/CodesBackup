
/*
区域生长例子
鼠标单击处作为生长点。
Type，选择生长方式。0：和最初单击点相比；1：和最新生长的点相比
diff，相差阈值.


			董岩 2018/3/21
*/
#include <opencv.hpp>  
#include <iostream>
#include <stack>


using namespace cv;
using namespace std;

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


void on_Mouse(int event, int x, int y, int flags, void*);
void on_Trackbar(int, void*);

int g_diff = 0;
int g_type = 0;

Mat src = imread("lena.jpg", 0);
Mat musk = Mat::zeros(Size(src.cols, src.rows), CV_8UC1);
Mat dst(musk);

int main(void) {

	namedWindow("原图", CV_WINDOW_FREERATIO);
	namedWindow("区域生长图", CV_WINDOW_FREERATIO);
	imshow("原图", src);
	setMouseCallback("原图", on_Mouse, 0);	//点哪个图片都可以
	createTrackbar("Type:", "区域生长图", &g_type, 1, on_Trackbar);
	createTrackbar("diff:", "区域生长图", &g_diff, 255, on_Trackbar);
	on_Trackbar(0,0);
	waitKey(0);
}

void on_Trackbar(int, void*) {
	//每次修改阈值时清空掩码图与画布图
	musk = Mat::zeros(Size(src.cols, src.rows), CV_8UC1);
	dst = Mat::zeros(Size(src.cols, src.rows), CV_8UC1);
	imshow("区域生长图", dst);	//每次修改阈值时重新画图
}


void on_Mouse(int event, int x, int y, int flags, void*) {
	
	//窗口外的鼠标点击不处理
	if (x < 0 || x >= src.cols || y < 0 || y >= src.rows)
		return;

	stack<Point> pointStack;//定义放置生长点的堆栈
	
	Point clickPoint;		//记录鼠标点击时的点
	int clickPointValue;

	if (event == EVENT_LBUTTONDOWN) {
		clickPoint = Point(x, y);
		clickPointValue = src.at<uchar>(clickPoint.y*src.cols + clickPoint.x);
		pointStack.push(clickPoint);
	}

	while (!pointStack.empty()) {

		Point seed = pointStack.top();	//生长区域从堆栈顶端开始
		int seedIndex = seed.y*src.cols + seed.x;	//生长点的索引值
		int seedValue = src.at<uchar>(seed.y*src.cols + seed.x);	//生长点灰度值

		pointStack.pop();

		for (int i = 0; i < 8; ++i) {	//搜索周围8-邻域

			Point searchPoint = seed + pointAround8[i];
			
			//边界约束。或许有更好的办法，但暂时还没遇见或者想出来
			if (searchPoint.x < 0 || searchPoint.y < 0 || searchPoint.x >= src.cols || searchPoint.y >= src.rows)
				continue;

			int searchPointIndex = searchPoint.y*src.cols + searchPoint.x;
			int searchPointValue = src.at<uchar>(searchPoint.y*src.cols + searchPoint.x);

			//使用musk图像来标注哪个点成为了生长点，下次不再计算
			if (g_type == 0) {	//与鼠标单击点进行比较，在阈值内生长
				if (abs(searchPointValue - clickPointValue) < g_diff && musk.at<uchar>(searchPointIndex) == 0) {
					musk.at<uchar>(searchPointIndex) = 1 ;
					pointStack.push(searchPoint);
				}
			}
			else {			//与新生长的点比较，在阈值内生长
				if (abs(searchPointValue - seedValue) < g_diff && musk.at<uchar>(searchPointIndex) == 0) {
					musk.at<uchar>(searchPointIndex) = 1;
					pointStack.push(searchPoint);
				}
			}
		}
	}

	src.copyTo(dst, musk);	//将生长的区域复制到画布上

	imshow("区域生长图", dst);
}

