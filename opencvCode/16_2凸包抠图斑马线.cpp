


#include <opencv.hpp>  
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;



class subRegion;
void recursiveDivide(subRegion originImage);	//每次divide的结果存在一个vector里
Mat findPossibleZebraArea(const Mat src, vector<subRegion> regionSequence);	//根据原图，和存储的vector进行复原
Mat findMaxArea(Mat bianryImg);

vector<subRegion> dividedRegion;
vector<string> regionIndex;

double calculatePolarity(const Mat src);
int OTSU(const Mat src);

class subRegion {
public:

	Mat regionMat;		//所在区域存储成相应的图像
	Point beginPoint, endPoint;	//这两个点确定了区域尺寸，
								//Point originPoint
	int width, height;
	int depth;		//当前子区域的层数/等级（0代表原始图像）
	string index;	//当前子区域的索引，左上-右上-右下-左下:0123
	bool isUniform;

	subRegion() { beginPoint = Point(-1, -1); endPoint = Point(-1, -1); }	//无参默认构造函数
	subRegion(Mat m) { 
		m.copyTo(regionMat); 
		beginPoint = Point(0, 0); 
		endPoint = Point(m.cols-1, m.rows-1); 
		width = m.cols;
		height = m.rows;
		depth = 0; 
		index = ""; 
		isUniform = false;
	}

	void showInfo(void);			//输出区域信息
	bool checkUniform(void);		//一致性检查函数

	void divide(subRegion &sub0, subRegion &sub1, subRegion &sub2, subRegion &sub3);
	subRegion &operator=(const subRegion &rhs) {
		rhs.regionMat.copyTo(regionMat);		//拷贝复制函数丢失了图像复制，bug了好久，特加注释以纪念
		beginPoint = rhs.beginPoint;
		endPoint = rhs.endPoint;
		depth = rhs.depth;
		index = rhs.index;
		isUniform = rhs.isUniform;
		return *this;
	}
};

void subRegion::showInfo(void) {	//输出区域信息函数
	cout << "depth: " << depth
		<< ".  Index: " << index
		<< ".  Region: " << beginPoint << " - " << endPoint
		<< ".  Uniform: " << checkUniform() << endl;
}

bool subRegion::checkUniform(void) {

	if (this->width >= 80)	//像素区域太大时直接不考虑
		return false;

	double gamma = calculatePolarity(this->regionMat);

	if (gamma >= 0.8)	//0.9，强限制；0.8，限制较弱
		return true;
	else
		return false;
}

//divide时需要给相应的信息，即：起始点、终止点、Mat格式图像
void subRegion::divide(subRegion &sub0, subRegion &sub1, subRegion &sub2, subRegion &sub3) {

	//每次分裂时增加层数，同时添加索引值
	sub0.depth = sub1.depth = sub2.depth = sub3.depth = this->depth + 1;
	sub0.index = this->index + "0";
	sub1.index = this->index + "1";
	sub2.index = this->index + "2";
	sub3.index = this->index + "3";

	//注意每个子区域的起止点都是对于原图来说的，但每个子区域的图像、宽和高都是独立的。
	//设置每一个子区域的起止点，同时更新子区域宽和高。由于起始点是(0,0)，所以有时需要将点-1。
	sub0.beginPoint = this->beginPoint;
	sub0.endPoint = Point((sub0.beginPoint.x + this->endPoint.x) / 2, (sub0.beginPoint.y + this->endPoint.y) / 2);
	sub0.width = sub0.endPoint.x - sub0.beginPoint.x+1;
	sub0.height = sub0.endPoint.y - sub0.beginPoint.y+1;
	sub0.regionMat = this->regionMat(Range(0, (this->height - 1) / 2), Range(0, (this->width - 1) / 2));
	sub0.isUniform = sub0.checkUniform();

	sub1.beginPoint = Point((this->beginPoint.x + this->endPoint.x) / 2 + 1, this->beginPoint.y);
	sub1.endPoint = Point(this->endPoint.x, (this->beginPoint.y + this->endPoint.y) / 2);
	sub1.width = sub1.endPoint.x - sub1.beginPoint.x+1;
	sub1.height = sub1.endPoint.y - sub1.beginPoint.y+1;
	sub1.regionMat = this->regionMat(Range(0, (this->height - 1) / 2), Range((this->width - 1) / 2 + 1, this->width-1));
	sub1.isUniform = sub1.checkUniform();

	sub2.beginPoint = Point((this->beginPoint.x + this->endPoint.x) / 2 + 1, (this->beginPoint.y + this->endPoint.y) / 2 + 1);
	sub2.endPoint = this->endPoint;
	sub2.width = sub2.endPoint.x - sub2.beginPoint.x+1;
	sub2.height = sub2.endPoint.y - sub2.beginPoint.y+1;
	sub2.regionMat = this->regionMat(Range((this->height - 1) / 2 + 1, this->height-1), Range((this->width - 1) / 2 + 1, this->width-1));
	sub2.isUniform = sub2.checkUniform();

	sub3.beginPoint = Point(this->beginPoint.x, (this->beginPoint.y+this->endPoint.y) / 2 + 1);
	sub3.endPoint = Point((this->beginPoint.x + this->endPoint.x) / 2, this->endPoint.y);
	sub3.width = sub3.endPoint.x - sub3.beginPoint.x+1;
	sub3.height = sub3.endPoint.y - sub3.beginPoint.y+1;
	sub3.regionMat = this->regionMat(Range((this->height - 1) / 2 + 1, this->height-1), Range(0, (this->width - 1) / 2));
	sub3.isUniform = sub3.checkUniform();
}

