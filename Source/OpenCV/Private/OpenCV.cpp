// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "OpenCV.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#define LOCTEXT_NAMESPACE "FOpenCV"

void FOpenCV::StartupModule()
{
	// Get the base directory of this plugin
	FString RootOpenCVPath = FPaths::ProjectPluginsDir() / FString::Printf(TEXT("OpenCV/")) / FString::Printf(TEXT("ThirdParty/OpenCVSDK/Dll/"));

	// Load Cuda dll
	OpenCVDllsHandles.Add("nppial64_80");
	OpenCVDllsHandles.Add("nppicc64_80");
	OpenCVDllsHandles.Add("nppidei64_80");
	OpenCVDllsHandles.Add("nppist64_80");
	OpenCVDllsHandles.Add("npps64_80");
	OpenCVDllsHandles.Add("nppif64_80");
	OpenCVDllsHandles.Add("nppim64_80");



	// Load OpenCV dll's
	OpenCVDllsHandles.Add("opencv_core340");
	OpenCVDllsHandles.Add("opencv_imgproc340");
	OpenCVDllsHandles.Add("opencv_imgcodecs340");
	OpenCVDllsHandles.Add("opencv_ml340");
	OpenCVDllsHandles.Add("opencv_videoio340");
	OpenCVDllsHandles.Add("opencv_highgui340");
	OpenCVDllsHandles.Add("opencv_cudabgsegm340");
	OpenCVDllsHandles.Add("opencv_cudawarping340");
	OpenCVDllsHandles.Add("opencv_objdetect340");
	OpenCVDllsHandles.Add("opencv_cudaarithm340");
	OpenCVDllsHandles.Add("opencv_cudafilters340");
	OpenCVDllsHandles.Add("opencv_cudaimgproc340");
	OpenCVDllsHandles.Add("opencv_flann340");
	OpenCVDllsHandles.Add("opencv_features2d340");
	OpenCVDllsHandles.Add("opencv_calib3d340");
	OpenCVDllsHandles.Add("opencv_cudalegacy340");
	OpenCVDllsHandles.Add("opencv_cudaobjdetect340");
	OpenCVDllsHandles.Add("opencv_cudacodec340");
	OpenCVDllsHandles.Add("opencv_cudafeatures2d340");
	OpenCVDllsHandles.Add("opencv_cudaoptflow340");
	OpenCVDllsHandles.Add("opencv_cudastereo340");
	OpenCVDllsHandles.Add("opencv_cudawarping340");
	OpenCVDllsHandles.Add("opencv_cudev340");
	OpenCVDllsHandles.Add("opencv_dnn340");
	OpenCVDllsHandles.Add("opencv_ffmpeg340_64");
	OpenCVDllsHandles.Add("opencv_photo340");

	OpenCVDllsHandles.Add("opencv_video340");
	OpenCVDllsHandles.Add("opencv_shape340");
	OpenCVDllsHandles.Add("opencv_stitching340");
	OpenCVDllsHandles.Add("opencv_superres340");
	OpenCVDllsHandles.Add("opencv_videostab340");

	for (auto& Val : OpenCVDllsHandles)
	{
		FString Filename = FString::Printf(TEXT("%s.dll"), *Val.Key);

		FString LibraryPath = FPaths::Combine(*RootOpenCVPath, Filename);
		Val.Value = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

		if (Val.Value)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s loaded"), *Val.Key);

			bIsOpenCVReady = true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s not loaded"), *Val.Key);

			bIsOpenCVReady = false;
		}
	}
}

void FOpenCV::ShutdownModule()
{
	// Free the dll handle
	for (auto& Val : OpenCVDllsHandles)
	{
		FPlatformProcess::FreeDllHandle(Val.Value);
		Val.Value = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenCV, OpenCV)