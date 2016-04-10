#include "TigerVision.h"

TigerVision::TigerVision(int resizeX = 320, int resizeY = 240) {
	imageSize = cv::Size(resizeX, resizeY);
	centerPixel = cv::Point(resizeX / 2, resizeY / 2);
	logFile.open(".\\log.txt");
	writer.open(".\\output.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, imageSize, true);
}

void TigerVision::InitCamera(int camId) {
	vidCap.open(camId);
}

void TigerVision::FindTarget() {
	for (int i = 0; i < 543; i++) {
		//resets 2D array of points for next time through loop
		contours.clear();
		selected.clear();
		
		//fileName
		finalFileName = std::to_string(i) + FILE_EXTENSION;
		logFile << "fileName: " << finalFileName << std::endl;

		//reads image from file
		imgOriginal = cv::imread(".\\images\\" + finalFileName);

		//if there is no file named current
		if (imgOriginal.empty()) {
			logFile << "error: image not read from file\n";
			continue;
		}

		//resizes it to desired Size
		cv::resize(imgOriginal, imgResize, imageSize, 0, 0, cv::INTER_LINEAR);
		//checks for RGB values in range
		cv::inRange(imgResize, LOWER_BOUNDS, UPPER_BOUNDS, imgThreshold);
		//clones image so we have a copy of the threshold matrix
		imgContours = imgThreshold.clone();
		//finds closed shapes within threshold image
		cv::findContours(imgContours, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		//prints number of unfiltered contours to log
		logFile << "number of unfiltered contours: " << contours.size() << std::endl;

		TigerVision::FilterContours();
		TigerVision::ShowTarget();

		logFile << "selected size: " << selected.size() << std::endl;

		if (selected.size() == 1) {
			cv::Rect targetRectangle = cv::boundingRect(selected[0]);
			centerX = targetRectangle.br().x - targetRectangle.width / 2;
			centerY = targetRectangle.br().y - targetRectangle.height / 2;
			targetCenter = cv::Point(centerX, centerY);
			cv::line(imgResize, centerPixel, targetCenter, RED);
			cv::circle(imgResize, targetCenter, 3, RED);
			TigerVision::DrawCoords(targetRectangle);
			degreesPerPixel = TigerVision::CalculatePixelToDegree();
			angleToTarget = TigerVision::CalculateAngleBetweenCameraAndPixel(degreesPerPixel);
			logFile << "center: " << centerX << ", " << centerY << std::endl;
			logFile << "angle to center: " << angleToTarget << std::endl;
		}

		outputFileName = "output" + finalFileName;
		cv::imwrite(".\\images\\output\\" + outputFileName, imgResize);
		writer.write(imgResize);
	}
	logFile.close();
}

void TigerVision::FilterContours() {
	//filters contours on aspect ration and min area and solidity
	for (int i = 0; i < contours.size(); i++) {
		cv::Rect rect = boundingRect(contours[i]);
		float aspect = (float)rect.width / (float)rect.height;

		//does solidity calculations
		convexHull(contours[i], hull);
		float area = contourArea(contours[i]);
		float hull_area = contourArea(hull);
		float solidity = (float)area / hull_area;

		if (aspect > ASPECT_RATIO && rect.area() > RECTANCLE_AREA_SIZE && (solidity >= SOLIDITY_MIN && solidity <= SOLIDITY_MAX)) {
			selected.push_back(contours[i]);
		}
	}
}

void TigerVision::ShowTarget() {
	//draw rectangles on selected contours
	for (int i = 0; i < selected.size(); i++) {
		cv::Rect rect = cv::boundingRect(selected[i]);
		cv::rectangle(imgResize, rect.br(), rect.tl(), RED);
	}
}

void TigerVision::DrawCoords(cv::Rect targetBoundingRect) {
	cv::Rect rect = targetBoundingRect;
	targetTextX = cv::Point(rect.br().x - rect.width / 2 - 15, rect.br().y - rect.height / 2 - 20);
	targetTextY = cv::Point(rect.br().x - rect.width / 2 - 15, rect.br().y - rect.height / 2);
	putText(imgResize, std::to_string(centerX), targetTextX, cv::FONT_HERSHEY_PLAIN, 1, RED);
	putText(imgResize, std::to_string(centerY), targetTextY, cv::FONT_HERSHEY_PLAIN, 1, RED);;
}

float TigerVision::CalculateAngleBetweenCameraAndPixel(float degreesPerPix) {
	float angle = (targetCenter.x - centerPixel.x) * degreesPerPix;
	return angle;
}

float TigerVision::CalculatePixelToDegree() {
	//calculate how many pixels across the diagonal
	float diagPixels = std::sqrt(std::pow(imageSize.width, 2) + std::pow(imageSize.height, 2));
	//calculate how many degrees across the screen
	float pixelToDegree = CAMERA_FOV / diagPixels;
	return pixelToDegree;
}

int main() {
	TigerVision tigerVision;
	tigerVision.FindTarget();
	return 0;
}