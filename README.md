- [Getting started](#getting-started)
  - [Checking out the source code](#checkout-source)
  - [Trying samples](#trying-samples) (**C++**, **C#**, **Java** and **Python**)
  - [Getting help](#technical-questions)


- Online web demo at https://www.doubango.org/webapps/face-liveness/
- Full documentation for the SDK at https://www.doubango.org/SDKs/face-liveness/docs/
- Supported languages (API): **C++**, **C#**, **Java** and **Python**
- Open source Computer Vision Library: https://github.com/DoubangoTelecom/compv
<hr />

To our knowledge we're **the only company in the world** that can perform 3D liveness check and [identity concealment](https://www.doubango.org/SDKs/face-liveness/docs/Identity_concealment.html) detection from a single 2D image. We outperform [the competition](https://www.doubango.org/SDKs/face-liveness/docs/Testing_the_competition.html) ([FaceTEC](https://www.doubango.org/SDKs/face-liveness/docs/Testing_the_competition.html#facetec), [BioID](https://www.doubango.org/SDKs/face-liveness/docs/Testing_the_competition.html#bioid), [Onfido](https://www.doubango.org/SDKs/face-liveness/docs/Testing_the_competition.html#onfido), [Huawei](https://www.doubango.org/SDKs/face-liveness/docs/Testing_the_competition.html#huawei)...) in speed and accuracy. **Our implementation is Passive/Frictionless and only takes few milliseconds.**

[Identity concealment](https://www.doubango.org/SDKs/face-liveness/docs/Identity_concealment.html) detects when a user tries to partially hide his/her face (e.g. 3D realistic mask, dark glasses...) or alter the facial features (e.g. heavy makeup, fake nose, fake beard...) to impersonate another user.

**A facial recognition system without liveness detector is just useless.**

We can detect and block all known spoofing attacks: `Paper Print, Screen, Video Replay, 3D (silicone, paper, tissue...) realistic face mask, 2D paper mask, Concealment...`

| 3D Liveness detection | Deepfake detection |
|--- | --|
| [![Doubango AI: 3D Face liveness detector stress test](https://doubango.org/videos/liveness/stress-doubango.jpg)](https://doubango.org/videos/liveness/stress-doubango-x264.mp4) | [![Doubango AI: Deepfake detection](https://doubango.org/videos/liveness/deepfake-zelinsky.jpg)](https://doubango.org/videos/liveness/deepfake-zelinsky-x264.mp4) |


<hr />

Our passive (frictionless) face liveness detector uses SOTA (State Of The Art) deep learning techniques and can be freely tested with your own images at https://www.doubango.org/webapps/face-liveness/
<hr />

<a name="getting-started"></a>
# Getting started #
This version supports both Windows and Linux x86_64.

<a name="checkout-source"></a>
## Checking out the source code ##
[The deep learning models](assets/FaceLivenessDetection-Models) are hosted on private repository for obvious reasons. You have to [send us a mail](https://www.doubango.org/#contact) with your company name and Github user name (to be added to the private repo). The mail must come from @YourCompanyName, mails from other domains (e.g. @Gmail) will be ignored. **The terms of use do not allow you to decompile or reverse engineer the models.**

```
git clone --recurse-submodules -j8 https://github.com/DoubangoTelecom/FaceLivenessDetection-SDK
```

If you already have the code and want to update to the latest version: `git pull --recurse-submodules`

<a name="trying-samples"></a>
## Trying samples (**C++**, **C#**, **Java** and **Python**) ##
Go to the [samples](samples) folder and choose your prefered language.

<a name="technical-questions"></a>
# Technical questions #
Please check our [discussion group](https://groups.google.com/forum/#!forum/doubango-ai) or [twitter account](https://twitter.com/doubangotelecom?lang=en)
