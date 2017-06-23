//yellow square represent food and triangle represent wood.

#define botwait 70
#define pathwidth 15
#include <opencv2\opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include "tserial.h"
#include "bot_control.h"
#include <conio.h>
serial comm;

using namespace std;
using namespace cv;

int foodsc = 100;
int woodsc = 200;
int flag_found = 1;

struct HSV
{
	int h;
	int s;
	int v;
}fwi = { 25, 250, 185 }, fwf = { 35, 255, 195 }, obst = { 90, 254, 190 }, tc = { 15, 244, 93 };     //fw->HSV value of food & wood. rc1->marker1 color. rc2->marker2 color.

VideoCapture vc;
Mat bgim(486, 484, CV_8UC3); //stores background image.
Mat binary(bgim.rows, bgim.cols, CV_8UC1, Scalar(0)); //stores only image of food and wood in binary form.
Mat det_contour(bgim.rows, bgim.cols, CV_8UC3, Scalar(0, 0, 0)); //to display the detected contours.
Mat frame;                                            //stores image received from video.
Mat finalim(bgim.rows, bgim.cols, CV_8UC1, Scalar(0));
//Mat towncenter(bgim.rows, bgim.cols, CV_8UC1, Scalar(0));
Mat path(bgim.rows, bgim.cols, CV_8UC1, Scalar(0));

int fwth = 20;                                        //threshold for objects food & wood of +-10 in HSV.
int arcth = 30;
int obth = 20;                                        //threshold for obstacle HSV.
int tcx = bgim.cols / 2;
int tcy = bgim.rows / 2;
int countn = 0;
int robosizeth = 30;
int tcth = 10;                                        //threshold for towncenter.
int rcth = 10;                                        //threshold for marker on bot.
int reachth = 10;
int pathxchange;
float angleth = 2;                                  //threshold for angle deviation from path.
float path_angle;
int ath = 8;                                    
int skipframec = 0;
float angle_pos_target;
float angle_pos_src;
int reach_res = 0;

int hl1 = 65, sl1 = 57, vl1 = 180;
int hh1 = 97, sh1 = 140, vh1 = 225;

int hl2 = 158, sl2 = 57, vl2 = 178;
int hh2 = 177, sh2 = 125, vh2 = 247;

struct point1
{
	int x;
	int y;
	long double calc;                                      //comparing so as to get max score.
}obj;                                                //to store co-ordinates of centroid of objects.

struct bot_pos
{
	int xc;
	int yc;
	int hx;
	int hy;
	float angle;
}pos;

vector<point1> tempn;
vector<point1> n;

float calc_angle(Point p1, Point p2)                  
{
	float angle = 0;

	if (p1.x - p2.x == 0)
	if (p2.y>p1.y)
		angle = 3.14159 / 2;
	else
		angle = -3.14159 / 2;
	else
		angle = atan((float)(p2.y - p1.y) / (p2.x - p1.x));

	angle *= -1;
	if (p2.x - p1.x >= 0)
	{
		if (angle < 0)
			angle += 2 * 3.14159;
	}
	else
	{
		angle += 3.14159;
	}
	return angle;
}

void convert_ref_path_angle(float *angle)      //convert angle with reference to path_angle=180.
{
	float path_angle_opp = path_angle > 180 ? path_angle - 180 : path_angle + 180;
	cout << "path_angle_opp=" << path_angle_opp << "  ";
	*angle = *angle > path_angle_opp ? *angle - path_angle_opp : 360 + *angle - path_angle_opp;
	cout << "orientation=" << *angle <<" pos.angle="<<pos.angle<< endl;
}

