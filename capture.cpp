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
//int main()
//{
//	Mat frame;
//	VideoCapture vc("http://10.108.31.165:8080/video?x.mjpeg");
//	//vc.open(192.168.43.1:8080/browserfs.html?dummy=param.mjpg);
//	if (!vc.isOpened())  // if not success, exit program
//	{
//		cout << "Cannot open the video cam" << endl;
//		getchar();
//		return -1;
//	}
//
//	vc >> frame;
//
//	cout << "nr=" <<frame.rows << " nc=" << frame.cols << endl;
//	
//	while (1)
//	{
//		vc >> frame;
//		//frame = imread("http://192.168.43.1:8080/browserfs.html");
//		imshow("Window", frame);
//		waitKey(50);
//	}
//	return 0;
//}
