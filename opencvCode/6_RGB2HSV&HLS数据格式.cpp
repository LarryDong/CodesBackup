

/*

此程序可以明确的表明HSV图像在opencv中的存储数值范围

结论是，最好将获得的RGB图像转化成32FC3，同时归一化到0-1(用Mat.convertTo())。
再转换成HSV或HLS格式时能够让H在0-360，其他两个量在0-1。

				董岩 2018/3/19

*/


#include <opencv.hpp>  
#include <iostream>

using namespace cv;
using namespace std;

int main(void){
	Mat src = imread("dot.bmp", 1);
	Mat img_32FC3, img_32FC3_normal;
	Mat hsv;
	cout << "H、S为一半，S分别为5,50,95:" << endl;

	cout << endl << "默认读入彩图(8UC3)的数据格式：：" << endl << src << endl;

	src.convertTo(img_32FC3, CV_32FC3);
	cout << endl << "8UC3转32FC3后数据格式（不归一化）：" << endl<< img_32FC3 << endl;

	src.convertTo(img_32FC3_normal, CV_32FC3,1.0/255);
	cout << endl << "8UC3转32FC3后数据格式（归一化）："<<endl<< img_32FC3_normal << endl;

	cvtColor(src, hsv, COLOR_BGR2HLS);	//COLOR_BGR2HSV
	cout << endl << "默认读入彩图转hls后数值：" << endl << hsv << endl;
	

	cvtColor(img_32FC3, hsv, COLOR_BGR2HLS);
	cout << endl << "32FC3（不归一化）转hls后数值：" << endl << hsv << endl;

	cvtColor(img_32FC3_normal, hsv, COLOR_BGR2HLS);
	cout << endl << "32FC3（归一化）转hls后数值：：" << endl << hsv << endl;

	system("pause");

	return 0;
}


