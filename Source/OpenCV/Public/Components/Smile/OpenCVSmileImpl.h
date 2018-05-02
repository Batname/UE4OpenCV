#pragma once

#include <future>
#include <iostream>

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "OpenCVSmileImpl.generated.h"

USTRUCT(BlueprintType)
struct FOpenCVSmileFrame
{
	GENERATED_BODY()

	/** The confidence score of the tracking data, ranging from 0 to 100. */
	UPROPERTY(BlueprintReadOnly, Category = "FOpenCVSmileFrame")
	float IntensityZeroOne;

	FOpenCVSmileFrame()
		: IntensityZeroOne(0.f)
	{}
};

class FOpenCVSmileImpl
{
public:

	FOpenCVSmileImpl(std::string CascadeName, std::string NestedCascadeName);
	FOpenCVSmileImpl(FOpenCVSmileImpl &&) {}
	FOpenCVSmileImpl(const FOpenCVSmileImpl&) = default;
	//~FOpenCVSmileImpl();

	static FOpenCVSmileImpl* Get(FString CascadeName, FString NestedCascadeName);
	static FOpenCVSmileImpl* OpenCVSmileImplPtr;

public:
	static void StartCamera()
	{
		if (OpenCVSmileImplPtr != nullptr)
		{
			OpenCVSmileImplPtr->StartCameraInst();
		}
	}

	static void StopCamera()
	{
		if (OpenCVSmileImplPtr != nullptr)
		{
			OpenCVSmileImplPtr->StopCameraInst();
		}
	}

	// Let program destroy object after exit
	static void Destroy()
	{
		//if (OpenCVSmileImplPtr != nullptr)
		//{
		//	delete OpenCVSmileImplPtr;
		//	OpenCVSmileImplPtr = nullptr;
		//}
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~ Camera thread ~~~~~~~~~~~~~~~~~~~~~~~*/
public:
	void StartCameraInst();
	void StopCameraInst();
	const FOpenCVSmileFrame& GetOpenCVSmileFrame() { return OpenCVSmileFrame; }

private:
	std::atomic_bool bIsSmileThreadRunning;
	std::thread CameraThread;
	std::mutex mutex;

	FOpenCVSmileFrame OpenCVSmileFrame;

	std::string CascadeName;
	std::string NestedCascadeName;

private:
	void RunCameraThread();
	void DetectSmile(cv::Mat& img, cv::CascadeClassifier& cascade, cv::CascadeClassifier& nestedCascade);

private:
	cv::VideoCapture capture;
	cv::Mat frame, image;
	cv::CascadeClassifier cascade, nestedCascade;

	std::vector<cv::Rect> faces, faces2;
	std::vector<cv::Rect> nestedObjects;
	cv::Scalar colors[8] =
	{
		cv::Scalar(255,0,0),
		cv::Scalar(255,128,0),
		cv::Scalar(255,255,0),
		cv::Scalar(0,255,0),
		cv::Scalar(0,128,255),
		cv::Scalar(0,255,255),
		cv::Scalar(0,0,255),
		cv::Scalar(255,0,255)
	};
	cv::Mat gray, smallImg;
	double fx = 1;
	double scale = 1;
	int FaceMinNeighbors = 30; // For tracking only one object make maighors factor bigger
	float FaceScaleFactor = 1.05f; // – Parameter specifying how much the image size is reduced at each image scale.
	int SimleMinNeighbors = 0;
	float SmileScaleFactor = 5.0f;

	int max_neighbors = -1;
	int min_neighbors = -1;
};
