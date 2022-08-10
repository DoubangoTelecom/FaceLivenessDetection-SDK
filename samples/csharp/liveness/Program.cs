/* Copyright (C) 2011-2021 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/FaceLivenessDetection-SDK
* WebSite: https://www.doubango.org/webapps/face-liveness/
*/
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Web.Script.Serialization;
using org.doubango.FaceLivenessDetection.Sdk;

namespace liveness
{
    class Program
    {
        static void Main(String[] args)
        {
            // Parse arguments
            IDictionary<String, String> parameters = ParseArgs(args);

            // Make sur the image is provided using args
            if (!parameters.ContainsKey("--image"))
            {
                Console.Error.WriteLine("--image required");
                throw new Exception("--image required");
            }
            // Extract assets folder
            // https://www.doubango.org/SDKs/face-liveness/docs/Configuration_options.html#assets-folder
            String assetsFolder = parameters.ContainsKey("--assets")
                ? parameters["--assets"] : String.Empty;

            // License data - Optional
            // https://www.doubango.org/SDKs/face-liveness/docs/Configuration_options.html#license-token-data
            String tokenDataBase64 = parameters.ContainsKey("--tokendata")
                ? parameters["--tokendata"] : String.Empty;

            // Initialize the engine: Load deep learning models and init GPU shaders
            // Make sure de disable VS hosting process to see logs from native code: https://social.msdn.microsoft.com/Forums/en-US/5da6cdb2-bc2b-4fff-8adf-752b32143dae/printf-from-dll-in-console-app-in-visual-studio-c-2010-express-does-not-output-to-console-window?forum=Vsexpressvcs
            // This function should be called once.
            FldSdkResult result = CheckResult("Init", FldSdkEngine.init(BuildJSON(assetsFolder, tokenDataBase64)));

            // Decode the JPEG/PNG/BMP file
            String file = parameters["--image"];
            if (!System.IO.File.Exists(file))
            {
                throw new System.IO.FileNotFoundException("File not found:" + file);
            }
            Bitmap image = new Bitmap(file);
            if (Image.GetPixelFormatSize(image.PixelFormat) == 24 && ((image.Width * 3) & 3) != 0)
            {
                //!\\ Not DWORD aligned -> the stride will be multiple of 4-bytes instead of 3-bytes
                // ultimateMICR requires stride to be in samples unit instead of in bytes
                Console.Error.WriteLine(String.Format("//!\\ The image width ({0}) not a multiple of DWORD.", image.Width));
                image = new Bitmap(image, new Size((image.Width + 3) & -4, image.Height));
            }
            int bytesPerPixel = Image.GetPixelFormatSize(image.PixelFormat) >> 3;
            if (bytesPerPixel != 1 && bytesPerPixel != 3 && bytesPerPixel != 4)
            {
                throw new System.Exception("Invalid BPP:" + bytesPerPixel);
            }

            // Extract Exif orientation
            const int ExifOrientationTagId = 0x112;
            int orientation = 1;
            if (Array.IndexOf(image.PropertyIdList, ExifOrientationTagId) > -1)
            {
                int orientation_ = image.GetPropertyItem(ExifOrientationTagId).Value[0];
                if (orientation_ >= 1 && orientation_ <= 8)
                {
                    orientation = orientation_;
                }
            }

            // Get the image format
            FLD_SDK_IMAGE_TYPE format = (bytesPerPixel == 1) ? FLD_SDK_IMAGE_TYPE.FLD_SDK_IMAGE_TYPE_Y : (bytesPerPixel == 4 ? FLD_SDK_IMAGE_TYPE.FLD_SDK_IMAGE_TYPE_BGRA32 : FLD_SDK_IMAGE_TYPE.FLD_SDK_IMAGE_TYPE_BGR24);

            // WarmUp: Force loading the models in memory (slow for first time) now and perform warmup calls.
            // Warmup not required but processing will be fast if you call warm up first.
            result = CheckResult("warmUp", FldSdkEngine.warmUp(format));

            // First inference is expected to be slow (deep learning models mapping to CPU/GPU memory)
            // Call warmUp function to avoid slow processing on first call
            BitmapData imageData = image.LockBits(new Rectangle(0, 0, image.Width, image.Height), ImageLockMode.ReadOnly, image.PixelFormat);
            try
            {
                // Processing
                result = CheckResult("Process", FldSdkEngine.process(
                         format,
                        imageData.Scan0,
                        (uint)imageData.Width,
                        (uint)imageData.Height,
                        (uint)(imageData.Stride / bytesPerPixel),
                        orientation
                    ));
                // Print result to console
                Console.WriteLine("Result: {0}", result.json());
            }
            finally
            {
                image.UnlockBits(imageData);
            }

            // Write until user press a key
            Console.WriteLine("Press any key to terminate !!");
            Console.Read();

            // Now that you're done, deInit the engine before exiting
            CheckResult("DeInit", FldSdkEngine.deInit());
        }

