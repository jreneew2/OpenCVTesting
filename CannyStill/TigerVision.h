#pragma once
#ifndef _TIGERVISION_H_INCLUDED
#define _TIGERVISION_H_INCLUDED

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<memory>

class TigerVision {
public:
	TigerVision(int resizeX, int resizeY);
	void InitCamera(int camId);
	void FindTarget();
	void FilterContours();
	void ShowTarget();
	void DrawCoords(cv::Rect targetBoundingRectangle);
private:
	cv::VideoCapture vidCap;
	cv::Mat imgOriginal, imgResize, imgThreshold, imgContours;
	cv::Size imageSize;
	std::vector<std::vector<cv::Point>> contours, selected;
	std::vector<cv::Point> hull;
	std::vector<cv::Vec4i> hierarchy;
	cv::Point targetTextX, targetTextY;

	const cv::Scalar LOWER_BOUNDS = cv::Scalar(128, 128, 0);
	const cv::Scalar UPPER_BOUNDS = cv::Scalar(198, 255, 166);
	const cv::Scalar RED = cv::Scalar(0, 0, 255);

	const int RECTANCLE_AREA_SIZE = 100;
	const float SOLIDITY_MIN = 0.04;
	const float SOLIDITY_MAX = 0.4;
	const int ASPECT_RATIO = 1;

	std::ofstream logFile;
	const std::string FILE_EXTENSION = ".jpg";
	std::string finalFileName;
	std::string outputFileName;

	cv::VideoWriter writer;

	int centerX;
	int centerY;
};

#endif