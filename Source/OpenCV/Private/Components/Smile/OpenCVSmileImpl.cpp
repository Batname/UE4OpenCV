#include "Components/Smile/OpenCVSmileImpl.h"

#include "Engine.h"


using namespace std;

// console variables
static TAutoConsoleVariable<float> CVarSmileScale(
	TEXT("opencv.smile.scale"),
	1.0f,
	TEXT("opencv.smile.scale")
);

static TAutoConsoleVariable<float> CVarSmileGpuScaleFactor(
	TEXT("opencv.smile.gpu.ScaleFactor"),
	1.05f,
	TEXT("opencv.smile.gpu.ScaleFactor")
);

static TAutoConsoleVariable<float> CVarSmileGpuMinNeighbors(
	TEXT("opencv.smile.gpu.MinNeighbors"),
	4.0f,
	TEXT("opencv.smile.gpu.MinNeighbors")
);

static TAutoConsoleVariable<int32> CVarSmileGpuNestedFindLargestObject(
	TEXT("opencv.smile.gpu.nested.FindLargestObject"),
	0,
	TEXT("opencv.smile.gpu.nested.FindLargestObject")
);

static TAutoConsoleVariable<float> CVarSmileGpuNestedScaleFactor(
	TEXT("opencv.smile.gpu.nested.ScaleFactor"),
	1.05f,
	TEXT("opencv.smile.gpu.nested.ScaleFactor")
);

static TAutoConsoleVariable<float> CVarSmileGpuNestedMinNeighbors(
	TEXT("opencv.smile.gpu.nested.MinNeighbors"),
	0.0f,
	TEXT("opencv.smile.gpu.nested.MinNeighbors")
);

static TAutoConsoleVariable<float> CVarSmileGpuNestedFaceFactor(
	TEXT("opencv.smile.gpu.nested.FaceFactor"),
	2.0f,
	TEXT("opencv.smile.gpu.nested.FaceFactor")
);

static TAutoConsoleVariable<float> CVarSmileGpuNestedFaceFactorHeight(
	TEXT("opencv.smile.gpu.nested.FaceFactorHeight"),
	2.0f,
	TEXT("opencv.smile.gpu.nested.FaceFactorHeight")
);

static TAutoConsoleVariable<float> CVarSmileCpuScaleFactor(
	TEXT("opencv.smile.cpu.ScaleFactor"),
	1.05f,
	TEXT("opencv.smile.cpu.ScaleFactor")
);

static TAutoConsoleVariable<float> CVarSmileCpuMinNeighbors(
	TEXT("opencv.smile.cpu.MinNeighbors"),
	4.0f,
	TEXT("opencv.smile.cpu.MinNeighbors")
);

static TAutoConsoleVariable<float> CVarSmileCpuNestedScaleFactor(
	TEXT("opencv.smile.cpu.nested.ScaleFactor"),
	1.1f,
	TEXT("opencv.smile.cpu.nested.ScaleFactor")
);

static TAutoConsoleVariable<float> CVarSmileCpuNestedMinNeighbors(
	TEXT("opencv.smile.cpu.nested.MinNeighbors"),
	0.0f,
	TEXT("opencv.smile.cpu.nested.MinNeighbors")
);


FOpenCVSmileImpl* FOpenCVSmileImpl::OpenCVSmileImplPtr = nullptr;
FOpenCVSmileImpl* FOpenCVSmileImpl::Get(FString CascadeName, FString NestedCascadeName, FString CascadeGpuName, FString NestedCascadeGpuName, bool useGPU)
{
	if (OpenCVSmileImplPtr == nullptr)
	{
		std::string cascadeName = std::string(TCHAR_TO_UTF8(*CascadeName));
		std::string nestedCascadeName = std::string(TCHAR_TO_UTF8(*NestedCascadeName));
		std::string cascadeGpuName = std::string(TCHAR_TO_UTF8(*CascadeGpuName));
		std::string nestedCascadeGpuName = std::string(TCHAR_TO_UTF8(*NestedCascadeGpuName));
		OpenCVSmileImplPtr = new FOpenCVSmileImpl(cascadeName, nestedCascadeName, cascadeGpuName, nestedCascadeGpuName, useGPU);
	}

	return OpenCVSmileImplPtr;
}

