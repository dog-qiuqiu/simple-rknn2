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
* Take the rk356x as an example
```
1.Modify the gcc path in the "build-linux_RK356X.sh" file，As follows:
  "export TOOL_CHAIN=/media/xuehao/0247cd9a-78fe-4129-ad60-00dfec633e2a/software/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu"
2.Run .sh compile script
  "./build-linux_RK356X.sh"
```
* After compilation, the "Install" folder will be generated
* In the "install/lib" directory, The .so file is the dependent library required to run NPU
* In the "install/include" directory, The .h file is the api definition file to run NPU
* Cross compiler download: 链接: https://pan.baidu.com/s/18auwovQp-PlktEG6w0iMxQ 提取码: 3sv1
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
* ***API use reference ./demo/mobilenetv2.cpp***
# Run Demo
1.Download opencv.zip,Download link: 链接: https://pan.baidu.com/s/19EfJyMfTLPzlI_mTiRs_SQ 提取码: rn3g \
2.Unzip opencv.zip and copy to the ***./demo*** directory,The demo directory structure is as follows：
```
├── build-android_RK3588.sh
├── build-linux_RK356X.sh
├── CMakeLists.txt
├── dog_224x224.jpg
├── mobilenetv2_3568.rknn
├── mobilenetv2_3588.rknn
├── mobilenetv2.cpp
└── opencv

```
3.Compile simple-rknn2 by executing the following command(***Note to modify the compiler path TOOL_CHAIN***),After compilation, the "Install" folder will be generated
```
xuehao@xuehao-Z370-HD3:~/Desktop/simple-rknn2$ ./build-linux_RK356X.sh
```
4.Then Compile mobilenetv2 demo，After compilation, corresponding executable files will be generated in the current directory
```
xuehao@xuehao-Z370-HD3:~/Desktop/simple-rknn2$ cd install
xuehao@xuehao-Z370-HD3:~/Desktop/simple-rknn2/install$ ./build-linux_RK356X.sh
```
5.Last run executable
```
rock@rock-3a:/home/rock/npu# sudo -i
root@rock-3a:/home/rock/npu# ./mobilenetv2 mobilenetv2_3568.rknn 
Load model:mobilenetv2_3568.rknn
sdk version: 1.2.0 (9db21b35d@2022-01-14T15:16:23) driver version: 0.4.2
model input num: 1, output num: 1
  index=0, name=data, n_dims=4, dims=[1, 224, 224, 3], n_elems=150528, size=602112, fmt=NHWC, type=FP32, qnt_type=AFFINE, zp=-13, scale=0.018317
  index=0, name=prob, n_dims=4, dims=[1, 1000, 1, 1], n_elems=1000, size=4000, fmt=NCHW, type=FP32, qnt_type=AFFINE, zp=0, scale=1.000000
model is NHWC input fmt
model input height=224, width=224, channel=3
Rga built version:1.04 788c430+2021-02-24 12:17:35
Forward time:19.2299995422 ms
=========================
index:0 c:1000 h:1 w:1
category:155 score:0.991211
=========================

```
* 注意加上sudo或者切换到root用户下运行，否则或提示没有权限调用RGA
