#include "Components/Smile/OpenCVSmileImpl.h"

using namespace std;

TSharedPtr<FOpenCVSmileImpl, ESPMode::NotThreadSafe> FOpenCVSmileImpl::OpenCVSmileImplPtr;
TSharedRef<class FOpenCVSmileImpl, ESPMode::NotThreadSafe> FOpenCVSmileImpl::Get(FString CascadeName, FString NestedCascadeName)
{
	if (!OpenCVSmileImplPtr.IsValid())
	{
		std::string cascadeName = std::string(TCHAR_TO_UTF8(*CascadeName));
		std::string nestedCascadeName = std::string(TCHAR_TO_UTF8(*NestedCascadeName));
		OpenCVSmileImplPtr = TSharedPtr<class FOpenCVSmileImpl, ESPMode::NotThreadSafe > (new FOpenCVSmileImpl(cascadeName, nestedCascadeName));

		check(OpenCVSmileImplPtr.IsValid());
	}

	return OpenCVSmileImplPtr.ToSharedRef();
}

FOpenCVSmileImpl::FOpenCVSmileImpl(std::string CascadeName, std::string NestedCascadeName)
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::FOpenCVSmileImpl()"));

	this->CascadeName = CascadeName;
	this->NestedCascadeName = NestedCascadeName;
	this->bIsSmileThreadRunning = false;
}

FOpenCVSmileImpl::~FOpenCVSmileImpl()
{
	UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::~FOpenCVSmileImpl()"));
	StopCameraInst();
}

void FOpenCVSmileImpl::DetectSmile(cv::Mat& img, cv::CascadeClassifier& cascade, cv::CascadeClassifier& nestedCascade, double scale, bool tryflip)
{
	static vector<cv::Rect> faces, faces2;
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


	const static int FaceMinNeighbors = 30; // For tracking only one object make maighors factor bigger
	const static float FaceScaleFactor = 1.05f; // – Parameter specifying how much the image size is reduced at each image scale.
	cascade.detectMultiScale(smallImg, faces,
		FaceScaleFactor, FaceMinNeighbors, 0
		//|CASCADE_FIND_BIGGEST_OBJECT
		//|CASCADE_DO_ROUGH_SEARCH
		| cv::CASCADE_SCALE_IMAGE,
		cv::Size(30, 30));

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

	for (size_t i = 0; i < faces.size(); i++)
	{
		cv::Rect r = faces[i];
		cv::Mat smallImgROI;
		static vector<cv::Rect> nestedObjects;
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

		const static int SimleMinNeighbors = 0; // Should be 0

												// – Parameter specifying how much the image size is reduced at each image scale.
												// - If it bigger it goig to track on longet distance
		const static float SmileScaleFactor = 5.0f;
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
		static int max_neighbors = -1;
		static int min_neighbors = -1;
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
	UE_LOG(LogTemp, Warning, TEXT("RunCameraThread() start"));

	string inputName;
	bool tryflip = false;

	double scale = 1.0;

	if (!cascade.load(CascadeName))
	{
		cerr << "ERROR: Could not load face cascade" << endl;
		check(false);
	}
	if (!nestedCascade.load(NestedCascadeName))
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

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	if (capture.isOpened())
	{
		cout << "Video capturing has been started ..." << endl;
		cout << endl << "NOTE: Smile intensity will only be valid after a first smile has been detected" << endl;

		while (bIsSmileThreadRunning == true)
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
	}
	else
	{
		cerr << "ERROR: Could not initiate capture" << endl;
		check(false);
	}


	capture.release();
	cv::destroyAllWindows();

	UE_LOG(LogTemp, Warning, TEXT("End RunCameraThread function"));
}

void FOpenCVSmileImpl::StartCameraInst()
{
	UE_LOG(LogTemp, Warning, TEXT("StartCameraInst"));
	
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

	if (OpenCVSmileImplPtr.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("FOpenCVSmileImpl::StopCameraInst() OpenCVSmileImplPtr.Reset()"));
		OpenCVSmileImplPtr.Reset();
		OpenCVSmileImplPtr = nullptr;
	}
}