/* Copyright (C) 2011-2022 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/FaceLivenessDetection-SDK
* WebSite: https://www.doubango.org/webapps/face-liveness/
*/
/**
\file FLD-SDK-API-PUBLIC.h
FaceLivenessDetection-SDK public header
*/
#ifndef _FACE_LIVENESS_SDK_API_PUBLIC_H_
#define _FACE_LIVENESS_SDK_API_PUBLIC_H_

#include <string>
#include <assert.h>

#define FLD_SDK_VERSION_MAJOR		1
#define FLD_SDK_VERSION_MINOR		5
#define FLD_SDK_VERSION_MICRO		0

// Windows's symbols export
#if defined(SWIG)
# 	define FACE_LIVENESS_SDK_PUBLIC_API
#else
#	if (defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE) || defined(_WIN16) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)) && !defined(FLD_SDK_STATIC)
#		if defined (FACE_LIVENESS_SDK_PUBLIC_EXPORTS)
# 			define FACE_LIVENESS_SDK_PUBLIC_API		__declspec(dllexport)
#		else
# 			define FACE_LIVENESS_SDK_PUBLIC_API		__declspec(dllimport)
#		endif
#	else
# 		define FACE_LIVENESS_SDK_PUBLIC_API			__attribute__((visibility("default")))
#	endif /* WIN32 */
#endif /* SWIG */

// Android OS detection
#if (defined(__ANDROID__) || defined(ANDROID)) && !defined(SWIG)
#	define FLD_SDK_OS_ANDROID	1
#endif /* FLD_SDK_OS_ANDROID */

// Macros to print logs to the console
#if FLD_SDK_OS_ANDROID
#	if !defined(SWIG)
#		include <android/log.h>
#		include <android/asset_manager.h>
#		include <jni.h>
#	endif
#	define FLD_SDK_PRINT_VERBOSE(FMT, ...) __android_log_print(ANDROID_LOG_VERBOSE, "org.doubango.ultimateAlpr.Sdk", "*[FLD_SDK VERBOSE]: " FMT "\n", ##__VA_ARGS__)
#	define FLD_SDK_PRINT_INFO(FMT, ...) __android_log_print(ANDROID_LOG_INFO, "org.doubango.ultimateAlpr.Sdk", "*[FLD_SDK INFO]: " FMT "\n", ##__VA_ARGS__)
#	define FLD_SDK_PRINT_WARN(FMT, ...) __android_log_print(ANDROID_LOG_WARN, "org.doubango.ultimateAlpr.Sdk", "**[FLD_SDK WARN]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#	define FLD_SDK_PRINT_ERROR(FMT, ...) __android_log_print(ANDROID_LOG_ERROR, "org.doubango.ultimateAlpr.Sdk", "***[FLD_SDK ERROR]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#	define FLD_SDK_PRINT_FATAL(FMT, ...) __android_log_print(ANDROID_LOG_FATAL, "org.doubango.ultimateAlpr.Sdk", "****[FLD_SDK FATAL]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#else
#	define FLD_SDK_PRINT_VERBOSE(FMT, ...) fprintf(stderr, "*[FLD_SDK VERBOSE]: " FMT "\n", ##__VA_ARGS__)
#	define FLD_SDK_PRINT_INFO(FMT, ...) fprintf(stderr, "*[FLD_SDK INFO]: " FMT "\n", ##__VA_ARGS__)
#	define FLD_SDK_PRINT_WARN(FMT, ...) fprintf(stderr, "**[FLD_SDK WARN]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#	define FLD_SDK_PRINT_ERROR(FMT, ...) fprintf(stderr, "***[FLD_SDK ERROR]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#	define FLD_SDK_PRINT_FATAL(FMT, ...) fprintf(stderr, "****[FLD_SDK FATAL]: function: \"%s()\" \nfile: \"%s\" \nline: \"%u\" \nmessage: " FMT "\n", __FUNCTION__,  __FILE__, __LINE__, ##__VA_ARGS__)
#endif /* !FLD_SDK_OS_ANDROID */

// Assertion function
#if defined(NDEBUG)
#	define FLD_SDK_ASSERT(x) do { bool __FLD_SDK_b_ret = (x); if (!__FLD_SDK_b_ret) { FLD_SDK_PRINT_FATAL("Assertion failed!"); abort(); } } while(0)
#else
#	define FLD_SDK_ASSERT(x) do { bool __FLD_SDK_b_ret = (x); assert(__FLD_SDK_b_ret); } while(0)
#endif /* !NDEBUG */