static void convertAndResize(const cv::Mat& src, cv::Mat& gray, cv::Mat& resized, double scale)
{
	if (src.channels() == 3)
	{
		cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
	}
	else
	{
		gray = src;
	}

	cv::Size sz(cvRound(gray.cols * scale), cvRound(gray.rows * scale));

	if (scale != 1)
	{
		cv::resize(gray, resized, sz);
	}
	else
	{
		resized = gray;
	}
}

static void convertAndResize(const cv::cuda::GpuMat& src, cv::cuda::GpuMat& gray, cv::cuda::GpuMat& resized, double scale)
{
	if (src.channels() == 3)
	{
		cv::cuda::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
	}
	else
	{
		gray = src;
	}

	cv::Size sz(cvRound(gray.cols * scale), cvRound(gray.rows * scale));

	if (scale != 1)
	{
		cv::cuda::resize(gray, resized, sz);
	}
	else
	{
		resized = gray;
	}
}

FOpenCVSmileImpl::FOpenCVSmileImpl(std::string CascadeName, std::string NestedCascadeName, std::string CascadeGpuName, std::string NestedCascadeGpuName, bool useGPU)
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::FOpenCVSmileImpl()"));

	this->CascadeName = CascadeName;
	this->NestedCascadeName = NestedCascadeName;
	this->CascadeGpuName = CascadeGpuName;
	this->NestedCascadeGpuName = NestedCascadeGpuName;
	this->bIsSmileThreadRunning = false;
	this->useGPU = useGPU;
}

// Let program destroy object after exit
//FOpenCVSmileImpl::~FOpenCVSmileImpl()
//{
//	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::~FOpenCVSmileImpl()"));
//
//	frame.release();
//	image.release();
//
//	faces.clear();
//	faces2.clear();
//	nestedObjects.clear();
//
//	gray.release();
//	smallImg.release();
//}


