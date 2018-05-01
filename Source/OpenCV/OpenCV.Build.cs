// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;

public class OpenCV : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    private string ThirdPartyOpenCVPath
    {
        get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "3rdparty")); }
    }

    private string BinariesPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Binaries/")); }
    }

    private string LibraryPath
    {
        get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "OpenCVSDK", "Lib")); }
    }

    private string DllPath
    {
        get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "OpenCVSDK", "Dll")); }
    }

    public OpenCV(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "Win32";
  
        string CudaDirectory = Environment.GetEnvironmentVariable("CUDA_PATH_V8_0");
        string CudaDirectoryInclude = CudaDirectory + "include";
        string CudaDirectoryLib = CudaDirectory + "/lib/" + PlatformString;


        PublicIncludePaths.AddRange(
			new string[] {
				"OpenCV/Public",
                Path.Combine(ThirdPartyPath, "OpenCVSDK", "Include"),
                Path.Combine(CudaDirectoryInclude),
            }
            );
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"OpenCV/Private",
                Path.Combine(ThirdPartyPath, "OpenCVSDK", "Include"),
                Path.Combine(CudaDirectoryInclude),
            }
            );
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);

        // Include Cuda libs
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "cudart.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppc.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppial.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppicc.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppicom.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppidei.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppif.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppig.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppim.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppist.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppisu.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "nppitc.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "npps.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "cublas.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CudaDirectoryLib, "cufft.lib"));

        // Include OpenCV 3rd party
        PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyOpenCVPath, "lib/Release/ippiw.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyOpenCVPath, "ippicv/ippicv_win/lib/intel64/ippicvmt.lib"));


        // Include libs
        PublicLibraryPaths.Add(LibraryPath);

        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_calib3d340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_core340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudaarithm340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudabgsegm340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudacodec340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudafeatures2d340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudafilters340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudaimgproc340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudalegacy340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudaobjdetect340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudaoptflow340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudastereo340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudawarping340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_cudev340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_dnn340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_features2d340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_flann340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_highgui340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_imgcodecs340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_imgproc340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_ml340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_objdetect340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_photo340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_shape340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_stitching340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_superres340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_ts340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_video340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_videoio340.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, "opencv_videostab340.lib"));

        RuntimeDependencies.Add(DllPath + "/" + "opencv_calib3d340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_core340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudaarithm340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudabgsegm340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudacodec340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudafeatures2d340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudafilters340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudaimgproc340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudalegacy340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudaobjdetect340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudaoptflow340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudastereo340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudawarping340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_cudev340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_dnn340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_features2d340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_ffmpeg340_64.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_flann340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_highgui340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_imgcodecs340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_imgproc340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_ml340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_objdetect340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_photo340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_shape340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_stitching340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_superres340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_video340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_videoio340.dll");
        RuntimeDependencies.Add(DllPath + "/" + "opencv_videostab340.dll");

        PublicDelayLoadDLLs.Add("opencv_calib3d340.dll");
        PublicDelayLoadDLLs.Add("opencv_core340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudaarithm340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudabgsegm340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudacodec340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudafeatures2d340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudafilters340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudaimgproc340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudalegacy340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudaobjdetect340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudaoptflow340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudastereo340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudawarping340.dll");
        PublicDelayLoadDLLs.Add("opencv_cudev340.dll");
        PublicDelayLoadDLLs.Add("opencv_dnn340.dll");
        PublicDelayLoadDLLs.Add("opencv_features2d340.dll");
        PublicDelayLoadDLLs.Add("opencv_ffmpeg340_64.dll");
        PublicDelayLoadDLLs.Add("opencv_flann340.dll");
        PublicDelayLoadDLLs.Add("opencv_highgui340.dll");
        PublicDelayLoadDLLs.Add("opencv_imgcodecs340.dll");
        PublicDelayLoadDLLs.Add("opencv_imgproc340.dll");
        PublicDelayLoadDLLs.Add("opencv_ml340.dll");
        PublicDelayLoadDLLs.Add("opencv_objdetect340.dll");
        PublicDelayLoadDLLs.Add("opencv_photo340.dll");
        PublicDelayLoadDLLs.Add("opencv_shape340.dll");
        PublicDelayLoadDLLs.Add("opencv_stitching340.dll");
        PublicDelayLoadDLLs.Add("opencv_superres340.dll");
        PublicDelayLoadDLLs.Add("opencv_video340.dll");
        PublicDelayLoadDLLs.Add("opencv_videoio340.dll");
        PublicDelayLoadDLLs.Add("opencv_videostab340.dll");

        // Cuda DLL
        RuntimeDependencies.Add(DllPath + "/" + "nppial64_80.dll");
        RuntimeDependencies.Add(DllPath + "/" + "nppicc64_80.dll");
        RuntimeDependencies.Add(DllPath + "/" + "nppidei64_80.dll");
        RuntimeDependencies.Add(DllPath + "/" + "nppist64_80.dll");
        RuntimeDependencies.Add(DllPath + "/" + "npps64_80.dll");
        RuntimeDependencies.Add(DllPath + "/" + "nppif64_80.dll");
        RuntimeDependencies.Add(DllPath + "/" + "nppim64_80.dll");



        PublicDelayLoadDLLs.Add("nppial64_80.dll");
        PublicDelayLoadDLLs.Add("nppicc64_80.dll");
        PublicDelayLoadDLLs.Add("nppidei64_80.dll");
        PublicDelayLoadDLLs.Add("nppist64_80.dll");
        PublicDelayLoadDLLs.Add("npps64_80.dll");
        PublicDelayLoadDLLs.Add("nppif64_80.dll");
        PublicDelayLoadDLLs.Add("nppim64_80.dll");
    }
}