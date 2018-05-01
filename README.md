### OpenCV integration with UE 4.19. In progress......

This is integration with all lib of OpenCV 3.40 and Cuda 8.0
For now this is UE4.19 plugin integration with libs without implementation of OpenCV libs and algorithms.
This is work in progress and we gonna to implement OpenCV implementation as Unreal Engine Actor components and Blueprint event interfaces

#### Examples

For now there is one simple UE4 Actor component for smile detection

+	Go to plugin content folder
![1](https://user-images.githubusercontent.com/6189196/39381054-0d832582-4a61-11e8-84ef-7d920e462f21.JPG)
+	Add you CascadeClassifiers
![2](https://user-images.githubusercontent.com/6189196/39381055-0da13d92-4a61-11e8-82e7-3f86d25906c7.JPG)
+	Run camera in your test actor
![3](https://user-images.githubusercontent.com/6189196/39381056-0dba8e32-4a61-11e8-974a-71191574ea60.JPG)

Example youtube video
[![UE4 first test of smile detection AI](https://i.ytimg.com/vi_webp/66xnhzeVZs0/sddefault.webp)](https://youtu.be/66xnhzeVZs0 "UE4 first test of smile detection AI")


#### Instalation
+	Clone github repo and rename to OpenCV or `git clone git@github.com:Batname/UE4OpenCV.git OpenCV`
+	Download and Install [Cuda 8.0](https://developer.nvidia.com/cuda-80-ga2-download-archive)
+	Copy ThirdParty to plugin folder [ThirdParty](https://www.dropbox.com/s/d9n5q3b1uwbb7ew/ThirdParty.rar?dl=0) and unrar
+	Add to Unreal Engine 4.19 project and compile