void current_pos(Mat frame)
{
	flag_found = 1;
	pos.hx = 0; pos.hy = 0; int tx = 0, ty = 0, counth = 0, countt = 0;
	Mat temp(frame.rows, frame.cols, CV_8UC1, Scalar(0));
	Mat col1(frame.rows, frame.cols, CV_8UC1, Scalar(0)); //head color
	Mat col2(frame.rows, frame.cols, CV_8UC1, Scalar(0)); //tail color
	
	cvtColor(frame, frame, CV_BGR2HSV);
	inRange(frame,  Scalar(hl1, sl1, vl1), Scalar(hh1,sh1,vh1), col1);
	inRange(frame, Scalar(hl2, sl2, vl2), Scalar(hh2, sh2, vh2), col2);

	for (int i = 0; i < frame.rows; i++)
	for (int j = 0; j < frame.cols; j++)
	if (col1.at<uchar>(i, j) == 255)
	{
		temp.at<uchar>(i, j) = 155;
		pos.hx += j;
		pos.hy += i;
		counth++;
	}
	else
	if (col2.at<uchar>(i, j) == 255)
	{
		temp.at<uchar>(i, j) = 255;
		tx += j;
		ty += i;
		countt++;
	}
	else
		temp.at<uchar>(i, j) = 0;

	imshow("detected", temp);
	//waitKey(1);
	//cout << "temp(r,c)=" << temp.rows << " " << temp.cols << endl;
	if (counth == 0 || countt == 0)
	{
		cout << "count=0";
		flag_found = 0;
		//pos.xc = 0;
		//pos.yc = 0;
		//pos.angle = 3 * 3.14159;
		comm.send_data('s');
		return;
	}
	else
	{
		pos.hx /= counth; pos.hy /= counth;
		tx /= countt; ty /= countt;
		pos.angle = calc_angle(Point(tx, ty), Point(pos.hx, pos.hy));
	}
	
	pos.xc = (pos.hx + tx) / 2;
	pos.yc = (pos.hy + ty) / 2;
	pos.angle *= 180 / 3.14159;
	//cout << "  pos.angle=" << pos.angle << "  \n";
}

void turn_left_out(Mat direction, int i)                          //get back to path after rotating right.
{
	if (reach_res == 0)
	{
		angle_pos_src = calc_angle(Point(pos.xc, pos.yc), Point(tcx, tcy));
		angle_pos_target = calc_angle(Point(pos.xc, pos.yc), Point(n[i].x, n[i].y));
	}
	else
	{
		angle_pos_src = calc_angle(Point(pos.xc, pos.yc), Point(n[i].x, n[i].y));
		angle_pos_target = calc_angle(Point(pos.xc, pos.yc), Point(tcx, tcy));
	}

	angle_pos_src *= 180.0 / 3.1459;
	angle_pos_target *= 180.0 / 3.1459;
	convert_ref_path_angle(&angle_pos_src);
	convert_ref_path_angle(&angle_pos_target);
	float pos_angle = pos.angle;
	convert_ref_path_angle(&pos_angle);

	while ((pos_angle>angle_pos_target-angleth||pos_angle<angle_pos_src+angleth)&&direction.at<uchar>(pos.yc, pos.xc) != 200)
	{
		Mat direction1(frame.rows, frame.cols, CV_8UC1, Scalar(0));

		cout << "pos_angle=" << pos_angle << " angle_pos_src=" << angle_pos_src << " angle_pos_target=" << angle_pos_target << endl;
		cout << "out turn right  pos_angle=" << pos_angle << "  path_angle=" << path_angle << endl;; comm.send_data('r');
		vc >> frame;

		waitKey(botwait-20);
		
		comm.send_data('s');
		while (skipframec++ % 5)
		{
			vc >> frame;
			continue;
		}
		current_pos(frame);
		if (reach_res == 0)
		{
			angle_pos_src = calc_angle(Point(pos.xc, pos.yc), Point(tcx, tcy));
			angle_pos_target = calc_angle(Point(pos.xc, pos.yc), Point(n[i].x, n[i].y));
		}
		else
		{
			angle_pos_src = calc_angle(Point(pos.xc, pos.yc), Point(n[i].x, n[i].y));
			angle_pos_target = calc_angle(Point(pos.xc, pos.yc), Point(tcx, tcy));
		}

		angle_pos_src *= 180.0 / 3.1459;
		if (angle_pos_src > 180)
			return;
		angle_pos_target *= 180.0 / 3.1459;
		convert_ref_path_angle(&angle_pos_src);
		convert_ref_path_angle(&angle_pos_target);
		pos_angle = pos.angle;
		convert_ref_path_angle(&pos_angle);
		if (flag_found == 0)
			continue;
		line(direction1, Point(pos.xc, pos.yc), Point(pos.hx, pos.hy), 255, 2);    //joining centroid and head of bot.

		if (pos.xc>2 && pos.yc>2)
		for (int k = -2; k <= 2; k++)
		for (int l = -2; l <= 2; l++)
			direction1.at<uchar>(pos.yc + k, pos.xc + l) = 155;
		line(direction1, Point(n[i].x, n[i].y), Point(tcx, tcy), 200, pathwidth);         //drawing path.
		direction = direction1;
		imshow("direction inside", direction);
		waitKey(1);


		//cout << "pos=" << pos.xc << "," << pos.yc << endl;
	}
	while (direction.at<uchar>(pos.yc, pos.xc) != 200)
	{

		Mat direction1(frame.rows, frame.cols, CV_8UC1, Scalar(0));
		cout << "pos_angle=" << pos_angle << " angle_pos_src=" << angle_pos_src << " angle_pos_target=" << angle_pos_target << endl;

		cout << "out on left of path, move straight\n"; comm.send_data('f');
		//vc >> frame;
		waitKey(botwait-30);
		
		//imshow("frame:", frame);
		comm.send_data('s');
		while (skipframec++ % 5)
		{
			vc >> frame;
			continue;
		}
		current_pos(frame);
		pos_angle = pos.angle;
		convert_ref_path_angle(&pos_angle);
		
		if (flag_found == 0)
			continue;
		if (pos_angle > angle_pos_target || pos_angle<angle_pos_src)
			return;
		line(direction1, Point(pos.xc, pos.yc), Point(pos.hx, pos.hy), 255, 2);    //joining centroid and head of bot.

		if (pos.xc>2 && pos.yc>2)
		for (int k = -2; k <= 2; k++)
		for (int l = -2; l <= 2; l++)
			direction1.at<uchar>(pos.yc + k, pos.xc + l) = 155;
		line(direction1, Point(n[i].x, n[i].y), Point(tcx, tcy), 200, pathwidth);         //drawing path.
		imshow("direction inside", direction1);
		direction = direction1;
		waitKey(1);


	}
}

