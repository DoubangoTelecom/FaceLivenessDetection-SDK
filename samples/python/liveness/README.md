- [Dependencies](#dependencies)
  - [Debugging missing dependencies](#dependencies-debugging)
- [GPGPU acceleration](#gpu-acceleration)
- [Prerequisite](#prerequisite)
- [Usage](#testing-usage)
- [Examples](#testing-examples)
- [Know issues](#testing-know-issues)

This application is used as reference code for developers to show how to use the Python bindings for the [C++ API](https://www.doubango.org/SDKs/face-liveness/docs/cpp-api.html) and could
be used to easily check the accuracy. The application accepts path to a JPEG/PNG/BMP file as input. 

If you don't want to build this sample and is looking for a quick way to check the accuracy then, try
our cloud-based solution at [https://www.doubango.org/webapps/face-liveness/](https://www.doubango.org/webapps/face-liveness/).

This sample is open source and doesn't require registration or license key.

<a name="dependencies"></a>
# Dependencies #
**The SDK is developed in C++11** and you'll need **glibc 2.27+** on *Linux* and **[Microsoft Visual C++ 2015 Redistributable(x64) - 14.0.24123](https://www.microsoft.com/en-us/download/details.aspx?id=52685)** (any later version is ok) on *Windows*.  **You most likely already have these dependencies on you machine** as almost every program require it.

<a name="dependencies-debugging"></a>
## Debugging missing dependencies ##
To check if all dependencies are present:
- **Windows x86_64:** Use [Dependency Walker](https://www.dependencywalker.com/) on [binaries/windows/x86_64/FaceLivenessDetectionSDK.dll](../../../binaries/windows/x86_64/FaceLivenessDetectionSDK.dll) and [binaries/windows/x86_64/ultimatePluginOpenVINO.dll](../../../binaries/windows/x86_64/ultimatePluginOpenVINO.dll) if you're planning to use [OpenVINO](https://docs.openvinotoolkit.org/).
- **Linux x86_64:** Use `ldd <your-shared-lib>` on [binaries/linux/x86_64/libFaceLivenessDetectionSDK.so](../../../binaries/linux/x86_64/libFaceLivenessDetectionSDK.so) and [binaries/linux/x86_64/libultimatePluginOpenVINO.so](../../../binaries/linux/x86_64/libultimatePluginOpenVINO.so) if you're planning to use [OpenVINO](https://docs.openvinotoolkit.org/).

<a name="gpu-acceleration"></a>
# GPGPU acceleration #
By default GPGPU acceleration is disabled. Check [here](../../cpp/README.md#gpu-acceleration) for more information on how to enable it.

<a name="prerequisite"></a>
# Prerequisite #

[**You must build the Python extension**](../../../python/README.md) before trying to run this sample. More information on how to build the extension could be found [here](../../../python/README.md)

<a name="testing-usage"></a>
# Usage #

`liveness.py` is a Python command line application with the following usage:
```
liveness.py \
      --image <path-to-image-to-process> \
      --assets <path-to-assets-folder> \
      [--tokenfile <path-to-license-token-file>] \
      [--tokendata <base64-license-token-data>]
```
Options surrounded with **[]** are optional.
- `--image` Path to the image(JPEG/PNG/BMP) to process. You can use default image at [../../../assets/images/disguise.jpg](../../../assets/images/disguise.jpg).
- `--assets` Path to the [assets](../../../assets) folder containing the configuration files and models.
- `--tokenfile` Path to the file containing the base64 license token if you have one. If not provided then, the application will act like a trial version. Default: *null*.
- `--tokendata` Base64 license token if you have one. If not provided then, the application will act like a trial version. Default: *null*.

<a name="testing-examples"></a>
# Examples #

## Move to the binaries folder ##
Before trying the next examples you have to navigate to the folder containing the [binaries](../binaries):
```
cd FaceLivenessDetection-SDK/binaries/<<os>>/<<arch>>
```
For example:
 * On Windows x86_64: [FaceLivenessDetection-SDK/binaries/windows/x86_64](../../../binaries/windows/x86_64)
 * On Linux x86_64: [FaceLivenessDetection-SDK/binaries/linux/x86_64](../../../binaries/linux/x86_64)
 * ... you got the idea

## Try ##

- On **Linux x86_64**, you may use the next command:
```
PYTHONPATH=$PYTHONPATH:.:../../../python \
LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH \
python ../../../samples/python/liveness/liveness.py --image ../../../assets/images/disguise.jpg --assets ../../../assets
```
Before trying to run the program on **Linux x86_64 you'll need to download libtensorflow.so as explained [here](../../cpp/README.md#gpu-acceleration-tensorflow-linux)**.

- On **Windows x86_64**, you may use the next command:
```
setlocal
set PYTHONPATH=%PYTHONPATH%;.;../../../python
set PATH=%PATH%;%~dp0
python ../../../samples/python/liveness/liveness.py --image ../../../assets/images/disguise.jpg --assets ../../../assets
endlocal
```
If you want to make your life easier run [python_liveness.bat](../../../binaries/windows/x86_64/python_liveness.bat) to test on Windows. You can edit the file using Notepad to change the parameters.

The test image looks like this:
![Test image](../../../assets/images/disguise.jpg)

<a name="testing-know-issues"></a>
# Know issues #
If you get `undefined symbol: PyUnicode_FromFormat` error message, then make sure you're using Python 3 and same version as the one used to buid the extension. We tested the code on version **3.6.9** (Windows 8), **3.6.8** (Ubuntu 18) and **3.7.3** (Raspbian Buster). Run `python --version` to print your Python version. You may use `python3` instead of `python` to make sure you're using version 3.


