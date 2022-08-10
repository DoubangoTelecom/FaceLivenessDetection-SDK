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
import argparse
import json
import os

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
    "detect_minscore": 0.9,
    "detect_face_minsize": 128,
    
    "liveness_detect_enabled": True,
    "liveness_tf_num_threads": -1,
    "liveness_tf_gpu_memory_alloc_max_percent": 0.2,
    "liveness_face_minsize": 128,
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

IMAGE_TYPES_MAPPING = { 
    'RGB': FaceLivenessDetectionSDK.FLD_SDK_IMAGE_TYPE_RGB24,
    'RGBA': FaceLivenessDetectionSDK.FLD_SDK_IMAGE_TYPE_RGBA32,
    'L': FaceLivenessDetectionSDK.FLD_SDK_IMAGE_TYPE_Y
}

# Load image
def load_pil_image(path):
    from PIL import Image, ExifTags, ImageOps
    import traceback
    pil_image = Image.open(path)
    img_exif = pil_image.getexif()
    ret = {}
    orientation  = 1
    try:
        if img_exif:
            for tag, value in img_exif.items():
                decoded = ExifTags.TAGS.get(tag, tag)
                ret[decoded] = value
            orientation  = ret["Orientation"]
    except Exception as e:
        print(TAG + "An exception occurred: {}".format(e))
        traceback.print_exc()

    if orientation > 1:
        pil_image = ImageOps.exif_transpose(pil_image)

    if pil_image.mode in IMAGE_TYPES_MAPPING:
        imageType = IMAGE_TYPES_MAPPING[pil_image.mode]
    else:
        raise ValueError(TAG + "Invalid mode: %s" % pil_image.mode)

    return pil_image, imageType

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

    # Decode the image and extract type
    image, imageType = load_pil_image(args.image)
    width, height = image.size

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
    checkResult("Process",
                FaceLivenessDetectionSDK.FldSdkEngine_process(
                    imageType,
                    image.tobytes(), # type(x) == bytes
                    width,
                    height,
                    0, # stride
                    1 # exifOrientation (already rotated in load_pil_image -> use default value: 1)
                    )
        )

    # Press any key to exit
    input("\nPress Enter to exit...\n") 

    # DeInit the engine
    checkResult("DeInit", 
                FaceLivenessDetectionSDK.FldSdkEngine_deInit()
               )
    
    