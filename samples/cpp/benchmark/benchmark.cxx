/* Copyright (C) 2011-2021 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/FaceLivenessDetection-SDK
* WebSite: https://www.doubango.org/webapps/face-liveness/
*/
#include <FLD-SDK-API-PUBLIC.h>

#include "fld_image_utils.h"
#include <chrono>
#include <vector>
#include <algorithm>
#include <random>
#include <mutex>
#include <condition_variable>
#if defined(_WIN32)
#include <algorithm> // std::replace
#endif

using namespace FaceLiveness;

// Asset manager used on Android to files in "assets" folder
#if FLD_SDK_OS_ANDROID 
#	define ASSET_MGR_PARAM() __sdk_android_assetmgr, 
#else
#	define ASSET_MGR_PARAM() 
#endif /* FLD_SDK_OS_ANDROID */

// Configuration for the deep learning engine
static const char* __jsonConfig =
"{"
"\"debug_level\": \"info\","
"\"debug_write_input_image_enabled\": false,"
"\"debug_internal_data_path\": \".\","
""
"\"num_threads\": -1,"
"\"gpgpu_enabled\": true,"
"\"max_latency\": -1,"
"\"image_interpolation\": \"bicubic\","
"\"asm_enabled\": true,"
"\"intrin_enabled\": true,"
""
"\"openvino_enabled\": false,"
"\"openvino_device\": \"CPU\","
""
"\"detect_tf_num_threads\": -1,"
"\"detect_tf_gpu_memory_alloc_max_percent\": 0.2,"
"\"detect_roi\": [0, 0, 0, 0],"
"\"detect_minscore\": 0.9,"
"\"detect_face_minsize\": 128,"
""
"\"liveness_tf_num_threads\": -1,"
"\"liveness_tf_gpu_memory_alloc_max_percent\": 0.2,"
"\"liveness_face_minsize\": 128,"
"\"liveness_genuine_minscore\": 0.98,"
"\"liveness_disputed_minscore\": 0.5,"
"\"liveness_toofar_threshold\": 0.5,"
""
"\"disguise_detect_enabled\": true,"
"\"disguise_tf_num_threads\": -1,"
"\"disguise_tf_gpu_memory_alloc_max_percent\": 0.2,"
"\"disguise_minscore\": 0.5"
""
;

/*
* Parallel callback function used for notification. Not mandatory.
* More info about parallel delivery: https://www.doubango.org/SDKs/anpr/docs/Parallel_versus_sequential_processing.html
*/
static size_t parallelNotifCount = 0;
static std::condition_variable parallelNotifCondVar;
class MyFldSdkParallelDeliveryCallback : public FldSdkParallelDeliveryCallback {
public:
	MyFldSdkParallelDeliveryCallback(const void* userData) : m_pMyDummyData(userData) {}
	virtual void onNewResult(const FldSdkResult* result) const override {
		// Use m_pMyDummyData here if you want
		FLD_SDK_ASSERT(result != nullptr);
		const std::string& json = result->json();
		// Printing to the console could be very slow and delayed -> stop displaying the result as soon as all faces are processed
		FLD_SDK_PRINT_INFO("MyFldSdkParallelDeliveryCallback::onNewResult(%d, %s, %zu): %s",
			result->code(),
			result->phrase(),
			++parallelNotifCount,
			!json.empty() ? json.c_str() : "{}"
		);
		parallelNotifCondVar.notify_one();
	}
private:
	const void* m_pMyDummyData;
};

static void printUsage(const std::string& message = "");

