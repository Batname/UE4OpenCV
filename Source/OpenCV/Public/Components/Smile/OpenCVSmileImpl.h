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
	~FOpenCVSmileImpl();

	static TSharedRef<class FOpenCVSmileImpl, ESPMode::NotThreadSafe> Get(FString CascadeName, FString NestedCascadeName);
	static TSharedPtr<class FOpenCVSmileImpl, ESPMode::NotThreadSafe> OpenCVSmileImplPtr;

public:
	static void StartCamera()
	{
		if (OpenCVSmileImplPtr.IsValid())
		{
			OpenCVSmileImplPtr->StartCameraInst();
		}
	}

	static void StopCamera()
	{
		if (OpenCVSmileImplPtr.IsValid())
		{
			OpenCVSmileImplPtr->StopCameraInst();
		}
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
	void DetectSmile(cv::Mat& img, cv::CascadeClassifier& cascade, cv::CascadeClassifier& nestedCascade, double scale, bool tryflip);

private:
	cv::VideoCapture capture;
	cv::Mat frame, image;
	cv::CascadeClassifier cascade, nestedCascade;
};
