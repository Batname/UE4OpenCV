// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Smile/OpenCVSmileComponent.h"

using namespace std;

FOpenCVSmileWorker* FOpenCVSmileWorker::Runnable = NULL;

static void DetectSmile(cv::Mat& img, cv::CascadeClassifier& cascade, cv::CascadeClassifier& nestedCascade, double scale, bool tryflip)
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileWorker::DetectSmile"));

	vector<cv::Rect> faces, faces2;
	const static cv::Scalar colors[] =
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
	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

	double fx = 1 / scale;
	cv::resize(gray, smallImg, cv::Size(), fx, fx, cv::INTER_LINEAR_EXACT);
	cv::equalizeHist(smallImg, smallImg);

	//cascade.detectMultiScale(smallImg, faces,
	//	1.1, 2, 0
	//	//|CASCADE_FIND_BIGGEST_OBJECT
	//	//|CASCADE_DO_ROUGH_SEARCH
	//	| cv::CASCADE_SCALE_IMAGE,
	//	cv::Size(30, 30));


	//if (tryflip)
	//{
	//	cv::flip(smallImg, smallImg, 1);
	//	cascade.detectMultiScale(smallImg, faces2,
	//		1.1, 2, 0
	//		//|CASCADE_FIND_BIGGEST_OBJECT
	//		//|CASCADE_DO_ROUGH_SEARCH
	//		| cv::CASCADE_SCALE_IMAGE,
	//		cv::Size(30, 30));
	//	for (vector<cv::Rect>::const_iterator r = faces2.begin(); r != faces2.end(); ++r)
	//	{
	//		faces.push_back(cv::Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
	//	}
	//}

	//for (size_t i = 0; i < faces.size(); i++)
	//{
	//	cv::Rect r = faces[i];
	//	cv::Mat smallImgROI;
	//	vector<cv::Rect> nestedObjects;
	//	cv::Point center;
	//	cv::Scalar color = colors[i % 8];
	//	int radius;

	//	double aspect_ratio = (double)r.width / r.height;
	//	if (0.75 < aspect_ratio && aspect_ratio < 1.3)
	//	{
	//		center.x = cvRound((r.x + r.width*0.5)*scale);
	//		center.y = cvRound((r.y + r.height*0.5)*scale);
	//		radius = cvRound((r.width + r.height)*0.25*scale);
	//		cv::circle(img, center, radius, color, 3, 8, 0);
	//	}
	//	else
	//		cv::rectangle(img, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
	//			cvPoint(cvRound((r.x + r.width - 1)*scale), cvRound((r.y + r.height - 1)*scale)),
	//			color, 3, 8, 0);

	//	const int half_height = cvRound((float)r.height / 2);
	//	r.y = r.y + half_height;
	//	r.height = half_height - 1;
	//	smallImgROI = smallImg(r);
	//	nestedCascade.detectMultiScale(smallImgROI, nestedObjects,
	//		1.1, 0, 0
	//		//|CASCADE_FIND_BIGGEST_OBJECT
	//		//|CASCADE_DO_ROUGH_SEARCH
	//		//|CASCADE_DO_CANNY_PRUNING
	//		| cv::CASCADE_SCALE_IMAGE,
	//		cv::Size(30, 30));

	//	// The number of detected neighbors depends on image size (and also illumination, etc.). The
	//	// following steps use a floating minimum and maximum of neighbors. Intensity thus estimated will be
	//	//accurate only after a first smile has been displayed by the user.
	//	const int smile_neighbors = (int)nestedObjects.size();
	//	static int max_neighbors = -1;
	//	static int min_neighbors = -1;
	//	if (min_neighbors == -1) min_neighbors = smile_neighbors;
	//	max_neighbors = MAX(max_neighbors, smile_neighbors);

	//	// Draw rectangle on the left side of the image reflecting smile intensity
	//	float intensityZeroOne = ((float)smile_neighbors - min_neighbors) / (max_neighbors - min_neighbors + 1);
	//	std::cout << intensityZeroOne << std::endl;
	//	int rect_height = cvRound((float)img.rows * intensityZeroOne);
	//	cv::Scalar col = cv::Scalar((float)255 * intensityZeroOne, 0, 0);
	//	cv::rectangle(img, cvPoint(0, img.rows), cvPoint(img.cols / 10, img.rows - rect_height), col, -1);
	//}

	UE_LOG(LogTemp, Warning, TEXT("img %p"), &img);
	cv::imshow("result", img);

	cv::waitKey(10);
}

FOpenCVSmileWorker::FOpenCVSmileWorker(FOpenCVSmileImpl* OpenCVSmileImpl)
	: OpenCVSmileImpl(OpenCVSmileImpl)
{
	Thread = FRunnableThread::Create(this, TEXT("FOpenCVSmileImpl"), 0, TPri_Highest); //windows default = 8mb for thread, could specify more

	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileWorker::FOpenCVSmileWorker"));

}

FOpenCVSmileWorker::~FOpenCVSmileWorker()
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileWorker::~FOpenCVSmileWorke"));

	frame.release();

	cvDestroyWindow("w");

	delete Thread;
	Thread = NULL;
}

