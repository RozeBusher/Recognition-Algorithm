#include <opencv2/opencv.hpp>  
#include <cv.h>  
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;

int Red = 160, Blue=130;
int sat = 1;
Mat CheckColor(Mat &inImg);
void Draw(Mat &inputImg, Mat foreImg);

int main(){
	VideoCapture capture("E:\\Purdue\\VS\\C++\\RedCar.avi");
	while (1){
		Mat light;
		capture >> light;
		if (light.empty()) break;
		namedWindow("Control", CV_WINDOW_AUTOSIZE);
		cvCreateTrackbar("Track", "Control", &Red, 10);
		cvCreateTrackbar("Saturation", "Control", &sat, 10);
		CheckColor(light);
		waitKey(1000);
	}
	return 0;
}

//Color check for light.
Mat CheckColor(Mat &inImg){
	Mat Img;
	Img.create(inImg.size(), CV_8UC1);
	Mat multiRGB[3];
	int a = inImg.channels();
	float B, G, R;
	split(inImg, multiRGB);
	for (int i = 0; i < inImg.rows; i++)
		for (int j = 0; j < inImg.cols; j++){			
			B = multiRGB[0].at<uchar>(i, j);
			G = multiRGB[1].at<uchar>(i, j);
			R = multiRGB[2].at<uchar>(i, j);
			float maxValue = max(B,max(G,R));
			float minValue = min(B,min(G,R));
			double S = (1 - 3.0*minValue / (R + G + B));
			if ((R > Red &&R >= G && R >= B && S >((255 - R) * sat / Red))|| (B > Blue &&B >= G && B >= R && S >((255 - B) * sat / Blue))) Img.at<uchar>(i, j) = 255;
			else Img.at<uchar>(i, j) = 0;
		} 
	medianBlur(Img, Img, 5);
	dilate(Img, Img, Mat(5, 5, CV_8UC1));
	Draw(inImg, Img);
	return Img;
}

//Draw track rectangles on graphical pictures.
void Draw(Mat &iptImg, Mat bImg){
	vector<vector<Point>> topo;
	findContours(bImg, topo, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Point pt1, pt2;
	float a = 0.4, b = 0.75, r;
	float xmin1 = a * iptImg.cols, ymin1 = iptImg.rows, xmax1 = 0, ymax1 = 0;
	float xmin2 = b * iptImg.cols, ymin2 = iptImg.rows, xmax2 = a * iptImg.cols, ymax2 = 0;
	float xmin3 = iptImg.cols, ymin3 = iptImg.rows, xmax3 = b * iptImg.cols, ymax3 = 0;
	Rect finalRect1, finalRect2, finalRect3, rect;
	Point2f c;
	vector<vector<Point> >::iterator iter = topo.begin();
	while (iter != topo.end()) {
		rect = boundingRect(*iter);
		minEnclosingCircle(*iter, c, r);
		if (rect.area() > 0) {
			pt1.x = rect.x;
			pt1.y = rect.y;
			pt2.x = pt1.x + rect.width;
			pt2.y = pt1.y + rect.height;
			if (pt2.x < a*iptImg.cols) {
				if (pt1.x < xmin1) xmin1 = pt1.x;
				if (pt1.y < ymin1) ymin1 = pt1.y;
				if (pt2.x > xmax1 && pt2.x < xmax2) xmax1 = pt2.x;
				if (pt2.y > ymax1) ymax1 = pt2.y;
			}
			if (pt2.x < b*iptImg.cols&&pt2.x > a*iptImg.cols) {
				if (pt1.x < xmin2 && pt1.x>xmin1) xmin2 = pt1.x;
				if (pt1.y < ymin2) ymin2 = pt1.y;
				if (pt2.x > xmax2 && pt2.x < xmax3) xmax2 = pt2.x;
				if (pt2.y > ymax2) ymax2 = pt2.y;
			}
			if (pt2.x < iptImg.cols&&pt2.x > b*iptImg.cols) {
				if (pt1.x < xmin3 && pt1.x>xmin2) xmin3 = pt1.x;
				if (pt1.y < ymin3) ymin3 = pt1.y;
				if (pt2.x > xmax3) xmax3 = pt2.x;
				if (pt2.y > ymax3) ymax3 = pt2.y;
			}
			++iter;
		}
		else iter = topo.erase(iter);
	}
	if (xmin1 == a * iptImg.cols&& ymin1 == iptImg.rows&&xmax1 == 0 && ymax1 == 0) xmin1 = ymin1 = xmax1 = ymax1 = 0;
	if (xmin2 == b * iptImg.cols&& ymin2 == iptImg.rows&& xmax2 == a * iptImg.cols&& ymax2 == 0) xmin2 = ymin2 = xmax2 = ymax2 = 0;
	if (xmin3 == iptImg.cols&&ymin3 == iptImg.rows&& xmax3 == b * iptImg.cols&& ymax3 == 0) xmin3 = ymin3 = xmax3 = ymax3 = 0;
	finalRect1 = Rect(xmin1, ymin1, xmax1 - xmin1, ymax1 - ymin1);
	finalRect2 = Rect(xmin2, ymin2, xmax2 - xmin2, ymax2 - ymin2);
	finalRect3 = Rect(xmin3, ymin3, xmax3 - xmin3, ymax3 - ymin3);
	rectangle(iptImg, finalRect1, Scalar(0, 255, 0));
	rectangle(iptImg, finalRect2, Scalar(0, 255, 0));
	rectangle(iptImg, finalRect3, Scalar(0, 255, 0));
	imshow("Detection", iptImg);
}