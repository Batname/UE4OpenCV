// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IOpenCV.h"
#include <future>


class FOpenCV : public IOpenCV
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TMap<FString, void*> OpenCVDllsHandles;

	/** Check if all dll is loaded */
	bool bIsOpenCVReady = false;
};