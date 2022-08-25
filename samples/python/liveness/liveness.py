'''
    * Copyright (C) 2011-2022 Doubango Telecom <https://www.doubango.org>
    * File author: Mamadou DIOP (Doubango Telecom, France).
    * License: For non commercial use only.
    * Source code: https://github.com/DoubangoTelecom/FaceLivenessDetection-SDK
    * WebSite: https://www.doubango.org/webapps/face-liveness/


    https://github.com/DoubangoTelecom/FaceLivenessDetection-SDK/blob/master/samples/python/liveness/README.md
	Usage: 
		recognizer.py \
			--image <path-to-image-with-face-to-process> \
			--assets <path-to-assets-folder>\
            [--tokenfile <path-to-license-token-file>] \
			[--tokendata <base64-license-token-data>]
	Example:
		python ../../../samples/python/liveness/liveness.py --image ../../../assets/images/disguise.jpg --assets ../../../assets
'''

import FaceLivenessDetectionSDK
import os, argparse, json

# Defines the default JSON configuration. More information at https://www.doubango.org/SDKs/face-liveness/docs/Configuration_options.html
JSON_CONFIG = {
    "debug_level": "info",
    "debug_write_input_image_enabled": False,
    "debug_internal_data_path": ".",

    "num_threads": -1,
    "gpgpu_enabled": True,
    "max_latency": -1,
    "mage_interpolation": "bicubic",
    "asm_enabled": True,
    "intrin_enabled": True,
    
    "openvino_enabled": False,
    "openvino_device": "CPU",
    
    "detect_tf_num_threads": -1,
    "detect_tf_gpu_memory_alloc_max_percent": 0.2,
    "detect_roi": [0, 0, 0, 0],
    "detect_minscore": 0.93,
    "detect_face_minsize": 65,
    
    "liveness_detect_enabled": True,
    "liveness_tf_num_threads": -1,
    "liveness_tf_gpu_memory_alloc_max_percent": 0.2,
    "liveness_face_minsize": 64,
    "liveness_genuine_minscore": 0.98,
    "liveness_disputed_minscore": 0.5,
    "liveness_toofar_threshold": 0.5,

    "deepfake_detect_enabled": True,
    "deepfake_tf_num_threads": -1,
    "deepfake_tf_gpu_memory_alloc_max_percent": 0.2,
    "deepfake_minscore": 0.5,
    
    "disguise_detect_enabled": True,
    "disguise_tf_num_threads": -1,
    "disguise_tf_gpu_memory_alloc_max_percent": 0.2,
    "disguise_minscore": 0.5
}

TAG = "[PythonLiveness] "

# Check result
def checkResult(operation, result):
    if not result.isOK():
        print(TAG + operation + ": failed -> " + result.phrase())
        assert False
    else:
        print(TAG + operation + ": OK -> " + result.json())

# Entry point
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="""
    This is the recognizer sample using python language
    """)

    parser.add_argument("--image", required=True, help="Path to the image with Face data to process")
    parser.add_argument("--assets", required=False, default="../../../assets", help="Path to the assets folder")
    parser.add_argument("--tokenfile", required=False, default="", help="Path to license token file")
    parser.add_argument("--tokendata", required=False, default="", help="Base64 license token data")

    args = parser.parse_args()

    # Check if image exist
    if not os.path.isfile(args.image):
        print(TAG + "File doesn't exist: %s" % args.image)
        assert False

    # Update JSON options using values from the command args
    JSON_CONFIG["assets_folder"] = args.assets
    JSON_CONFIG["license_token_file"] = args.tokenfile
    JSON_CONFIG["license_token_data"] = args.tokendata

    # Initialize the engine
    checkResult("Init", 
                FaceLivenessDetectionSDK.FldSdkEngine_init(json.dumps(JSON_CONFIG))
               )

    # Process
    # Please note that the first time you call this function all deep learning models will be loaded 
    # and initialized which means it will be slow. In your application you've to initialize the engine
    # once and do all the recognitions you need, then deinitialize it.
    # Call warmUp to avoid a slow processing for the first call.
    with open(args.image, 'rb') as file:
        checkResult("Process",
                    FaceLivenessDetectionSDK.FldSdkEngine_process(
                        file.read(),
                        os.fstat(file.fileno()).st_size
                        )
            )

    # Press any key to exit
    input("\nPress Enter to exit...\n") 

    # DeInit the engine
    checkResult("DeInit", 
                FaceLivenessDetectionSDK.FldSdkEngine_deInit()
               )
    
    