cmake_minimum_required(VERSION 3.4.1)

project(Mobilenetv2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--allow-shlib-undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--allow-shlib-undefined")

# rga
set(RGA_LIB ${CMAKE_SOURCE_DIR}/lib//librga.so)
# rknn api
set(RKNN_RT_LIB ${CMAKE_SOURCE_DIR}/lib/librknnrt.so)
# simple rknn
set(SIMPLE_RKNN2_LIB ${CMAKE_SOURCE_DIR}/lib/libsimple_rknn2.so)

if (CMAKE_C_COMPILER MATCHES "aarch64")
  set(LIB_ARCH aarch64)
else()
  set(LIB_ARCH armhf)
endif()

message(STATUS "ARCH ABI:${LIB_ARCH}")

# opencv
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/opencv/OpenCV-android-sdk/sdk/native/jni/abi-${CMAKE_ANDROID_ARCH_ABI})
else()
  if(LIB_ARCH STREQUAL "armhf")
    set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/opencv/opencv-linux-armhf/share/OpenCV)
  else()
    set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/opencv/opencv-linux-aarch64/share/OpenCV)
  endif()
endif()
find_package(OpenCV REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(mobilenetv2
               mobilenetv2.cpp
)

target_link_libraries(mobilenetv2
	${RKNN_RT_LIB}
  ${RGA_LIB}
  ${OpenCV_LIBS}
  ${SIMPLE_RKNN2_LIB}
)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
install(TARGETS  mobilenetv2 DESTINATION ./)