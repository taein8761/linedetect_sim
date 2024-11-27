#ifndef VISION_HPP
#define VISION_HPP
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void preprocess(VideoCapture& source, Mat& frame, Mat& gray, Mat& bin);
void detectObj(const Mat& bin, Point& mainPoint, Point& subPoint, Mat& outputImg, Mat& stats, Mat& centroids);
void markObj(const Mat& stats, const Mat& centroids, const Point& mainPoint, Mat& outputImg);

#endif //VISION_HPP
