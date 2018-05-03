// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Smile/OpenCVSmileComponent.h"

// Sets default values for this component's properties
UOpenCVSmileComponent::UOpenCVSmileComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bUseGPU = true;
}


// Called when the game starts
void UOpenCVSmileComponent::BeginPlay()
{
	Super::BeginPlay();

	FOpenCVSmileImpl::UpdateParams(bUseGPU);
}

void UOpenCVSmileComponent::InitializeComponent()
{
	FOpenCVSmileImpl::Get(CascadeName, NestedCascadeName, CascadeGPUName, NestedCascadeGPUName, bUseGPU);
}


// Called every frame
void UOpenCVSmileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto OpenCVSmileImpl = FOpenCVSmileImpl::Get(CascadeName, NestedCascadeName, CascadeGPUName, NestedCascadeGPUName, bUseGPU);
	OpenCVSmileFrame = OpenCVSmileImpl->GetOpenCVSmileFrame();
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

