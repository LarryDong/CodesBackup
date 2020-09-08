

#include <opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void own_eq(Mat& srcImage, Mat& outputImage, Mat& srcHist);

int main(){

	Mat srcImg = imread("baboon.jpg", 0);

	int nimages = 1;//图像的个数
	int channels = 0;//需要统计通道的索引
	Mat mask = Mat();
	Mat histImg;//存放输出的直方图
	int dims = 1;//需要计算的直方图的维度
	int histSize =  256 ;//计算的直方图的分组数
	float range[] = { 0,256 };//表示直方图每一维度的取值范围
	const float* ranges[] = { range };//参数形式需要,表示每一维度数值的取值范围

	calcHist(&srcImg, nimages, &channels, mask, histImg, dims, &histSize, ranges);
	

	double minValue = 0;
	double maxValue = 0;
	minMaxLoc(histImg, &minValue, &maxValue);//得到计算出的直方图中的最小值和最大值

	int scale = 2;
	int width = scale*histSize;
	//int width = 400;//定义绘制直方图的宽度
	int height = 400;//定义绘制直方图的高度
	Mat dstImg = Mat::zeros(Size(width, height), CV_8UC1);//宽为histSize，高为height

	for (int i = 0; i < histSize; i++){
		float binValue = histImg.at<float>(i);//得到histImg中每一分组的值
		float realValue = (binValue / maxValue)*height;//归一化数据，缩放到图像的height之内
		rectangle(dstImg, Point(scale*i, height - 1 - realValue), Point(scale * i, height - 1), Scalar(255,255,255), -1);
	}

	imshow("srcImg", srcImg);
	imshow("Histogram", dstImg);
	
	Mat newImg = imread("baboon.jpg", 0);

	/*灰度图手算法
	int grayArr[256] = { 0 };
	for (int i = 0; i < newImg.rows; ++i) {
		for (int j = 0; j < newImg.cols; ++j) {
			int gray_level = newImg.at<uchar>(i*newImg.cols + j);
			grayArr[gray_level]++;
		}
	}*/

	Mat ownOutputImg;
	own_eq(srcImg, ownOutputImg,histImg);
	imshow("Own_que", ownOutputImg);

	equalizeHist(srcImg, srcImg);
	calcHist(&srcImg, nimages, &channels, mask, histImg, dims, &histSize, ranges);
	for (int i = 0; i < histSize; i++) {
		//遍历histImg
		float binValue = histImg.at<float>(i);//得到histImg中每一分组的值
		float realValue = (binValue / maxValue)*height;//归一化数据，缩放到图像的height之内

		//用矩形方法绘制直方图，注意左上点和右下点坐标的计算
		rectangle(dstImg, Point(scale*i, height - 1 - realValue), Point(scale * i, height - 1), Scalar(255, 255, 255), -1);
	}
	imshow("eq_srcImg", srcImg);
	imshow("eq_Histogram", dstImg);

	waitKey(0);
	return 0;
}

//一定要注意数据类型。
//输入输出图像是灰色uchar，输入的hist是通过calcHist计算出的为float类型。
void own_eq(Mat& inputImage, Mat& outputImage, Mat& inputHist) {
	
	float table[256] = { 0 };	//这个table用于数据转化前暂存float的概率密度
	Mat lookUpTable(Size(1,256), CV_8UC1);
	int nPix = inputImage.cols*inputImage.rows;

	for (int i = 0; i < 256; ++i) {
		float temp = inputHist.at <float> (i)/ nPix * 255;
		if (i != 0)
			table[i] = table[i - 1] + temp;
		else
			table[i] = 0;
	}

	for (int i = 0; i <256; i++) 
		lookUpTable.at<uchar>(i) =	static_cast<uchar>(table[i]);

	LUT(inputImage, lookUpTable, outputImage);
}