void turn_right_out(Mat direction, int i)                           //get back to path by rotating left.
{
	if (reach_res == 0)
	{
		angle_pos_src = calc_angle(Point(pos.xc, pos.yc), Point(tcx, tcy));
		angle_pos_target = calc_angle(Point(pos.xc, pos.yc), Point(n[i].x, n[i].y));
	}
	else
	{
		angle_pos_src = calc_angle(Point(pos.xc, pos.yc), Point(n[i].x, n[i].y));
		angle_pos_target = calc_angle(Point(pos.xc, pos.yc), Point(tcx, tcy));
	}

	angle_pos_src *= 180.0 / 3.1459;
	angle_pos_target *= 180.0 / 3.1459;
	convert_ref_path_angle(&angle_pos_src);
	convert_ref_path_angle(&angle_pos_target);
	float pos_angle = pos.angle;
	convert_ref_path_angle(&pos_angle);
	//if (abs(angle_pos_src - angle_pos_target) < 2 * angleth)
	//{

//	}

	while ((pos_angle<angle_pos_target+angleth||pos_angle>angle_pos_src-angleth)&&direction.at<uchar>(pos.yc,pos.xc)!=200)
	{
		cout << "path_angle=" << path_angle << "pos_angle=" << pos_angle << "pos.angle=" << pos.angle << endl;
		cout << "pos_angle=" << pos_angle << " angle_pos_src=" << angle_pos_src << " angle_pos_target=" << angle_pos_target << endl;

		Mat direction1(frame.rows, frame.cols, CV_8UC1, Scalar(0));
		cout << "out turn left    \n"; comm.send_data('l');
		//vc >> frame;
		waitKey(botwait-20);
		comm.send_data('s');
		while (skipframec++ % 5)
		{
			vc >> frame;
			continue;
		}

		current_pos(frame);
		if (reach_res == 0)
		{
			angle_pos_src = calc_angle(Point(pos.xc, pos.yc), Point(tcx, tcy));
			angle_pos_target = calc_angle(Point(pos.xc, pos.yc), Point(n[i].x, n[i].y));
		}
		else
		{
			angle_pos_src = calc_angle(Point(pos.xc, pos.yc), Point(n[i].x, n[i].y));
			angle_pos_target = calc_angle(Point(pos.xc, pos.yc), Point(tcx, tcy));
		}

		angle_pos_src *= 180.0 / 3.1459;
		if (angle_pos_src < 180)
			return;
		angle_pos_target *= 180.0 / 3.1459;
		convert_ref_path_angle(&angle_pos_src);
		convert_ref_path_angle(&angle_pos_target);
		pos_angle = pos.angle;
		convert_ref_path_angle(&pos_angle);
		if (flag_found == 0)
			continue;
		line(direction1, Point(pos.xc, pos.yc), Point(pos.hx, pos.hy), 255, 2);    //joining centroid and head of bot.

		if (pos.xc>2 && pos.yc>2)
		for (int k = -2; k <= 2; k++)
		for (int l = -2; l <= 2; l++)
			direction1.at<uchar>(pos.yc + k, pos.xc + l) = 155;
		line(direction1, Point(n[i].x, n[i].y), Point(tcx, tcy), 200, pathwidth);         //drawing path.
		direction = direction1;
		imshow("direction inside", direction);
		waitKey(1);
	}
	while (direction.at<uchar>(pos.yc, pos.xc) != 200)
	{
		cout << "pos_angle=" << pos_angle << " angle_pos_src=" << angle_pos_src << " angle_pos_target=" << angle_pos_target << endl;
		Mat direction1(frame.rows, frame.cols, CV_8UC1, Scalar(0));
		cout<<"out on right of path, move straight\n"; comm.send_data('f');
		//vc >> frame;
		waitKey(botwait-20);
		//imshow("frame:", frame);

		comm.send_data('s');
		while (skipframec++ % 5)
		{
			vc >> frame;
			continue;
		}

		current_pos(frame);
		pos_angle = pos.angle;
		convert_ref_path_angle(&pos_angle);
		if (flag_found == 0)
			continue;
		if (pos_angle<angle_pos_target || pos_angle>angle_pos_src)
			return;

		line(direction1, Point(pos.xc, pos.yc), Point(pos.hx, pos.hy), 255, 2);    //joining centroid and head of bot.

		if (pos.xc>2 && pos.yc>2)
		for (int k = -2; k <= 2; k++)
		for (int l = -2; l <= 2; l++)
			direction1.at<uchar>(pos.yc + k, pos.xc + l) = 155;
		line(direction1, Point(n[i].x, n[i].y), Point(tcx, tcy), 200, pathwidth);         //drawing path.
		direction = direction1;
		imshow("direction inside", direction1);
		waitKey(1);

	}
}

