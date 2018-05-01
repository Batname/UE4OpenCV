// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/Smile/OpenCVSmileImpl.h"
#include "OpenCVSmileComponent.generated.h"


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

	UPROPERTY(BlueprintReadOnly, Category = "OpenCVSmileComponent")
	FOpenCVSmileFrame OpenCVSmileFrame;

};
