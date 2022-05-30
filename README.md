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
* Take the rk3588 Android platform as an example
```
1.Modify the NDK path in the "build android_rk3588.sh" file，As follows:
  "ANDROID_NDK_PATH=/media/xuehao/0247cd9a-78fe-4129-ad60-00dfec633e2a/software/android-ndk-r17c"
2.Run .sh compile script
  "./build-android_RK3588.sh"
```
* After compilation, the "Install" folder will be generated
* In the "install/lib" directory, The .so file is the dependent library required to call rk npu2
* In the "install/include" directory, The .h file is the interface file to call NPU