void main() {
	Mat src = imread("crossing.jpg", 0);
	imshow("原图", src);
	subRegion srcRegion(src);
	recursiveDivide(srcRegion);
	Mat possibleArea = findPossibleZebraArea(src, dividedRegion);

	Mat zebraAreaMask=findMaxArea(possibleArea);
	Mat dst = Mat::zeros(src.size(),src.type());
	src.copyTo(dst, zebraAreaMask);
	imshow("斑马线区域示意图", dst);

	waitKey(0);
}

#define MAX_ITERATION 3
//递归分解调用。
void recursiveDivide(subRegion father) {
	//father.isUniform = father.checkUniform();
	if ((father.isUniform == false) && father.depth<MAX_ITERATION) {	//限制一下递归次数，避免太多电脑受不了
		subRegion sub0, sub1, sub2, sub3;
		//将不符合一致性的区域分成四个子区域，并对每个子区域进行递归操作
		father.divide(sub0, sub1, sub2, sub3);
		recursiveDivide(sub0);
		recursiveDivide(sub1);
		recursiveDivide(sub2);
		recursiveDivide(sub3);
	}
	else {	//分解到底时，存储分解的每个图像
		father.showInfo();
		dividedRegion.push_back(father);		//存储线性分割的图像
		//namedWindow(father.index, WINDOW_FREERATIO);
		//imshow(father.index, father.regionMat);
		//regionIndex.push_back(father.index);
	}
}

//图象恢复。将每个符合条件的子区域作为掩码，将原图中对应区域抠出来。回传掩码图
Mat findPossibleZebraArea(const Mat src, vector<subRegion> regionSequence) {
	Mat showMat = Mat::zeros(src.size(), src.type());
	Mat mask = Mat::zeros(src.size(), src.type());
	Mat rectangleMat = Mat::zeros(src.size(), src.type());

	string id;
	subRegion region;

	for (auto it = regionSequence.begin(); it != regionSequence.end(); ++it) {
		region = *it;
		if (region.isUniform ==true) {
			//cout << region.beginPoint << "  " << region.endPoint << endl;
			rectangle(mask, region.beginPoint, region.endPoint, 255, CV_FILLED);	//可能区域填充掩码
			rectangle(rectangleMat, region.beginPoint, region.endPoint, 255, 1);
		}
	}

	src.copyTo(showMat, mask);
	//imshow("分割矩形示意图", rectangleMat);
	//imshow("展示可能区域", showMat);
	return mask;
}

