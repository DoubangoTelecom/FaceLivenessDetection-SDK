- [Dependencies](#dependencies)
  - [Debugging missing dependencies](#dependencies-debugging)
- [GPGPU acceleration](#gpu-acceleration)
- [Peformance numbers](#peformance-numbers)
- [Pre-built binaries](#prebuilt)
- [Building](#building)
- [Testing](#testing)
  - [Usage](#testing-usage)
  - [Examples](#testing-examples)


This application is used to check everything is ok and running as fast as expected. 
It's open source and doesn't require registration or license key.

More information about the benchmark rules at [https://www.doubango.org/SDKs/face-liveness/docs/Benchmark.html](https://www.doubango.org/SDKs/face-liveness/docs/Benchmark.html).

<a name="dependencies"></a>
# Dependencies #
**The SDK is developed in C++11** and you'll need **glibc 2.27+** on *Linux* and **[Microsoft Visual C++ 2015 Redistributable(x64) - 14.0.24123](https://www.microsoft.com/en-us/download/details.aspx?id=52685)** (any later version is ok) on *Windows*.  **You most likely already have these dependencies on you machine** as almost every program require it.

If you're planning to use [OpenVINO](https://docs.openvinotoolkit.org/), then you'll need [Intel C++ Compiler Redistributable](https://software.intel.com/en-us/articles/intel-compilers-redistributable-libraries-by-version) (choose newest). Please note that OpenVINO is packaged in the SDK as plugin and loaded (`dlopen`) at runtime. The engine will fail to load the plugin if [Intel C++ Compiler Redistributable](https://software.intel.com/en-us/articles/intel-compilers-redistributable-libraries-by-version) is missing on your machine **but the program will work as expected** with Tensorflow as fallback. We highly recommend using [OpenVINO](https://docs.openvinotoolkit.org/) to speedup the inference time. See benchmark numbers with/without [OpenVINO](https://docs.openvinotoolkit.org/) at https://www.doubango.org/SDKs/face-liveness/docs/Benchmark.html#core-i7-windows.

<a name="dependencies-debugging"></a>
## Debugging missing dependencies ##
To check if all dependencies are present:
- **Windows x86_64:** Use [Dependency Walker](https://www.dependencywalker.com/) on [binaries/windows/x86_64/FaceLivenessDetectionSDK.dll](../../../binaries/windows/x86_64/FaceLivenessDetectionSDK.dll) and [binaries/windows/x86_64/ultimatePluginOpenVINO.dll](../../../binaries/windows/x86_64/ultimatePluginOpenVINO.dll) if you're planning to use [OpenVINO](https://docs.openvinotoolkit.org/).
- **Linux x86_64:** Use `ldd <your-shared-lib>` on [binaries/linux/x86_64/libFaceLivenessDetectionSDK.so](../../../binaries/linux/x86_64/libFaceLivenessDetectionSDK.so) and [binaries/linux/x86_64/libultimatePluginOpenVINO.so](../../../binaries/linux/x86_64/libultimatePluginOpenVINO.so) if you're planning to use [OpenVINO](https://docs.openvinotoolkit.org/).


<a name="gpu-acceleration"></a>
# GPGPU acceleration #
- On x86-64, GPGPU acceleration is disabled by default. Check [here](../README.md#gpu-acceleration) for more information on how to enable it.


<a name="peformance-numbers"></a>
# Peformance numbers #

These performance numbers are obtained using **version 0.5.0**. You can use any later version.


We run the benchmark application for **#100 times (loops)** on **720p (1280x720)** images:

| | [Spoof (Parallel)](../../../assets/images/spoof.jpg) | [Spoof (Sequential)](../../../assets/images/spoof.jpg) | [Disguise (P)](../../../assets/images/disguise.jpg) | [Disguise (S)](../../../assets/images/disguise.jpg) |
| --- | --- | --- | --- | --- |
| AMD Ryzen 7 3700X 8-Core<br/>RTX 3060<br/>Ubuntu 20 | 2140 millis<br/>46.70 fps | 2977 millis<br/>33.58 fps | 2062 millis<br/>48.48 fps | 4130 millis<br/>24.20 fps |
| Intel(R) Xeon(R) E3-1230 v6 @ 3.50GHz<br/>GTX 1070<br/>Ubuntu 18 | 2655 millis<br/>37.65 fps | 3553 millis<br/>28.13 fps | 2721 millis<br/>36.73 fps | 4436 millis<br/>22.54 fps |
| Intel(R) i7-4790K @4.40GHz<br/>No GPU<br/>Windows 8 | 7197 millis<br/>13.89 fps | 7281 millis<br/>13.73 fps | 11480 millis<br/>8.71 fps | 11643 millis<br/>8.58 fps |


Some important notes:
 - **You can increase the speed (significantly) by choosing higher value for `detect_face_minsize` JSON config entry (default is 128).**
 - The engine is faster on spoofs. That's normal as there is more checks on disguises and genuines.
 - Parallel mode is faster than sequential mode on GPU. When parallel mode is enabled we perform detection and liveness check in //.

The test image looks like this:
![Test image](../../../assets/images/disguise.jpg)

<a name="prebuilt"></a>
# Pre-built binaries #

If you don't want to build this sample by yourself, then use the pre-built versions:
 - Windows x86_64: [benchmark.exe](../../../binaries/windows/x86_64/benchmark.exe) under [binaries/windows/x86_64](../../../binaries/windows/x86_64)
 - Linux x86_64: [benchmark](../../../binaries/linux/x86_64/benchmark) under [binaries/linux/x86_64](../../../binaries/linux/x86_64). Built on Ubuntu 18. **You'll need to download libtensorflow.so as explained [here](../README.md#gpu-acceleration-tensorflow-linux)**.

On **Windows**, the easiest way to try this sample is to navigate to [binaries/windows/x86_64](../../../binaries/windows/x86_64/) and run [binaries/windows/x86_64/benchmark.bat](../../../binaries/windows/x86_64/benchmark.bat). You can edit these files to use your own images and configuration options.

<a name="building"></a>
# Building #

You'll need [CMake](https://cmake.org/) to build this sample.

- Create build folder and move into it: `mkdir build && cd build`

To generate the build files:
- Windows (Visual Studio files): `cmake .. -DCMAKE_GENERATOR_PLATFORM=x64 -DCMAKE_BUILD_TYPE=Release`
- Linux (Makefile): `cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release`

To build the project:
- Windows: Open the VS solution and build the projet
- Linux: Run `make` to build the project 

<a name="testing"></a>
# Testing #
After [building](#building) the application you can test it on your local machine.

<a name="testing-usage"></a>
## Usage ##

Benchmark is a command line application with the following usage:
```
benchmark \
      --image <path-to-image-to-process> \
      --assets <path-to-assets-folder> \
      [--tokenfile <path-to-license-token-file>] \
      [--tokendata <base64-license-token-data>]
```
Options surrounded with **[]** are optional.
- `--image` Path to an image (JPEG/PNG/BMP) to process. This image will be used to evaluate the liveness detector. You can use default image at [../../../assets/images/disguise.jpg](../../../assets/images/disguise.jpg).
- `--assets` Path to the [assets](../../../assets) folder containing the configuration files and models.
- `--tokenfile` Path to the file containing the base64 license token if you have one. If not provided, then the application will act like a trial version. Default: *null*.
- `--tokendata` Base64 license token if you have one. If not provided, then the application will act like a trial version. Default: *null*.

<a name="testing-examples"></a>
## Examples ##


- On **Linux x86_64**, you may use the next command:
```
LD_LIBRARY_PATH=../../../binaries/linux/x86_64:$LD_LIBRARY_PATH ./benchmark \
    --image ../../../assets/images/disguise.jpg \
    --assets ../../../assets \
    --loops 100 \
    --parallel true
```

- On **Windows x86_64**, you may use the next command:
```
benchmark.exe ^
    --image ../../../assets/images/disguise.jpg ^
    --assets ../../../assets ^
    --loops 100 ^
    --parallel true
```