void FOpenCVSmileImpl::DetectSmile()
{
	frame_gpu.upload(frame_cpu);

	float scale = CVarSmileScale.GetValueOnAnyThread();
	convertAndResize(frame_cpu, gray_cpu, resized_cpu, scale);
	convertAndResize(frame_gpu, gray_gpu, resized_gpu, scale);

	if (useGPU)
	{
		float SmileGpuScaleFactor = CVarSmileGpuScaleFactor.GetValueOnAnyThread();
		float SmileGpuMinNeighbors = CVarSmileGpuMinNeighbors.GetValueOnAnyThread();

		cascade_gpu->setScaleFactor(SmileGpuScaleFactor);
		cascade_gpu->setMinNeighbors(SmileGpuMinNeighbors);
		cascade_gpu->setMinObjectSize(cv::Size(30, 30));

		cascade_gpu->detectMultiScale(resized_gpu, facesBuf_gpu);
		cascade_gpu->convert(facesBuf_gpu, faces);
	}
	else
	{
		float SmileCpuScaleFactor = CVarSmileCpuScaleFactor.GetValueOnAnyThread();
		float SmileCpuMinNeighbors = CVarSmileCpuMinNeighbors.GetValueOnAnyThread();

		cascade.detectMultiScale(resized_cpu, faces,
			SmileCpuScaleFactor, SmileCpuMinNeighbors, 0
			//|CASCADE_FIND_BIGGEST_OBJECT
			//|CASCADE_DO_ROUGH_SEARCH
			| cv::CASCADE_SCALE_IMAGE,
			cv::Size(30, 30)
		);
	}


	for (size_t i = 0; i < faces.size(); i++)
	{
		cv::Rect r = faces[i];
		cv::Point center;
		cv::Scalar color = colors[i % 8];
		int radius;

		// Draw a circle around the face
		double aspect_ratio = (double)r.width / r.height;
		if (0.75 < aspect_ratio && aspect_ratio < 1.3)
		{
			center.x = cvRound((r.x + r.width*0.5)*scale);
			center.y = cvRound((r.y + r.height*0.5)*scale);
			radius = cvRound((r.width + r.height)*0.25*scale);
			cv::circle(resized_cpu, center, radius, color, 3, 8, 0);
		}
		else
			cv::rectangle(resized_cpu, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
				cvPoint(cvRound((r.x + r.width - 1)*scale), cvRound((r.y + r.height - 1)*scale)),
				color, 3, 8, 0);

		const int half_height = cvRound((float)r.height / 2);
		r.y = r.y + half_height;
		r.height = half_height - 1;

		smallImgROI = resized_cpu(r);

		if (useGPU)
		{
			// load console variables
			bool SmileGpuNestedFindLargestObject = !!FMath::Clamp(CVarSmileGpuNestedFindLargestObject.GetValueOnAnyThread(), 0, 1);
			float SmileGpuNestedScaleFactor = CVarSmileGpuNestedScaleFactor.GetValueOnAnyThread();
			float SmileGpuNestedMinNeighbors = CVarSmileGpuNestedMinNeighbors.GetValueOnAnyThread();
			float SmileGpuNestedFaceFactor = CVarSmileGpuNestedFaceFactor.GetValueOnAnyThread(); // 6
			float SmileGpuNestedFaceFactorHeight = CVarSmileGpuNestedFaceFactorHeight.GetValueOnAnyThread(); // 2

			smallImgROIGPU.upload(smallImgROI);

			nestedCascade_gpu->setFindLargestObject(SmileGpuNestedFindLargestObject); // true
			nestedCascade_gpu->setScaleFactor(SmileGpuNestedScaleFactor); // 5
			nestedCascade_gpu->setMinNeighbors(SmileGpuNestedMinNeighbors); // 32


			cv::Size MinObjectSize = cv::Size(cv::Size(r.width / SmileGpuNestedFaceFactor, r.width / (SmileGpuNestedFaceFactor * SmileGpuNestedFaceFactorHeight)));
			nestedCascade_gpu->setMinObjectSize(MinObjectSize);

			nestedCascade_gpu->detectMultiScale(smallImgROIGPU, faces1Buf_gpu);
			nestedCascade_gpu->convert(faces1Buf_gpu, nestedObjects);

			for (int i = 0; i < nestedObjects.size(); i++)
			{
				cv::rectangle(resized_cpu, cvPoint(r.x + nestedObjects[i].x, r.y + nestedObjects[i].y),
					cvPoint(r.x + nestedObjects[i].x + nestedObjects[i].width, r.y + nestedObjects[i].y + nestedObjects[i].height), cv::Scalar(0.0f, 255.0f, 0.0f));
			}

			UE_LOG(LogTemp, Warning, TEXT("SmileGpuNestedFindLargestObject %d, SmileGpuNestedScaleFactor %f, MinNeighbors %f, FaceFactor %f"),
				SmileGpuNestedFindLargestObject, SmileGpuNestedScaleFactor, SmileGpuNestedMinNeighbors, SmileGpuNestedFaceFactor);
			UE_LOG(LogTemp, Warning, TEXT("MinObjectSize width %d MinObjectSize height %d"), MinObjectSize.width, MinObjectSize.height);

		}
		else
		{
			float SmileCpuNestedScaleFactor = CVarSmileCpuNestedScaleFactor.GetValueOnAnyThread();
			float SmileCpuNestedMinNeighbors = CVarSmileCpuNestedMinNeighbors.GetValueOnAnyThread();

			nestedCascade.detectMultiScale(smallImgROI, nestedObjects,
				SmileCpuNestedScaleFactor, SmileCpuNestedMinNeighbors, 0
				//|CASCADE_FIND_BIGGEST_OBJECT
				//|CASCADE_DO_ROUGH_SEARCH
				//|CASCADE_DO_CANNY_PRUNING
				| cv::CASCADE_SCALE_IMAGE,
				cv::Size(30, 30));
		}

		// The number of detected neighbors depends on image size (and also illumination, etc.). The
		// following steps use a floating minimum and maximum of neighbors. Intensity thus estimated will be
		//accurate only after a first smile has been displayed by the user.
		const int smile_neighbors = (int)nestedObjects.size();

		// Define the minimum of the smiling
		if (min_neighbors == -1) min_neighbors = smile_neighbors;
		max_neighbors = MAX(max_neighbors, smile_neighbors);

		// Define Smiling level from 0 to 1
		float intensityZeroOne = ((float)smile_neighbors - min_neighbors) / (max_neighbors - min_neighbors + 1);
		UE_LOG(LogTemp, Warning, TEXT("smile_neighbors %d, intensityZeroOne %f"), smile_neighbors, intensityZeroOne);

		mutex.lock();
		OpenCVSmileFrame.IntensityZeroOne = intensityZeroOne;
		mutex.unlock();

		// Draw the smile scale level
		int rect_height = cvRound((float)resized_cpu.rows * intensityZeroOne);
		cv::Scalar col = cv::Scalar((float)255 * intensityZeroOne, 0, 0);
		cv::rectangle(resized_cpu, cvPoint(0, resized_cpu.rows), cvPoint(resized_cpu.cols / 10, resized_cpu.rows - rect_height), col, -1);
	}

	// Draw result to window
	cv::cvtColor(resized_cpu, frameDisp, cv::COLOR_GRAY2BGR);
	cv::imshow("frameDisp result", frameDisp);

	// Sleep thread
	cv::waitKey(10);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void FOpenCVSmileImpl::RunCameraThread()
{
	float scale = CVarSmileScale.GetValueOnAnyThread();

	if (!cascade.load(CascadeName))
	{
		check(false);
	}
	if (!nestedCascade.load(NestedCascadeName))
	{
		check(false);
	}

	// Use cuda version
	cv::cuda::printShortCudaDeviceInfo(cv::cuda::getDevice());
	cascade_gpu = cv::cuda::CascadeClassifier::create(CascadeGpuName);
	nestedCascade_gpu = cv::cuda::CascadeClassifier::create(NestedCascadeGpuName);


	// Open 0 camera
	static const int CameraID = 0;
	if (!capture.open(CameraID))
	{
		check(false);
	}
	{
		float width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
		float height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
		UE_LOG(LogTemp, Warning, TEXT("width x height %f x %f"), width, height);
		UE_LOG(LogTemp, Warning, TEXT("scaled width x height %f x %f"), width * scale, height * scale);
	}


	// Limit frame rate
	//capture.set(CV_CAP_PROP_FPS, 5);

	if (capture.isOpened())
	{
		while (bIsSmileThreadRunning == true)
		{
			capture >> frame;
			if (frame.empty())
			{
				UE_LOG(LogTemp, Warning, TEXT("frame.empty()"));
				continue;
			}
			frame.copyTo(frame_cpu);
			DetectSmile();
		}
	}
	else
	{
		check(false);
	}


	capture.release();
	cv::destroyAllWindows();
}

void FOpenCVSmileImpl::Relese()
{
	max_neighbors = -1;
	min_neighbors = -1;
}

void FOpenCVSmileImpl::StartCameraInst()
{	
	if (bIsSmileThreadRunning == false) {
		bIsSmileThreadRunning = true;
		CameraThread = std::thread([this]() { RunCameraThread(); });
	}
}

void FOpenCVSmileImpl::StopCameraInst()
{
	if (bIsSmileThreadRunning) {
		bIsSmileThreadRunning = false;
		CameraThread.join();
		Relese();
	}
}