bool FOpenCVSmileWorker::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileWorker::Init"));

	return true;
}

uint32 FOpenCVSmileWorker::Run()
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileWorker::Run before loop"));

	string inputName;
	bool tryflip = false;

	cv::CascadeClassifier cascade, nestedCascade;
	double scale = 1.0;

	if (!cascade.load(OpenCVSmileImpl->CascadeName))
	{
		cerr << "ERROR: Could not load face cascade" << endl;
		check(false);
	}
	if (!nestedCascade.load(OpenCVSmileImpl->NestedCascadeName))
	{
		cerr << "ERROR: Could not load smile cascade" << endl;
		check(false);
	}

	// Open 0 camera
	if (!capture.open(0))
	{
		cout << "Capture from camera #" << 0 << " didn't work" << endl;
		check(false);
	}

	if (capture.isOpened())
	{
		cout << "Video capturing has been started ..." << endl;
		cout << endl << "NOTE: Smile intensity will only be valid after a first smile has been detected" << endl;

		while (OpenCVSmileImpl->bIsSmileThreadRunning == true)
		{
			capture >> frame;
			if (frame.empty())
			{
				UE_LOG(LogTemp, Warning, TEXT("frame.empty()"));
				continue;
			}
			//cv::Mat frame1 = frame.clone();
			DetectSmile(frame, cascade, nestedCascade, scale, tryflip);

		}

		UE_LOG(LogTemp, Warning, TEXT("NO capture.release"));
		//capture.release();
	}
	else
	{
		cerr << "ERROR: Could not initiate capture" << endl;
		check(false);
	}

	return 0;
}

void FOpenCVSmileWorker::Stop()
{
	StopTaskCounter.Increment();
}

void FOpenCVSmileWorker::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

FOpenCVSmileWorker* FOpenCVSmileWorker::JoyInit(FOpenCVSmileImpl* OpenCVSmileImpl)
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileWorker::JoyInit()"));

	if (!Runnable && FGenericPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FOpenCVSmileWorker(OpenCVSmileImpl);
	}
	return Runnable;
}

void FOpenCVSmileWorker::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = NULL;
	}
}

TSharedPtr<FOpenCVSmileImpl, ESPMode::ThreadSafe> FOpenCVSmileImpl::OpenCVSmileImplPtr;
TSharedRef<class FOpenCVSmileImpl, ESPMode::ThreadSafe> FOpenCVSmileImpl::Get(FString CascadeName, FString NestedCascadeName)
{
	if (!OpenCVSmileImplPtr.IsValid())
	{
		std::string cascadeName = std::string(TCHAR_TO_UTF8(*CascadeName));
		std::string nestedCascadeName = std::string(TCHAR_TO_UTF8(*NestedCascadeName));
		OpenCVSmileImplPtr = TSharedPtr<class FOpenCVSmileImpl, ESPMode::ThreadSafe>(new FOpenCVSmileImpl(cascadeName, nestedCascadeName));

		check(OpenCVSmileImplPtr.IsValid());
	}

	return OpenCVSmileImplPtr.ToSharedRef();
}

FOpenCVSmileImpl::FOpenCVSmileImpl(std::string CascadeName, std::string NestedCascadeName)
	: CascadeName(CascadeName), NestedCascadeName(NestedCascadeName)
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::FOpenCVSmileImpl()"));

	bIsSmileThreadRunning = false;
}

FOpenCVSmileImpl::~FOpenCVSmileImpl()
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::~FOpenCVSmileImpl()"));
	StopCameraInst();
}

void FOpenCVSmileImpl::StartCameraInst()
{
	if (bIsSmileThreadRunning == false) {
		bIsSmileThreadRunning = true;
		FOpenCVSmileWorker::JoyInit(this);
	}
}

void FOpenCVSmileImpl::StopCameraInst()
{
	if (bIsSmileThreadRunning) {
		bIsSmileThreadRunning = false;
		FOpenCVSmileWorker::Shutdown();
	}

	if (OpenCVSmileImplPtr.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::StopCameraInst() OpenCVSmileImplPtr.Reset()"));
		OpenCVSmileImplPtr.Reset();
		OpenCVSmileImplPtr = nullptr;
	}
}


// Sets default values for this component's properties
UOpenCVSmileComponent::UOpenCVSmileComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

}


// Called when the game starts
void UOpenCVSmileComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UOpenCVSmileComponent::InitializeComponent()
{
	// initialize
	FOpenCVSmileImpl::Get(CascadeName, NestedCascadeName);
	UE_LOG(LogTemp, Warning, TEXT("UOpenCVSmileComponent::InitializeComponent()"));

}


// Called every frame
void UOpenCVSmileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UOpenCVSmileComponent::StartCamera()
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::StartCamera()"));

	FOpenCVSmileImpl::StartCamera();
}

void UOpenCVSmileComponent::StopCamera()
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::StopCamera()"));
	FOpenCVSmileImpl::StopCamera();

}