int main(int argc, char *argv[])
{
	// local variables
	FldSdkResult result;
	MyFldSdkParallelDeliveryCallback parallelDeliveryCallbackCallback(nullptr);
	std::string assetsFolder, licenseTokenData, licenseTokenFile;
	bool isParallelDeliveryEnabled = true;
	size_t loopCount = 100;
	std::string imagePath;

	// Parsing args
	std::map<std::string, std::string > args;
	if (!fldParseArgs(argc, argv, args)) {
		printUsage();
		return -1;
	}
	if (args.find("--image") == args.end()) {
		printUsage("--image required");
		return -1;
	}
	if (args.find("--assets") == args.end()) {
		printUsage("--assets required");
		return -1;
	}
	imagePath = args["--image"];
	
	if (args.find("--loops") != args.end()) {
		const int loops = std::atoi(args["--loops"].c_str());
		if (loops < 1) {
			printUsage("--loops must be within [1, inf]");
			return -1;
		}
		loopCount = static_cast<size_t>(loops);
	}
	if (args.find("--parallel") != args.end()) {
		isParallelDeliveryEnabled = (args["--parallel"].compare("true") == 0);
	}
	if (args.find("--assets") != args.end()) {
		assetsFolder = args["--assets"];
#if defined(_WIN32)
		std::replace(assetsFolder.begin(), assetsFolder.end(), '\\', '/');
#endif
	}
	
	if (args.find("--tokenfile") != args.end()) {
		licenseTokenFile = args["--tokenfile"];
#if defined(_WIN32)
		std::replace(licenseTokenFile.begin(), licenseTokenFile.end(), '\\', '/');
#endif
	}
	if (args.find("--tokendata") != args.end()) {
		licenseTokenData = args["--tokendata"];
	}


	// Update JSON config
	std::string jsonConfig = __jsonConfig;
	if (!assetsFolder.empty()) {
		jsonConfig += std::string(",\"assets_folder\": \"") + assetsFolder + std::string("\"");
	}
	if (!licenseTokenFile.empty()) {
		jsonConfig += std::string(",\"license_token_file\": \"") + licenseTokenFile + std::string("\"");
	}
	if (!licenseTokenData.empty()) {
		jsonConfig += std::string(",\"license_token_data\": \"") + licenseTokenData + std::string("\"");
	}

	jsonConfig += "}"; // end-of-config

	// Decode image
	FldFile fldFile;
	if (!fldDecodeFile(imagePath, fldFile)) {
		FLD_SDK_PRINT_INFO("Failed to read image file: %s", imagePath.c_str());
		return -1;
	}

	// Init
	FLD_SDK_PRINT_INFO("Starting benchmark...");
	FLD_SDK_ASSERT((result = FldSdkEngine::init(
		ASSET_MGR_PARAM()
		jsonConfig.c_str(),
		isParallelDeliveryEnabled ? &parallelDeliveryCallbackCallback : nullptr
	)).isOK());

	// WarmUp: Force loading the models in memory (slow for first time) now and perform warmup calls.
	// Warmup not required by processing will be fast if you call warm up first.
	FLD_SDK_ASSERT((result = FldSdkEngine::warmUp(fldFile.type)).isOK());

	// Processing
	const std::chrono::high_resolution_clock::time_point timeStart = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < loopCount; ++i) {
		FLD_SDK_ASSERT((result = FldSdkEngine::process(
			fldFile.type,
			fldFile.uncompressedData,
			fldFile.width,
			fldFile.height
		)).isOK());
	}
	// Compute the estimated frame rate.
	// At this step all frames are already processed but the result could be still on the delivery
	// queue due to the console display latency. You can move here the code used to wait until all
	// messages are displayed to include the delivery latency.
	const std::chrono::high_resolution_clock::time_point timeEnd = std::chrono::high_resolution_clock::now();
	const double elapsedTimeInMillis = std::chrono::duration_cast<std::chrono::duration<double >>(timeEnd - timeStart).count() * 1000.0;
	FLD_SDK_PRINT_INFO("Elapsed time (Liveness) = [[[ %lf millis ]]]", elapsedTimeInMillis);

	// Printing to the console is very slow and use a low priority thread.
	// Wait until all results are displayed.
	if (isParallelDeliveryEnabled) {
		static std::mutex parallelNotifMutex;
		std::unique_lock<std::mutex > lk(parallelNotifMutex);
		parallelNotifCondVar.wait_for(lk,
			std::chrono::milliseconds(1500), // maximum number of millis to wait for before giving up, must never wait this long
			[&loopCount] { return (parallelNotifCount == loopCount); }
		);
	}

	// Print latest result
	const std::string& json_ = result.json();
	if (!json_.empty()) {
		FLD_SDK_PRINT_INFO("result: %s", json_.c_str());
	}

	// Print estimated frame rate
	const double estimatedFps = 1000.f / (elapsedTimeInMillis / (double)loopCount);
	FLD_SDK_PRINT_INFO("*** elapsedTimeInMillis: %lf, estimatedFps: %lf ***", elapsedTimeInMillis, estimatedFps);

	FLD_SDK_PRINT_INFO("Press any key to terminate !!");
	getchar();

	// DeInit
	FLD_SDK_PRINT_INFO("Ending benchmark...");
	FLD_SDK_ASSERT((result = FldSdkEngine::deInit()).isOK());

	return 0;
}

static void printUsage(const std::string& message /*= ""*/)
{
	if (!message.empty()) {
		FLD_SDK_PRINT_ERROR("%s", message.c_str());
	}

	FLD_SDK_PRINT_INFO(
		"\n********************************************************************************\n"
		"benchmark\n"
		"\t--image <path-to-image-with-a-face-to-analyse> \n"
		"\t--assets <path-to-assets-folder> \n"
		"\t[--loops <number-of loops>] \n"
		"\t[--parallel <whether-to-enable-parallel-mode:true / false>] \n"
		"\t[--tokenfile <path-to-license-token-file>] \n"
		"\t[--tokendata <base64-license-token-data>] \n"
		"\n"
		"Options surrounded with [] are optional.\n"
		"\n"
		"--image: Path to an image(JPEG/PNG/BMP) with a license face. This image will be used to evaluate the liveness detector. You can use default image at ../../../assets/images/disguise.jpg.\n\n"
		"--assets: Path to the assets folder containing the configuration files and models.\n\n"
		"--loops: Number of times to run the processing function. Default: 100. \n\n"
		"--parallel: Whether to enabled the parallel mode. More info about the parallel mode at https ://www.doubango.org/SDKs/anpr/docs/Parallel_versus_sequential_processing.html. Default: true.\n\n"
		"--tokenfile: Path to the file containing the base64 license token if you have one. If not provided then, the application will act like a trial version. Default: null.\n\n"
		"--tokendata: Base64 license token if you have one. If not provided then, the application will act like a trial version. Default: null.\n\n"
		"********************************************************************************\n"
	);
}