void extract_from_bg(void)
{
	vector<vector<Point>> tempcontours;
	vector<vector<Point>> contours;
	vector<Point> cnt;
	vector<Vec4i> hierarchy;
	Moments mu;
	//cout << "in extract from bg\n";
	cvtColor(bgim, bgim, CV_BGR2HSV);

	inRange(bgim, Scalar(obst.h - obth, obst.s - obth, obst.v - obth), Scalar(obst.h + obth, obst.s + obth, obst.v + obth), finalim);
	
	int xc = 0, yc = 0, count = 0;
	
	//imshow("obstacle start: ", finalim);

	Mat element = getStructuringElement(CV_SHAPE_RECT, Size(3, 3));
	dilate(finalim, finalim, element, Point(-1, -1), 1);
	erode(finalim, finalim, element, Point(-1, -1), 2);
	//cout << "after erode\n";
	for (int i = 0; i < finalim.rows; i++)
	{
		finalim.at<uchar>(i, 0) = 255; finalim.at<uchar>(i, finalim.cols - 1) = 255;
	}
	for (int j = 0; j < finalim.cols; j++)
	{
		finalim.at<uchar>(0, j) = 255; finalim.at<uchar>(finalim.rows - 1, j) = 255;
	}
	dilate(finalim, finalim, element, Point(-1, -1), int(robosizeth / 2) + 1);

	//imshow("obstacle dilated:  ", finalim);


	inRange(bgim, Scalar(fwi.h , fwi.s , fwi.v ), Scalar(fwf.h , fwf.s, fwf.v ), binary);
	//imshow("Binary: ", binary);
	findContours(binary, tempcontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	for (int i = 0; i < tempcontours.size(); i++)
	{
		if (arcLength(tempcontours[i], true) >= arcth)
		{

			approxPolyDP(Mat(tempcontours[i]), cnt, 0.1*arcLength(tempcontours[i], true), true);

			if (cnt.size() == 3)
			{
				mu = moments(Mat(tempcontours[i]), true);
				obj.x = mu.m10 / mu.m00;
				obj.y = mu.m01 / mu.m00;
				long double dist = sqrt((obj.x - tcx)*(obj.x - tcx) + (obj.y - tcy)*(obj.y - tcy));
				obj.calc = woodsc*10000.0 / dist;
				//cout << "Triangle found\n   centroid=" << obj.x << "," << obj.y << "    dist=" << dist << "  obj.calc=" << obj.calc << endl;
				tempn.push_back(obj);
				//cout << "\nfrom temp=" << tempn[tempn.size() - 1].calc << endl;
			}
			if (cnt.size() == 4)
			{
				mu = moments(Mat(tempcontours[i]), true);
				obj.x = mu.m10 / mu.m00;
				obj.y = mu.m01 / mu.m00;
				long double dist = sqrt((obj.x - tcx)*(obj.x - tcx) + (obj.y - tcy)*(obj.y - tcy));
				obj.calc = foodsc*10000.0 / dist;
				//cout << "Square found\n    centroid=" << obj.x << "," << obj.y <<"    dist="<<dist<<"  obj.calc="<<obj.calc<< endl;
				tempn.push_back(obj);
				//cout << "\nfrom temp=" << tempn[tempn.size() - 1].calc << endl;
			}
		}
	}

	//cout << "before tempn loop\n";
	for (int j = 0; j < tempn.size(); j++)
	{
		int p = 0;
		obj = tempn[0];
		for (int k = 0; k < tempn.size(); k++)
		if (obj.calc < tempn[k].calc)
		{
			obj = tempn[k];
			p = k;
		}
		n.push_back(obj);
		tempn[p].calc = 0;
		countn++;
		//cout << n.size() << " size of n\n";
	}
	//cout << "after tempn loop\n";
}

void forward(void)
{
	cout << "func continue forward\n"; comm.send_data('f');
	waitKey(botwait);
	comm.send_data('s');
}

void left(void)
{
	cout << "func turn left\n"; comm.send_data('l');
	cout << "\n\n";
	waitKey(botwait-30);
	comm.send_data('s');
}

void right(void)
{
	cout<<"func turn right\n"; comm.send_data('r');
	waitKey(botwait-30);
	comm.send_data('s');
}

void towncentre_res(int i)
{
	
	while (abs(n[i].x - pos.xc) > reachth || abs(n[i].y - pos.yc) > reachth)
	{
		Mat direction(frame.rows, frame.cols, CV_8UC1, Scalar(0));
		//Mat orig = bgim.clone();
		line(direction, Point(pos.xc, pos.yc), Point(pos.hx, pos.hy), 255, 2);    //joining centroid and head of bot.
		if (pos.xc > 2 && pos.yc > 2)
		for (int k = -2; k <= 2; k++)
		for (int l = -2; l <= 2; l++)
		{
			direction.at<uchar>(pos.yc + k, pos.xc + l) = 155;                    //marking the centroid of bot.
			//orig.at<Vec3b>(pos.yc + k, pos.xc + l)[0] = 0;
			//orig.at<Vec3b>(pos.yc + k, pos.xc + l)[1] = 255;
			//orig.at<Vec3b>(pos.yc + k, pos.xc + l)[2] = 255;

		}
		line(direction, Point(n[i].x, n[i].y), Point(tcx, tcy), 200, pathwidth);         //drawing path.
		imshow("direction inside", direction);
		//imshow("orig", orig);
		waitKey(1);
		if (pos.xc >= 0)
		if ((int)direction.at<uchar>(pos.yc, pos.xc) == 200)
		{
			if (abs(pos.angle - path_angle) < 180)
			{
				if (pos.angle<path_angle - ath)
					left();

				else
				if (pos.angle>path_angle + ath)
					right();
				else
					forward();
			}
			else
			{
				if (pos.angle<path_angle - ath)
					right();
				else
				if (pos.angle>path_angle + ath)
					left();
				else
					forward();
			}
		}
		else                                   //if bot centroid is out of path
		{
			cout << "out of path!  ";
			comm.send_data('s');
			float angle_tc_pos = calc_angle(Point(tcx, tcy),Point(pos.xc, pos.yc));
			angle_tc_pos *= 180 / 3.14159;
			convert_ref_path_angle(&angle_tc_pos);
			if (angle_tc_pos<180)                          //bot is out on the right side.
			{
				turn_right_out(direction,i);
			}
			else                                           //bot is out on left side.
			{
				turn_left_out(direction,i);
			}
		}
		while (skipframec++ % 5)
			vc >> frame;
		current_pos(frame);
	}
}

void resource_tc(int i)
{
	while (abs(tcx - pos.xc) > reachth || abs(tcy - pos.yc) > reachth)//checking nearness and condition for while loop
	{
	//	Mat orig=bgim.clone();
		Mat direction(frame.rows, frame.cols, CV_8UC1, Scalar(0));
		cout << "path_angle: " << path_angle << "\n";
		line(direction, Point(pos.xc, pos.yc), Point(pos.hx, pos.hy), 255, 2);    //joining centroid and head of bot.
		if (pos.xc > 2 && pos.yc > 2)// this part can be shifted to current pos
		for (int k = -2; k <= 2; k++)
		for (int l = -2; l <= 2; l++)
		{
			//orig.at<Vec3b>(pos.yc + k, pos.xc + l)[0] = 0;
			//orig.at<Vec3b>(pos.yc + k, pos.xc + l)[1] = 255;
			//orig.at<Vec3b>(pos.yc + k, pos.xc + l)[2] = 255;

			direction.at<uchar>(pos.yc + k, pos.xc + l) = 155;                    //marking the centroid of bot.
		}
		line(direction, Point(n[i].x, n[i].y), Point(tcx, tcy), 200, pathwidth);         //drawing path.
		imshow("direction inside", direction);
		//imshow("orig", orig);
		waitKey(1);
		if (pos.xc >= 0)
		if ((int)direction.at<uchar>(pos.yc, pos.xc) == 200)//if bot on the line
		{
			if (abs(pos.angle - path_angle) < 180)//bot pointing towrds target
			{
				if (pos.angle<path_angle - ath)
					left();

				else
				if (pos.angle>path_angle + ath)
					right();
				else
					forward();
			}
			else//bot pointing towards target in the other direction
			{
				if (pos.angle<path_angle - ath)
					right();
				else
				if (pos.angle>path_angle + ath)
					left();
				else
					forward();
			}
		}
		else                                   //if bot centroid is out of path
		{
			cout << "out of path!  ";
			comm.send_data('s');
			float angle_ref_pos = calc_angle(Point(n[i].x, n[i].y), Point(pos.xc, pos.yc));
			angle_ref_pos *= 180 / 3.14159;
			convert_ref_path_angle(&angle_ref_pos);
			if (angle_ref_pos < 180)
					turn_right_out(direction, i);            //bot is out on right side of path, seen standing on path facing Target.
			
			else
					turn_left_out(direction, i);            //bot is out on left side of path, seen standing on path facing Target.
		}
		while (skipframec++ % 8)
			vc >> frame;
		current_pos(frame);
	}
}
int main()
{
	comm.startDevice("COM6",9600);
	comm.send_data('s');
	vc.open("http://192.168.43.1:8080/video?x.mjpeg");
	//vc.open(2);
	//vc>>bgim;
	bgim = imread("G:\\conquest\\round1.jpg");
	
	imshow("bgim", bgim);
	extract_from_bg();
	cout << "after extract from bg function\n";
	vc >> frame;
	current_pos(frame);
	cout << "after calling current_pos func\n"; 
	tcx = pos.xc;            //initialising TC x,y values.
	tcy = pos.yc;
	cout << "size of n" << n.size() << endl;
	for (int i = 0; i < n.size(); i++)
	{
		reach_res = 0;
		cout << "Current target=" << n[i].x << "," << n[i].y<<endl;
		cout << "TC=" << tcx << "," << tcy << endl;
		if (n[i].x-tcx==0)
		  if (n[i].y>tcy)//finding initial path angle
			path_angle = 3.14159 / 2;
		  else
			path_angle = -3.14159 / 2;
		else
		{
			path_angle = -atan((n[i].y - tcy)*1.0 / (n[i].x - tcx));//finding the path angle
		}

		if (n[i].x - tcx >= 0)
		{
			if (path_angle<0)
				path_angle += 2 * 3.14159;
		}
		else
			path_angle += 3.14159;//as atan returns -ve value

		path_angle *= 180 / 3.14159;//converting rads to degrees
		cout << "Path_angle=" << path_angle << endl;
		
		towncentre_res(i);// town centre to resource
	//	imshow("Resource collected", frame);
		cout << i + 1 << " resource collected" << endl;

		comm.send_data('e');                                         //to blink LED.
		reach_res = 1;
		
		if (path_angle < 180)
			path_angle += 180;//setting path angle for return function
		else
			path_angle -= 180;

		resource_tc(i);// calling resource to town centre
	//	imshow("Back to town centre", frame);
		cout << "Back to Town Center";
		comm.send_data('e');                                        //to blink LED.
			while (skipframec++ % 5)
			{
				vc >> frame;
			}
			//imshow("frame", frame);
			comm.send_data('s');
			current_pos(frame);
			while (flag_found == 0)
			{
				vc >> frame;
				current_pos(frame);
			}
			cout << "Reached the yellow part.\n\n";//the hell man
			cout << "position of centroid=" << pos.xc << "," << pos.yc << "    yellow part=" << n[i].x << "," << n[i].y << endl;
		}    //end of while loop.
	waitKey(0);
}