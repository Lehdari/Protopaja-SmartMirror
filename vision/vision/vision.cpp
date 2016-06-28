// vision.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "vision.h"

#include <Windows.h>
#include <Kinect.h>

#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>


using namespace cv;


namespace Vision
{

	inline void CHECKERROR(HRESULT n) {
		if (!SUCCEEDED(n)) {
			std::stringstream ss;
			ss << "ERROR " << std::hex << n << std::endl;
			std::cin.ignore();
			std::cin.get();
			throw std::runtime_error(ss.str().c_str());
		}
	}

	// Safe release for interfaces
	template <typename Interface>
	inline void SAFERELEASE(Interface *& pInterfaceToRelease) {
		if (pInterfaceToRelease != nullptr) {
			pInterfaceToRelease->Release();
			pInterfaceToRelease = nullptr;
		}
	}

	IDepthFrameReader* depthFrameReader = nullptr; // depth reader

	void processIncomingData() {
		IDepthFrame *data = nullptr;
		IFrameDescription *frameDesc = nullptr;
		HRESULT hr = -1;
		UINT16 *depthBuffer = nullptr;
		USHORT nDepthMinReliableDistance = 0;
		USHORT nDepthMaxReliableDistance = 0;
		int height = 424, width = 512;

		hr = depthFrameReader->AcquireLatestFrame(&data);
		if (SUCCEEDED(hr)) hr = data->get_FrameDescription(&frameDesc);
		if (SUCCEEDED(hr)) hr = data->get_DepthMinReliableDistance(
			&nDepthMinReliableDistance);
		if (SUCCEEDED(hr)) hr = data->get_DepthMaxReliableDistance(
			&nDepthMaxReliableDistance);

		if (SUCCEEDED(hr)) {
			if (SUCCEEDED(frameDesc->get_Height(&height)) &&
				SUCCEEDED(frameDesc->get_Width(&width))) {
				depthBuffer = new UINT16[height * width];
				hr = data->CopyFrameDataToArray(height * width, depthBuffer);
				if (SUCCEEDED(hr)) {
					cv::Mat depthMap = cv::Mat(height, width, CV_16U, depthBuffer);
					cv::Mat img0 = cv::Mat::zeros(height, width, CV_8UC1);
					cv::Mat img1;
					double scale = 255.0 / (nDepthMaxReliableDistance -
						nDepthMinReliableDistance);
					depthMap.convertTo(img0, CV_8UC1, scale);
					applyColorMap(img0, img1, cv::COLORMAP_JET);
					cv::imshow("Depth Only", img1);
				}
			}
		}
		if (depthBuffer != nullptr) {
			delete[] depthBuffer;
			depthBuffer = nullptr;
		}
		SAFERELEASE(data);
	}

	int testMain(void) {
		HRESULT hr;
		IKinectSensor* kinectSensor = nullptr;     // kinect sensor

												   // initialize Kinect Sensor
		hr = GetDefaultKinectSensor(&kinectSensor);
		if (FAILED(hr) || !kinectSensor) {
			std::cout << "ERROR hr=" << hr << "; sensor=" << kinectSensor << std::endl;
			return -1;
		}

		CHECKERROR(kinectSensor->Open());

		// initialize depth frame reader
		IDepthFrameSource* depthFrameSource = nullptr;
		CHECKERROR(kinectSensor->get_DepthFrameSource(&depthFrameSource));
		CHECKERROR(depthFrameSource->OpenReader(&depthFrameReader));
		SAFERELEASE(depthFrameSource);

		while (depthFrameReader) {
			processIncomingData();
			int key = cv::waitKey(10);
			if (key == 'q') {
				break;
			}
		}

		// de-initialize Kinect Sensor
		CHECKERROR(kinectSensor->Close());
		SAFERELEASE(kinectSensor);
		return 0;

		/*
		Mat image;          //Create Matrix to store image
		VideoCapture cap;          //initialize capture
		cap.open(0);
		namedWindow("window", 1);          //create window to show image
		while (1) {
			cap >> image;				//copy webcam stream to image
			imshow("window", image);    //print image to screen
			waitKey(33);				//delay 33ms
		}
		return 0;*/
	}

}