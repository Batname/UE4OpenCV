// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <future>
#include <iostream>

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "OpenCVSmileComponent.generated.h"

class FOpenCVSmileImpl;

//~~~~~ Multi Threading ~~~
class FOpenCVSmileWorker : public FRunnable
{
	static FOpenCVSmileWorker* Runnable;

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;

private:
	FOpenCVSmileImpl* OpenCVSmileImpl;

	FCriticalSection mutex;
public:

	//~~~ Thread Core Functions ~~~

	//Constructor / Destructor
	FOpenCVSmileWorker(FOpenCVSmileImpl* OpenCVSmileImpl);
	virtual ~FOpenCVSmileWorker();

	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface

	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();

	/*
	Start the thread and the worker from static (easy access)!
	This code ensures only 1 Prime Number thread will be able to run at a time.
	This function returns a handle to the newly started instance.
	*/
	static FOpenCVSmileWorker* JoyInit(FOpenCVSmileImpl* OpenCVSmileImpl);

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();

private:
	cv::VideoCapture capture;
	cv::Mat frame, image;
};

class FOpenCVSmileImpl
{
	friend FOpenCVSmileWorker;

public:
	FOpenCVSmileImpl(std::string CascadeName, std::string NestedCascadeName);
	~FOpenCVSmileImpl();

	static TSharedRef<class FOpenCVSmileImpl, ESPMode::ThreadSafe> Get(FString CascadeName, FString NestedCascadeName);
	static TSharedPtr<class FOpenCVSmileImpl, ESPMode::ThreadSafe> OpenCVSmileImplPtr;

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
private:
	std::atomic_bool bIsSmileThreadRunning;
	std::thread CameraThread;
	std::mutex mutex;

	std::string CascadeName;
	std::string NestedCascadeName;
};


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENCV_API UOpenCVSmileComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenCVSmileComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Initializes the component.
	virtual void InitializeComponent() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

/*~~~~~~~~~~~~~~~~~~~~~~~~ Connection to smile camera thread ~~~~~~~~~~~~~~~~~~~~~~~*/
protected:
	/** Start OpenCV camera */
	UFUNCTION(BlueprintCallable, Category = "OpenCVSmileComponent")
	void StartCamera();

	/** Stop OpenCV camera */
	UFUNCTION(BlueprintCallable, Category = "OpenCVSmileComponent")
	void StopCamera();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCVSmileComponent")
	FString CascadeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OpenCVSmileComponent")
	FString NestedCascadeName;
};
