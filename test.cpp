/*#include <stdio.h>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <vector>
#include <math.h>
using namespace cv;
using namespace std;

float area(Point2f corners[])
{
	float a, b;
	a = sqrt(pow(corners[0].x - corners[1].x, 2) + pow(corners[0].y - corners[1].y, 2));
	b = sqrt(pow(corners[1].x - corners[2].x, 2) + pow(corners[1].y - corners[2].y, 2));
	return(a*b);
}

void foot()
{
	int i;
	VideoCapture vc(0);
	Mat img;
	vc >> img;
	int big_index = 0;
	Mat img2(img.rows, img.cols, CV_8UC3);
	Mat img3(img.rows, img.cols, CV_8UC3);
	int frame = 0;

	int lh = 0, uh = 96, ls = 0, us = 96, lv = 159, uv = 255, lt = 100;
	while (1)
	{

		vc >> img;

		//  if(!(frame++%5==0))continue;
		// cvtColor(img,img,CV_BGR2HSV);
		inRange(img, Scalar(lh, ls, lv), Scalar(uh, us, uv), img2);

		//erode(img2,img2,Mat(),Point(-1,-1),6);
		//dilate(img2,img2,Mat(),Point(-1,-1),4);
		//erode(img2,img2,Mat(),Point(-1,-1),4);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		imshow("threshold", img2);
		Canny(img2, img2, lt, lt * 3, 3);

		imshow("canny", img2);
		findContours(img2, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(-1, -1));

		Mat drawing = Mat::zeros(img2.size(), CV_8UC3);
		cout << contours.size() << endl;
		for (i = 0; (i< contours.size())/*&&arcLength(contours[i],true)>600; i++)
		{
			Scalar color = Scalar(255, 255, 255);
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
			break;
		}
		cout << i << endl;
		imshow("contours", drawing);

		if (char c = waitKey(50) == 'a')break;
		if (contours.size() == 0)continue;
		RotatedRect boundingBox = minAreaRect(contours[i]);


		Mat img3(img.rows, img.cols, CV_8UC3);
		Point2f corners[4];

		boundingBox.points(corners);



		line(img3, corners[0], corners[1], Scalar(255, 255, 255), 5);
		line(img3, corners[1], corners[2], Scalar(255, 255, 255), 5);
		line(img3, corners[2], corners[3], Scalar(255, 255, 255), 5);
		line(img3, corners[3], corners[0], Scalar(255, 255, 255), 5);

		//erode(img3,img3,Mat(),Point(-1,-1),2);
		//dilate(img3,img3,Mat(),Point(-1,-1),2);
		//erode(img3,img3,Mat(),Point(-1,-1),4);
		//imshow("Final",img3);


	}
}



int main()
{
	foot();


	waitKey(0);
	return 0;
}*/