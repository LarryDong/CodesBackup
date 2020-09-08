
/*
程序功能：

加载图片，分离HSV中H分量，进行阈值分割(OTSU法)，效果不好。
将图片进行SSR算法(单尺度Retinex算法)处理，分离HSV中H分量，进行阈值分割(OTSU法)，效果不错。存储二值化图片。
载入刚刚存储的二值化图片，进行形态学处理(开闭运算)，保存闭运算结果。

后续会进行区域合并，争取获得完整盲道。

其中，SSR算法中，高斯核尺寸(199,199)，(可修改成任意奇数，越大相对准确)，Sigma为高斯分布参数(可修改，建议30以上)。
开闭运算中尺寸可修改。


					董岩 2018/3/22
*/

#include <opencv.hpp>  
#include <iostream>

using namespace cv;
using namespace std;

void openCloseProcess(Mat src, Mat &dst);
void rgb2hsv_3channels(Mat src, Mat &H, Mat &S, Mat &V);
void SSR_MultipleChannels(Mat src, Mat &dst);
void SSR_SingleChannels(Mat src, Mat &dst);


#define PICTURE_NAME "盲道.jpg"
const Mat src = imread(PICTURE_NAME, 1);

int  main(void) {

	if (!src.data) {
		cout << "请在.exe所在文件夹中放入名为\"盲道.jpg\"的图像。" << endl;
		system("pause");
		return -1;
	}

	namedWindow("原图");
	namedWindow("原图H通道灰度图");
	namedWindow("原图H通道二值化图");

	namedWindow("SSR效果图");
	namedWindow("SSR后H通道灰度图");
	namedWindow("SSR后H通道二值化图");

	//先显示原图，以及直接阈值分割得到的图像
	imshow("原图", src);
	Mat Hgray, Sgray, Vgray;
	rgb2hsv_3channels(src, Hgray, Sgray, Vgray);
	imshow("原图H通道灰度图", Hgray);
	
	Mat originBinaryImage;
	threshold(Hgray, originBinaryImage, 0, 255, THRESH_OTSU);
	imshow("原图H通道二值化图", originBinaryImage);

	//再进行SSR算法后的图像处理
	Mat ssrColorImage;
	SSR_MultipleChannels(src, ssrColorImage);
	imshow("SSR效果图", ssrColorImage);

	Mat ssrHgray, ssrSgray, ssrVgray;	//ssr图像HSV三通道灰度值
	rgb2hsv_3channels(ssrColorImage, ssrHgray, ssrSgray, ssrVgray);
	imshow("SSR后H通道灰度图", ssrHgray);

	Mat ssrBinaryImage;
	threshold(ssrHgray, ssrBinaryImage, 0, 255, THRESH_OTSU);
	imshow("SSR后H通道二值化图", ssrBinaryImage);

	imwrite("二值化图像.jpg", ssrBinaryImage);	//存储SSR计算后的图像，为以后使用做准备。
	waitKey(0);
	destroyAllWindows();

	//接下来进行开闭运算操作
	const Mat binarySrc = imread("二值化图像.jpg", 0);
	Mat resultImage;
	openCloseProcess(binarySrc, resultImage);
	imwrite("形态学处理后图像.jpg", resultImage);

	waitKey(0);
	return 0;
}

//将3通道RGB图转为HSV图
void rgb2hsv_3channels(Mat src, Mat &H, Mat &S, Mat &V) {
	
	if (src.type() != CV_8UC3) {
		cout << "ERROR when loading src image. Should be RGB not GRAY picture." << endl;
		system("pause");
		return;
	}

	Mat hsv;
	src.convertTo(src, CV_32FC3, 1.0 / 255, 0);	//先将图像转化成32F
	cvtColor(src, hsv, COLOR_BGR2HSV);

	vector<Mat> hsvChannels(3);
	split(hsv, hsvChannels);
	//H通道是0-360，S和V是0-1
	hsvChannels.at(0).convertTo(H, CV_8UC1, 255.0 / 360);
	hsvChannels.at(1).convertTo(S, CV_8UC1, 255);
	hsvChannels.at(2).convertTo(V, CV_8UC1, 255);
}

//多通道SSR(单尺度Retinex算法)算法，内含SSR处理单通道的算法
void SSR_MultipleChannels(Mat src, Mat& dst) {
	if (src.type() != CV_8UC3) {
		cout << "ERROR when loading src image. Should be RGB not GRAY picture." << endl;
		system("pause");
		return;
	}
	vector<Mat> srcChannels(3), dstChannels(3);
	split(src, srcChannels);
	SSR_SingleChannels(srcChannels.at(0), dstChannels.at(0));
	SSR_SingleChannels(srcChannels.at(1), dstChannels.at(1));
	SSR_SingleChannels(srcChannels.at(2), dstChannels.at(2));
	merge(dstChannels, dst);	//返回多通道图像融合图
}

void SSR_SingleChannels(Mat src, Mat& dst) {

	Mat src_log;
	//mat.converTo代替了旧版的convertScale(src,dst...)函数，方便了很多
	src.convertTo(src, CV_8UC1, 1.0, 1);		//所有像素点+1，避免计算log(0)
	src.convertTo(src, CV_32FC1, 1.0 / 255, 0);	//将1-256的点映射到 0-1的float

	log(src, src_log);	//计算src的log值。此函数代替了旧版的cvLog

	Mat L, L_log;		//入射光线强度，和其对数

	int gaussSigma = 60;					//~~高斯正态分布参数，30以上效果较好
	Size gaussKernelSize = Size(199, 199);	//~~高斯滤波核大小，越大在光照变化不大的区域效果越好，计算时间越长
	GaussianBlur(src, L, gaussKernelSize, gaussSigma);	//高斯模糊参数30不知道是哪里来的。Size(0,0)使用自动尺寸
	log(L, L_log);

	Mat R;
	subtract(src_log, L_log, R);	//整个图像相减，每个像素相减

	Mat mean, std;
	meanStdDev(R, mean, std);		//计算图像均值与方差
	double min = mean.at<double>(0) - 2 * std.at<double>(0);	//可以认为所有点基本都在正负2sigma之间，进行平衡化
	double max = mean.at<double>(0) + 2 * std.at<double>(0);

	//将图像拉伸映射到0-255
	for (int i = 0; i < R.rows; ++i) {
		for (int j = 0; j < R.cols; ++j) {
			R.at<float>(i*R.cols + j) = (R.at<float>(i*R.cols + j) - min) / (max - min) * 255;
		}
	}
	R.convertTo(dst, CV_8UC1);

}

//开闭运算处理函数
void openCloseProcess(Mat src, Mat& dst) {

	namedWindow("原二值化图像");
	namedWindow("开运算示意图");
	namedWindow("闭运算示意图");

	imshow("原二值化图像", src);

	Mat src_copy1, src_copy2;
	src.copyTo(src_copy1);
	src.copyTo(src_copy2);

	Size kernelSize = Size(5, 5);	//~~这个值可以修改
	Mat element = getStructuringElement(MORPH_RECT, kernelSize);
	
	morphologyEx(src_copy1, src_copy1, MORPH_OPEN, element);
	//morphologyEx(src_copy1, src_copy1, MORPH_CLOSE, element);
	imshow("开运算示意图", src_copy1);

	morphologyEx(src_copy2, src_copy2, MORPH_CLOSE, element);
	//morphologyEx(src_copy2, src_copy2, MORPH_OPEN, element);
	imshow("闭运算示意图", src_copy2);

	src_copy2.copyTo(dst);	//采用闭运算为后续区域合并做准备
}


