
#include "zebraProcess.h"
#include "main.h"

extern bool isZebraExist;

//在原图中指定mask区域寻找边界
Mat findBoundary(const Mat src, Mat mask, int thresh) {
	if (!isZebraExist) {
#ifndef NO_INFO_MODE
		cout << "[Warning] No zebra. -- Do not run \"findBoundary()\"." << endl;
#endif
		return Mat();
	}

	Mat boundary, blur, binImg;
	GaussianBlur(src, blur, Size(3, 3), 0);

	threshold(blur, binImg, thresh, 255, THRESH_BINARY);

	imshow("二值化图", binImg);

	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(binImg, binImg, MORPH_OPEN, element);
	morphologyEx(binImg, binImg, MORPH_CLOSE, element);
	

	imshow("开闭运算图", binImg);

	Canny(binImg, boundary, 3, 9, 3);

	Mat boundaryROI = Mat::zeros(src.size(), src.type());
	boundary.copyTo(boundaryROI, mask);

#ifndef FAST_MODE
	imshow("斑马线区域边沿提取", boundaryROI);
	//imwrite("Canny提取的边界图.jpg", boundary);
#endif
	return boundaryROI;
}


//在某个图中绘制(Mat格式的)边界
void drawBoundary(const Mat src, const Mat boundary,const string windowName,int lineThickness, Scalar color) {

	//扩宽边界区域，使看得更清楚
	if (lineThickness % 2 == 0)
		lineThickness++;
	Mat element2 = getStructuringElement(MORPH_RECT, Size(lineThickness, lineThickness));
	Mat boundaryCopy;
	dilate(boundary, boundaryCopy, element2);

	Mat srcCopy(src);	//避免修改src图像
	if(srcCopy.type()!=CV_8UC3)
		cvtColor(srcCopy, srcCopy, CV_GRAY2BGR);//转为彩图

	//如果图像对应边界点，则染成指定颜色
	for (int i = 0; i < srcCopy.rows; ++i) {
		for (int j = 0; j < srcCopy.cols; ++j) {
			if (boundaryCopy.at<uchar>(i, j) != 0) {
				srcCopy.at<Vec3b>(i, j)[0] = color[0];
				srcCopy.at<Vec3b>(i, j)[1] = color[1];
				srcCopy.at<Vec3b>(i, j)[2] = color[2];
			}
		}
	}

	imshow(windowName, srcCopy);
	imwrite(windowName+".jpg", srcCopy);
}


//找直线
#define MIN_LINE_NUMBER 8
#define MAX_LINE_NUMBER 16
vector<Vec4i> findLines(const Mat contourImg) {

	if (!isZebraExist) {
#ifndef NO_INFO_MODE
		cout << "[Warning] No zebra. Do not run \"findLines()\"." << endl;
#endif
		vector<Vec4i> tmpVector;
		return tmpVector;
	}

	if(contourImg.type()!=CV_8UC1)
		cvtColor(contourImg, contourImg, CV_BGR2GRAY);
	
	vector<Vec4i> lines;
	int minLength = 20;
	int minLineGap = 10;	//40
	int rho = 1;
	double theta = CV_PI / 180;
	int thresholdMax = (contourImg.rows+ contourImg.cols)/2, thresholdMin = 20;		//迭代进行阈值分割的初始条件
	int threshold = (thresholdMax + thresholdMin) / 2;
	while ((lines.size() <MIN_LINE_NUMBER || lines.size()>MAX_LINE_NUMBER) && (thresholdMax - thresholdMin) > 1) {
		if (lines.size() > MAX_LINE_NUMBER) {	//直线过多，提高阈值
			thresholdMin = threshold;
			threshold = (threshold + thresholdMax) / 2;
		}
		else {			//少于指定根数，阈值过大。二分法降低阈值
			thresholdMax = threshold;
			threshold = (threshold + thresholdMin) / 2;
		}
		//检查边沿提取图像中的直线
		HoughLinesP(contourImg, lines, rho, theta, threshold, minLength, minLineGap);
	}

	if (lines.size() < MIN_LINE_NUMBER) {
		isZebraExist = false;
#ifndef NO_INFO_MODE
		cout << "[Warning] No zebra. -- Cannot find enough lines in max ROI." << endl;
#endif
		vector<Vec4i> tmpVector;
		return tmpVector;
	}

	Mat lineMat = Mat::zeros(contourImg.size(), contourImg.type());
	for (int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		line(lineMat, Point(l[0], l[1]), Point(l[2], l[3]), 255, 1, LINE_AA);
	}

#ifndef FAST_MODE
	imshow("直线提取图像", lineMat);
	imwrite("直线提取图像.jpg", lineMat);
#endif
	return lines;
}

