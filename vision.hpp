#ifndef VISION_HPP
#define VISION_HPP

#include <signal.h> // SIGINT와 signal() 선언 헤더
#include <iostream>
#include "opencv2/opencv.hpp"
#include "dxl.hpp"
using namespace std;
using namespace cv;

extern volatile bool ctrl_c_pressed;  // 전역 변수 선언 (main.cpp에서 정의됨)

void preprocess(VideoCapture& source, Mat& frame, Mat& gray, Mat& bin);
void detectObj(const Mat& bin, Point& mainPoint, Point& subPoint, Mat& outputImg, Mat& stats, Mat& centroids);
void markObj(const Mat& stats, const Mat& centroids, const Point& mainPoint, Mat& outputImg);
void signalHandler(int);

#endif //VISION_HPP
