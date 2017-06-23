//#include "stdafx.h"
//#include <stdio.h>
//#include <iostream>
//#include <opencv2\opencv.hpp>
//#include<math.h>
//#include<queue>
//
//using namespace std;
//using namespace cv;
//
//Mat image = imread("G:\\conquest\\round1.jpg");
////Mat imagehsv(image.rows, image.cols, CV_8UC3);
//
////Mat image;// = imread("G:\\c1\\o1.jpg");
//Mat imagehsv(image.rows, image.cols, CV_8UC3);
//
//
//void detect(int event, int x, int y, int flag, void *data)
//{
//	if (event == EVENT_LBUTTONDOWN)
//	{
//		cout << "Row, Col" << y << x << endl;
//		cout << "H=" << (int)imagehsv.at<Vec3b>(y, x)[0] << endl;
//		cout << "S=" << (int)imagehsv.at<Vec3b>(y, x)[1] << endl;
//		cout << "V=" << (int)imagehsv.at<Vec3b>(y, x)[2] << endl << endl;
//	}
//}
//
//
//
//void main()
//{
//	//VideoCapture vc(2);
//
//	namedWindow("Window", WINDOW_NORMAL);
//
//	setMouseCallback("Window", detect, NULL);
//
//	while (1)
//	{
//		//vc >> image;
//		cout << "rows=" << image.rows << "cols=" << image.cols << endl;
//		cvtColor(image, imagehsv, CV_BGR2HSV);
//		imshow("Window", image);
//		imwrite("G:\\c1\\pic2.jpg", image);
//		waitKey(50);
//
//	}
//
//	waitKey(0);
//}