//在某个图中绘制直线
void drawLines(const Mat src, const vector<Vec4i> lines,const string windowName,int lineThickness, Scalar color) {
	Mat srcCopy(src);
	if (srcCopy.type() != CV_8UC3)
		cvtColor(srcCopy, srcCopy, CV_GRAY2BGR);
	for (int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		line(srcCopy, Point(l[0], l[1]), Point(l[2], l[3]), color, lineThickness, LINE_AA);
	}
#ifndef FAST_MODE
	imshow(windowName, srcCopy);
#endif
	imwrite(windowName + ".jpg", srcCopy);
}

//寻找平行线
#define PARALLEL_TORLERANCE 10		//这个值是平行线间误差容忍度（角度）
vector<Vec4i> findParallelLines(const vector<Vec4i> lines) {
	
	if (!isZebraExist) {
#ifndef NO_INFO_MODE
		cout << "[Warning] No zebra. Do not run \"findParallelLines()\"." << endl;
#endif
		vector<Vec4i> tmpVector;
		return tmpVector;
	}

	vector<Vec4i> parallelLines;
	vector<double> linesAngle;				//记录每根直线的角度
	vector<int> linesVotes(lines.size(),0);	//每根直线可能是平行线的投票

	//计算每一条直线角度
	for (int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		Point lineBegin = Point(l[0], l[1]);
		Point lineEnd = Point(l[2], l[3]);
		double line_x = l[2] - l[0];
		double line_y = l[3] - l[1];

		//用反正弦，因为反余弦会出现正负映射到相同值
		double angle = asin((line_x * 0 - line_y * 1) / sqrt(line_x*line_x + line_y*line_y)) * 180 / CV_PI;
		linesAngle.push_back(angle);
		//cout << "Line from " << lineBegin << " to " << lineEnd << ". Angle: " << angle << endl;
	}

	for (int i = 0; i < linesAngle.size(); ++i) {
		for (int j = 0; j < linesAngle.size(); ++j) {
			double rawDiff = linesAngle.at(i) - linesAngle.at(j);
			double angleDiff =min(abs(rawDiff),abs(180-rawDiff));
			//cout << "Line angle between no." << i << " and no. " << j << " is: " << angleDiff<<endl;
			if (angleDiff < PARALLEL_TORLERANCE)
				linesVotes.at(i)++;
		}
	}

	int countThresh = lines.size() / 2;		//过半数认为找对了
	for (int i = 0; i < lines.size(); ++i) {
		if (linesVotes.at(i) > countThresh)
			parallelLines.push_back(lines[i]);
	}

#define MIN_PARALLEL_NUMBER 4
	if (parallelLines.size() <= MIN_PARALLEL_NUMBER) {
		isZebraExist = false;
#ifndef NO_INFO_MODE
		cout << "[Warning] No zebra. -- Cannot find enough paralled lines." << endl;
#endif
		vector<Vec4i> tmpVector;
		return tmpVector;
	}

	//cout << "After checking, the parallel lines are: " << endl;
	for (int i = 0; i < parallelLines.size(); ++i) {
		Vec4i l = parallelLines[i];
		Point lineBegin = Point(l[0], l[1]);
		Point lineEnd = Point(l[2], l[3]);
		double line_x = l[2] - l[0];
		double line_y = l[3] - l[1];
		double angle = acos((line_x * 1 + line_y * 0) / sqrt(line_x*line_x + line_y*line_y)) * 180 / CV_PI;
		//cout << "Line from " << lineBegin << " to " << lineEnd << ". Angle: " << angle << endl;
	}

	return parallelLines;
}



