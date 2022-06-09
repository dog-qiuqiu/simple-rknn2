# simple-rknn2
* The rknn2 API uses the secondary encapsulation of the process, which is easy for everyone to call. It is applicable to rk356x rk3588
# RK SDK version
```
  sdk version: 1.2.0 (1867aec5b@2022-01-14T15:12:19) driver version: 0.6.4

  I Connect to Device success!
  I NPUTransfer: Starting NPU Transfer Client, Transfer version 2.1.0 (b5861e7@2020-11-23T11:50:36)
  D NPUTransfer: Transfer spec = local:transfer_proxy
  D NPUTransfer: Transfer interface successfully opened, fd = 3
  D RKNNAPI: ==============================================
  D RKNNAPI: RKNN VERSION:
  D RKNNAPI:   API: 1.2.0 (4c3573e build: 2022-01-13 20:03:13)
  D RKNNAPI:   DRV: rknn_server: 1.2.0 (4c3573e build: 2022-01-14 11:09:42)
  D RKNNAPI:   DRV: rknnrt: 1.2.0 (1867aec5b@2022-01-14T15:12:19)
  D RKNNAPI: ==============================================
```
* Verified rockchip system software version information that can be run
# How to compile
## Android platform
* Take the rk3588 as an example
```
1.Modify the NDK path in the "build-android_rk3588.sh" file，As follows:
  "ANDROID_NDK_PATH=/media/xuehao/0247cd9a-78fe-4129-ad60-00dfec633e2a/software/android-ndk-r17c"
2.Run .sh compile script
  "./build-android_RK3588.sh"
```
* After compilation, the "Install" folder will be generated
* In the "install/lib" directory, The .so file is the dependent library required to run NPU
* In the "install/include" directory, The .h file is the api definition file to run NPU
* RK推荐版本 NDK r17c
## Linux platform
* Take the rk3588 as an example
```
1.Modify the gcc path in the "build-linux_RK3588.sh" file，As follows:
  "export TOOL_CHAIN=/media/xuehao/0247cd9a-78fe-4129-ad60-00dfec633e2a/software/gcc-9.3.0-x86_64_arrch64-linux-gnu"
2.Run .sh compile script
  "./build-linux_RK3588.sh"
```
* After compilation, the "Install" folder will be generated
* In the "install/lib" directory, The .so file is the dependent library required to run NPU
* In the "install/include" directory, The .h file is the api definition file to run NPU
* RK推荐版本 gcc-9.3.0-x86_64_arrch64-linux-gnu
# Data structure
* The data structure is defined in type.h
```c++
typedef struct _FeatureMap {
    int channel;
    int width;
    int height;
    const void *buf;
} FeatureMap;

```

| Member parameters  | describe                                       
| -------------------| ------------------------------------------|
| channel            | feature map tensor channel                |
| width              | feature map tensor width                  |
| height             | feature map tensor height                 |
| buf                | feature map tensor buffer memory address  |

```c++
typedef struct _InputImg {
    unsigned char *data;    //图像buffer的地址
    int width;              //输入图像的宽
    int height;             //输入图像的高
} InputImg;eatureMap;

```
| Member parameters  | describe                                       
| -------------------| ------------------------------------------|
| width              | image width                               |
| height             | image height                              |
| buf                | image buffer memory address               |
# API definition
* The api fun is defined in simple_rknn2_pimpl.h
```C++
int LoadModel(const char *model_path);
```
> load rknn model

| Parameter | Description                          |
| --------- | ------------------------------------ |
| model_path| **[required]** input .rknn model path|

```C++
int Forward(const InputImg &src_img, std::vector<FeatureMap> &dst_feature_map);
```
> model forward

| Parameter      | Description                          |
| ---------------| ------------------------------------ |
| src_img        | **[required]** input image           |
| dst_feature_map| **[required]** output feature map    |

* Only RGB888 or BGR888 three channel images can be input