        static IDictionary<String, String> ParseArgs(String[] args)
        {
            Console.WriteLine("Args: {0}", string.Join(" ", args));

            if ((args.Length & 1) != 0)
            {
                String errMessage = String.Format("Number of args must be even: {0}", args.Length);
                Console.Error.WriteLine(errMessage);
                throw new Exception(errMessage);
            }

            // Parsing
            Dictionary<String, String> values = new Dictionary<String, String>();
            for (int index = 0; index < args.Length; index += 2)
            {
                String key = args[index];
                if (key.Length < 2 || key[0] != '-' || key[1] != '-')
                {
                    String errMessage = String.Format("Invalid key: {0}", key);
                    Console.Error.WriteLine(errMessage);
                    throw new Exception(errMessage);
                }
                values[key] = args[index + 1].Replace("$(ProjectDir)", Properties.Resources.LivenessProjectDir.Trim()); // Patch path to use project directory
            }
            return values;
        }

        static FldSdkResult CheckResult(String functionName, FldSdkResult result)
        {
            if (!result.isOK())
            {
                String errMessage = String.Format("{0}: Execution failed: {1}", new String[] { functionName, result.json() });
                Console.Error.WriteLine(errMessage);
                throw new Exception(errMessage);
            }
            return result;
        }

        // https://www.doubango.org/SDKs/mrz/docs/Configuration_options.html
        static String BuildJSON(String assetsFolder = "", String tokenDataBase64 = "")
        {
            return new JavaScriptSerializer().Serialize(new
            {
                debug_level = "info",
                debug_write_input_image_enabled = false,
                debug_internal_data_path = ".",
                    
                num_threads = -1,
                gpgpu_enabled = true,
                max_latency = -1,
                image_interpolation = "bicubic",
                asm_enabled = true,
                intrin_enabled = true,
                    
                openvino_enabled = false,
                openvino_device = "CPU",
                    
                detect_tf_num_threads = -1,
                detect_tf_gpu_memory_alloc_max_percent = 0.2,
                detect_roi = new[] { 0f, 0f, 0f, 0f },
                detect_minscore = 0.9,
                detect_face_minsize = 128,

                liveness_detect_enabled = true,
                liveness_tf_num_threads = -1,
                liveness_tf_gpu_memory_alloc_max_percent = 0.2,
                liveness_face_minsize = 128,
                liveness_genuine_minscore = 0.98,
                liveness_disputed_minscore = 0.5,
                liveness_toofar_threshold = 0.5,

                deepfake_detect_enabled = true,
                deepfake_tf_num_threads = -1,
                deepfake_tf_gpu_memory_alloc_max_percent = 0.2,
                deepfake_minscore = 0.5,

                disguise_detect_enabled = true,
                disguise_tf_num_threads = -1,
                disguise_tf_gpu_memory_alloc_max_percent = 0.2,
                disguise_minscore = 0.5,

                // Value added using command line args
                assets_folder = assetsFolder,
                license_token_data = tokenDataBase64,
            });
        }
    }
}
