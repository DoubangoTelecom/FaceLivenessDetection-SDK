/* Copyright (C) 2011-2022 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: For non commercial use only.
* Source code: https://github.com/DoubangoTelecom/FaceLivenessDetection-SDK
* WebSite: https://www.doubango.org/webapps/face-liveness/
*/

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Hashtable;
import java.util.IllegalFormatException;
import java.util.List;
import java.util.Arrays;
import java.util.stream.Collectors;
import java.lang.IllegalArgumentException;

import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

import org.doubango.FaceLivenessDetection.Sdk.FLD_SDK_IMAGE_TYPE;
import org.doubango.FaceLivenessDetection.Sdk.FldSdkEngine;
import org.doubango.FaceLivenessDetection.Sdk.FldSdkResult;

public class Liveness {

   public static void main(String[] args) throws IllegalArgumentException, FileNotFoundException, IOException {
      // Parse arguments
      final Hashtable<String, String> parameters = ParseArgs(args);

      // Make sur the image is provided using args
      if (!parameters.containsKey("--image"))
      {
         System.err.println("--image required");
         throw new IllegalArgumentException("--image required");
      }
      // Extract assets folder
      // https://www.doubango.org/SDKs/face-liveness/docs/Configuration_options.html#assets-folder
      String assetsFolder = parameters.containsKey("--assets")
          ? parameters.get("--assets") : "";

      // License data - Optional
      // https://www.doubango.org/SDKs/face-liveness/docs/Configuration_options.html#license-token-data
      String tokenDataBase64 = parameters.containsKey("--tokendata")
          ? parameters.get("--tokendata") : "";

      // Load the native library
      System.loadLibrary("FaceLivenessDetectionSDK");

      // Initialize the engine
      FldSdkResult result = CheckResult("Init", FldSdkEngine.init(BuildJSON(assetsFolder, tokenDataBase64)));

      // Decode the JPEG/PNG/BMP file
      final File file = new File(parameters.get("--image"));
      if (!file.exists())
      {
          throw new FileNotFoundException("File not found: " + file.getAbsolutePath());
      }

      // WarmUp: Force loading the models in memory (slow for first time) now and perform warmup calls.
      // Warmup not required but processing will be fast if you call warm up first.
      result = CheckResult("warmUp", FldSdkEngine.warmUp(FLD_SDK_IMAGE_TYPE.FLD_SDK_IMAGE_TYPE_RGB24));

      // Reading The data
      ByteBuffer nativeBuffer = null;
      try(RandomAccessFile inFile = new RandomAccessFile(file, "r")) {
         final FileChannel inChannel = inFile.getChannel();
         nativeBuffer = ByteBuffer.allocateDirect((int)inChannel.size());
         inChannel.read(nativeBuffer);
         nativeBuffer.flip();         
      } catch (IOException e) {
         e.printStackTrace();
         throw e;
      }
      
      // Processing
      // First inference is expected to be slow (deep learning models mapping to CPU/GPU memory)
      // Call warmUp function to avoid slow processing on first call
      result = CheckResult("Process", FldSdkEngine.process(
            nativeBuffer,
            nativeBuffer.remaining()
         ));
      // Print result to console
      System.out.println("Result: " + result.json() + System.lineSeparator());

       // Wait until user press a key
       System.out.println("Press any key to terminate !!" + System.lineSeparator());
       final java.util.Scanner scanner = new java.util.Scanner(System.in);
       if (scanner != null) {
         scanner.nextLine();
         scanner.close();
       }

       // Now that you're done, deInit the engine before exiting
       CheckResult("DeInit", FldSdkEngine.deInit());
   }

   static Hashtable<String, String> ParseArgs(String[] args) throws IllegalArgumentException
   {
      System.out.println("Args: " + String.join(" ", args) + System.lineSeparator());

      if ((args.length & 1) != 0)
      {
            String errMessage = String.format("Number of args must be even: %d", args.length);
            System.err.println(errMessage);
            throw new IllegalArgumentException(errMessage);
      }

      // Parsing
      Hashtable<String, String> values = new Hashtable<String, String>();
      for (int index = 0; index < args.length; index += 2)
      {
            String key = args[index];
            if (!key.startsWith("--"))
            {
               String errMessage = String.format("Invalid key: %s", key);
               System.err.println(errMessage);
               throw new IllegalArgumentException(errMessage);
            }
            values.put(key, args[index + 1].replace("$(ProjectDir)", System.getProperty("user.dir").trim()));
      }
      return values;
   }

   static FldSdkResult CheckResult(String functionName, FldSdkResult result) throws IOException
   {
      if (!result.isOK())
      {
            String errMessage = String.format("%s: Execution failed: %s", functionName, result.json());
            System.err.println(errMessage);
            throw new IOException(errMessage);
      }
      return result;
   }

   // https://www.doubango.org/SDKs/face-liveness/docs/Configuration_options.html
   static String BuildJSON(String assetsFolder, String tokenDataBase64)
   {
      return String.format(
         "{" +
         "\"debug_level\": \"info\"," +
         "\"debug_write_input_image_enabled\": false," +
         "\"debug_internal_data_path\": \".\"," +
         "" +
         "\"num_threads\": -1," +
         "\"gpgpu_enabled\": true," +
         "\"max_latency\": -1," +
         "\"image_interpolation\": \"bicubic\"," +
         "\"asm_enabled\": true," +
         "\"intrin_enabled\": true," +
         "" +
         "\"openvino_enabled\": false," +
         "\"openvino_device\": \"CPU\"," +
         "" +
         "\"detect_tf_num_threads\": -1," +
         "\"detect_tf_gpu_memory_alloc_max_percent\": 0.2," +
         "\"detect_roi\": [0, 0, 0, 0]," +
         "\"detect_minscore\": 0.9," +
         "\"detect_face_minsize\": 65," +
         "" +
         "\"liveness_detect_enabled\": true," +
         "\"liveness_tf_num_threads\": -1," +
         "\"liveness_tf_gpu_memory_alloc_max_percent\": 0.2," +
         "\"liveness_face_minsize\": 64," +
         "\"liveness_genuine_minscore\": 0.98," +
         "\"liveness_disputed_minscore\": 0.5," +
         "\"liveness_toofar_threshold\": 0.5," +
         "" +
         "\"deepfake_detect_enabled\": true," +
         "\"deepfake_tf_num_threads\": -1," +
         "\"deepfake_tf_gpu_memory_alloc_max_percent\": 0.2," +
         "\"deepfake_minscore\": 0.5," +
         "" +
         "\"disguise_detect_enabled\": true," +
         "\"disguise_tf_num_threads\": -1," +
         "\"disguise_tf_gpu_memory_alloc_max_percent\": 0.2," +
         "\"disguise_minscore\": 0.5," +
         "" +
         "\"assets_folder\": \"%s\"," +
         "\"license_token_data\": \"%s\"" +
         "}"
         , 
         // Value added using command line args
         assetsFolder,
         tokenDataBase64
      );
   }
}