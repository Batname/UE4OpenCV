#include "Components/Smile/OpenCVSmileImpl.h"

using namespace std;

FOpenCVSmileImpl* FOpenCVSmileImpl::OpenCVSmileImplPtr = nullptr;
FOpenCVSmileImpl* FOpenCVSmileImpl::Get(FString CascadeName, FString NestedCascadeName)
{
	if (OpenCVSmileImplPtr == nullptr)
	{
		std::string cascadeName = std::string(TCHAR_TO_UTF8(*CascadeName));
		std::string nestedCascadeName = std::string(TCHAR_TO_UTF8(*NestedCascadeName));
		OpenCVSmileImplPtr = new FOpenCVSmileImpl(cascadeName, nestedCascadeName);
	}

	return OpenCVSmileImplPtr;
}

FOpenCVSmileImpl::FOpenCVSmileImpl(std::string CascadeName, std::string NestedCascadeName)
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::FOpenCVSmileImpl()"));

	this->CascadeName = CascadeName;
	this->NestedCascadeName = NestedCascadeName;
	this->bIsSmileThreadRunning = false;
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


void FOpenCVSmileImpl::DetectSmile(cv::Mat& img, cv::CascadeClassifier& cascade, cv::CascadeClassifier& nestedCascade)
{
	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	cv::resize(gray, smallImg, cv::Size(), fx, fx, cv::INTER_LINEAR_EXACT);
	cv::equalizeHist(smallImg, smallImg);

	cascade.detectMultiScale(smallImg, faces,
		FaceScaleFactor, FaceMinNeighbors, 0
		//|CASCADE_FIND_BIGGEST_OBJECT
		//|CASCADE_DO_ROUGH_SEARCH
		| cv::CASCADE_SCALE_IMAGE,
		cv::Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		cv::Rect r = faces[i];
		cv::Mat smallImgROI;
		cv::Point center;
		cv::Scalar color = colors[i % 8];
		int radius;

		double aspect_ratio = (double)r.width / r.height;
		if (0.75 < aspect_ratio && aspect_ratio < 1.3)
		{
			center.x = cvRound((r.x + r.width*0.5)*scale);
			center.y = cvRound((r.y + r.height*0.5)*scale);
			radius = cvRound((r.width + r.height)*0.25*scale);
			cv::circle(img, center, radius, color, 3, 8, 0);
		}
		else
			cv::rectangle(img, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
				cvPoint(cvRound((r.x + r.width - 1)*scale), cvRound((r.y + r.height - 1)*scale)),
				color, 3, 8, 0);

		const int half_height = cvRound((float)r.height / 2);
		r.y = r.y + half_height;
		r.height = half_height - 1;
		smallImgROI = smallImg(r);


		nestedCascade.detectMultiScale(smallImgROI, nestedObjects,
			SmileScaleFactor, SimleMinNeighbors, 0
			//|CASCADE_FIND_BIGGEST_OBJECT
			//|CASCADE_DO_ROUGH_SEARCH
			//|CASCADE_DO_CANNY_PRUNING
			| cv::CASCADE_SCALE_IMAGE,
			cv::Size(30, 30));

		// The number of detected neighbors depends on image size (and also illumination, etc.). The
		// following steps use a floating minimum and maximum of neighbors. Intensity thus estimated will be
		//accurate only after a first smile has been displayed by the user.
		const int smile_neighbors = (int)nestedObjects.size();

		if (min_neighbors == -1) min_neighbors = smile_neighbors;
		max_neighbors = MAX(max_neighbors, smile_neighbors);

		// Draw rectangle on the left side of the image reflecting smile intensity
		float intensityZeroOne = ((float)smile_neighbors - min_neighbors) / (max_neighbors - min_neighbors + 1);
		std::cout << intensityZeroOne << std::endl;
		int rect_height = cvRound((float)img.rows * intensityZeroOne);
		cv::Scalar col = cv::Scalar((float)255 * intensityZeroOne, 0, 0);
		cv::rectangle(img, cvPoint(0, img.rows), cvPoint(img.cols / 10, img.rows - rect_height), col, -1);

		mutex.lock();
		OpenCVSmileFrame.IntensityZeroOne = intensityZeroOne;
		mutex.unlock();
	}

	cv::imshow("result", img);

	cv::waitKey(10);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

}

void FOpenCVSmileImpl::RunCameraThread()
{
	if (!cascade.load(CascadeName))
	{
		check(false);
	}
	if (!nestedCascade.load(NestedCascadeName))
	{
		check(false);
	}

	// Open 0 camera
	if (!capture.open(0))
	{
		check(false);
	}

	// Limit frame rate
	capture.set(CV_CAP_PROP_FPS, 5);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

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
			cv::Mat frame1 = frame.clone();
			DetectSmile(frame1, cascade, nestedCascade);
		}
	}
	else
	{
		check(false);
	}


	capture.release();
	cv::destroyAllWindows();
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
	}
}