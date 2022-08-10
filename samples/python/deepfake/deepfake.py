'''
    * Copyright (C) 2011-2022 Doubango Telecom <https://www.doubango.org>
    * File author: Mamadou DIOP (Doubango Telecom, France).
    * License: For non commercial use only.
    * Source code: https://github.com/DoubangoTelecom/FaceLivenessDetection-SDK
    * WebSite: https://www.doubango.org/webapps/face-liveness/


    https://github.com/DoubangoTelecom/FaceLivenessDetection-SDK/blob/master/samples/python/deepfake/README.md
	Usage: 
		recognizer.py \
			--video <path-to-video-to-process> \
			--assets <path-to-assets-folder>\
			[--tokenfile <path-to-license-token-file>] \
			[--tokendata <base64-license-token-data>]
	Example:
		python ../../../samples/python/deepfake/deepfake.py --video myvideo.mp4 --assets ../../../assets

    To add audio: `ffmpeg -y -i output.mp4 -i youtube.mp4 -c copy -map 0:v:0 -map 1:a:0 mixed.mp4`
    To generate Twitter-friendly video: `ffmpeg -y -i mixed.mp4 -c:v libx264 -crf 23 -vf crop=iw:ih-1 -c:a copy mixed-twitter.mp4`
'''

import FaceLivenessDetectionSDK
import argparse
import json
import cv2
import os
import numpy as np

QUEUE_SIZE = 25 # Queue used to average the scores
DEEPFAKE_MINSCORE_PERCENT = 0.5 # Threshold (.0=20%)
OUTPUT_VIDEO_PATH = './output.mp4'

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
    "detect_face_minsize": 64,
    
    "liveness_detect_enabled": False,
    "liveness_tf_num_threads": -1,
    "liveness_tf_gpu_memory_alloc_max_percent": 0.2,
    "liveness_face_minsize": 64,
    "liveness_genuine_minscore": 0.98,
    "liveness_disputed_minscore": 0.5,
    "liveness_toofar_threshold": 0.5,

    "deepfake_detect_enabled": True,
    "deepfake_tf_num_threads": -1,
    "deepfake_tf_gpu_memory_alloc_max_percent": 0.2,
    "deepfake_minscore": DEEPFAKE_MINSCORE_PERCENT,
    
    "disguise_detect_enabled": False,
    "disguise_tf_num_threads": -1,
    "disguise_tf_gpu_memory_alloc_max_percent": 0.2,
    "disguise_minscore": 0.5
}

TAG = "[PythonDeepfake] "

