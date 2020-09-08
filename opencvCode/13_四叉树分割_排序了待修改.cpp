
/*
可以优化的工作：
1、beginPoint与endPoint这两个点没有存在的意义，可以用 图像起始点+图像尺寸Size代替。
2、分割的过程中实时通过this->originPoint和this->size确定新分割图像的相应值，同时保留索引。
3、搜索时可以按照索引值（连续）或者图像起始点（格式统一）寻找，但还未确定用何种形式
4、不会合并……

*/


#include <opencv.hpp>  
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

// Point (int x, int y);

class subRegion;
void recursiveDivide(subRegion originImage);
void restorePicture(vector<subRegion> subRegionSet, Mat &dst,Size originImageSize);

//vector<subRegion> dividedRegion;
vector<string> regionIndex;


class subRegion{
public:

	Mat regionMat;		//所在区域存储成相应的图像
	Point beginPoint, endPoint;	//这两个点确定了区域尺寸，
	//Point originPoint
	int depth;		//当前子区域的层数/等级（0代表原始图像）
	string index;	//当前子区域的索引，左上-右上-右下-左下:0123
	
	
	subRegion() { beginPoint = Point(-1, -1); endPoint = Point(-1, -1); }	//无参默认构造函数
	subRegion(Mat m) { m.copyTo(regionMat); beginPoint = Point(0, 0); endPoint = Point(m.cols, m.rows); depth = 0; index = ""; }
	
	void showInfo(void);			//输出区域信息
	bool checkUniform(void);		//一致性检查函数

	void showSubMat(string subMatName) { imshow(subMatName, regionMat); }	//显示当前子区域的图像
	void divide(subRegion &sub0, subRegion &sub1, subRegion &sub2, subRegion &sub3);
};

void subRegion::showInfo(void) {	//输出区域信息函数
	cout << "depth: " << depth
		<< ".  Index: " << index
		<< ".  Region: " << beginPoint << " - " << endPoint 
		<< ".  Uniform: " << checkUniform() << endl;
}

bool subRegion::checkUniform(void) {
	double min, max;
	Mat gray;
	cvtColor(regionMat, gray, CV_RGB2GRAY);
	minMaxLoc(gray, &min, &max);	//minMaxLoc只能用单通道图处理
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


	//划分时是对于当前图像的
	Point relativeBeginPoint = Point (0, 0);
	Point relativeEndPoint = this->endPoint - this->beginPoint;

	//四个子区域分割原则。注意不要越界、不要重复划分、不要漏掉
	sub0.beginPoint = Point(relativeBeginPoint.x, relativeBeginPoint.y);
	sub0.endPoint = Point(relativeEndPoint.x / 2, relativeEndPoint.y / 2);
	sub0.regionMat = this->regionMat(Range(sub0.beginPoint.y, sub0.endPoint.y), Range(sub0.beginPoint.x, sub0.endPoint.x));

	sub1.beginPoint = Point(relativeEndPoint.x / 2 + 1, relativeBeginPoint.y);
	sub1.endPoint = Point(relativeEndPoint.x, relativeEndPoint.y / 2);
	sub1.regionMat = this->regionMat(Range(sub1.beginPoint.y, sub1.endPoint.y), Range(sub1.beginPoint.x, sub1.endPoint.x));

	sub2.beginPoint = Point(relativeEndPoint.x / 2 + 1, relativeEndPoint.y / 2 + 1);
	sub2.endPoint = Point(relativeEndPoint.x, relativeEndPoint.y);
	sub2.regionMat = this->regionMat(Range(sub2.beginPoint.y, sub2.endPoint.y), Range(sub2.beginPoint.x, sub2.endPoint.x));

	sub3.beginPoint = Point(relativeBeginPoint.x, relativeEndPoint.y / 2 + 1);
	sub3.endPoint = Point(relativeEndPoint.x / 2, relativeEndPoint.y);
	sub3.regionMat = this->regionMat(Range(sub3.beginPoint.y, sub3.endPoint.y), Range(sub3.beginPoint.x, sub3.endPoint.x)); 

}

bool compare_str(const string &a, const string &b) {

	if (a.size() == b.size()) {
		if (a.compare(b) == 0)	//相同的时候一定要返回false
			return false;
		else
			return  (a.compare(b) == -1 ? true : false);	//a的字符小于b，则true
	}
	else {
		return (a.size() > b.size());	//长的先排
	}
}

void main(){
	
	Mat src = imread("region.jpg",1);

	subRegion srcRegion(src);

	recursiveDivide(srcRegion);

	//排序显示当前的索引情况
	sort(regionIndex.begin(), regionIndex.end(), compare_str);
	for (auto it = regionIndex.begin(); it != regionIndex.end(); ++it) {
		cout << *it << endl;
	}

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
		//dividedRegion.push_back(father);		//存储线性分割的图像
		imshow(father.index, father.regionMat);
		regionIndex.push_back(father.index);
	}
}
//
//void restorePicture(vector<subRegion> subRegionSet, Mat &dst, Size originImageSize) {
//	
//}
