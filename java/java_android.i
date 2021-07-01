/* File : java_android.i 
* http://www.swig.org/Doc1.3/Java.html
*/

// On Android we force loading the library while on Linux/Windows it's up to the
// developer to decide when to do it
%pragma(java) jniclasscode=%{
  static {
    System.loadLibrary("FaceLivenessDetectionSDK");
  }
%}

// Other java options are the same
%include ./java.i