namespace FaceLiveness
{
	/*! Defines the image types.
	*/
	enum FLD_SDK_IMAGE_TYPE {
		/*! Each pixel is stored on 3 bytes. Each channel (R, G, B) is stored with 8 bits of precision (256 possible values).
		* Here is how the pixels are packed:
		* \code{.cpp}
		* const int pixel = (B & 0xff) << 16 | (G & 0xff) << 8 | (R & 0xff);
		* \endcode
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_RGB24,

		/*! Each pixel is stored on 4 bytes. Each channel (R, G, B, A) is stored with 8 bits (1 byte) of precision (256 possible values).
		* The R channel is stored at the lowest memory address followed by G, B then A channels. If you're using Android then,
		* this is the same as <a href="https://developer.android.com/reference/android/graphics/Bitmap.Config#ARGB_8888">ARGB_8888</a>.
		* Here is how the pixels are packed:
		* \code{.cpp}
		* const int pixel = (A & 0xff) << 24 | (B & 0xff) << 16 | (G & 0xff) << 8 | (R & 0xff);
		* \endcode
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_RGBA32,

		/*! Each pixel is stored on 4 bytes. Each channel (B, G, R, A) is stored with 8 bits (1 byte) of precision (256 possible values).
		* The B channel is stored at the lowest memory address followed by G, R then A channels. If you're using iOS then,
		* this is the same as <a href="https://developer.apple.com/documentation/corevideo/1563591-pixel_format_identifiers/kcvpixelformattype_32bgra?language=objc">kCVPixelFormatType_32BGRA</a>.
		* Here is how the pixels are packed:
		* \code{.cpp}
		* const int pixel = (A & 0xff) << 24 | (R & 0xff) << 16 | (G & 0xff) << 8 | (B & 0xff);
		* \endcode
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_BGRA32,

		/*! Each pixel is stored on 3 bytes. Each channel (B, G, R) is stored with 8 bits (1 byte) of precision (256 possible values).
		* The B channel is stored at the lowest memory address followed by G then R channels. If you're using C# then,
		* this is the same as <b>PixelFormat.Format24bppRgb</b>.
		* Here is how the pixels are packed:
		* \code{.cpp}
		* const int pixel = (R & 0xff) << 16 | (G & 0xff) << 8 | (B & 0xff);
		* \endcode
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_BGR24,

		/*! YUV 4:2:0 image with a plane of 8 bit Y samples followed by an interleaved U/V plane containing 8 bit 2x2 subsampled colour difference samples.
		*	More information at https://www.fourcc.org/pixel-format/yuv-nv12/
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_NV12,

		/*! YUV 4:2:0 image with a plane of 8 bit Y samples followed by an interleaved V/U plane containing 8 bit 2x2 subsampled chroma samples.
		* The same as \ref FLD_SDK_IMAGE_TYPE_NV12 except the interleave order of U and V is reversed.
		*	More information at https://www.fourcc.org/pixel-format/yuv-nv21/
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_NV21,

		/*! These formats are identical to YV12 except that the U and V plane order is reversed.
		* They comprise an NxM Y plane followed by (N/2)x(M/2) U and V planes.
		* This is the format of choice for many software MPEG codecs.
		* More information at https://www.fourcc.org/pixel-format/yuv-i420/
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_YUV420P,

		/*! Same as \ref FLD_SDK_IMAGE_TYPE_YUV420P except the order of U and V is reversed.
		* More information at https://www.fourcc.org/pixel-format/yuv-yv12/
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_YVU420P,

		/*! YUV 4:2:2 image with an NxM Y plane followed by (N/2)x(M) V and U planes.
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_YUV422P,

		/*! YUV 4:4:4 image with an NxM Y plane followed by NxM V and U planes.
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_YUV444P,

		/*! Grayscale image with single channel (luminance only). Each pixel is stored in single byte (8 bit Y samples).
		*
		* Available since: 0.3.0
		*/
		FLD_SDK_IMAGE_TYPE_Y,
	};

	/*! Result returned by the \ref FldSdkEngine "engine" at initialization, deInitialization and processing stages.
	*/
	class FACE_LIVENESS_SDK_PUBLIC_API FldSdkResult {
	public:
		FldSdkResult();
		FldSdkResult(const int code, const char* phrase, const char* json, const size_t numFaces = 0);
		FldSdkResult(const FldSdkResult& other);
		virtual ~FldSdkResult();
#if !defined(SWIG)
		FldSdkResult& operator=(const FldSdkResult& other) { return operatorAssign(other); }
#endif

