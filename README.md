 - Online web demo at https://www.doubango.org/webapps/face-liveness/
 - Full documentation for the SDK at https://www.doubango.org/SDKs/face-liveness/docs/
 - Supported languages (API): **C++**, **C#**, **Java** and **Python**
 - Open source Computer Vision Library: https://github.com/DoubangoTelecom/compv
<hr />

_**Question:**_ Which one of these next 3 pictures is genuine?

| Merkel | Trudeau | Ardern |
|---|---|---|
| ![alt text](https://www.doubango.org/SDKs/face-liveness/docs/_images/spoof/cropped/20210331_202952.jpg "Merkel") | ![alt text](https://www.doubango.org/SDKs/face-liveness/docs/_images/spoof/cropped/20210331_203403.jpg "Trudeau") | ![alt text](https://www.doubango.org/SDKs/face-liveness/docs/_images/spoof/cropped/20210331_204408.jpg "Ardern") |

_**Response:**_ **None. They are all spoofs.**

The original images are from [wikimedia](https://upload.wikimedia.org/) and can be found [here](https://upload.wikimedia.org/wikipedia/commons/0/0f/Angela_Merkel_2019_cropped.jpg), [here](https://upload.wikimedia.org/wikipedia/commons/9/98/Prime_Minister_Trudeau_-_2020_(cropped).jpg) and [here](https://upload.wikimedia.org/wikipedia/commons/b/b6/New_Zealand_Prime_Minister_Jacinda_Ardern_in_2018.jpg). We recaptured the original images using a **Galaxy S10+ with 4K resolution** on an **iMac with 4K retina display**. Then, we cropped the images. The entire images can be found [here](https://www.doubango.org/SDKs/face-liveness/docs/_images/spoof/20210331_202952.jpg), [here](https://www.doubango.org/SDKs/face-liveness/docs/_images/spoof/20210331_203403.jpg) and [here](https://www.doubango.org/SDKs/face-liveness/docs/_images/spoof/20210331_204408.jpg).

These kind of high resolution (quality) Print-Attacks are hard to detect and most liveness detectors would fail the test. 
Our passive (frictionless) face liveness detector uses **SOTA (State Of The Art)** deep learning techniques to spot both **Print-Attack** and **Replay-Attack**. You can freely test our implementation with your own images at https://www.doubango.org/webapps/face-liveness/

We're working to package and release the source code of the SDK. This document will be updated to include the API reference and a Getting Started guide.

The next [video](https://youtu.be/4Z8VRTS8WrA) ([https://youtu.be/4Z8VRTS8WrA](https://youtu.be/4Z8VRTS8WrA)) shows the liveness detector on **Replay-Attack**.<br />
[![Replay-Attack on Passive Face Liveness detector (anti-spoofing)](docs/images/youtube/Replay-Attack_thumbnail-0.jpg)](https://www.youtube.com/watch?v=4Z8VRTS8WrA)
<hr />