Vec4i findCoPointLines(const vector<Vec4i> lines) {
	if (lines.size()==0) {
#ifndef NO_INFO_MODE
		cout << "[Error] No lines . Do not run \"findCoPoinLines()\"." << endl;
#endif
		Vec4i emptyLines;
		return emptyLines;
	}

	vector<Point> kbVector;	//用于存储 斜率-截距 的point
	
#define K_SCALAR 400		//由于斜率K是小数，所以加入缩放。由于图像宽300，所以 最小0.003
#define MAT_SIZE 2000		//绘图用图像尺寸
 	Mat drawImg = Mat::zeros(Size(MAT_SIZE, MAT_SIZE), CV_8UC3);
	int offsetX = drawImg.cols / 2, offsetY = drawImg.rows / 2;
	Point offsetPoint(offsetX, offsetY);	//偏移点用来将图像绘制到中央

	for (int i = 0; i < lines.size(); ++i) {
		Point lineBegin = Point(lines[i][0], lines[i][1]);
		Point lineEnd = Point(lines[i][2], lines[i][3]);

		int deltaX, deltaY;
		deltaX = lineEnd.x - lineBegin.x;
		deltaY = lineEnd.y - lineBegin.y;

		if (deltaX == 0)	//避免分母为0。宽度为1与0看起来没啥区别
			deltaX = 1;
		double k = static_cast<double>(deltaY) / deltaX;
		double b = lineEnd.y - (lineEnd.x*k);

		//kbVector.push_back(Point(k*K_SCALAR, b));		//存储斜率与截距的值
		kbVector.push_back(Point(static_cast<int>(k*K_SCALAR), b));		//存储斜率与截距的值
		
		circle(drawImg, Point(k*K_SCALAR, b)+ offsetPoint, 10, Scalar(0, 0, 255), -1);

		cout << "第 " << i << " 条直线起止点：" << lineBegin << ", " << lineEnd << "斜率:" << k << "  截距 : " << b << endl;
	}

	Mat kbCoordinateImg = Mat::zeros(Size(MAT_SIZE, MAT_SIZE), CV_8UC3);
	Vec4f linePara;
	//fitLine(kbVector, linePara, DIST_L2, 0, 1e-2, 1e-2);	//拟合曲线
	/*    DIST_USER    = -1,  //!< User defined distance
    DIST_L1      = 1,   //!< distance = |x1-x2| + |y1-y2|
    DIST_L2      = 2,   //!< the simple euclidean distance
    DIST_C       = 3,   //!< distance = max(|x1-x2|,|y1-y2|)
    DIST_L12     = 4,   //!< L1-L2 metric: distance = 2(sqrt(1+x*x/2) - 1))
    DIST_FAIR    = 5,   //!< distance = c^2(|x|/c-log(1+|x|/c)), c = 1.3998
    DIST_WELSCH  = 6,   //!< distance = c^2/2(1-exp(-(x/c)^2)), c = 2.9846
    DIST_HUBER   = 7    //!< distance = |x|<c ? x^2/2 : c(|x|-c/2), c=1.345*/
	fitLine(kbVector, linePara, DIST_FAIR, 0, 1e-2, 1e-2);	//拟合曲线
	
	//计算直线的端点(y = k(x - x0) + y0)  
	Point point0, point1, point2;
	point0.x = linePara[2];
	point0.y = linePara[3];
	double k_kbFrame = linePara[1] / linePara[0];
	double b_kbFrame = point0.y + k_kbFrame*(0 - point0.x);

	double x0 = - k_kbFrame*K_SCALAR;
	double y0 = b_kbFrame ;

	for (int i = 0; i < kbVector.size(); ++i) {
		cout << i<<"-Point " << kbVector[i] << endl;
	}

	cout << "交点是：( " << x0 << " , " << y0 << " )" << endl;

	point1.x = 0;
	point1.y = k_kbFrame * (0 - point0.x) + point0.y;
	point2.x = MAT_SIZE;
	point2.y = k_kbFrame * (MAT_SIZE - point0.x) + point0.y;


	//绘制拟合直线 与 端点
	line(drawImg, point1+ offsetPoint, point2 + offsetPoint, Scalar(255, 255, 255), 1, 8, 0);
	circle(drawImg, point1+ offsetPoint, 50, Scalar(255, 0, 0), 5);
	circle(drawImg, point2 + offsetPoint, 50, Scalar(255, 0, 0), 5);
	
	namedWindow("坐标图",WINDOW_FREERATIO);
	//imshow("坐标图", drawImg);
	//imwrite("kb坐标图.jpg", drawImg);

	//直线拟合
	
	Point crossPoint = Point(x0, y0);

	for (int i = 0; i < lines.size(); ++i) {
		double dis = calPoint2LineDistance(crossPoint, lines[i]);
		cout << "点 " << crossPoint << " 到第 " << i << " 条直线的距离是：" << dis << endl;
	}

	Vec4i v;
	return v;
}


double calPoint2LineDistance(const Point point, const Vec4i lines) {
	double a, b, c, dis;
	Point point1 = Point(lines[0], lines[1]);
	Point point2 = Point(lines[2], lines[3]);

	// 化简两点式为一般式
	// 两点式公式为(y - y1)/(x - x1) = (y2 - y1)/ (x2 - x1)
	// 化简为一般式为(y2 - y1)x + (x1 - x2)y + (x2y1 - x1y2) = 0
	// A = y2 - y1
	// B = x1 - x2
	// C = x2y1 - x1y2

	a = point2.y - point1.y;
	b = point1.x - point2.x;
	c = point2.x*point1.y - point1.x*point2.y;
	dis = abs(a*point.x + b*point.y + c) / sqrt(a*a + b*b);
	//double tmp1= sqrt(a*a + b*b);
	//double tmp2 = abs(a*static_cast<double>(point1.x) + b*point1.y + c);
	//double tmp3 = a*point1.x + b*point1.y + c;
	//dis = tmp2 / tmp1;
	return dis;
}