		/*! The result code. 0 if success, nonzero otherwise.
		*/
		inline int code()const { return code_; }
		/*! Short description for the \ref code.
		*/
		inline const char* phrase()const { return phrase_; }
		/*! The license plates as JSON content string. May be null if no plate found.
		*/
		inline const char* json()const { return json_; }
		/*! Number of faces in \ref json string. This is a helper function to quickly check whether the result contains faces
		without parsing the \ref json string.
		*/
		inline const size_t numFaces()const { return numFaces_; }
		/*! Whether the result is success. true if success, false otherwise.
		*/
		inline bool isOK()const { return (code_ == 0); }
#if !defined(SWIG)
		static FldSdkResult bodyless(const int code, const char* phrase) { return FldSdkResult(code, phrase, ""); }
		static FldSdkResult bodylessOK() { return FldSdkResult(0, "OK", ""); }
#endif /* SWIG */

	private:
		void ctor(const int code, const char* phrase, const char* json, const size_t numFaces);
#if !defined(SWIG)
		FldSdkResult& operatorAssign(const FldSdkResult& other);
#endif /* SWIG */

	private:
		int code_;
		char* phrase_;
		char* json_;
		size_t numFaces_;
	};

	/*! Callback function to be used to get asynchronous notifications.
	*/
	struct FldSdkParallelDeliveryCallback
	{
	protected:
		FldSdkParallelDeliveryCallback() { }
	public:
		virtual ~FldSdkParallelDeliveryCallback() {  }
		/*! Notification function to override in order to receive the results. */
		virtual void onNewResult(const FldSdkResult* newResult) const = 0;
	};

	/*! The Face Liveness Detection (FLD) engine.
	*/
	class FACE_LIVENESS_SDK_PUBLIC_API FldSdkEngine
	{
#if !defined(SWIG)
	protected:
		FldSdkEngine() = delete;
#endif /* SWIG */
	public:

#if FLD_SDK_OS_ANDROID
		/*! Initializes the engine. This function must be the first one to call.
		This function is only available on Android.
		\param assetManager AssetManager to use to read the content of the "assets" folder containing the models and configuration files.
		\param jsonConfig JSON string containing configuration entries. May be null. More info at https://www.doubango.org/SDKs/face-liveness/docs/Configuration_options.html
		\param parallelDeliveryCallback Callback function to enable parallel mode. Use null value to use sequential instead of parallel mode. More info at https://www.doubango.org/SDKs/face-liveness/docs/Parallel_versus_sequential_processing.html.
		\returns a result
		*/
		static FldSdkResult init(jobject assetManager, const char* jsonConfig = nullptr, const FldSdkParallelDeliveryCallback* parallelDeliveryCallback = nullptr);
#else
		/*! Initializes the engine. This function must be the first one to call.
			\param jsonConfig JSON string containing configuration entries. May be null. More info at https://www.doubango.org/SDKs/face-liveness/docs/Configuration_options.html
			\param parallelDeliveryCallback \ref FldSdkParallelDeliveryCallback "callback" function to enable parallel mode. Use null value to use sequential instead of parallel mode. More info at https://www.doubango.org/SDKs/face-liveness/docs/Parallel_versus_sequential_processing.html.
			\returns a \ref FldSdkResult "result"
		*/
		static FldSdkResult init(const char* jsonConfig = nullptr, const FldSdkParallelDeliveryCallback* parallelDeliveryCallback = nullptr);
#endif /* FLD_SDK_OS_ANDROID */

		/*! DeInitialize the engine. This function must be the last one to be call.
		Deallocate all the resources allocated using \ref init function.
		\returns a \ref FldSdkResult "result"
		*/
		static FldSdkResult deInit();

		/*! Performs face liveness detection operations.
		<br />
		If you're using OpenCV to capture images from the camera or RTSP stream, the function could be used like this:
		\code{.cpp}
		VideoCapture cap(...);

		while (1) {
			Mat frame;
			cap >> frame;

			if (frame.empty()) {
				break;
			}

			FLD_SDK_ASSERT((result = FldSdkEngine::process(
				FLD_SDK_IMAGE_TYPE_BGR24,
				frame.ptr(),
				frame.size().width,
				frame.size().height
			)).isOK());
		}
		\endcode
		\param imageType The image type.
		\param imageDataPtr Pointer to the image data.
		\param imageWidthInSamples Image width in samples.
		\param imageHeightInSamples Image height in samples.
		\param imageStrideInSamples Image stride in samples. Should be zero unless your the data is strided.
		\param imageExifOrientation Image EXIF/JPEG orientation. Must be within [1, 8]. More information at https://www.impulseadventure.com/photo/exif-orientation.html. Available since: 0.3.0.
		\returns a \ref FldSdkResult "result"
		*/
		static FldSdkResult process(
			const FLD_SDK_IMAGE_TYPE imageType,
			const void* imageDataPtr,
			const size_t imageWidthInSamples,
			const size_t imageHeightInSamples,
			const size_t imageStrideInSamples = 0,
			const int imageExifOrientation = 1
		);

