#pragma once

#include <future>
#include <iostream>

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

// Gpu version
#include "opencv2/cudaobjdetect.hpp"
#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudawarping.hpp"

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

	FOpenCVSmileImpl(std::string CascadeName, std::string NestedCascadeName, std::string CascadeGpuName, std::string NestedCascadeGpuName, bool useGPU);
	FOpenCVSmileImpl(FOpenCVSmileImpl &&) {}
	FOpenCVSmileImpl(const FOpenCVSmileImpl&) = default;
	//~FOpenCVSmileImpl();

	static FOpenCVSmileImpl* Get(FString CascadeName, FString NestedCascadeName, FString CascadeGpuName, FString NestedCascadeGpuName, bool useGPU);
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

	static void UpdateParams(bool useGPU)
	{
		if (OpenCVSmileImplPtr != nullptr)
		{
			OpenCVSmileImplPtr->useGPU = useGPU;
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

private:
	void RunCameraThread();
	void DetectSmile();
	void Relese();

// Webcam
private:
	cv::VideoCapture capture;

// General
private:
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

// Detection
private:
	int max_neighbors = -1;
	int min_neighbors = -1;

// Objects buffer
private:
	std::vector<cv::Rect> faces, faces2;
	std::vector<cv::Rect> nestedObjects;

// CPU version
private:
	cv::Mat smallImgROI;
	cv::CascadeClassifier cascade, nestedCascade;
	cv::Mat frame, frame_cpu, gray_cpu, resized_cpu, frameDisp;

	std::string CascadeName;
	std::string NestedCascadeName;

// GPU version
private:
	bool useGPU;

	cv::Ptr<cv::cuda::CascadeClassifier> cascade_gpu;
	cv::Ptr<cv::cuda::CascadeClassifier> nestedCascade_gpu;

	cv::cuda::GpuMat frame_gpu, smallImgROIGPU, gray_gpu, resized_gpu, facesBuf_gpu, faces1Buf_gpu;

	std::string CascadeGpuName;
	std::string NestedCascadeGpuName;
};