double calculatePolarity(const Mat src) {

	int thresh = OTSU(src);	//获得分割阈值

	//亮和暗图像，以及相应掩码图
	Mat whitePart, whiteMask, blackPart, blackMask;

	threshold(src, whiteMask, thresh, 1, THRESH_BINARY);
	threshold(src, blackMask, thresh, 1, THRESH_BINARY_INV);
	src.copyTo(whitePart, whiteMask);
	src.copyTo(blackPart, blackMask);

	//imshow("白像素图", whitePart);
	//imshow("黑像素图", blackPart);

	/*//显示亮部和暗部的直方图
	showHist(src);
	showHist(whitePart,"白色直方图", whiteMask);
	showHist(blackPart, "黑色直方图",blackMask);
	*/

	//以下为双极系数gamma的计算
	Mat srcMean, srcSigma, whiteMean, whiteSigma, blackMean, blackSigma;
	double u0, u1, u2, sigma0, sigma1, sigma2, alpha, gamma;

	meanStdDev(src, srcMean, srcSigma, Mat());
	meanStdDev(whitePart, whiteMean, whiteSigma, whiteMask);
	meanStdDev(blackPart, blackMean, blackSigma, blackMask);

	sigma0 = srcSigma.at<double>(0);
	sigma1 = whiteSigma.at<double>(0);
	sigma2 = blackSigma.at<double>(0);
	u1 = whiteMean.at<double>(0);
	u2 = blackMean.at<double>(0);

	double n1 = 0, n2 = 0;
	for (int i = 0; i < src.rows; ++i) {
		for (int j = 0; j < src.cols; ++j) {
			if (src.at<uchar>(i, j) <= thresh)
				n1++;
			else
				n2++;
		}
	}
	alpha = n1 / (n1 + n2);

	gamma = 1 / (sigma0*sigma0)*alpha*(1 - alpha)*(u1 - u2)*(u1 - u2);

	cout << "该区域中双极系数gamma为：" << gamma << endl;
	return gamma;
}

int OTSU(const Mat srcImg) {

	if (srcImg.type() != CV_8UC1)
		while (1)
			cerr << "请输入8位灰度图像！" << endl;

	int T = 0;
	int totalNumber = srcImg.rows*srcImg.cols;
	double varValue = 0;
	double w0 = 0, w1 = 0, u0 = 0, u1 = 0;
	vector<int> grayLevel(256);

	//累加灰度等级
	for (int i = 0; i<srcImg.rows; i++)
		for (int j = 0; j<srcImg.cols; j++)
			grayLevel[srcImg.at<uchar>(i*srcImg.cols + j)]++;

	for (int i = 0; i < 255; ++i) {
		w1 = 0; w0 = 0; u0 = 0; u1 = 0;

		//计算背景相关
		for (int j = 0; j <= i; ++j) {
			w1 += grayLevel[j];
			u1 += j*grayLevel[j];
		}
		if (w1 == 0)
			continue;		//避免除数为0，但还要继续进行
		u1 = u1 / w1;
		w1 = w1 / totalNumber;

		//计算前景相关
		for (int j = i + 1; j <= 255; ++j) {
			w0 += grayLevel[j];
			u0 += j*grayLevel[j];
		}
		if (w0 == 0)
			continue;
		u0 = u0 / w0;
		w0 = w0 / totalNumber;

		double varValueNew = w0*w1*(u1 - u0)*(u1 - u0);
		if (varValueNew > varValue) {
			varValue = varValueNew;
			T = i;
		}
	}

	return T;
}

//寻找最大轮廓区域
Mat findMaxArea(Mat binaryImg) {
	vector<vector<Point>>contours;
	vector<Vec4i> hierarchy;
	
	//搜寻图像外边界
	findContours(binaryImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	//四叉树结果直接抠图
	Mat mask = Mat::zeros(binaryImg.size(), binaryImg.type());
	drawContours(mask, contours, -1, 255, FILLED);
	imshow("四叉树抠图区域", mask);

	//寻找凸包边界
	vector<vector<Point>>hull(contours.size());
	for (int i = 0; i < contours.size(); ++i)
		convexHull(Mat(contours[i]), hull[i], false);

	//绘制凸包掩码图
	Mat convexMask = Mat::zeros(binaryImg.size(), binaryImg.type());
	for (int i = 0; i < hull.size(); ++i)
		drawContours(convexMask, hull, i, 255, FILLED);
	imshow("凸包抠图区域", convexMask);

	//寻找最大区域
	double maxAreaValue = 0;
	int maxAreaIndex = 0;
	for (int index = 0; index >= 0; index = hierarchy[index][0]) {
		double area = contourArea(hull[index]);
		if (area > maxAreaValue) {
			maxAreaIndex = index;
			maxAreaValue = area;
		}
	}
	Mat maxConvexMask = Mat::zeros(binaryImg.size(), binaryImg.type());
	drawContours(maxConvexMask, hull, maxAreaIndex, 255, FILLED);	//仅绘制最大的凸包区域
	imshow("最大凸包掩码图", maxConvexMask);

	return maxConvexMask;
}
