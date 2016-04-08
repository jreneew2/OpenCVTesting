// CannyStill.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>

using namespace cv;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

	Mat imgOriginal, imgResize, imgThreshold, imgContours;
	const Scalar LOWER_BOUNDS = Scalar(128, 128, 0),
		UPPER_BOUNDS = Scalar(198, 255, 166),
		RED = Scalar(0, 0, 255);
	Size resizeFactor = Size(320, 240);
	vector<vector<Point>> contours, selected;
	vector<Point> hull;
	vector<Vec4i> hierarchy;
	Point center, centerw;
	string fileExtension = ".jpg";
	string finalFileName;
	ofstream logFile;
	logFile.open("log.txt");
	VideoWriter writer;
	writer.open("output.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, resizeFactor, true);

	for (int i = 0; i < 542; i++) {
		contours.clear();
		selected.clear();
		finalFileName = to_string(i) + fileExtension;
		logFile << "fileName: " << finalFileName << endl;
		imgOriginal = imread(finalFileName);

		if (imgOriginal.empty()) {
			logFile << "error: image not read from file\n";
			continue;
		}

		//resize image to make it easier to process
		resize(imgOriginal, imgResize, resizeFactor, 0, 0, INTER_LINEAR);
		//filter to certain BGR values
		inRange(imgResize, LOWER_BOUNDS, UPPER_BOUNDS, imgThreshold);
		//clone threshold mat to contours mat so original isnt destroyed so we can display it
		imgContours = imgThreshold.clone();
		//finds contours
		findContours(imgContours, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		//prints number of unfiltered contours to log
		logFile << "number of unfiltered contours: " << contours.size() << endl;

		//filters contours on aspect ration and min area and solidity
		for (int i = 0; i < contours.size(); i++) {
			Rect rect = boundingRect(contours[i]);
			float aspect = (float)rect.width / (float)rect.height;

			//does solidity calculations
			convexHull(contours[i], hull);
			float area = contourArea(contours[i]);
			float hull_area = contourArea(hull);
			float solidity = (float)area / hull_area;

			if (aspect > 1 && rect.area() > 100 && (solidity >= .04 && solidity <= .4)) {
				selected.push_back(contours[i]);
			}
		}

		//draw rectangles on selected contours
		for (int i = 0; i < selected.size(); i++) {
			Rect rect = cv::boundingRect(selected[i]);
			rectangle(imgResize, rect.br(), rect.tl(), RED);
		}

		logFile << "selected size: " << selected.size() << endl;

		//draws center text on image and bounding rectangle around the target
		if (selected.size() == 1) {
			Rect rect = boundingRect(selected[0]);
			center = Point(rect.br().x - rect.width / 2 - 15, rect.br().y - rect.height / 2);
			centerw = Point(rect.br().x - rect.width / 2 - 15, rect.br().y - rect.height / 2 - 20);
			putText(imgResize, std::to_string(center.x), center, FONT_HERSHEY_PLAIN, 1, RED);
			putText(imgResize, std::to_string(center.y), centerw, FONT_HERSHEY_PLAIN, 1, RED);
			logFile << "center: " << center.x << ", " << center.y << endl;
			//outputs image to file
			//string outputFileName = "output" + finalFileName;
			//imwrite(outputFileName, imgResize);
			//outputs images to video
			writer.write(imgResize);
		}

		// declare windows
		//namedWindow("imgResize", CV_WINDOW_AUTOSIZE);
		//namedWindow("imgThreshold", CV_WINDOW_AUTOSIZE);
		//namedWindow("imgContours", CV_WINDOW_AUTOSIZE);

		// show windows
		//imshow(finalFileName, imgResize);
		//imshow("imgThreshold", imgThreshold);
		//imshow("imgContours", imgContours);

		//waitKey(0);

		//return(0);
	}
	logFile.close();
	return 0;
}