//#include <opencv2\opencv.hpp>
//#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//
//using namespace std;
//using namespace cv;
//
//Mat frame;
//VideoCapture vc;
//
//int hl1 = 65, sl1 = 57, vl1 = 180;
//int hh1 = 97, sh1 = 140, vh1 = 225;
//
//int hl2 = 158, sl2 = 57, vl2 = 178;
//int hh2 = 177, sh2 = 125, vh2 = 247;
//
///* of=n arena
//int hl1=68, sl1=45, vl1=224;
//int hh1=77, sh1=60, vh1=245;
//
//int hl2 = 175, sl2 = 54, vl2 = 250;
//int hh2 = 180, sh2 = 75, vh2 = 255;
//*/
//void update_detected(int d, void *p)
//{
//	vc >> frame;
//	Mat temp(frame.rows, frame.cols, CV_8UC3, Scalar(0,0,0));
//	imshow("frame", frame);
//	Mat col1(frame.rows, frame.cols, CV_8UC1, Scalar(0));
//	Mat col2(frame.rows, frame.cols, CV_8UC1, Scalar(0));
//	Mat detected(frame.rows, frame.cols, CV_8UC1, Scalar(0));
//	cvtColor(frame, temp, CV_BGR2HSV);
//	inRange(temp, Scalar(hl1, sl1, vl1), Scalar(hh1, sh1, vh1), col1);
//	inRange(temp, Scalar(hl2, sl2, vl2), Scalar(hh2, sh2, vh2), col2);
//
//	
//	cout << "in function: \n";
//	//imshow("col1", col1);
//
//	for (int i = 0; i < frame.rows;i++)
//	for (int j = 0; j < frame.cols;j++)
//	if (col1.at<uchar>(i, j) == 255)
//		detected.at<uchar>(i, j) = 155;
//	else
//	if (col2.at<uchar>(i, j) == 255)
//		detected.at<uchar>(i, j) = 255;
//
//	imshow("Detected", detected);
//	waitKey(10);
//}
//
//int main()
//{
//     vc.open("http://10.3.152.72:8080/video?x.mjpeg");
//	//vc.open(2);
//	vc >> frame;
//	//imwrite("G:\\pics.jpg", frame);
//	//frame = imread("G:\\pics.jpg");
//	namedWindow("Change", WINDOW_AUTOSIZE);
//	namedWindow("Change1", WINDOW_AUTOSIZE);
//
//	createTrackbar("hl1", "Change", &hl1, 255, update_detected, NULL);
//	createTrackbar("sl1", "Change", &sl1, 255, update_detected, NULL);
//	createTrackbar("vl1", "Change", &vl1, 255, update_detected, NULL);
//	createTrackbar("hh1", "Change", &hh1, 255, update_detected, NULL);
//	createTrackbar("sh1", "Change", &sh1, 255, update_detected, NULL);
//	createTrackbar("vh1", "Change", &vh1, 255, update_detected, NULL);
//
//	createTrackbar("hl2", "Change1", &hl2, 255, update_detected, NULL);
//	createTrackbar("sl2", "Change1", &sl2, 255, update_detected, NULL);
//	createTrackbar("vl2", "Change1", &vl2, 255, update_detected, NULL);
//	createTrackbar("hh2", "Change1", &hh2, 255, update_detected, NULL);
//	createTrackbar("sh2", "Change1", &sh2, 255, update_detected, NULL);
//	createTrackbar("vh2", "Change1", &vh2, 255, update_detected, NULL);
//
//	while (frame.data)
//	{
//		update_detected(5, &hl1);
//	}
//	
//	imshow("frame", frame);
//	waitKey(0);
//}