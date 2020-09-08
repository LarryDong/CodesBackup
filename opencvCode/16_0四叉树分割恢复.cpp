


#include <opencv.hpp>  
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

// Point (int x, int y);

class subRegion;
void recursiveDivide(subRegion originImage);
Mat restorePicture(const Mat src, vector<subRegion> regionSequence);

vector<subRegion> dividedRegion;
vector<string> regionIndex;


class subRegion {
public:

	Mat regionMat;		//所在区域存储成相应的图像
	Point beginPoint, endPoint;	//这两个点确定了区域尺寸，
								//Point originPoint
	int width, height;
	int depth;		//当前子区域的层数/等级（0代表原始图像）
	string index;	//当前子区域的索引，左上-右上-右下-左下:0123


	subRegion() { beginPoint = Point(-1, -1); endPoint = Point(-1, -1); }	//无参默认构造函数
	subRegion(Mat m) { 
		m.copyTo(regionMat); 
		beginPoint = Point(0, 0); 
		endPoint = Point(m.cols-1, m.rows-1); 
		width = m.cols;
		height = m.rows;
		depth = 0; 
		index = ""; 
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
	double min, max;
	//Mat gray
	//cvtColor(regionMat, gray, CV_RGB2GRAY);
	minMaxLoc(this->regionMat, &min, &max);	//minMaxLoc只能用单通道图处理
	if (max - min >= 5)	//一致性检查准则，目前用差值
		return false;
	else
		return true;
}

//divide时需要给相应的信息，即：起始点、终止点、Mat格式图像
void subRegion::divide(subRegion &sub0, subRegion &sub1, subRegion &sub2, subRegion &sub3) {

	//每次分裂时增加层数，同时添加索引值
	sub0.depth = sub1.depth = sub2.depth = sub3.depth = this->depth + 1;
	sub0.index = this->index + "0";
	sub1.index = this->index + "1";
	sub2.index = this->index + "2";
	sub3.index = this->index + "3";

	sub0.beginPoint = this->beginPoint;
	sub0.endPoint = Point((sub0.beginPoint.x + this->endPoint.x) / 2, (sub0.beginPoint.y + this->endPoint.y) / 2);
	sub0.width = sub0.endPoint.x - sub0.beginPoint.x+1;
	sub0.height = sub0.endPoint.y - sub0.beginPoint.y+1;
	sub0.regionMat = this->regionMat(Range(0, (this->height - 1) / 2), Range(0, (this->width - 1) / 2));
	
	sub1.beginPoint = Point((this->beginPoint.x + this->endPoint.x) / 2 + 1, this->beginPoint.y);
	sub1.endPoint = Point(this->endPoint.x, (this->beginPoint.y + this->endPoint.y) / 2);
	sub1.width = sub1.endPoint.x - sub1.beginPoint.x+1;
	sub1.height = sub1.endPoint.y - sub1.beginPoint.y+1;
	sub1.regionMat = this->regionMat(Range(0, (this->height - 1) / 2), Range((this->width - 1) / 2 + 1, this->width-1));

	sub2.beginPoint = Point((this->beginPoint.x + this->endPoint.x) / 2 + 1, (this->beginPoint.y + this->endPoint.y) / 2 + 1);
	sub2.endPoint = this->endPoint;
	sub2.width = sub2.endPoint.x - sub2.beginPoint.x+1;
	sub2.height = sub2.endPoint.y - sub2.beginPoint.y+1;
	sub2.regionMat = this->regionMat(Range((this->height - 1) / 2 + 1, this->height-1), Range((this->width - 1) / 2 + 1, this->width-1));

	sub3.beginPoint = Point(this->beginPoint.x, (this->beginPoint.y+this->endPoint.y) / 2 + 1);
	sub3.endPoint = Point((this->beginPoint.x + this->endPoint.x) / 2, this->endPoint.y);
	sub3.width = sub3.endPoint.x - sub3.beginPoint.x+1;
	sub3.height = sub3.endPoint.y - sub3.beginPoint.y+1;
	sub3.regionMat = this->regionMat(Range((this->height - 1) / 2 + 1, this->height-1), Range(0, (this->width - 1) / 2));
}

void main() {

	Mat src = imread("divide.jpg", 0);
	imshow("原图", src);

	subRegion srcRegion(src);
	
	recursiveDivide(srcRegion);


	restorePicture(src,dividedRegion);

	waitKey(0);
}


//递归分解调用。
void recursiveDivide(subRegion father) {

	//if ((father.checkUniform() == false) && (father.endPoint.x-father.beginPoint.x)>150) {	//限制一下递归次数，避免太多
	if ((father.checkUniform() == false) && father.depth<3) {	//限制一下递归次数，避免太多电脑受不了
		subRegion sub0, sub1, sub2, sub3;
		father.divide(sub0, sub1, sub2, sub3);
		recursiveDivide(sub0);
		recursiveDivide(sub1);
		recursiveDivide(sub2);
		recursiveDivide(sub3);
	}
	else {
		father.showInfo();
		dividedRegion.push_back(father);		//存储线性分割的图像
		//namedWindow(father.index, WINDOW_FREERATIO);
		//imshow(father.index, father.regionMat);
		regionIndex.push_back(father.index);
	}
}


Mat restorePicture(const Mat src, vector<subRegion> regionSequence) {
	Mat dst = Mat::zeros(src.size(), src.type());
	Mat mask = Mat::zeros(src.size(), src.type());

	string id;
	subRegion region;
	cout << "REGIONSIZE:" << regionSequence.size() << endl;
	for (auto it = regionSequence.begin(); it != regionSequence.end(); ++it) {
		region = *it;
		for (auto str = region.index.begin(); str != region.index.end(); ++str) {

		}
		if (region.checkUniform()==true) {
			cout << region.beginPoint << "  " << region.endPoint << endl;

			rectangle(dst, Rect(region.beginPoint, region.endPoint), 255, CV_FILLED);
		}
	}
	//namedWindow("效果图", WINDOW_FREERATIO);
	imshow("效果图", dst);

	return dst;
}