# Quick&Dirty Tracker (MUST NOT USE, use a real tracker like DeepSort)
class QuicKNDirtyTracker:
    def __init__(self, queue_size=QUEUE_SIZE):
        self.queue_size = queue_size
        self.deepfakeMinscore = JSON_CONFIG['deepfake_minscore'] * 100.0
        self.faceId = 0
        self.reset()

    def update(self, result_dict):
        faces = result_dict['faces']
        if not faces is None and len(faces) > 0:
            mainFace = faces[0]
            warpedBox = mainFace['warpedBox']

            # Check if it's the same face
            left_top, right_bottom = (warpedBox[0], warpedBox[1]), (warpedBox[4], warpedBox[5])
            padx = (right_bottom[0] - left_top[0]) * 0.2
            pady = (right_bottom[1] - left_top[1]) * 0.2
            left_top_extended = (left_top[0] - padx, left_top[1] - pady)
            right_bottom_extended = (left_top[0] + padx, left_top[1] + pady)
            overlaps = self.faceCorner[0] >= left_top_extended[0] and self.faceCorner[0] <= right_bottom_extended[0] \
                and self.faceCorner[1] >= left_top_extended[1] and self.faceCorner[1] <= right_bottom_extended[1]

            if not overlaps:
                print('Got new face:', self.faceId)
                self.reset()

            # Averaging the score the score
            if 'deepfake_score' in mainFace:
                self.avgQueue[self.frameIndex % self.queue_size] = mainFace['deepfake_score']
            isGenuineStrike = False#(self.avgQueue < self.deepfakeMinscore).all()
            self.isDeepfake = self.isDeepfake and not isGenuineStrike # Switch the flag if we get a strike
            self.avgScore = np.mean(self.avgQueue)
            self.isDeepfake = self.isDeepfake or ((mainFace['liveness_code'] == 's_deepfake') and (self.avgScore >= self.deepfakeMinscore))
            self.faceCorner = left_top
            self.frameIndex += 1
        
        pass

    def reset(self):
        self.avgQueue = np.full((self.queue_size), self.deepfakeMinscore, dtype=np.float)
        self.avgScore = np.mean(self.avgQueue)
        self.faceCorner = (-1000.0, -1000.0)
        self.isDeepfake = False
        self.frameIndex = 0
        self.faceId += 1

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

    parser.add_argument("--video", required=True, help="Path to the video to process")
    parser.add_argument("--assets", required=False, default="../../../assets", help="Path to the assets folder")
    parser.add_argument("--tokenfile", required=False, default="", help="Path to license token file")
    parser.add_argument("--tokendata", required=False, default="", help="Base64 license token data")

    args = parser.parse_args()

    # Check if image exist
    if not os.path.isfile(args.video):
        print(TAG + "File doesn't exist: %s" % args.video)
        exit(-1)

    # Open the video
    cap = cv2.VideoCapture(args.video)
    if (not cap.isOpened() == True):
        print(TAG + "Failed to open video: %s" % args.video)
        exit(-1)

    # Update JSON options using values from the command args
    JSON_CONFIG["assets_folder"] = args.assets
    JSON_CONFIG["license_token_file"] = args.tokenfile
    JSON_CONFIG["license_token_data"] = args.tokendata

    # Initialize the engine
    checkResult("Init", 
                FaceLivenessDetectionSDK.FldSdkEngine_init(json.dumps(JSON_CONFIG))
               )

    # WarmUp: Force loading the models in memory (slow for first time) now and perform warmup calls.
    # Warmup not required but processing will be fast if you call warm up first.
    checkResult("warmUp", 
                FaceLivenessDetectionSDK.FldSdkEngine_warmUp(FaceLivenessDetectionSDK.FLD_SDK_IMAGE_TYPE_BGR24)
               )

    # Averaging queue, this is a quick&dirty solution that you must **not** use.
    # You should use a face tracker and average the scores using the faceIds.
    tracker = QuicKNDirtyTracker(queue_size=QUEUE_SIZE)

    # Create video writer
    video_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    video_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    video_fps = int(cap.get(cv2.CAP_PROP_FPS))
    video_out = cv2.VideoWriter(OUTPUT_VIDEO_PATH, cv2.VideoWriter_fourcc('M', 'P', '4', 'V'), video_fps, (video_width, video_height))

    # Looping through the frames
    while (cap.isOpened()):
        # Read next frame
        ret, frame = cap.read()

        # Check
        if (frame is None or not ret == True):
            print(TAG + "Done reading the video or error occured: %s" % args.video)
            break

        # Please note that the first time you call this function all deep learning models will be loaded 
        # and initialized which means it will be slow. In your application you've to initialize the engine
        # once and do all the recognitions you need, then deinitialize it.
        # Call warmUp to avoid a slow processing for the first call.
        result = FaceLivenessDetectionSDK.FldSdkEngine_process(
                        FaceLivenessDetectionSDK.FLD_SDK_IMAGE_TYPE_BGR24,
                        frame.tobytes(), # data
                        frame.shape[1], # width
                        frame.shape[0], # height
                        0, # stride
                        1 # exifOrientation
                )
        
        # Check result
        if not result.isOK():
            print(TAG + "Process : failed -> " + result.phrase())
            break

        # Parse JSON result
        result_dict = json.loads(result.json())

        # Update tracker
        tracker.update(result_dict)

        # Draw faces
        faces = result_dict['faces']
        if not faces is None and len(faces) > 0:
            # https://stackoverflow.com/a/65146731/11169713
            def draw_text(img, text,
                font=cv2.FONT_HERSHEY_PLAIN,
                pos=(0, 0),
                font_scale=3,
                font_thickness=2,
                text_color=(0, 255, 0),
                text_color_bg=(0, 0, 0)
                ):

                x, y = pos
                text_size, _ = cv2.getTextSize(text, font, font_scale, font_thickness)
                text_w, text_h = text_size
                cv2.rectangle(img, pos, (x + text_w, y - text_h), text_color_bg, thickness=-1)
                return cv2.putText(img, text, (x, y), font, font_scale, text_color, font_thickness)

            for index, face in enumerate(faces):
                warpedBox = face['warpedBox']

                # Draw rectancle around the face
                color = ((0, 0, 255) if tracker.isDeepfake else (0, 255, 0)) if index == 0 else (0, 255, 255)
                pt1, pt2 = (int(warpedBox[0]), int(warpedBox[1])), (int(warpedBox[4]), int(warpedBox[5]))
                frame = cv2.rectangle(img=frame, pt1=pt1, pt2=pt2, color=color, thickness=(2 if index==0 else 1))
                # Draw Text (for main face only - First)
                if index == 0:
                    text = ('DeepFake' if tracker.isDeepfake else 'Real') + ' (%.2f%%)' % tracker.avgScore
                    frame = draw_text(img=frame, text=text, pos=pt1, font=cv2.FONT_HERSHEY_PLAIN, font_scale=1, text_color=(0,0,0), font_thickness=2, text_color_bg=color)

        # Dispplay the result
        cv2.imshow('Frame', frame)

        # Write frame
        video_out.write(frame)

        # Press 'q' to break the loop
        if (cv2.waitKey(1) & 0xFF == ord('q')):
            break

    # Close the video streams
    cap.release()
    video_out.release()

    # Close all windows
    cv2.destroyAllWindows()

    # DeInit the engine
    checkResult("DeInit", 
                FaceLivenessDetectionSDK.FldSdkEngine_deInit()
               )
    
    