		/*! Performs face liveness detection operations.
		<br />
		If you're using FFmpeg, the function could be used like this:
		\code{.cpp}
		FLD_SDK_ASSERT((result = FldSdkEngine::process(
		    FLD_SDK_IMAGE_TYPE_YUV420P,
		    frame->data[0], // Y
		    frame->data[1], // U
		    frame->data[2], // V
		    frame->width, // Width
		    frame->height, // Height
		    frame->linesize[0], // Y-stride
		    frame->linesize[1], // U-stride
		    frame->linesize[2] // V-stride
		)).isOK());
		\endcode
		\param imageType The image type.
		\param yPtr Pointer to the start of the Y (luma) samples.
		\param uPtr Pointer to the start of the U (chroma) samples.
		\param vPtr Pointer to the start of the V (chroma) samples.
		\param widthInSamples Image width in samples.
		\param heightInSamples Image height in samples.
		\param yStrideInBytes Stride in bytes for the Y (luma) samples.
		\param uStrideInBytes Stride in bytes for the U (chroma) samples.
		\param vStrideInBytes Stride in bytes for the V (chroma) samples.
		\param uvPixelStrideInBytes Pixel stride in bytes for the UV (chroma) samples. Should be 1 for planar and 2 for semi-planar formats. Set to 0 for auto-detect.
		\param exifOrientation Image EXIF/JPEG orientation. Must be within [1, 8]. More information at https://www.impulseadventure.com/photo/exif-orientation.html. Available since: 0.3.0.
		\returns a \ref FldSdkResult "result"
		*/
		static FldSdkResult process(
			const FLD_SDK_IMAGE_TYPE imageType,
			const void* yPtr,
			const void* uPtr,
			const void* vPtr,
			const size_t widthInSamples,
			const size_t heightInSamples,
			const size_t yStrideInBytes,
			const size_t uStrideInBytes,
			const size_t vStrideInBytes,
			const size_t uvPixelStrideInBytes = 0,
			const int exifOrientation = 1
		);


		/*! Performs face liveness detection operations.
			\param encodedDataPtr Pointer to the encoded data. Could be JPEG, PNG, BMP or any encoded data.
			\param encodedDataSize The size of the encoded data in bytes.
			\returns a \ref FldSdkResult "result"

			Available since: 1.3.0
		*/
		static FldSdkResult process(
			const void* encodedDataPtr,
			const size_t encodedDataSize
		);

		/*! Retrieve EXIF orientation value from JPEG meta-data.
			\param jpegMetaDataPtr Pointer to the meta-data.
			\param jpegMetaDataSize Size of the meta-data.
			\returns Image's EXIF/JPEG orientation. Must be within [1, 8]. More information at https://www.impulseadventure.com/photo/exif-orientation.html.

			Available since: 0.6.0
		*/
		static int exifOrientation(const void* jpegMetaDataPtr, const size_t jpegMetaDataSize);

		/*! Build a unique runtime license key associated to this device.
			You must \ref init "initialize" the engine before calling this function.
			This function doesn't require internet connection.
			The runtime key must be activated to obtain a token. The activation procedure is explained at https://www.doubango.org/SDKs/LicenseManager/docs/Activation_use_cases.html.
			\param rawInsteadOfJSON Whether to output the runtime key as raw string intead of JSON entry. Requesting raw
			string instead of JSON could be helpful for applications without JSON parser to extract the key.
			\returns a \ref FldSdkResult "result"
		*/
		static FldSdkResult requestRuntimeLicenseKey(const bool& rawInsteadOfJSON = false);

		static FldSdkResult warmUp(const FLD_SDK_IMAGE_TYPE imageType);
#if !defined(SWIG)
		static FldSdkResult optimizeTRT(const char* models_folder);
#endif /* !defined(SWIG) */

#if FLD_SDK_OS_ANDROID && !defined(SWIG)
		static void setAssetManager(AAssetManager* assetManager);
		static void setJavaVM(JavaVM* vm);

	private:
		static bool s_bOweAAssetManager;
#endif /* FLD_SDK_OS_ANDROID */
	};

} // namespace FaceLiveness 

#endif // _FACE_LIVENESS_SDK_API_PUBLIC_H_
