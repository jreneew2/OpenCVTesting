#pragma once

#include "TargetInfo.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>

class TigerVision {
public:
	TigerVision(int imageSizeX, int imageSizeY);
	void InitCamera(int camId);
	void FindTarget();
	std::vector<std::vector<cv::Point>> FilterContours(const std::vector<std::vector<cv::Point>>& contours);
	void DrawInfo(const cv::Mat& imageToDrawTo, const TargetInfo& info);
	void DrawInfo(const cv::Mat& imageToDrawTo, const cv::Rect& rect);
	double CalculateAngleBetweenCameraAndPixel(const TargetInfo& info);
private:
	cv::VideoCapture vidCap;
	cv::Size imageSize;

	const cv::Scalar LOWER_BOUNDS = cv::Scalar(65, 186, 99);
	const cv::Scalar UPPER_BOUNDS = cv::Scalar(112, 255, 255);
	const cv::Scalar RED = cv::Scalar(0, 0, 255);
	const cv::Scalar PURPLE = cv::Scalar(255, 0, 255);

	const int RECTANCLE_AREA_SIZE = 100;
	const double SOLIDITY_MIN = .5;
	const double SOLIDITY_MAX = 1;
	const int ASPECT_RATIO = 1;
	const double CAMERA_FOV = 47;
	const double PI = 3.1415926535897;

	std::ofstream logFile;
	const std::string FILE_EXTENSION = ".jpg";

	cv::VideoWriter writer;
};